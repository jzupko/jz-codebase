#ifndef _MATERIAL_FXH_
#define _MATERIAL_FXH_

#define JZ_ENABLE_SHADOW_SLOPE_SCALE_DEPTH_BIAS 0

#include "../media/common.fxh"
#include "../media/fb_common.fxh"

///////////////////////////////////////////////////////////////////////////////
// static constants
///////////////////////////////////////////////////////////////////////////////
static const float kSlopeScaleBiasFactor = 1.0;

///////////////////////////////////////////////////////////////////////////////
// helper macros
///////////////////////////////////////////////////////////////////////////////
#if defined(DIFFUSE_COLOR) || defined(DIFFUSE_TEXTURE) || defined(DIFFUSE_VERTEX)
#	define DIFFUSE
#endif

#if defined(AMBIENT_COLOR) || defined(AMBIENT_TEXTURE)
#	define AMBIENT
#endif

#if defined(EMISSION_COLOR) || defined(EMISSION_TEXTURE)
#	define EMISSION
#endif

#if defined(TRANSPARENT_COLOR) || defined(TRANSPARENT_TEXTURE)
#	define TRANSPARENT
#endif

#if defined(REFLECTIVE_COLOR) || defined(REFLECTIVE_TEXTURE)
#	define REFLECTIVE
#endif

#if defined(SPECULAR_COLOR) || defined(SPECULAR_TEXTURE)
#	define SPECULAR
#endif

#if defined(DIFFUSE) || defined(SPECULAR) || defined(REFLECTIVE)
#	define LIGHTABLE
#endif

#if defined(BUMP_TEXTURE)
#	define BUMP
#endif

///////////////////////////////////////////////////////////////////////////////
// runtime constants
///////////////////////////////////////////////////////////////////////////////
struct ThreePointLighting
{
	float3 KeyPosition;
	float3 KeyDiffuse;
	float3 KeySpecular;
	float3 FillDirection;
	float KeyToFillRatio;
	float3 BackPosition;
	float3 BackDiffuse;
	float3 BackSpecular;
};

ThreePointLighting ThreePoint : jz_ThreePoint =
	{ kZero3,
	  kZero3,
	  kZero3,
	  float3(0, 0, -1),
	  0.0f,
	  kZero3,
	  kZero3,
	  kZero3 };
	  
///////////////////////////////////////////////////////////////////////////////
// material parameters
///////////////////////////////////////////////////////////////////////////////
#if defined(EMISSION_COLOR)
	float4 EmissionColor : EMISSION_COLOR;
#elif defined(EMISSION_TEXTURE)
	texture EmissionTexture : EMISSION_TEXTURE;
	
	sampler EmissionSampler = sampler_state
	{
		AddressU = EMISSION_ADDRESSU;
		AddressV = EMISSION_ADDRESSV;
		AddressW = EMISSION_ADDRESSW;
		MinFilter = EMISSION_MIN_FILTER;
		MagFilter = EMISSION_MAG_FILTER;
		MipFilter = EMISSION_MIP_FILTER;
        BorderColor = EMISSION_BORDER_COLOR;
        MaxMipLevel = EMISSION_MAX_MIP_LEVEL;
        MipMapLodBias = EMISSION_MIP_MAP_LOD_BIAS;		
		Texture = (EmissionTexture);
	};
#endif

#if defined(REFLECTIVE_COLOR)
	float4 ReflectiveColor : REFLECTIVE_COLOR;
#elif defined(REFLECTIVE_TEXTURE)	
	texture ReflectiveTexture : REFLECTIVE_TEXTURE;
	
	sampler ReflectiveSampler = sampler_state
	{
		AddressU = REFLECTIVE_ADDRESSU;
		AddressV = REFLECTIVE_ADDRESSV;
		AddressW = REFLECTIVE_ADDRESSW;
		MinFilter = REFLECTIVE_MIN_FILTER;
		MagFilter = REFLECTIVE_MAG_FILTER;
		MipFilter = REFLECTIVE_MIP_FILTER;
        BorderColor = REFLECTIVE_BORDER_COLOR;
        MaxMipLevel = REFLECTIVE_MAX_MIP_LEVEL;
        MipMapLodBias = REFLECTIVE_MIP_MAP_LOD_BIAS;			
		Texture = (ReflectiveTexture);
	};	
#endif

#if defined(REFLECTIVE)
	float Reflectivity : REFLECTIVITY;
#endif

#if defined(TRANSPARENT_COLOR)
	float4 TransparentColor : TRANSPARENT_COLOR;
#elif defined(TRANSPARENT_TEXTURE)
	texture TransparentTexture : TRANSPARENT_TEXTURE;
	
	sampler TransparentSampler = sampler_state
	{
		AddressU = TRANSPARENT_ADDRESSU;
		AddressV = TRANSPARENT_ADDRESSV;
		AddressW = TRANSPARENT_ADDRESSW;
		MinFilter = TRANSPARENT_MIN_FILTER;
		MagFilter = TRANSPARENT_MAG_FILTER;
		MipFilter = TRANSPARENT_MIP_FILTER;
        BorderColor = TRANSPARENT_BORDER_COLOR;
        MaxMipLevel = TRANSPARENT_MAX_MIP_LEVEL;
        MipMapLodBias = TRANSPARENT_MIP_MAP_LOD_BIAS;		
		Texture = (TransparentTexture);
	};	
#endif

#if defined(TRANSPARENT)
	float Transparency : TRANSPARENCY;
#endif

#if defined(AMBIENT_COLOR)
	float4 AmbientColor : AMBIENT_COLOR;
#elif defined(AMBIENT_TEXTURE)
	texture AmbientTexture : AMBIENT_TEXTURE;
	
	sampler AmbientSampler = sampler_state
	{
		AddressU = AMBIENT_ADDRESSU;
		AddressV = AMBIENT_ADDRESSV;
		AddressW = AMBIENT_ADDRESSW;
		MinFilter = AMBIENT_MIN_FILTER;
		MagFilter = AMBIENT_MAG_FILTER;
		MipFilter = AMBIENT_MIP_FILTER;
        BorderColor = AMBIENT_BORDER_COLOR;
        MaxMipLevel = AMBIENT_MAX_MIP_LEVEL;
        MipMapLodBias = AMBIENT_MIP_MAP_LOD_BIAS;			
		Texture = (AmbientTexture);
	};	
#endif

#if defined(DIFFUSE_COLOR)
	float4 DiffuseColor : DIFFUSE_COLOR;
#elif defined(DIFFUSE_TEXTURE)
	texture DiffuseTexture : DIFFUSE_TEXTURE;
	
	sampler DiffuseSampler = sampler_state
	{
		AddressU = DIFFUSE_ADDRESSU;
		AddressV = DIFFUSE_ADDRESSV;
		AddressW = DIFFUSE_ADDRESSW;
		MinFilter = DIFFUSE_MIN_FILTER;
		MagFilter = DIFFUSE_MAG_FILTER;
		MipFilter = DIFFUSE_MIP_FILTER;
        BorderColor = DIFFUSE_BORDER_COLOR;
        MaxMipLevel = DIFFUSE_MAX_MIP_LEVEL;
        MipMapLodBias = DIFFUSE_MIP_MAP_LOD_BIAS;		
		Texture = (DiffuseTexture);
	};
#endif

#if defined(SPECULAR_COLOR)
	float4 SpecularColor : SPECULAR_COLOR;
#elif defined(SPECULAR_TEXTURE)
	texture SpecularTexture : SPECULAR_TEXTURE;
	
	sampler SpecularSampler = sampler_state
	{
		AddressU = SPECULAR_ADDRESSU;
		AddressV = SPECULAR_ADDRESSV;
		AddressW = SPECULAR_ADDRESSW;
		MinFilter = SPECULAR_MIN_FILTER;
		MagFilter = SPECULAR_MAG_FILTER;
		MipFilter = SPECULAR_MIP_FILTER;
        BorderColor = SPECULAR_BORDER_COLOR;
        MaxMipLevel = SPECULAR_MAX_MIP_LEVEL;
        MipMapLodBias = SPECULAR_MIP_MAP_LOD_BIAS;		
		Texture = (SpecularTexture);
	};	
#endif

#if defined(SPECULAR)
	float Shininess : SHININESS;
#endif

#if defined(INDEX_OF_REFRACTION)
	float IndexOfRefraction;
#endif

#if defined(BUMP_TEXTURE)
	texture BumpTexture : BUMP_TEXTURE;
	
	sampler BumpSampler = sampler_state
	{
		AddressU = BUMP_ADDRESSU;
		AddressV = BUMP_ADDRESSV;
		AddressW = BUMP_ADDRESSW;
		MinFilter = BUMP_MIN_FILTER;
		MagFilter = BUMP_MAG_FILTER;
		MipFilter = BUMP_MIP_FILTER;
        BorderColor = BUMP_BORDER_COLOR;
        MaxMipLevel = BUMP_MAX_MIP_LEVEL;
        MipMapLodBias = BUMP_MIP_MAP_LOD_BIAS;		
		Texture = (BumpTexture);
	};	
#endif

///////////////////////////////////////////////////////////////////////////////
// in/out
///////////////////////////////////////////////////////////////////////////////
struct vsIn
{
	float3 Normal : NORMAL;
	float4 Position : POSITION;

#	if defined(DIFFUSE_VERTEX)
		float4 DiffuseColor : COLOR0;
#	endif

#	if defined(ANIMATED)
		float4 BoneIndices : BLENDINDICES;
		float4 BoneWeights : BLENDWEIGHT;
#	endif

#	if defined(BUMP)
		float3 Tangent : TANGENT;
#	endif

#   if (TEXCOORDS_COUNT > 0)
		float2 Texcoords0 : TEXCOORD0;
#	endif
#   if (TEXCOORDS_COUNT > 1)
		float2 Texcoords1 : TEXCOORD1;
#	endif
#   if (TEXCOORDS_COUNT > 2)
		float2 Texcoords2 : TEXCOORD2;
#	endif
#   if (TEXCOORDS_COUNT > 3)
		float2 Texcoords3 : TEXCOORD3;
#	endif
#   if (TEXCOORDS_COUNT > 4)
		float2 Texcoords4 : TEXCOORD4;
#	endif
#   if (TEXCOORDS_COUNT > 5)
		float2 Texcoords5 : TEXCOORD5;
#	endif
#   if (TEXCOORDS_COUNT > 6)
		float2 Texcoords6 : TEXCOORD6;
#	endif
#   if (TEXCOORDS_COUNT > 7)
		float2 Texcoords7 : TEXCOORD7;
#	endif

};

struct vsOutPicking
{
	float4 Position : POSITION;
	float4 PositionH : TEXCOORD0;
	
#	if defined(TRANSPARENT_TEXTURE)
		float2 TransparentTexCoords : TEXCOORD1;
#	endif	
};

struct vsOutShadow
{
	float4 Position : POSITION;
	float3 ViewPosition : TEXCOORD0;
	
#	if defined(TRANSPARENT_TEXTURE)
		float2 TransparentTexCoords : TEXCOORD1;
#	endif		
};

struct vsOut
{
	float4 Position : POSITION;
	float3 ViewPosition : TEXCOORD0;
	float3 ViewNormal : TEXCOORD1;

#	if defined(BUMP)			
		float3 ViewTangent : TEXCOORD2;
#	endif	

#	if defined(DIFFUSE_TEXTURE) || defined(AMBIENT_TEXTURE)
		float4 DiffuseAmbientTexCoords : TEXCOORD3;
#	endif	
	
#	if defined(EMISSION_TEXTURE) || defined(TRANSPARENT_TEXTURE)
		float4 EmissionTransparentTexCoords : TEXCOORD4;
#	endif

#	if defined(REFLECTIVE_TEXTURE) || defined(SPECULAR_TEXTURE)
		float4 ReflectiveSpecularTexCoords : TEXCOORD5;
#	endif

#	if defined(BUMP_TEXTURE)
		float2 BumpTexCoords : TEXCOORD6;
#	endif
};

struct vsOutReflection
{
	float4 Position : POSITION;

#	if defined(DIFFUSE_TEXTURE) || defined(AMBIENT_TEXTURE)
		float4 DiffuseAmbientTexCoords : TEXCOORD0;
#	endif	
	
#	if defined(EMISSION_TEXTURE) || defined(TRANSPARENT_TEXTURE)
		float4 EmissionTransparentTexCoords : TEXCOORD1;
#	endif

#	if defined(REFLECTIVE_TEXTURE)
		float2 ReflectiveTexCoords : TEXCOORD2;
#	endif
};

///////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////
float3x3 GetWitV(vsIn aIn)
{
#	if defined(ANIMATED)
		return mul(mul(GetSkinning3x3(aIn.BoneWeights, aIn.BoneIndices), GetWit()), GetView());
#	else
		return GetWitV();
#	endif
}

float3x3 GetWit(vsIn aIn)
{
#	if defined(ANIMATED)
		return mul(GetSkinning3x3(aIn.BoneWeights, aIn.BoneIndices), GetWit());
#	else
		return GetWit();
#	endif
}

float4x4 GetWorld(vsIn aIn)
{
#	if defined(ANIMATED)
		return mul(GetSkinning4x4(aIn.BoneWeights, aIn.BoneIndices), GetWorld());
#	else
		return GetWorld();
#	endif
}

///////////////////////////////////////////////////////////////////////////////
// vertex shaders
///////////////////////////////////////////////////////////////////////////////
vsOutPicking VertexPicking(vsIn aIn)
{
	vsOutPicking ret;
	
	float4 world = mul(aIn.Position, GetWorld(aIn));
	float4 view = mul(world, GetView());
	float4 proj = mul(view, GetProjection());
	
	ret.Position = proj;
	ret.PositionH = proj;
	
#   if defined(TRANSPARENT_TEXTURE)
		ret.TransparentTexCoords = aIn.TRANSPARENT_TEXCOORDS;
#   endif	
	
	return ret;
}

vsOutShadow VertexShadow(vsIn aIn)
{
	vsOutShadow ret;
	
	float4 world = mul(aIn.Position, GetWorld(aIn));
	float4 view = mul(world, GetView());
	float4 proj = mul(view, GetProjection());
	
	ret.Position = proj;
	ret.ViewPosition = view.xyz;
	
#   if defined(TRANSPARENT_TEXTURE)
		ret.TransparentTexCoords = aIn.TRANSPARENT_TEXCOORDS;
#   endif		
	
	return ret;
}

vsOutReflection VertexRenderReflection(vsIn aIn)
{
	vsOutReflection ret;

    float4x4 reflection = GetReflection(GetReflectionPlane());

	float4 world = mul(aIn.Position, mul(GetWorld(aIn), reflection));
    float4 view = mul(world, GetView());
	float4 proj = mul(view, GetProjection());

    ret.Position = proj;

#	if defined(DIFFUSE_TEXTURE)
		ret.DiffuseAmbientTexCoords.xy = aIn.DIFFUSE_TEXCOORDS;
#	elif defined(AMBIENT_TEXTURE)
		ret.DiffuseAmbientTexCoords.xy = float2(0, 0);
#	endif
#	if defined(AMBIENT_TEXTURE)
		ret.DiffuseAmbientTexCoords.zw = aIn.AMBIENT_TEXCOORDS;
#	elif defined(DIFFUSE_TEXTURE)
		ret.DiffuseAmbientTexCoords.zw = float2(0, 0);
#	endif

#	if defined(EMISSION_TEXTURE)
		ret.EmissionTransparentTexCoords.xy = aIn.EMISSION_TEXCOORDS;
#	elif defined(TRANSPARENT_TEXTURE)
		ret.EmissionTransparentTexCoords.xy = float2(0, 0);
#	endif
#	if defined(TRANSPARENT_TEXTURE)
		ret.EmissionTransparentTexCoords.zw = aIn.TRANSPARENT_TEXCOORDS;
#	elif defined(EMISSION_TEXTURE)
		ret.EmissionTransparentTexCoords.zw = float2(0, 0);
#	endif

#	if defined(REFLECTIVE_TEXTURE)
		ret.ReflectiveTexCoords.xy = aIn.REFLECTIVE_TEXCOORDS;
#	endif

    return ret;
}

vsOut VertexRender(vsIn aIn)
{
	vsOut ret;

	float4 world = mul(aIn.Position, GetWorld(aIn));
	float4 view = mul(world, GetView());
	float4 proj = mul(view, GetProjection());
	
	ret.Position = proj;
	ret.ViewPosition = view.xyz;
	ret.ViewNormal = mul(aIn.Normal, GetWitV(aIn));
	
#	if defined(BUMP)
		ret.ViewTangent = mul(aIn.Tangent, GetWitV(aIn));
#	endif	

#	if defined(DIFFUSE_TEXTURE)
		ret.DiffuseAmbientTexCoords.xy = aIn.DIFFUSE_TEXCOORDS;
#	elif defined(AMBIENT_TEXTURE)
		ret.DiffuseAmbientTexCoords.xy = float2(0, 0);
#	endif
#	if defined(AMBIENT_TEXTURE)
		ret.DiffuseAmbientTexCoords.zw = aIn.AMBIENT_TEXCOORDS;
#	elif defined(DIFFUSE_TEXTURE)
		ret.DiffuseAmbientTexCoords.zw = float2(0, 0);
#	endif

#	if defined(EMISSION_TEXTURE)
		ret.EmissionTransparentTexCoords.xy = aIn.EMISSION_TEXCOORDS;
#	elif defined(TRANSPARENT_TEXTURE)
		ret.EmissionTransparentTexCoords.xy = float2(0, 0);
#	endif
#	if defined(TRANSPARENT_TEXTURE)
		ret.EmissionTransparentTexCoords.zw = aIn.TRANSPARENT_TEXCOORDS;
#	elif defined(EMISSION_TEXTURE)
		ret.EmissionTransparentTexCoords.zw = float2(0, 0);
#	endif

#	if defined(REFLECTIVE_TEXTURE)
		ret.ReflectiveSpecularTexCoords.xy = aIn.REFLECTIVE_TEXCOORDS;
#	elif defined(SPECULAR_TEXTURE)
		ret.ReflectiveSpecularTexCoords.xy = float2(0, 0);
#	endif
#	if defined(SPECULAR_TEXTURE)
		ret.ReflectiveSpecularTexCoords.zw = aIn.SPECULAR_TEXCOORDS;
#	elif defined(REFLECTIVE_TEXTURE)
		ret.ReflectiveSpecularTexCoords.zw = float2(0, 0);
#	endif

#	if defined(BUMP_TEXTURE)
		ret.BumpTexCoords = aIn.BUMP_TEXCOORDS;
#	endif
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////
float GetAlpha(float4 c)
{
#	if defined(TRANSPARENT)
#		if defined(ALPHA_ONE)
			return (c.a * Transparency);
#		elif defined(RGB_ZERO)
			return 1.0 - (((0.212671 * c.r) 
				       +   (0.715160 * c.g)
					   +   (0.072169 * c.b)) * Transparency);
#		else
#			error "One of ALPHA_ONE or RGB_ZERO must be defined."
#		endif
#	else
		return 1.0;
#	endif
}

float4 _GetAmbient(float2 tc)
{
#	if defined(AMBIENT_COLOR)
		return GammaColor(AmbientColor);
#	elif defined(AMBIENT_TEXTURE)
		return GammaTex2D(AmbientSampler, tc);
#	else
		return kBlack4;		
#	endif
}

float4 GetAmbient(vsOut aIn)
{
#   if defined(AMBIENT_TEXTURE)
        float2 tc = aIn.DiffuseAmbientTexCoords.zw;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetAmbient(tc));
}

float4 GetAmbient(vsOutReflection aIn)
{
#   if defined(AMBIENT_TEXTURE)
        float2 tc = aIn.DiffuseAmbientTexCoords.zw;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetAmbient(tc));
}

float4 _GetDiffuse(float2 aDiffuseTc, float2 aReflectiveTc)
{
	float4 ret;
	
#	if defined(DIFFUSE_COLOR)
		float4 diffuse = GammaColor(DiffuseColor);
#	elif defined(DIFFUSE_TEXTURE)
		float4 diffuse = GammaTex2D(DiffuseSampler, aDiffuseTc);
#	endif

#	if defined(REFLECTIVE_COLOR)
		float4 reflective = GammaColor(ReflectiveColor);
#	elif defined(REFLECTIVE_TEXTURE)
		float4 reflective = GammaTex2D(ReflectiveSampler, aReflectiveTc);
#	endif

#	if defined(REFLECTIVE)
#		if defined(DIFFUSE)
			return lerp(diffuse, reflective, Reflectivity);
#		else
			return reflective;
#		endif			
#	elif defined(DIFFUSE)
		return diffuse;
#	else
		return kBlack4;
#	endif
}

float4 GetDiffuse(vsOut aIn)
{
#   if defined(DIFFUSE_TEXTURE)
        float2 dtc = aIn.DiffuseAmbientTexCoords.xy;
#   else
        float2 dtc = float2(0, 0);
#   endif

#   if defined(REFLECTIVE_TEXTURE)
        float2 rtc = aIn.ReflectiveSpecularTexCoords.xy;
#   else
        float2 rtc = float2(0, 0);
#   endif

    return (_GetDiffuse(dtc, rtc));
}

float4 GetDiffuse(vsOutReflection aIn)
{
#   if defined(DIFFUSE_TEXTURE)
        float2 dtc = aIn.DiffuseAmbientTexCoords.xy;
#   else
        float2 dtc = float2(0, 0);
#   endif

#   if defined(REFLECTIVE_TEXTURE)
        float2 rtc = aIn.ReflectiveTexCoords.xy;
#   else
        float2 rtc = float2(0, 0);
#   endif

    return (_GetDiffuse(dtc, rtc));
}

float4 _GetEmission(float2 tc)
{
#	if defined(EMISSION_COLOR)
		return GammaColor(EmissionColor);
#	elif defined(EMISSION_TEXTURE)
		return GammaTex2D(EmissionSampler, tc);
#	else
		return kBlack4;		
#	endif
}

float4 GetEmission(vsOut aIn)
{
#   if defined(EMISSION_TEXTURE)
        float2 tc = aIn.EmissionTransparentTexCoords.xy;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetEmission(tc));
}

float4 GetEmissionForReflection(vsOutReflection aIn)
{
#   if defined(EMISSION_TEXTURE)
        float2 tc = aIn.EmissionTransparentTexCoords.xy;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetEmission(tc));
}

float4 GetSpecular(vsOut aIn)
{
#	if defined(SPECULAR_COLOR)
		return GammaColor(SpecularColor);
#	elif defined(SPECULAR_TEXTURE)
		return GammaTex2D(SpecularSampler, aIn.ReflectiveSpecularTexCoords.zw);
#	else
		return kBlack4;
#	endif
}

float4 _GetTransparent(float2 tc)
{
#	if defined(TRANSPARENT_COLOR)
		return GammaColor(TransparentColor);
#	elif defined(TRANSPARENT_TEXTURE)
		return GammaTex2D(TransparentSampler, tc);
#	else
		return kBlack4;
#	endif
}

float4 GetTransparent(vsOut aIn)
{
#   if defined (TRANSPARENT_TEXTURE)
        float2 tc = aIn.EmissionTransparentTexCoords.zw;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetTransparent(tc));
}

float4 GetTransparent(vsOutPicking aIn)
{
#   if defined (TRANSPARENT_TEXTURE)
        float2 tc = aIn.TransparentTexCoords.xy;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetTransparent(tc));
}

float4 GetTransparentForReflection(vsOutReflection aIn)
{
#   if defined (TRANSPARENT_TEXTURE)
        float2 tc = aIn.EmissionTransparentTexCoords.zw;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetTransparent(tc));
}

float4 GetTransparent(vsOutShadow aIn)
{
#   if defined (TRANSPARENT_TEXTURE)
        float2 tc = aIn.TransparentTexCoords.xy;
#   else
        float2 tc = float2(0, 0);
#   endif

    return (_GetTransparent(tc));
}

float3 GetBaseColor(float3 aDiffuse, float3 aEmission, float3 aAmbient)
{
#	if (defined(DIFFUSE) || defined(REFLECTIVE)) && defined(AMBIENT)
		float3 ret  = (aDiffuse.rgb * aAmbient.rgb);
#	else
		float3 ret = float3(0, 0, 0);
#	endif

#	if defined(EMISSION)
		ret += aEmission.rgb;
#	endif

	return ret;
}

float3 NormalInTangentToView(
	float3 aNormalInTangentSpace,
	float3 aViewNormal,
	float3 aViewTangent)
{
	float3 binormal = cross(aViewNormal, aViewTangent);
	
	float3 t = normalize(aViewTangent);
	float3 b = normalize(binormal);
	float3 n = normalize(aViewNormal);
	
	// this transforms a normal in tangent space into view space
	float3x3 tv = float3x3(
		t.x, t.y, t.z,
		b.x, b.y, b.z,
		n.x, n.y, n.z);
		
	float3 ret = mul(aNormalInTangentSpace, tv);
	
	return ret;
}

float4 GetL(float3 lv, float3 nv, float ndotl, vsOut aIn)
{
# if defined(SPECULAR)
	float3 ev = normalize(-aIn.ViewPosition);
		
#	if defined(BLINN)
		float3 hv = normalize(ev + lv);
		
		return float4(
			1.0, 
			max(ndotl, 0.0f),
			(ndotl > 0.0) ? pow(max(dot(hv, nv), 0.0), max(Shininess, kLooseTolerance)) : 0.0, 
			1.0);
#	elif defined(PHONG)
		float3 rv = (2.0 * ndotl * nv) - lv;
		
		return float4(
			1.0,
			max(ndotl, 0.0f),
			(ndotl > 0.0) ? pow(max(dot(rv, ev), 0.0), max(Shininess, kLooseTolerance)) : 0.0,
			1.0); 
#	endif
# elif defined(DIFFUSE)
	return float4(
		1.0,
		max(ndotl, 0.0f),
		0.0, 
		1.0);
# endif
}

float3 GetNormal(vsOut aIn)
{
#	if defined(LIGHTABLE)
#		if defined(BUMP)
			float4 bump = tex2D(BumpSampler, aIn.BumpTexCoords);
			float3 tnv;
			tnv.xyz = (bump.xyz * 2.0) - 1.0;
			return normalize(NormalInTangentToView(tnv, aIn.ViewNormal, aIn.ViewTangent));
#		else
			return normalize(aIn.ViewNormal);
#		endif
#	else
		return float3(0, 0, 1);
#	endif
}

// TODO: implement specular for key and back lights.
float3 GetLitColor(float3 nv, float3 aDiffuse, float3 aSpecular, vsOut aIn)
{
#	if (defined(DIFFUSE) || defined(REFLECTIVE))
		float3 klv = (mul(float4(ThreePoint.KeyPosition, 1), GetView()).xyz - aIn.ViewPosition.xyz);
		float kd2 = dot(klv, klv);
		float3 flv = (-mul(ThreePoint.FillDirection, (float3x3)GetView()));
		float3 blv = (mul(float4(ThreePoint.BackPosition, 1), GetView()).xyz - aIn.ViewPosition.xyz);
		float bd2 = dot(blv, blv);
		
		klv = normalize(klv);
		blv = normalize(blv);
		
		float kdot = max(dot(klv, nv), 0.0);
		float fdot = max(dot(flv, nv), 0.0);
		float bdot = max(dot(blv, nv), 0.0);
		
		float3 lcol = (kdot * ThreePoint.KeyDiffuse) 
					+ (fdot * ThreePoint.KeyToFillRatio * ThreePoint.KeyDiffuse)
					+ (bdot * ThreePoint.BackDiffuse);

		float3 ret = (lcol * aDiffuse);
		
		return ret;
#	else
		return (kZero3);
#	endif
}

float3 ApplyAlpha(float3 aFbColor, float aAlpha)
{
	return (aFbColor * aAlpha);
}

///////////////////////////////////////////////////////////////////////////////
// fragment shaders
///////////////////////////////////////////////////////////////////////////////
float4 FragmentPicking(vsOutPicking aIn) : COLOR
{
#	if defined(TRANSPARENT)
		float4 transparent = GetTransparent(aIn);
		float alpha = GetAlpha(transparent);
		
		clip(alpha - ALPHA_IS_OPAQUE_F);
#	endif

	return float4(
		(aIn.PositionH.z / aIn.PositionH.w),
		ConstantColor.gb,
		1.0);
}

float4 FragmentShadow(vsOutShadow aIn) : COLOR
{
#	if defined(TRANSPARENT)
		float4 transparent = GetTransparent(aIn);
		float alpha = GetAlpha(transparent);
		
		clip(alpha - ALPHA_IS_OPAQUE_F);
#	endif

	float l = (length(aIn.ViewPosition) - GetNear()) / (GetFar() - GetNear());
	
#   if JZ_ENABLE_SHADOW_SLOPE_SCALE_DEPTH_BIAS
        float dx = ddx(l);
        float dy = ddx(l);

        l = saturate(l + kSlopeScaleBiasFactor * sqrt((dx * dx) + (dy * dy)));
#   endif

	return float4(l, 0, 0, 0);
}

fullFbOut FragmentRender(vsOut aIn, uniform bool abIncludeInDeferred)
{
	float alpha = GetAlpha(GetTransparent(aIn));
	
	float3 ambient = GetAmbient(aIn).rgb;
	float3 emissiv = GetEmission(aIn).rgb;
	
	float3 diffuse = GetDiffuse(aIn).rgb;
	float3 specular = GetSpecular(aIn).rgb;

	float3 base = GetBaseColor(diffuse, emissiv, ambient);
	float3 nv   = GetNormal(aIn); 
	float3 lit  = GetLitColor(nv, diffuse, specular, aIn);
	
	float3 ret = ApplyAlpha(base + lit, alpha);

#	if (defined(DIFFUSE) || defined(REFLECTIVE) || defined(SPECULAR))
        fullFbOut fbOut;
        if (abIncludeInDeferred)
        {
#           if defined(SPECULAR)
                fbOut = Out(float4(ret, alpha), nv, aIn.ViewPosition.z, float4(diffuse, 1.0), specular, Shininess);
#           else
                fbOut = Out(float4(ret, alpha), nv, aIn.ViewPosition.z, float4(diffuse, 1.0), kBlack3, 0.0);
#           endif
        }
        else
        {
            fbOut = Out(float4(ret, alpha), nv, aIn.ViewPosition.z, kBlack4, kBlack3, 0.0);
        }
#	else
		fullFbOut fbOut = Out(float4(ret, alpha), nv, aIn.ViewPosition.z, kBlack4, kBlack3, 0.0);
#	endif
   
    return fbOut;
}

float4 FragmentRenderReflection(vsOutReflection aIn) : COLOR0
{
	float alpha = GetAlpha(GetTransparentForReflection(aIn));
	
	float3 ambient = GetAmbient(aIn).rgb;
	float3 emissiv = GetEmissionForReflection(aIn).rgb;
	
	float3 diffuse = GetDiffuse(aIn).rgb;

	float3 base = GetBaseColor(diffuse, emissiv, ambient);
	
	float3 ret = ApplyAlpha(base + diffuse, alpha);

    // Convert back to linear, reflection buffers are expected to be ldr 8-bit textures.
    return LinearToNonLinear(float4(ret, alpha));
}

technique jz_Render
#if defined(TRANSPARENT_TEXTURE) && defined(TRANSPARENT_TEXTURE_1_BIT)
	<bool jz_IsTransparent = true; bool jz_IsAlpha1Bit = true;>
#elif defined(TRANSPARENT)
	<bool jz_IsTransparent = true;>
#endif
{ 
#if defined(TRANSPARENT_TEXTURE)
	pass Pass0
	{
		JZ_FULL_OUTPUT 
		DISABLE_STENCIL
		
		AlphaBlendEnable = false;
		AlphaTestEnable = true;
		AlphaFunc = GreaterEqual;
		AlphaRef = ALPHA_IS_OPAQUE;
		CullMode = None;
		FillMode = Solid;
		ZEnable = true;
		ZWriteEnable = true;
		
		VertexShader = compile vs_2_a VertexRender();
		PixelShader = compile ps_2_a FragmentRender(true);
	}
#	if !defined(TRANSPARENT_TEXTURE_1_BIT)
		pass Pass1
		{
			JZ_FULL_OUTPUT 
			DISABLE_STENCIL

			AlphaBlendEnable = true;
			AlphaTestEnable = true;
			AlphaFunc = Less;
			AlphaRef = ALPHA_IS_OPAQUE;
			CullMode = FRONT_FACE_CULLING;
			DestBlend = InvSrcAlpha;
			FillMode = Solid;
			SrcBlend = One;		
			ZEnable = true;
			ZWriteEnable = false;
			
			VertexShader = compile vs_2_a VertexRender();
			PixelShader = compile ps_2_a FragmentRender(true);			
		}
			
		pass Pass2
		{
			JZ_FULL_OUTPUT 
			DISABLE_STENCIL

			AlphaBlendEnable = true;
			AlphaTestEnable = true;
			AlphaFunc = Less;
			AlphaRef = ALPHA_IS_OPAQUE;
			CullMode = BACK_FACE_CULLING;
			DestBlend = InvSrcAlpha;
			FillMode = Solid;
			SrcBlend = One;			
			ZEnable = true;
			ZWriteEnable = false;
			
			VertexShader = compile vs_2_a VertexRender();
			PixelShader = compile ps_2_a FragmentRender(true);			
		}
#	endif
#elif defined(TRANSPARENT)
	pass Pass0
	{
		JZ_FULL_OUTPUT 
		DISABLE_STENCIL

		AlphaBlendEnable = true;
		AlphaTestEnable = false;
		CullMode = FRONT_FACE_CULLING;
		DestBlend = InvSrcAlpha;
		FillMode = Solid;
		SrcBlend = One;		
		ZEnable = true;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_a VertexRender();
		PixelShader = compile ps_2_a FragmentRender(true);			
	}
		
	pass Pass1
	{
		JZ_FULL_OUTPUT 
		DISABLE_STENCIL

		AlphaBlendEnable = true;
		AlphaTestEnable = false;
		CullMode = BACK_FACE_CULLING;
		DestBlend = InvSrcAlpha;
		FillMode = Solid;
		SrcBlend = One;		
		ZEnable = true;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_a VertexRender();
		PixelShader = compile ps_2_a FragmentRender(true);			
	}
#else
	pass Pass0
	{
		JZ_FULL_OUTPUT 
		DISABLE_STENCIL

		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		CullMode = BACK_FACE_CULLING;
		FillMode = Solid;
		ZEnable = true;
		ZWriteEnable = true;
		
		VertexShader = compile vs_2_a VertexRender();
		PixelShader = compile ps_2_a FragmentRender(true);
	}
#endif
}

#if !defined(TRANSPARENT)
	technique jz_RenderNonDeferred
	{
		pass Pass0
		{
			JZ_FULL_OUTPUT 
			DISABLE_STENCIL

			AlphaBlendEnable = false;
			AlphaTestEnable = false;
			CullMode = BACK_FACE_CULLING;
			FillMode = Solid;
			ZEnable = true;
			ZWriteEnable = true;
			
			VertexShader = compile vs_2_a VertexRender();
			PixelShader = compile ps_2_a FragmentRender(false);
		}
	}
#endif

technique jz_Pick
{
	pass
	{
# if defined(TRANSPARENT)
		CullMode = None;
# else
		CullMode = BACK_FACE_CULLING;
# endif

		JZ_LIT_OUTPUT	
		DISABLE_STENCIL
		
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		FillMode = Solid;
		ZEnable = true;
		ZWriteEnable = true;
	
		VertexShader = compile vs_2_0 VertexPicking();
		PixelShader = compile ps_2_0 FragmentPicking();
	}
}

technique jz_Shadow
{
	pass
	{
# if defined(TRANSPARENT)
		CullMode = None;
# else
        // One-sided triangles should still cast shadow from both directions?
        // The long term fix is probably to make sure all meshes are sealed, so this will not happen.
		CullMode = None;//BACK_FACE_CULLING; 
# endif

		JZ_SHADOW_OUTPUT	
		DISABLE_STENCIL
		
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		FillMode = Solid;
		ZEnable = true;
		ZWriteEnable = true;
	
#if JZ_ENABLE_SHADOW_SLOPE_SCALE_DEPTH_BIAS
        VertexShader = compile vs_2_a VertexShadow();
		PixelShader = compile ps_2_a FragmentShadow();
#else
        VertexShader = compile vs_2_0 VertexShadow();
		PixelShader = compile ps_2_0 FragmentShadow();
#endif
	}
}

// Render for reflection - note that all the culling states are flipped.
technique jz_RenderReflection
#if defined(TRANSPARENT_TEXTURE) && defined(TRANSPARENT_TEXTURE_1_BIT)
	<bool jz_IsTransparent = true; bool jz_IsAlpha1Bit = true;>
#elif defined(TRANSPARENT)
	<bool jz_IsTransparent = true;>
#endif
{ 
#if defined(TRANSPARENT_TEXTURE)
	pass Pass0
	{
		JZ_LIT_OUTPUT 
		DISABLE_STENCIL
		
		AlphaBlendEnable = false;
		AlphaTestEnable = true;
		AlphaFunc = GreaterEqual;
		AlphaRef = ALPHA_IS_OPAQUE;
		CullMode = None;
		FillMode = Solid;
		ZEnable = true;
		ZWriteEnable = true;
		
		VertexShader = compile vs_2_a VertexRenderReflection();
		PixelShader = compile ps_2_a FragmentRenderReflection();
	}
#	if !defined(TRANSPARENT_TEXTURE_1_BIT)
		pass Pass1
		{
			JZ_LIT_OUTPUT 
			DISABLE_STENCIL

			AlphaBlendEnable = true;
			AlphaTestEnable = true;
			AlphaFunc = Less;
			AlphaRef = ALPHA_IS_OPAQUE;
			CullMode = BACK_FACE_CULLING;
			DestBlend = InvSrcAlpha;
			FillMode = Solid;
			SrcBlend = One;		
			ZEnable = true;
			ZWriteEnable = false;
			
			VertexShader = compile vs_2_a VertexRenderReflection();
			PixelShader = compile ps_2_a FragmentRenderReflection();			
		}
			
		pass Pass2
		{
			JZ_LIT_OUTPUT 
            DISABLE_STENCIL
			
			AlphaBlendEnable = true;
			AlphaTestEnable = true;
			AlphaFunc = Less;
			AlphaRef = ALPHA_IS_OPAQUE;
			CullMode = FRONT_FACE_CULLING;
			DestBlend = InvSrcAlpha;
			FillMode = Solid;
			SrcBlend = One;			
			ZEnable = true;
			ZWriteEnable = false;
			
			VertexShader = compile vs_2_a VertexRenderReflection();
			PixelShader = compile ps_2_a FragmentRenderReflection();			
		}
#	endif
#elif defined(TRANSPARENT)
	pass Pass0
	{
		JZ_LIT_OUTPUT 
		DISABLE_STENCIL

		AlphaBlendEnable = true;
		AlphaTestEnable = false;
		CullMode = BACK_FACE_CULLING;
		DestBlend = InvSrcAlpha;
		FillMode = Solid;
		SrcBlend = One;		
		ZEnable = true;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_a VertexRenderReflection();
		PixelShader = compile ps_2_a FragmentRenderReflection();			
	}
		
	pass Pass1
	{
		JZ_LIT_OUTPUT 
		DISABLE_STENCIL

		AlphaBlendEnable = true;
		AlphaTestEnable = false;
		CullMode = FRONT_FACE_CULLING;
		DestBlend = InvSrcAlpha;
		FillMode = Solid;
		SrcBlend = One;		
		ZEnable = true;
		ZWriteEnable = false;
		
		VertexShader = compile vs_2_a VertexRenderReflection();
		PixelShader = compile ps_2_a FragmentRenderReflection();			
	}
#else
	pass Pass0
	{
		JZ_LIT_OUTPUT 
		DISABLE_STENCIL

		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		CullMode = FRONT_FACE_CULLING;
		FillMode = Solid;
		ZEnable = true;
		ZWriteEnable = true;
		
		VertexShader = compile vs_2_a VertexRenderReflection();
		PixelShader = compile ps_2_a FragmentRenderReflection();
	}
#endif
}

#endif
