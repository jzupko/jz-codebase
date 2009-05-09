#ifndef _SCREEN_SPACE_COMMON_FXH_
#define _SCREEN_SPACE_COMMON_FXH_

#include "../media/common.fxh"

//////////////////////////////////////////////////////////////////////////////////////////
// screenspace functions
//////////////////////////////////////////////////////////////////////////////////////////

/// 
/// <summary>Calculates homogenous screen-space texture coordinates from a vertex's
///          homogenous clip space position.
/// </summary>
/// <remarks>
/// The function below generates texture coordinates from the homogenous clip space position
/// of a vertex. It scales and shift the (x,y) position coordinates to map from [-1,1] to
/// [0,1]. It also flips the y component. It also adds an offset of (0.5 / ScreenDimensions),
/// which is a half-pixel offset, due to DirectX texture addressing conventions. Finally,
/// it accounts for the hyperbolic texture coordinate interpolation that normally occurs in
/// hardware, since it needs to be a linear interpolation in this case.
///
/// Texture coordinates are interpolated in hardware using a hyperbolic interpolation:
/// (u, v, w) in the vertex shader is divided by the hpos w component. These values, 
/// along with (1 / hpos.w) are linearly interpolated. For each pixel, the values are 
/// calculated as ((c / hpos.w) / (1 / hpos.w)), where c = u, v, w and (c / hpos.w) and
/// (1 / hpos.w) are the interpolate values at the pixel.
///
/// However, for texture coordinates for screen space lookup, we want a linear interpolation.
/// To achieve this, the texture coorinates are left homogenous and the shift portion of the
/// math below is multipled by hpos.w. In the pixel shader, the final homogenous texture
/// coordinates are divided by the interpolated w to get the final (u, v) texture coordinates.
/// </remarks>
float4 GetScreenHTexCoords(float4 hpos)
{
    float t = (0.5 * hpos.w);
    float2 f = t * float2(
        (1.0 + (1.0 / ScreenDimensions.x)),
        (1.0 + (1.0 / ScreenDimensions.y)));

    float4 ret;
    ret.x = f.x + (hpos.x * 0.5),
    ret.y = f.y - (hpos.y * 0.5),
    ret.z = hpos.z;
    ret.w = hpos.w;

    return ret;
}

float2 GetScreenUV(float4 screenHTexCoords)
{
    float2 ret = (screenHTexCoords.xy / screenHTexCoords.w);

    return ret;
}

float4 OffsetHuvH(float4 hpos, float aOffsetInPixels)
{
	float offset = (aOffsetInPixels * (1.0 / ScreenDimensions.x)) * hpos.w;
	float4 ret = float4((hpos.x + offset), hpos.yzw);
	
	return ret;
}

float4 OffsetHuvV(float4 hpos, float aOffsetInPixels)
{
	float offset = (aOffsetInPixels * (1.0 / ScreenDimensions.y)) * hpos.w;
	float4 ret = float4(hpos.x, (hpos.y + offset), hpos.zw);
	
	return ret;
}

float4 OffsetHuv(float4 hpos, float2 aOffsetInPixels)
{
	float2 offset = (aOffsetInPixels * (1.0 / ScreenDimensions)) * hpos.w;
	float4 ret = float4(hpos.xy + offset, hpos.zw);
	
	return ret;
}

float3 GetViewPosition(float4 huv, float aViewDepth)
{
	float2 uv = GetScreenUV(huv);
	float2 sc = (uv * float2(2, -2)) + float2(-1, 1);
	float3 ret = float3(sc * CameraFocalLength * -aViewDepth, aViewDepth);
	
	return ret;	
}

#endif
