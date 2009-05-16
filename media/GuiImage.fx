///////////////////////////////////////////////////////////////////////////////
// static constants
///////////////////////////////////////////////////////////////////////////////
#define ALPHA_IS_OPAQUE 127

static const float3 kOne3 = float3(1, 1, 1);
static const int kInstanceDataCount = 80;
static const int kInstanceDataSize = (80 * 3);

///////////////////////////////////////////////////////////////////////////////
// in/out
///////////////////////////////////////////////////////////////////////////////
struct vsIn { float4 Position : POSITION; float3 TexCoordsAndIndex : TEXCOORD0; };
struct vsOut
{
	float4 Position : POSITION;
	float3 Pos : TEXCOORD0;
	float2 TexCoords : TEXCOORD1;
	float Alpha : TEXCOORD2;
};

///////////////////////////////////////////////////////////////////////////////
// runtime constants
///////////////////////////////////////////////////////////////////////////////
float4 InstanceData[kInstanceDataSize] : jz_InstanceData;
bool bFiltered : jz_Filtered = true;

texture ImageTexture : jz_ImageTexture;
sampler ImageSamplerUnfiltered = sampler_state { texture = <ImageTexture>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; };
sampler ImageSampler = sampler_state { texture = <ImageTexture>; AddressU = clamp; AddressV = clamp; MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = LINEAR; };

///////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////
vsOut Process(vsIn aIn)
{
	int i = (aIn.TexCoordsAndIndex.z);
	
	float4 scaleShift = InstanceData[i + 0];
	float4 textureScaleShift = InstanceData[i + 1];
	float4 depthAlpha = InstanceData[i + 2];
	
	vsOut ret;
	ret.Position.xy = (aIn.Position.xy * scaleShift.xy) + scaleShift.zw;
	ret.Position.z = depthAlpha.x;
	ret.Position.w = 1.0;
	
	ret.Pos = ret.Position;
	ret.TexCoords = ((aIn.TexCoordsAndIndex.xy * textureScaleShift.xy) + textureScaleShift.zw);
	ret.Alpha = depthAlpha.y;
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// vertex shaders
///////////////////////////////////////////////////////////////////////////////
vsOut Vertex(vsIn aIn)
{
	vsOut ret = Process(aIn);
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// fragment shaders
///////////////////////////////////////////////////////////////////////////////
float4 Fragment(vsOut aIn) : COLOR0
{
	float4 c;
    
    if (bFiltered)
    {
        c = tex2D(ImageSampler, aIn.TexCoords);
    }
    else
    {
        c = tex2D(ImageSamplerUnfiltered, aIn.TexCoords);
    }

    float4 ret = float4(c.rgb, (c.a * aIn.Alpha));

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
// techniques
///////////////////////////////////////////////////////////////////////////////
technique Opaque
{
    pass
    {
	    AlphaBlendEnable = false;
	    AlphaTestEnable = true;
	    AlphaFunc = GreaterEqual;
	    AlphaRef = ALPHA_IS_OPAQUE;
		ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
		ColorWriteEnable1 = 0;
		ColorWriteEnable2 = 0;
		ColorWriteEnable3 = 0;
		CullMode = Ccw;
		FillMode = Solid;
		StencilEnable = false;
		TwoSidedStencilMode = false;
		ZEnable = true;
		ZFunc = LessEqual;
		ZWriteEnable = true;
		
		VertexShader = compile vs_2_0 Vertex();
		PixelShader = compile ps_2_0 Fragment();
    }
}

technique Transparent
{
    pass
    {
	    AlphaBlendEnable = true;
	    AlphaTestEnable = false;
		ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
		ColorWriteEnable1 = 0;
		ColorWriteEnable2 = 0;
		ColorWriteEnable3 = 0;
		CullMode = Ccw;
		DestBlend = InvSrcAlpha;
		FillMode = Solid;
		SrcBlend = SrcAlpha;
		StencilEnable = false;
		TwoSidedStencilMode = false;
		ZEnable = true;
		ZFunc = LessEqual;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_0 Vertex();
		PixelShader = compile ps_2_0 Fragment();
    }
}

