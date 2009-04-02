///////////////////////////////////////////////////////////////////////////////
// static constants
///////////////////////////////////////////////////////////////////////////////
static const int kMaxPaletteSize = 256;

// filtering
static const float2 kDimensions = float2(640, 480);
static const float kZeroTolerance = 1e-6f;

static const float3 kTolerances = float3((1.0 / 6.0), 0.07, 0.1);
	  
static const int kTaps = 9;
static const int kCenter = floor(kTaps / 2);
static const float2 kOff = (1.0 / kDimensions);
static const float2 kOffsets[kTaps] =
	{
		float2(-kOff.x, -kOff.y), float2(0, -kOff.y), float2(kOff.x, -kOff.y),
		float2(-kOff.x,       0), float2(0,       0), float2(kOff.x,       0),
		float2(-kOff.x,  kOff.y), float2(0,  kOff.y), float2(kOff.x,  kOff.y)
	};

///////////////////////////////////////////////////////////////////////////////
// runtime constants
///////////////////////////////////////////////////////////////////////////////
texture FbTexture : jz_FbTexture;
texture HSLPaletteTexture : jz_Palette;

sampler FbSampler = sampler_state { texture = <FbTexture>; AddressU = clamp; AddressV = clamp; MinFilter = NONE; MagFilter = NONE; MipFilter = NONE; };
sampler HSLPaletteSampler = sampler_state { texture = <HSLPaletteTexture>; AddressU = clamp; AddressV = clamp; MinFilter = NONE; MagFilter = NONE; MipFilter = NONE; };

///////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////
bool about_zero(float v)
{
	return (abs(v) < kZeroTolerance);
}

bool about_equal(float a, float b)
{
	return (abs(a - b) < kZeroTolerance);
}

float hue_distance(float ha, float hb)
{
	if (ha > hb) { if ((ha - hb) > 0.5) { ha -= 1.0; } }
	else         { if ((hb - ha) > 0.5) { hb -= 1.0; } }
	
	return abs(ha - hb);
}

float hsl_to_rgb_helper(float v1, float v2, float h)
{
	static const float k1 = (2.0 / 3.0);
	
	if (h < 0.0) { h += 1.0; }
	if (h > 1.0) { h -= 1.0; }
	
	float ret;
	
	     if ((6.0 * h) < 1.0) { ret = (v1 + ((v2 - v1) * 6.0 * h)); }
	else if ((2.0 * h) < 1.0) { ret = v2; }
	else if ((3.0 * h) < 2.0) { ret = (v1 + ((v2 - v1) * 6.0 * (k1 - h))); }
	else                      { ret = v1; }
	
	return ret;
}

float3 hsl_to_rgb(float3 c)
{
	static const float k1 = (1.0 / 3.0);
	
	float h = c.x;
	float s = c.y;
	float l = c.z;
	
	float3 ret;
	
	if (about_zero(s))
	{
		ret.rgb = float3(l, l, l);
	}
	else
	{
		float v2;
		if (l < 0.5) { v2 = (l * (1.0 + s)); }
		else         { v2 = (l + s) - (l * s); }
		
		float v1 = (2.0 * l) - v2;
		
		ret.r = hsl_to_rgb_helper(v1, v2, h + k1);
		ret.g = hsl_to_rgb_helper(v1, v2, h);
		ret.b = hsl_to_rgb_helper(v1, v2, h - k1);
	}
	
	return ret;
}

float3 rgb_to_hsl(float3 c)
{
	static const float k1 = (1.0 / 6.0);
	static const float k2 = (1.0 / 3.0);
	static const float k3 = (2.0 / 3.0);

	float vmin = min(min(c.r, c.g), c.b);
	float vmax = max(max(c.r, c.g), c.b);
	float diff = (vmax - vmin);
	float sum  = (vmax + vmin);

	float h;
	float s;
	float l = (sum * 0.5f);

	if (about_zero(diff))
	{
		h = 0.0;
		s = 0.0;
	}
	else
	{
		if (l < 0.5) { s = (diff / sum); }
		else         { s = (diff / (2.0 - sum)); }

			 if (vmax == c.r) { h = ((c.g - c.b) / diff); }
		else if (vmax == c.g) { h = (2.0 + ((c.b - c.r) / diff)); }
		else                  { h = (4.0 + ((c.r - c.g) / diff)); }
		
		h *= k1;
	}
	
	return float3(h, s, l);
}

///////////////////////////////////////////////////////////////////////////////
// fragment
///////////////////////////////////////////////////////////////////////////////
bool comp_hsl(float3 a, float3 b)
{
	float ha = a.x;
	float sa = a.y;
	float la = a.z;
	
	float hb = b.x;
	float sb = b.y;
	float lb = b.z;
	
	bool bReturn = (((sa < kTolerances.y) || (sb < kTolerances.y)) || (((hue_distance(ha, hb) < kTolerances.x) && abs(sa - sb) < kTolerances.y)))
		&& abs(la - lb) < kTolerances.z;
		
	return bReturn;
}

float4 Fragment(float2 aTex : TEXCOORD0, uniform bool abUsePalette, uniform bool abFilter) : COLOR0
{
	float3 ret;
	
	if (abFilter)
	{
		float3 hsl[kTaps];
		for (int i = 0; i < kTaps; i++)
		{
			float4 c = tex2D(FbSampler, aTex + kOffsets[i]);
			
			if (abUsePalette) { hsl[i] = tex1D(HSLPaletteSampler, c.a).xyz; }
			else              { hsl[i] = rgb_to_hsl(c.rgb); }
		}
		
		float2 accum = float2(0, 0);
		float denom = 0.0;
		
		for (int i = 0; i < kTaps; i++)
		{
			if (comp_hsl(hsl[i], hsl[kCenter]))
			{
				accum += hsl[i].yz;
				denom += 1.0;
			}
		}
		accum /= denom;
		
		ret = hsl_to_rgb(float3(hsl[kCenter].x, accum));
	}
	else
	{
		float4 c = tex2D(FbSampler, aTex);
		
		if (abUsePalette) { ret = hsl_to_rgb(tex1D(HSLPaletteSampler, c.a).xyz); }
		else              { ret = c.rgb; }
	}
		
	return float4(ret, 1.0);
}

///////////////////////////////////////////////////////////////////////////////
// techniques
///////////////////////////////////////////////////////////////////////////////
technique jz_PalettedTechnique
{
	pass { PixelShader = compile ps_3_0 Fragment(true, true); }
}

technique jz_UnpalettedTechnique
{
	pass { PixelShader = compile ps_3_0 Fragment(false, true); }
}
