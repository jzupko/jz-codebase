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

#ifndef _AO_FX_
#define _AO_FX_

#define JZ_DEBUG_AO 0
#define JZ_DISABLE_BLUR 0
#define JZ_DISABLE_DIRECTIONAL_AO 0

#include "../media/deferred_lighting.fxh"
#include "../media/fb_common.fxh"
#include "../media/screen_space_common.fxh"

//////////////////////////////////////////////////////////////////////////////////////////
// constants
//////////////////////////////////////////////////////////////////////////////////////////
static const int kKernelRadius = 5;
static const int kCircularTaps = 16;
static const float kNormalDistanceMin = 1e-2;

//////////////////////////////////////////////////////////////////////////////////////////
// runtime constants
//////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////
// fragment
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

technique AOPass1
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

technique AOPass2
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
		PixelShader = compile ps_2_0 FragmentAOPass2();
    }
}

technique AOPass3
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
		PixelShader = compile ps_2_0 FragmentAOPass3();
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

#endif
