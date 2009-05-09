#ifndef _MIRROR_FX_
#define _MIRROR_FX_

#include "../media/common.fxh"
#include "../media/fb_common.fxh"
#include "../media/screen_space_common.fxh"

texture ReflectionTexture : jz_ReflectionTexture;
sampler ReflectionSampler = sampler_state { texture = <ReflectionTexture>; AddressU = clamp; AddressV = clamp; MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = NONE; };
float4 ReflectionScaleShift : jz_ReflectionScaleShift = float4(0, 0, 0, 0);

///////////////////////////////////////////////////////////////////////////////
// in/out
///////////////////////////////////////////////////////////////////////////////
struct vsIn { float4 Position : POSITION; };
struct vsOut
{
	float4 Position : POSITION;
    float3 ViewPosition : TEXCOORD0;
    float4 ScreenHTexCoords : TEXCOORD1;
};

///////////////////////////////////////////////////////////////////////////////
// vertex shaders
///////////////////////////////////////////////////////////////////////////////
vsOut Vertex(vsIn aIn)
{
	vsOut ret;
	
	float4 world = mul(aIn.Position, GetWorld());
	float4 view = mul(world, GetView());
	float4 proj = mul(view, GetProjection());
	
	ret.Position = proj;
    ret.ViewPosition = view.xyz;
    ret.ScreenHTexCoords = GetScreenHTexCoords(ret.Position);
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// fragment shaders
///////////////////////////////////////////////////////////////////////////////
fullFbOut Fragment(vsOut aIn) : COLOR0
{
    float2 coords = ((aIn.ScreenHTexCoords.xy / aIn.ScreenHTexCoords.w) * ReflectionScaleShift.xy) + ReflectionScaleShift.zw;
    float3 color = GammaTex2D(ReflectionSampler, coords).rgb;

    fullFbOut ret;
    ret = Out(float4(color, 1), kZero3, aIn.ViewPosition.z, kZero4, kZero3, 0.0f);

    return ret;
}

technique jz_Render
{
	pass
	{
		JZ_FULL_OUTPUT	
		DISABLE_STENCIL

		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		CullMode = BACK_FACE_CULLING;
		FillMode = Solid;
		ZEnable = true;
		ZWriteEnable = true;
	
		VertexShader = compile vs_2_0 Vertex();
		PixelShader = compile ps_2_0 Fragment();
	}
}

#endif
