///////////////////////////////////////////////////////////////////////////////
// fire effect - based on "Perlin Fire" from "NVIDIA Direct3D SDK 10"
//     http://developer.download/nvidia/com/SDK/10.5/direct3D/samples.html
///////////////////////////////////////////////////////////////////////////////

#include "../media/improved_noise.fxh"
#include "../media/fb_common.fxh"

///////////////////////////////////////////////////////////////////////////////
// input/output
///////////////////////////////////////////////////////////////////////////////
struct vsIn  { float4 Position : POSITION; float2 Tex : TEXCOORD0; };
struct vsOut { float4 Position : POSITION; float2 Pos : TEXCOORD0; };

///////////////////////////////////////////////////////////////////////////////
// static constants
///////////////////////////////////////////////////////////////////////////////
static const float kFireTexWidth = 64;

///////////////////////////////////////////////////////////////////////////////
// runtime constants
///////////////////////////////////////////////////////////////////////////////
texture FireTexture : jz_FireTexture;
sampler FireSampler = sampler_state { texture = <FireTexture>; AddressU = Border; AddressV = Border; BorderColor = { 0, 0, 0, 0 }; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

float FireRoughness : jz_FireRoughness = 3.2f;
float FireScale : jz_FireScale = 1.35f;
float Transparency : jz_Transparency = 0.5f;

///////////////////////////////////////////////////////////////////////////////
// vertex shaders
///////////////////////////////////////////////////////////////////////////////
vsOut Vertex(vsIn aIn)
{
	vsOut ret;
	float4 world = mul(aIn.Position, GetWorld());
	float4 view  = mul(world, GetView());
	float4 proj  = mul(view, GetProjection());
	
	float2 pos = (aIn.Tex - 0.5);
	
	ret.Position = proj;
	ret.Pos = pos;
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// fragmanet shaders
///////////////////////////////////////////////////////////////////////////////
litFbOut Fragment(vsOut aIn)
{
	float2 pos = aIn.Pos;

	float3 coord = float3(0.0, (pos.y - Time), pos.x);
	float turb = abs(Turbulence(coord * FireScale, 3));

    float2 tc;
    tc.x = (abs(pos.x) * 2.0) + (1.0 / (kFireTexWidth * 2));
    tc.y = (0.5 - pos.y - FireRoughness * turb * pow((0.5 + pos.y), 0.5));

	float3 color = 6 * tex2D(FireSampler, tc).rgb;
	litFbOut ret = Out(float4(color, 1));
	
	return ret;
}

technique jz_Render
{
	pass
	{
		JZ_LIT_OUTPUT
		DISABLE_STENCIL
		
		AlphaBlendEnable = true;
		AlphaTestEnable = false;
		BlendOp = Add;
		CullMode = None;
		DestBlend = One;
		FillMode = Solid;
		SrcBlend = InvDestColor;
		ZWriteEnable = false;
	
		VertexShader = compile vs_2_a Vertex();
		PixelShader = compile ps_2_a Fragment();
	}
}
