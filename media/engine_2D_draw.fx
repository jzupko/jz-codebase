///////////////////////////////////////////////////////////////////////////////
// static constants
///////////////////////////////////////////////////////////////////////////////
#define ALPHA_IS_OPAQUE 127

static const float3 kOne3 = float3(1, 1, 1);
static const int kInstanceDataCount = 80;
static const int kInstanceDataSize = (80 * 3);

static const float kSubDepthFactor = (254.0 / (255.0 * 255.0));
static const float kGamma = 2.2;

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
struct vsShadowOut { float4 Position : POSITION; };

///////////////////////////////////////////////////////////////////////////////
// runtime constants
///////////////////////////////////////////////////////////////////////////////
float4 InstanceData[kInstanceDataSize] : jz_InstanceData;
float LightAttenuation : jz_LightAttenuation = 1.0;
float3 LightColor : jz_LightColor = float3(0, 0, 0);
float3 LightV : jz_LightV = float3(0, 0, -1);

texture DiffuseTexture : jz_Diffuse;
texture NormalDepthTexture : jz_NormalDepth;

sampler DiffuseSampler = sampler_state { texture = <DiffuseTexture>; AddressU = clamp; AddressV = clamp; MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = LINEAR; SRGBTexture = TRUE; };
sampler HdrSampler = sampler_state { texture = <DiffuseTexture>; AddressU = clamp; AddressV = clamp; MinFilter = POINT; MagFilter = POINT; MipFilter = NONE; SRGBTexture = FALSE; };
sampler NormalDepthSampler = sampler_state { texture = <NormalDepthTexture>; AddressU = clamp; AddressV = clamp; MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = LINEAR; SRGBTexture = FALSE; };

///////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////
float3 ToNonLinear(float3 v)
{
	return (pow(abs(v), (1.0 / kGamma)));
}

vsOut Process(vsIn aIn)
{
	int i = (aIn.TexCoordsAndIndex.z);
	
	float4 scaleShift = InstanceData[i + 0];
	float4 textureScaleShift = InstanceData[i + 1];
	float4 depthAlphaXYFactor = InstanceData[i + 2];
	
	vsOut ret;
	ret.Position.xy = (aIn.Position.xy * scaleShift.xy) + scaleShift.zw;
	ret.Position.z = depthAlphaXYFactor.x + dot(depthAlphaXYFactor.zw, ret.Position.xy);
	ret.Position.w = 1.0;
	
	ret.Pos = ret.Position;
	ret.TexCoords = ((aIn.TexCoordsAndIndex.xy * textureScaleShift.xy) + textureScaleShift.zw);
	ret.Alpha = depthAlphaXYFactor.y;
	
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

vsShadowOut VertexShadow(vsIn aIn)
{
	vsShadowOut ret;
	ret.Position = aIn.Position;
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// fragment shaders
///////////////////////////////////////////////////////////////////////////////
float4 FragmentBase(vsOut aIn) : COLOR0
{
	float4 df = tex2D(DiffuseSampler, aIn.TexCoords);
	
	// Temp:
	return (float4(0, 0, 0, (df.a * aIn.Alpha)));
}
	
float4 FragmentDirectional(vsOut aIn) : COLOR0
{
	float4 df = tex2D(DiffuseSampler, aIn.TexCoords);
	float alpha = (df.a * aIn.Alpha);
	// premultiplied alpha.
	df.rgb *= alpha;
	float3 n = (tex2D(NormalDepthSampler, aIn.TexCoords).xyz) * 2.0 - 1.0;

	float3 lv = float3(-LightV.xy, LightV.z);
	float ndotl = dot(n, lv);
	float3 lt = max(ndotl, 0.0) * LightColor;
	float3 c = (df.rgb * lt);

	return float4(c, alpha);
}

float4 FragmentPoint(vsOut aIn) : COLOR0
{
	float4 df = tex2D(DiffuseSampler, aIn.TexCoords);
	float alpha = (df.a * aIn.Alpha);
	// premultiplied alpha.
	df.rgb *= alpha;
	float4 nd = tex2D(NormalDepthSampler, aIn.TexCoords);
	float3 pos = float3(aIn.Pos.xy, aIn.Pos.z + (nd.w * kSubDepthFactor));

	float3 n = (nd.xyz) * 2.0 - 1.0;
	float3 lv = float3(LightV.xy - pos.xy, pos.z - LightV.z);
	float d2 = dot(lv, lv);
	lv = normalize(lv);
	
	float att = 1.0 / (d2);
	float ndotl = dot(lv, n);
	
	float3 lt = max(ndotl, 0.0) * LightColor;
	float3 c = (att * (df.rgb * lt));

	return float4(c, alpha);
}

float4 FragmentShadow(vsShadowOut aIn) : COLOR0
{
	return float4(0, 0, 0, 1);
}

float4 FragmentHdrToLdr(vsOut aIn) : COLOR0
{
	float4 c = tex2D(HdrSampler, aIn.TexCoords);
	float4 ret = float4(ToNonLinear(c.rgb), c.a);
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// techniques
///////////////////////////////////////////////////////////////////////////////
technique BaseOpaque
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
		PixelShader = compile ps_2_0 FragmentBase();
    }
}

technique BaseTransparent
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
		PixelShader = compile ps_2_0 FragmentBase();
    }
}

technique HdrToLdr
{
	pass
	{
	    AlphaBlendEnable = false;
	    AlphaTestEnable = false;
		ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
		ColorWriteEnable1 = 0;
		ColorWriteEnable2 = 0;
		ColorWriteEnable3 = 0;
		CullMode = None;
		FillMode = Solid;
		StencilEnable = false;
		TwoSidedStencilMode = false;
		ZEnable = false;
		ZFunc = LessEqual;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_0 Vertex();
		PixelShader = compile ps_2_0 FragmentHdrToLdr();	
	}
}

technique OpaqueDirectionalLight
{
    pass
    {
	    AlphaBlendEnable = true;
	    AlphaTestEnable = true;
	    AlphaFunc = GreaterEqual;
	    AlphaRef = ALPHA_IS_OPAQUE;
		ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
		ColorWriteEnable1 = 0;
		ColorWriteEnable2 = 0;
		ColorWriteEnable3 = 0;
		CullMode = Ccw;
		DestBlend = One;
		FillMode = Solid;
		StencilEnable = true;
		StencilRef = 0;
		StencilFunc = Equal;
		TwoSidedStencilMode = false;
		SrcBlend = One;
        StencilMask = 0xFF;
        StencilWriteMask = 0xFF;
		ZEnable = true;
		ZFunc = LessEqual;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_0 Vertex();
		PixelShader = compile ps_2_0 FragmentDirectional();
    }
}

technique OpaquePointLight
{
    pass
    {
	    AlphaBlendEnable = true;
	    AlphaTestEnable = true;
	    AlphaFunc = GreaterEqual;
	    AlphaRef = ALPHA_IS_OPAQUE;
		ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
		ColorWriteEnable1 = 0;
		ColorWriteEnable2 = 0;
		ColorWriteEnable3 = 0;
		CullMode = Ccw;
		DestBlend = One;
		FillMode = Solid;
		SrcBlend = One;
		StencilEnable = true;
		StencilRef = 0;
		StencilFunc = Equal;
		TwoSidedStencilMode = false;
        StencilMask = 0xFF;
        StencilWriteMask = 0xFF;
		ZEnable = true;
		ZFunc = LessEqual;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_a Vertex();
		PixelShader = compile ps_2_a FragmentPoint();
    }
}

technique ShadowVolume
{
    pass
    {
	    AlphaBlendEnable = false;
	    AlphaTestEnable = false;
		ColorWriteEnable  = 0;
		ColorWriteEnable1 = 0;
		ColorWriteEnable2 = 0;
		ColorWriteEnable3 = 0;
		CullMode = None;
		FillMode = Solid;
		StencilEnable = true;
        // StencilRef = 0;
        // StencilFail = Keep;
        StencilFunc = Always;
		StencilPass = Incr;
		StencilZFail = Keep;
		TwoSidedStencilMode = true;
        // Ccw_StencilFail = Keep;
        Ccw_StencilFunc = Always;
		Ccw_StencilPass = Decr;
		Ccw_StencilZFail = Keep;
        StencilMask = 0xFF;
        StencilWriteMask = 0xFF;
		ZEnable = true;
		ZFunc = LessEqual;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_0 VertexShadow();
		PixelShader = compile ps_2_0 FragmentShadow();
    }
}

technique TransparentDirectionalLight
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
		DestBlend = One;
		FillMode = Solid;
		SrcBlend = One;
		StencilEnable = true;
		StencilRef = 0;
		StencilFunc = Equal;
		TwoSidedStencilMode = false;
        StencilMask = 0xFF;
        StencilWriteMask = 0xFF;
		ZEnable = true;
		ZFunc = LessEqual;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_0 Vertex();
		PixelShader = compile ps_2_0 FragmentDirectional();
    }
}

technique TransparentPointLight
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
		DestBlend = One;
		FillMode = Solid;
		SrcBlend = One;
		StencilEnable = true;
		StencilRef = 0;
		StencilFunc = Equal;
		TwoSidedStencilMode = false;
        StencilMask = 0xFF;
        StencilWriteMask = 0xFF;
		ZEnable = true;
		ZFunc = LessEqual;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_a Vertex();
		PixelShader = compile ps_2_a FragmentPoint();
    }
}
