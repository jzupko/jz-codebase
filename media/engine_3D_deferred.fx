//
// Copyright (c) 2009 Joseph A. Zupko
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#ifndef _ENGINE_3D_DEFERRED_FX_
#define _ENGINE_3D_DEFERRED_FX_

#define JZ_DEBUG_AO 0
#define JZ_DISABLE_BLUR 0
#define JZ_DISABLE_DIRECTIONAL_AO 0
#define JZ_MATH_CORRECT_SHADOW_FILTERING 0

#include "../media/fb_common.fxh"
#include "../media/lighting_common.fxh"
#include "../media/screen_space_common.fxh"

//////////////////////////////////////////////////////////////////////////////////////////
// constants
//////////////////////////////////////////////////////////////////////////////////////////
static const int kKernelRadius = 11;
static const int kMotionBlurTaps = kKernelRadius;
static const int kCircularTaps = 16;
static const float kNormalDistanceMin = 1e-2;

# if !NDEBUG
    static const float3 kDebugDirectionalColor = float3(1, 0, 0);
    static const float3 kDebugPointColor = float3(0, 1, 0);
    static const float3 kDebugSpotColor = float3(0, 0, 1);
    
    bool bDebugDeferred : jz_bDebugDeferred = false;
# endif  

//////////////////////////////////////////////////////////////////////////////////////////
// in/out
//////////////////////////////////////////////////////////////////////////////////////////
struct vsIn { float4 Position : POSITION; };
struct vsOut { float4 Position : POSITION; float4 ScreenHTexCoords : TEXCOORD0; };

//////////////////////////////////////////////////////////////////////////////////////////
// runtime constants
//////////////////////////////////////////////////////////////////////////////////////////
texture ShadowTexture : jz_ShadowTexture;
sampler ShadowSampler = sampler_state { texture = <ShadowTexture>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; };
sampler ShadowSamplerFilterable = sampler_state { texture = <ShadowTexture>; AddressU = clamp; AddressV = clamp; MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = NONE; };

float2 ShadowDelta : jz_ShadowDelta = float2(0, 0);
float2 ShadowNearFar : jz_ShadowNearFar = float2(0, 0);
float4 ShadowScaleShift : jz_ShadowScaleShift = float4(0, 0, 0, 0);
float4x4 ShadowTransform : jz_ShadowTransform = kIdentity;
float ShadowControlTerm : jz_ShadowControlTerm = 1.0;

float4x4 PrevProjection : jz_PrevProjection = kIdentity;
float4x4 PrevView : jz_PrevView = kIdentity;
float MotionBlurAmount : jz_MotionBlurAmount = 0.25;

float BloomThreshold : jz_BloomThreshold = 0.9;

float GaussianWeights[kKernelRadius] : jz_GaussianWeights;

texture HdrTexture : jz_HdrTexture;
sampler HdrSampler = sampler_state { texture = <HdrTexture>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; };

float3 RandomVectors[kCircularTaps] : jz_RandomVectors;

float AoRadius : jz_AoRadius = 16.0f;
float AoScale : jz_AoScale = 0.0f;
bool bDebugAO : jz_bDebugAO = false;

//////////////////////////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////////////////////////
float3 GetNonDirectionalLight(vsOut aIn, out float3 arLv, out float arDistance, out float3 arViewPos)
{
	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float3 diffuse = GetDeferredDiffuse(aIn.ScreenHTexCoords);
    float4 specularShininess = GetDeferredSpecularAndShininess(aIn.ScreenHTexCoords);
	arViewPos = GetViewPosition(aIn.ScreenHTexCoords, nd.w);

    arLv = (LightV - arViewPos);
    float distanceSquare = dot(arLv, arLv);
    arDistance = sqrt(distanceSquare);
    if (arDistance > kZeroTolerance) { arLv = normalize(arLv); }

    float3 ev = normalize(-arViewPos);
	float3 nv = (nd.xyz);
	float ndotl = dot(nv, arLv);

    float3 color = GetLitColorAtt(
        LightColor, 
        diffuse, GetDiffuseFactor(ndotl), 
        specularShininess.rgb, GetSpecularFactor(ev, GetReflectionVector(nv, arLv, ndotl), ndotl, specularShininess.a),
        GetLightAttenuation(LightAttenuation, arDistance, distanceSquare));
	
	return color;
}

float GetShadowFactor(sampler aSampler, float2 aShadowDelta, float4 aShadowTexCoords, float4 aShadowScaleShift, float aPixelDepth, float aShadowControlTerm)
{
	//float2 coords = ((aShadowTexCoords.xy / aShadowTexCoords.w) * aShadowScaleShift.xy) + aShadowScaleShift.zw;
    float4 coords =
        float4(aShadowTexCoords.xy * aShadowScaleShift.xy + (aShadowTexCoords.w * aShadowScaleShift.zw),
        aShadowTexCoords.zw);

    float pd = (aPixelDepth);
    // float sd = tex2D(aSampler, coords).r;
    float sd = tex2Dproj(aSampler, coords).r;

    float ret = exp(512.0 * aShadowControlTerm * min(sd - pd, 0.0));

    return ret;
}

float3 GetAO(float4 huv)
{
	return tex2Dproj(HdrSampler, huv).xyz;
}

float3 BlurAOPass2(float4 huv)
{
	float3 ret = kZero3;
	int max = (kKernelRadius - 1);
	
	for (int i = -max; i <= -1; i++)
	{
		ret += GetAO(OffsetHuvH(huv, i)) * GaussianWeights[-i];
	}
	
	ret += GetAO(huv) * GaussianWeights[0];
	
	for (int i = 1; i <= max; i++)
	{
		ret += GetAO(OffsetHuvH(huv, i)) * GaussianWeights[i];
	}
	
	return ret;
}

float3 BlurAOPass3(float4 huv)
{
	float3 ret = kZero3;
	int max = (kKernelRadius - 1);
	
	for (int i = -max; i <= -1; i++)
	{
		ret += GetAO(OffsetHuvV(huv, i)) * GaussianWeights[-i];
	}
	
	ret += GetAO(huv) * GaussianWeights[0];
	
	for (int i = 1; i <= max; i++)
	{
		ret += GetAO(OffsetHuvV(huv, i)) * GaussianWeights[i];
	}
	
	return ret;
}

float GetShadow(float4 huv)
{
	return tex2Dproj(ShadowSampler, huv).r;
}

float GetShadowFiltered(float4 huv)
{
	return tex2Dproj(ShadowSamplerFilterable, huv).r;
}

#if JZ_MATH_CORRECT_SHADOW_FILTERING
float LogConvolution(float c0, float c1, float a, float b)
{
    return (a + log(c1 * exp(b - a) + c0));
}

float GetShadowBlurPass1(float4 huv)
{
	float ret = 0.0;
	int max = (kKernelRadius - 1);
	
    ret = LogConvolution(0.5, 0.5, GetShadowFiltered(OffsetHuvH(huv, -max)), GetShadowFiltered(OffsetHuvH(huv, -max + 1)));

    for (int i = (-max + 2); i <= max; i++)
    {
        ret = LogConvolution(0.5, 0.5, GetShadowFiltered(OffsetHuvH(huv, i)), ret);
    }
	
	return ret;
}

float GetShadowBlurPass2(float4 huv)
{
	float ret = 0.0;
	int max = (kKernelRadius - 1);
	
    ret = LogConvolution(0.5, 0.5, GetShadowFiltered(OffsetHuvV(huv, -max)), GetShadowFiltered(OffsetHuvV(huv, -max + 1)));

    for (int i = (-max + 2); i <= max; i++)
    {
        ret = LogConvolution(0.5, 0.5, GetShadowFiltered(OffsetHuvV(huv, i)), ret);
    }
	
	return ret;
}
#else
float GetShadowBlurPass1(float4 huv)
{
	float ret = 0.0;
	int max = (kKernelRadius - 1);
	
	for (int i = -max; i <= -1; i++)
	{
		ret += GetShadowFiltered(OffsetHuvH(huv, i)) * GaussianWeights[-i];
	}
	
	ret += GetShadowFiltered(huv) * GaussianWeights[0];
	
	for (int i = 1; i <= max; i++)
	{
		ret += GetShadowFiltered(OffsetHuvH(huv, i)) * GaussianWeights[i];
	}
	
	return ret;
}

float GetShadowBlurPass2(float4 huv)
{
	float ret = 0.0;
	int max = (kKernelRadius - 1);
	
	for (int i = -max; i <= -1; i++)
	{
		ret += GetShadowFiltered(OffsetHuvV(huv, i)) * GaussianWeights[-i];
	}
	
	ret += GetShadowFiltered(huv) * GaussianWeights[0];
	
	for (int i = 1; i <= max; i++)
	{
		ret += GetShadowFiltered(OffsetHuvV(huv, i)) * GaussianWeights[i];
	}
	
	return ret;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// vertexes
//////////////////////////////////////////////////////////////////////////////////////////
vsOut VertexQuad(vsIn aIn)
{
    vsOut ret;
    
	ret.Position = float4(aIn.Position.xy, 0, 1); 
    ret.ScreenHTexCoords = GetScreenHTexCoords(ret.Position);
	
    return ret;
}

vsOut VertexDeferredLight(vsIn aIn)
{
    vsOut ret;

	float4 world = mul(aIn.Position, GetWorld());
	float4 view = mul(world, GetView());
	
	ret.Position = GetInfiniteHPosFromView(view);
    ret.ScreenHTexCoords = GetScreenHTexCoords(ret.Position);
	
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
// fragments
//////////////////////////////////////////////////////////////////////////////////////////
float3 AoVer2Contrib(float4 coords, float3 aViewPosition, float4 aPixelPlane)
{
	float3 vp = GetViewPosition(coords, GetDeferredDepth(coords));
	float  dc = DotCoordinate(aPixelPlane, vp);
	float3 df = (vp - aViewPosition);
	float3 ao = df;
	
#	if JZ_DISABLE_DIRECTIONAL_AO
		float f = (dc > kNormalDistanceMin) ? (1.0 - (1.0 / (1.0 + dot(ao, ao)))) : 1.0;
		float3 ret = float3(f, f, f);
#	else
		float3 ret = (dc > kNormalDistanceMin) ? (kOne3 - (1.0 / (kOne3 + (ao * ao)))) : kOne3;
#	endif
	
	return ret;
}

float4 FragmentAOPass1(vsOut aIn) : COLOR
{
	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float3 vPos = GetViewPosition(aIn.ScreenHTexCoords, nd.w);
	float4 plane = GetPlane(nd.xyz, vPos);
	
	float2 scale = AoRadius * (-CameraFocalLength / vPos.z);
	
	float3 accum = kZero3;
	for (float i = 0; i < kCircularTaps; i++)
	{
		float2 t = (RandomVectors[i].xy * scale);
		float4 c = OffsetHuv(aIn.ScreenHTexCoords, t);
		
		accum += AoVer2Contrib(c, vPos, plane);
	}

	float3 f = (accum / kCircularTaps);
	
	return float4(f, 0.0);
}

float4 FragmentAOPass2(vsOut aIn) : COLOR
{
#	if JZ_DISABLE_BLUR
		float3 f = GetAO(aIn.ScreenHTexCoords);
#	else
		float3 f = BlurAOPass2(aIn.ScreenHTexCoords);
#	endif

	return float4(f, 0.0);
}

#define JZ_ENABLE_AO_SCALE 1
float3 ApplyAoScale(float3 f)
{
#	if JZ_ENABLE_AO_SCALE
		float3 ret = saturate((f + float3(AoScale, AoScale, AoScale)) / (kOne3 + float3(AoScale, AoScale, AoScale)));
	
		return ret;
#	else
		return f;
#	endif
}

float4 FragmentAOPass3(vsOut aIn) : COLOR0
{
#	if JZ_DISABLE_BLUR
		float3 f = ApplyAoScale(GetAO(aIn.ScreenHTexCoords));
#	else
		float3 f = ApplyAoScale(BlurAOPass3(aIn.ScreenHTexCoords));
#	endif

	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);

#	if JZ_DEBUG_AO
		float4 ret = float4(f, nd.w);
#	else
		float4 ret = float4(nd.xyz * f, nd.w);
#	endif
	
	return ret;	
}

litFbOut FragmentLdrPass(vsOut aIn)
{
#	if JZ_DEBUG_AO
		litFbOut ret = Out(float4(GetDeferredNormal(aIn.ScreenHTexCoords), 1.0));
		return ret;
#	else	
		float4 litColor = GetDeferredLit(aIn.ScreenHTexCoords);
		
		litFbOut ret = Out(LinearToNonLinear(litColor));
		return ret;
#	endif
}

float4 FragmentShadowBlurPass1(vsOut aIn) : COLOR0
{
#   if JZ_DISABLE_BLUR
        float4 ret = float4(GetShadow(aIn.ScreenHTexCoords), 0, 0, 0);
#   else
        float4 ret = float4(GetShadowBlurPass2(aIn.ScreenHTexCoords), 0, 0, 0);
#   endif

    return ret;
}

float4 FragmentShadowBlurPass2(vsOut aIn) : COLOR0
{
#   if JZ_DISABLE_BLUR
        float4 ret = float4(GetShadow(aIn.ScreenHTexCoords), 0, 0, 0);
#   else
        float4 ret = float4(GetShadowBlurPass2(aIn.ScreenHTexCoords), 0, 0, 0);
#   endif

    return ret;
}

float4 FragmentBloomProcess(vsOut aIn) : COLOR
{
    float4 b = GetDeferredLit(aIn.ScreenHTexCoords);

    float3 c = (kOne3 * BloomThreshold);
    float4 ret = float4(max(b - c, kZero3), 1.0);

    return ret;
}

float4 FragmentBloomBlurPass1(vsOut aIn) : COLOR
{
	float3 f = BlurAOPass2(aIn.ScreenHTexCoords);

	return float4(f, 0.0);
}

float4 FragmentBloomBlurPass2(vsOut aIn) : COLOR0
{
    float4 b = GetDeferredLit(aIn.ScreenHTexCoords);
    float3 f = BlurAOPass3(aIn.ScreenHTexCoords);

    float4 ret = float4(b.rgb + f, b.a);

    return ret;
}

float4 FragmentDirectional(vsOut aIn) : COLOR
{
	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float3 diffuse = GetDeferredDiffuse(aIn.ScreenHTexCoords);

	float3 nv = (nd.xyz);
	float ndotl = dot(nv, -LightV);

	float3 lightColor = max(ndotl, 0.0) * LightColor;
	float3 color = (diffuse * lightColor);
		
#	if !NDEBUG
		if (bDebugDeferred) { color = kDebugDirectionalColor; }
#	endif

    return float4(color, 1);
}

float4 FragmentPoint(vsOut aIn) : COLOR
{
    float3 lv;
    float distance;
    float3 vPos;
    
	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos);
#   if !NDEBUG
		if (bDebugDeferred) { color = kDebugPointColor; }
#	endif

    return float4(color, 1);
}

float4 FragmentSpot(vsOut aIn) : COLOR
{
    float3 lv;
    float distance;
    float3 vPos;

	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos);
	float spot = GetSpotFactor(lv, SpotDirection, SpotFalloffExponent, SpotFalloffCosHalfAngle);

#   if !NDEBUG
		if (bDebugDeferred) { color = (kDebugSpotColor); }
		else { color *= spot; }
#	else
		color *= spot;
#	endif

    return float4(color, 1);
}

float4 FragmentSpotWithShadow(vsOut aIn) : COLOR
{
    float3 lv;
    float distance;
    float3 vPos;

	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos);
	float spot = GetSpotFactor(lv, SpotDirection, SpotFalloffExponent, SpotFalloffCosHalfAngle);
	
	float4 shadowTexCoords = mul(float4(vPos, 1.0), ShadowTransform);
	float pixelDepth = ((distance - ShadowNearFar.x) / (ShadowNearFar.y - ShadowNearFar.x));
	float shadow = GetShadowFactor(ShadowSampler, ShadowDelta, shadowTexCoords, ShadowScaleShift, pixelDepth, ShadowControlTerm);
	
#   if !NDEBUG
		if (bDebugDeferred) { color = (spot * shadow * kDebugSpotColor); }
		else { color *= (spot * shadow); }
#	else
		color *= (spot * shadow);
#	endif

    return float4(color, 1);
}

float4x4 GetInverseViewPrevViewProjection()
{
    float4x4 ret = mul(mul(GetInverseView(), PrevView), PrevProjection);

    return ret;
}

float4 FragmentMotionBlur(vsOut aIn) : COLOR0
{
    static const float kInverseTaps = (1.0 / kMotionBlurTaps);

	float viewDepth = GetDeferredDepth(aIn.ScreenHTexCoords);
    float4 viewPosition = float4(GetViewPosition(aIn.ScreenHTexCoords, viewDepth), 1.0);

    float4 ch = mul(viewPosition, GetProjection());
    float4 ph = mul(viewPosition, GetInverseViewPrevViewProjection());
    
    float2 current = (ch.xy / ch.w);
    float2 prev = clamp(ph.xy / ph.w, float2(-1, -1), float2(1, 1));

    float2 displacement = (prev - current);
    float2 step = MotionBlurAmount * (displacement * kInverseTaps);

    // Scale from [-2, 2] to [-1, 1] and invert y for texture lookups
    float4 offset = float4(step.x * 0.5, -step.y * 0.5, 0, 0);

    float4 uv = aIn.ScreenHTexCoords;
    float3 color = kZero3;
    for (int i = 0; i < kMotionBlurTaps; i++)
    {
        color += GetDeferredLit(uv).rgb * GaussianWeights[i];
        uv += offset;
    }

    float4 ret = float4(color, 1.0);

    return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
// techniques
//////////////////////////////////////////////////////////////////////////////////////////
technique AoPass1
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT		
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_a VertexQuad();
		PixelShader = compile ps_2_a FragmentAOPass1();
    }
}

technique AoPass2
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_a VertexQuad();
		PixelShader = compile ps_2_a FragmentAOPass2();
    }
}

technique AoPass3
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_a VertexQuad();
		PixelShader = compile ps_2_a FragmentAOPass3();
    }
}

technique LdrPass
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentLdrPass();
    }
}

technique ShadowBlurPass1
{
    pass
    {
		DISABLE_STENCIL
		JZ_SHADOW_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

#if JZ_MATH_CORRECT_SHADOW_FILTERING
		VertexShader = compile vs_2_a VertexQuad();
		PixelShader = compile ps_2_a FragmentShadowBlurPass1();
#else
		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentShadowBlurPass1();
#endif
    }
}

technique ShadowBlurPass2
{
    pass
    {
		DISABLE_STENCIL
		JZ_SHADOW_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

#if JZ_MATH_CORRECT_SHADOW_FILTERING
		VertexShader = compile vs_2_a VertexQuad();
		PixelShader = compile ps_2_a FragmentShadowBlurPass2();
#else
		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentShadowBlurPass2();
#endif
    }
}

technique BloomProcess
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentBloomProcess();
    }
}

technique BloomBlurPass1
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentBloomBlurPass1();
    }
}

technique BloomBlurPass2
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentBloomBlurPass2();
    }
}

technique MotionBlur
{
    pass
    {
		DISABLE_STENCIL
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = BACK_FACE_CULLING;
        FillMode = Solid;
		ZEnable = false;
        ZWriteEnable = false;

		VertexShader = compile vs_2_a VertexQuad();
		PixelShader = compile ps_2_a FragmentMotionBlur();        
    }
}

#define DISABLE_MASKING
#define DRAW_FRONT_FACES
#define VERTEX_SHADER_NAME VertexQuad
#define TECHNIQUE_NAME Directional
#include "../media/deferred_lighting_technique_helper.fxh"

#define TECHNIQUE_NAME Point
#include "../media/deferred_lighting_technique_helper.fxh"

#define TECHNIQUE_NAME Spot
#include "../media/deferred_lighting_technique_helper.fxh"

#define TECHNIQUE_NAME SpotWithShadow
#include "../media/deferred_lighting_technique_helper.fxh"

#define DISABLE_MASKING
#define DRAW_FRONT_FACES
#define FRAGMENT_SHADER_NAME FragmentPoint
#define VERTEX_SHADER_NAME VertexQuad
#define TECHNIQUE_NAME PointAsQuad
#include "../media/deferred_lighting_technique_helper.fxh"

#define DISABLE_MASKING
#define DRAW_FRONT_FACES
#define FRAGMENT_SHADER_NAME FragmentSpot
#define VERTEX_SHADER_NAME VertexQuad
#define TECHNIQUE_NAME SpotAsQuad
#include "../media/deferred_lighting_technique_helper.fxh"

#define DISABLE_MASKING
#define DRAW_FRONT_FACES
#define FRAGMENT_SHADER_NAME FragmentSpotWithShadow
#define VERTEX_SHADER_NAME VertexQuad
#define TECHNIQUE_NAME SpotWithShadowAsQuad
#include "../media/deferred_lighting_technique_helper.fxh"

#endif
