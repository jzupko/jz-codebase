#ifndef _DEFERRED_LIGHTING_FXH_
#define _DEFERRED_LIGHTING_FXH_

#include "../media/fb_common.fxh"
#include "../media/lighting_common.fxh"
#include "../media/screen_space_common.fxh"

//////////////////////////////////////////////////////////////////////////////////////////
// in/out
//////////////////////////////////////////////////////////////////////////////////////////
struct vsIn { float4 Position : POSITION; };
struct vsOut { float4 Position : POSITION; float4 ScreenHTexCoords : TEXCOORD0; };

texture ShadowTexture : jz_ShadowTexture;
sampler ShadowSampler = sampler_state { texture = <ShadowTexture>; AddressU = clamp; AddressV = clamp; MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = NONE; };
float2 ShadowDelta : jz_ShadowDelta = float2(0, 0);
float2 ShadowNearFar : jz_ShadowNearFar = float2(0, 0);
float4 ShadowScaleShift : jz_ShadowScaleShift = float4(0, 0, 0, 0);
float4x4 ShadowTransform : jz_ShadowTransform = kIdentity;
float ShadowBleedReduction : jz_ShadowBleedReduction = 0.0f;

//////////////////////////////////////////////////////////////////////////////////////////
// constants
//////////////////////////////////////////////////////////////////////////////////////////
# if !NDEBUG
    static const float3 kDebugDirectionalColor = float3(1, 0, 0);
    static const float3 kDebugPointColor = float3(0, 1, 0);
    static const float3 kDebugSpotColor = float3(0, 0, 1);
    
    bool bDebugDeferred : jz_bDebugDeferred = false;
# endif  

//////////////////////////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////////////////////////
float3 GetNonDirectionalLight(vsOut aIn, out float3 arLv, out float arDistance, out float3 arViewPos)
{
	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float3 diffuse = GetDeferredDiffuse(aIn.ScreenHTexCoords);
	arViewPos = GetViewPosition(aIn.ScreenHTexCoords, nd.w);

    arLv = (LightV - arViewPos);
    float distanceSquare = dot(arLv, arLv);
    arDistance = sqrt(distanceSquare);
    if (arDistance > kZeroTolerance) { arLv = normalize(arLv); }
	
	float att = GetLightAttenuation(LightAttenuation, arDistance, distanceSquare);
	
	float3 nv = (nd.xyz);
	float ndotl = dot(nv, arLv);

	float3 lightColor = max(ndotl, 0.0) * LightColor;
	float3 color = (att * diffuse * lightColor);
	
	return color;
}

//////////////////////////////////////////////////////////////////////////////////////////
// vertex
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
// fragment
//////////////////////////////////////////////////////////////////////////////////////////
float4 FragmentDirectional(vsOut aIn) : COLOR
{
	float4 nd = GetDeferredNormalAndDepth(aIn.ScreenHTexCoords);
	float3 diffuse = GetDeferredDiffuse(aIn.ScreenHTexCoords);

	float3 nv = (nd.xyz);
	float ndotl = dot(nv, -LightV);

	float3 lightColor = max(ndotl, 0.0) * LightColor;
	float3 color = (diffuse * lightColor);
		
#	if !NDEBUG
		if (bDebugDeferred) { color = kDebugDirectionalColor; }
#	endif

    return float4(color, 1);
}

float4 FragmentPoint(vsOut aIn) : COLOR
{
    float3 lv;
    float distance;
    float3 vPos;
    
	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos);
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

	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos);
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

	float3 color = GetNonDirectionalLight(aIn, lv, distance, vPos);
	float spot = GetSpotFactor(lv, SpotDirection, SpotFalloffExponent, SpotFalloffCosHalfAngle);
	
	float4 shadowTexCoords = mul(float4(vPos, 1.0), ShadowTransform);
	float pixelDepth = ((distance - ShadowNearFar.x) / ShadowNearFar.y);
	float shadow = GetShadowFactor(ShadowSampler, ShadowDelta, shadowTexCoords, ShadowScaleShift, pixelDepth, ShadowBleedReduction);
	
#   if !NDEBUG
		if (bDebugDeferred) { color = (kDebugSpotColor); }
		else { color *= (spot * shadow); }
#	else
		color *= (spot * shadow);
#	endif

    return float4(color, 1);
}

#define DISABLE_MASKING
#define DRAW_FRONT_FACE
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
#define DRAW_FRONT_FACE
#define FRAGMENT_SHADER_NAME FragmentPoint
#define VERTEX_SHADER_NAME VertexQuad
#define TECHNIQUE_NAME PointAsQuad
#include "../media/deferred_lighting_technique_helper.fxh"

#define DISABLE_MASKING
#define DRAW_FRONT_FACE
#define FRAGMENT_SHADER_NAME FragmentSpot
#define VERTEX_SHADER_NAME VertexQuad
#define TECHNIQUE_NAME SpotAsQuad
#include "../media/deferred_lighting_technique_helper.fxh"

#endif
