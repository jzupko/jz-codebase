#ifndef _FB_COMMON_FXH_
#define _FB_COMMON_FXH_

#define JZ_FULL_OUTPUT \
	ColorWriteEnable  = RED|GREEN|BLUE|ALPHA; \
	ColorWriteEnable1 = RED|GREEN|BLUE|ALPHA; \
	ColorWriteEnable2 = RED|GREEN|BLUE|ALPHA; \
	ColorWriteEnable3 = RED|GREEN|BLUE|ALPHA;
	
#define JZ_LIT_OUTPUT \
	ColorWriteEnable  = RED|GREEN|BLUE|ALPHA; \
	ColorWriteEnable1 = 0; \
	ColorWriteEnable2 = 0; \
	ColorWriteEnable3 = 0;
	
#define JZ_SHADOW_OUTPUT \
	ColorWriteEnable = RED; \
	ColorWriteEnable1 = 0; \
	ColorWriteEnable2 = 0; \
	ColorWriteEnable3 = 0;
	
struct litFbOut
{
	float4 Mrt0 : COLOR0;
};
	
struct fullFbOut
{
	float4 Mrt0 : COLOR0;
	float4 Mrt1 : COLOR1;
	float4 Mrt2 : COLOR2;
    float4 Mrt3 : COLOR3;
};

fullFbOut Out(
	float4 aLitColor,
	float3 aViewNormal,
	float  aViewDepth,
	float4 aDiffuse,
    float3 aSpecular,
    float aShininess)
{
	fullFbOut ret;
	ret.Mrt0 = aLitColor;
	ret.Mrt1.xyz = aViewNormal;
	ret.Mrt1.w = aViewDepth;
	ret.Mrt2 = aDiffuse;
    ret.Mrt3.xyz = aSpecular;
    ret.Mrt3.w = aShininess;
	
	return ret;
}

litFbOut Out(float4 aLitColor)
{
	litFbOut ret;
	ret.Mrt0 = aLitColor;
	
	return ret;
}

texture MrtTexture0 : jz_MrtTexture0;
texture MrtTexture1 : jz_MrtTexture1;
texture MrtTexture2 : jz_MrtTexture2;
texture MrtTexture3 : jz_MrtTexture3;

sampler MrtSampler0 = sampler_state { texture = <MrtTexture0>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; };
sampler MrtSampler1 = sampler_state { texture = <MrtTexture1>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; };
sampler MrtSampler2 = sampler_state { texture = <MrtTexture2>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; };
sampler MrtSampler3 = sampler_state { texture = <MrtTexture3>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; };

float4 GetDeferredLit(float4 huv)
{
	return tex2Dproj(MrtSampler0, huv);
}

float3 GetDeferredNormal(float4 huv)
{
	return tex2Dproj(MrtSampler1, huv).xyz;
}

float GetDeferredDepth(float4 huv)
{
	return tex2Dproj(MrtSampler1, huv).w;
}

float4 GetDeferredNormalAndDepth(float4 huv)
{
	return tex2Dproj(MrtSampler1, huv);
}

float4 GetDeferredDiffuse(float4 huv)
{
	return tex2Dproj(MrtSampler2, huv);
}

float4 GetDeferredSpecularAndShininess(float4 huv)
{
    return tex2Dproj(MrtSampler3, huv);
}

#endif
