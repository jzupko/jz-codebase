#ifndef _RADIOSITY_FX_
#define _RADIOSITY_FX_

#include "../media/common.fxh"
#include "../media/fb_common.fxh"

//////////////////////////////////////////////////////////////////////////////
// in/out
///////////////////////////////////////////////////////////////////////////////
struct vsIn { float4 Position : POSITION; float3 Color : COLOR0; };
struct vsOut
{
	float4 Position : POSITION;
	float3 Color : TEXCOORD0;
};

///////////////////////////////////////////////////////////////////////////////
// vertex shaders
///////////////////////////////////////////////////////////////////////////////
vsOut Vertex(vsIn aIn)
{
	vsOut ret;
	
	float4 world = mul(aIn.Position, kIdentity);
	float4 view = mul(world, GetView());
	float4 proj = mul(view, GetProjection());
	
	ret.Position = proj;
	ret.Color = aIn.Color;
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// fragment shaders
///////////////////////////////////////////////////////////////////////////////
float4 Fragment(vsOut aIn) : COLOR0
{
	return float4(aIn.Color, 1.0);
}

technique jz_Render
{
	pass
	{
		JZ_LIT_OUTPUT	
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
