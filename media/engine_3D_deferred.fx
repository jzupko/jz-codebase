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

#define JZ_DISABLE_DOF_HALO_COMPENSATION 0

#include "../media/fb_common.fxh"
#include "../media/lighting_common.fxh"
#include "../media/screen_space_common.fxh"

//////////////////////////////////////////////////////////////////////////////////////////
// constants
//////////////////////////////////////////////////////////////////////////////////////////
// Bloom and Motion Blur constants
static const int kKernelRadius = 9;
static const int kShadowTapRadius = 5;
static const int kMotionBlurTaps = kKernelRadius;

// Ao Constants
static const float kAoBiasFactor = 512.0;
static const int kTapsRt = 5;
static const int kCircularTaps = (kTapsRt * kTapsRt);
static const float kNormalDistanceMin = 0.01;

// Dof Constants
static const float kMaxF2 = (3.0 / 4.0);

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

float AoRadius : jz_AoRadius = 128.0f;
float AoScale : jz_AoScale = 0.0f;
bool bDebugAO : jz_bDebugAO = false;

float3 FocusDistances : jz_FocusDistances = float3(0.0, 0.0, 0.0);

//////////////////////////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////////////////////////
float3 GetNonDirectionalLight(vsOut aIn, out float3 arLv, out float arDistance, out float3 arViewPos, bool abIncludeAo)
{
	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float4 diffuse = GetDeferredDiffuse(aIn.ScreenHTexCoords);
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
        diffuse.rgb, GetDiffuseFactor(ndotl), 
        specularShininess.rgb, GetSpecularFactor(ev, GetReflectionVector(nv, arLv, ndotl), ndotl, specularShininess.a),
        GetLightAttenuation(LightAttenuation, arDistance, distanceSquare));
	
    return (abIncludeAo) ? color * diffuse.a : color;
}

float GetShadowFactor(sampler aSampler, float2 aShadowDelta, float4 aShadowTexCoords, float4 aShadowScaleShift, float aPixelDepth, float aShadowControlTerm)
{
    float4 coords =
        float4(aShadowTexCoords.xy * aShadowScaleShift.xy + (aShadowTexCoords.w * aShadowScaleShift.zw),
        aShadowTexCoords.zw);

    float pd = (aPixelDepth);
    float sd = tex2Dproj(aSampler, coords).r;

    float ret = exp(512.0 * aShadowControlTerm * min(sd - pd, 0.0));

    return ret;
}

float GetShadow(float4 huv)
{
	return tex2Dproj(ShadowSampler, huv).r;
}

float GetShadowBlurPass1(float4 huv)
{
    static const int kMax = (kShadowTapRadius);
    static const float kFactor = 1.0 / (kMax * 2.0 + 1.0);
	
    float ret = 0.0;
    for (int i = -kMax; i <= kMax; i++)
    {
        ret += exp(GetShadow(OffsetHuvH(huv, i)));
    }

    ret = log(ret * kFactor);
	
	return ret;
}

float GetShadowBlurPass2(float4 huv)
{
    static const int kMax = (kShadowTapRadius);
    static const float kFactor = 1.0 / (kMax * 2.0 + 1.0);
	
    float ret = 0.0;
    for (int i = -kMax; i <= kMax; i++)
    {
        ret += exp(GetShadow(OffsetHuvV(huv, i)));
    }

    ret = log(ret * kFactor);
	
	return ret;
}

float3 GetHdr(float4 huv)
{
	return tex2Dproj(HdrSampler, huv).rgb;
}

float3 GetHdrBlurPass1(float4 huv)
{
	float3 ret = kZero3;
	int max = (kKernelRadius - 1);
	
	for (int i = -max; i <= -1; i++)
	{
		ret += GetHdr(OffsetHuvH(huv, i)) * GaussianWeights[-i];
	}
	
	ret += GetHdr(huv) * GaussianWeights[0];
	
	for (int i = 1; i <= max; i++)
	{
		ret += GetHdr(OffsetHuvH(huv, i)) * GaussianWeights[i];
	}
	
	return ret;
}

float3 GetHdrBlurPass2(float4 huv)
{
	float3 ret = kZero3;
	int max = (kKernelRadius - 1);
	
	for (int i = -max; i <= -1; i++)
	{
		ret += GetHdr(OffsetHuvV(huv, i)) * GaussianWeights[-i];
	}
	
	ret += GetHdr(huv) * GaussianWeights[0];
	
	for (int i = 1; i <= max; i++)
	{
        ret += GetHdr(OffsetHuvV(huv, i)) * GaussianWeights[i];
	}
	
	return ret;
}

float GetDofBlurinessFactor(float4 c)
{
    float viewDepth = GetDeferredDepth(c);
    float3 viewPosition = GetViewPosition(c, viewDepth);

    float f = abs(FocusDistances.x - length(viewPosition));

    float t = 0.0;
    if (FocusDistances.z > kZeroTolerance)
    {
        if (f >= FocusDistances.z) { t = 1.0; }
        else if (f <= FocusDistances.y) { t = 0.0; }
        else
        {
            t = (f - FocusDistances.y) / (FocusDistances.z - FocusDistances.y);
        } 
    }

    return t;
}

float3 GetDofBlurPass1(float4 huv)
{
	float3 ret = kZero3;
	int max = (kKernelRadius - 1);
	
    float sum = 0.0;
	for (int i = -max; i <= -1; i++)
	{
        float4 c = OffsetHuvH(huv, i);
        float weight = (GetDofBlurinessFactor(c) * GaussianWeights[-i]);

        sum += weight;
        ret += GetHdr(c) * weight;
	}
	
    {
        float weight = (GetDofBlurinessFactor(huv) * GaussianWeights[0]);

        sum += weight;
        ret += GetHdr(huv) * weight;
    }
	
	for (int i = 1; i <= max; i++)
	{
        float4 c = OffsetHuvH(huv, i);
        float weight = (GetDofBlurinessFactor(c) * GaussianWeights[i]);

        sum += weight;
        ret += GetHdr(c) * weight;
    }

    ret /= sum;
	
	return ret;
}

float3 GetDofBlurPass2(float4 huv)
{
	float3 ret = kZero3;
	int max = (kKernelRadius - 1);
	
    float sum = 0.0;
	for (int i = -max; i <= -1; i++)
	{
        float4 c = OffsetHuvV(huv, i);
        float weight = (GetDofBlurinessFactor(c) * GaussianWeights[-i]);

        sum += weight;
        ret += GetHdr(c) * weight;
	}
	
    {
        float weight = (GetDofBlurinessFactor(huv) * GaussianWeights[0]);

        sum += weight;
        ret += GetHdr(huv) * weight;
    }
	
	for (int i = 1; i <= max; i++)
	{
        float4 c = OffsetHuvV(huv, i);
        float weight = (GetDofBlurinessFactor(c) * GaussianWeights[i]);

        sum += weight;
        ret += GetHdr(c) * weight;
    }

    ret /= sum;
	
	return ret;
}

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
float AoContrib(float4 coords, float3 aViewPosition, float4 aPixelPlane)
{
	float3 vp = GetViewPosition(coords, GetDeferredDepth(coords));
	float  dc = DotCoordinate(aPixelPlane, vp);
	
    float f = (exp(kAoBiasFactor * min(kNormalDistanceMin - dc, 0.0)));

	return f;
}

#define JZ_ENABLE_AO_SCALE 1
float ApplyAoScale(float f)
{
#	if JZ_ENABLE_AO_SCALE
        static const float kCompressionFactor = 0.1;
        if (AoScale < -kCompressionFactor - kZeroTolerance)
        {
            float absAoScale = abs(AoScale) - kCompressionFactor;
            if (f <= absAoScale) { f = (f / absAoScale) * 0.01; }
            else { f = (f - absAoScale) / (1.0 - absAoScale); }

            return f;
        }
        else
        {
		    float ret = saturate((f + AoScale) / (1.0 + AoScale));
	
		    return ret;
        }
#	else
		return f;
#	endif
}

float4 FragmentAO(vsOut aIn) : COLOR
{
    static const float kAngleFactor = (kTwoPi / kTapsRt);
    static const float kScaleFactor = (1.0 / kTapsRt);

	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float3 vPos = GetViewPosition(aIn.ScreenHTexCoords, nd.w);
	float4 plane = GetPlane(nd.xyz, vPos);
	
	float2 scale = AoRadius * (-CameraFocalLength / vPos.z);
	
	float accum = 0.0;
    [unroll(kTapsRt)]
    for (int i = 0; i < kTapsRt; i++)
    {
        [unroll(kTapsRt)]
        for (int j = 0; j < kTapsRt; j++)
        {
            float scl = (i * kScaleFactor);
            float ang = (j * kAngleFactor);

            float2 v = float2(cos(ang) * scl, sin(ang) * scl);
		    float2 t = (v * scale);
		    float4 c = OffsetHuv(aIn.ScreenHTexCoords, t);
		
		    accum += AoContrib(c, vPos, plane);
        }
    }

    float3 diffuse = GetDeferredDiffuse(aIn.ScreenHTexCoords).rgb;
	float f = ApplyAoScale(accum / kCircularTaps);
	
    return float4(diffuse, f);
}

litFbOut FragmentLdrPass(vsOut aIn)
{
    litFbOut ret;
    if (bDebugAO)
    {
        float ao = GetDeferredDiffuse(aIn.ScreenHTexCoords).a;
        ret = Out(float4(ao, ao, ao, 1.0));
    }
    else
    {
		float4 litColor = GetDeferredLit(aIn.ScreenHTexCoords);
		
		ret = Out(LinearToNonLinear(litColor));
    }
	
    return ret;
}

float4 FragmentShadowBlurPass1(vsOut aIn) : COLOR0
{
    float4 ret = float4(GetShadowBlurPass2(aIn.ScreenHTexCoords), 0, 0, 0);

    return ret;
}

float4 FragmentShadowBlurPass2(vsOut aIn) : COLOR0
{
    float4 ret = float4(GetShadowBlurPass2(aIn.ScreenHTexCoords), 0, 0, 0);

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
	float3 f = GetHdrBlurPass1(aIn.ScreenHTexCoords);

	return float4(f, 0.0);
}

float4 FragmentBloomBlurPass2(vsOut aIn) : COLOR0
{
    float4 b = GetDeferredLit(aIn.ScreenHTexCoords);
    float3 f = GetHdrBlurPass2(aIn.ScreenHTexCoords);

    float4 ret = float4(b.rgb + f, b.a);

    return ret;
}

float4 FragmentDofBlurPass1(vsOut aIn) : COLOR0
{
#   if JZ_DISABLE_DOF_HALO_COMPENSATION
        float3 bc = GetHdrBlurPass1(aIn.ScreenHTexCoords);
#   else
        float3 bc = GetDofBlurPass1(aIn.ScreenHTexCoords);
#   endif

    return float4(bc, 0.0);
}

float4 FragmentDofBlurPass2(vsOut aIn) : COLOR0
{
#   if JZ_DISABLE_DOF_HALO_COMPENSATION
        float3 bc = GetHdrBlurPass2(aIn.ScreenHTexCoords);
#   else
        float3 bc = GetDofBlurPass2(aIn.ScreenHTexCoords);
#   endif

    float4 c = GetDeferredLit(aIn.ScreenHTexCoords);
    float t = GetDofBlurinessFactor(aIn.ScreenHTexCoords);

    float3 ret = lerp(c.rgb, bc, t);

    return float4(ret, c.a);
}

float4 FragmentDirectional(vsOut aIn) : COLOR
{
	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float4 diffuse = GetDeferredDiffuse(aIn.ScreenHTexCoords);

	float3 nv = (nd.xyz);
	float ndotl = dot(nv, -LightV);

	float3 lightColor = max(ndotl, 0.0) * LightColor;
	float3 color = (diffuse.rgb * lightColor);
		
#	if !NDEBUG
		if (bDebugDeferred) { color = kDebugDirectionalColor; }
#	endif

    return float4(color * diffuse.a, 1);
}

float4 FragmentPoint(vsOut aIn) : COLOR
{
    float3 lv;
    float distance;
    float3 vPos;
    
	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos, true);
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

	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos, true);
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

	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos, true);
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

    // Renormalize, GaussianWeights are normalized for symmetrical kernel
    float sum = 0.0;
    for (int i = 0; i < kMotionBlurTaps; i++)
    {
        sum += GaussianWeights[i];
        color += GetDeferredLit(uv).rgb * GaussianWeights[i];
        uv += offset;
    }
    color /= sum;

    float4 ret = float4(color, 1.0);

    return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
// techniques
//////////////////////////////////////////////////////////////////////////////////////////
technique Ao
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
		PixelShader = compile ps_2_a FragmentAO();
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

		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentShadowBlurPass1();
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

		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentShadowBlurPass2();
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

technique DofBlurPass1
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
		PixelShader = compile ps_2_a FragmentDofBlurPass1();
    }
}

technique DofBlurPass2
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
		PixelShader = compile ps_2_a FragmentDofBlurPass2();
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

		VertexShader = compile vs_2_0 VertexQuad();
		PixelShader = compile ps_2_0 FragmentMotionBlur();        
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
