#ifndef _SCREEN_SPACE_COMMON_FXH_
#define _SCREEN_SPACE_COMMON_FXH_

#include "../media/common.fxh"

//////////////////////////////////////////////////////////////////////////////////////////
// screenspace functions
//////////////////////////////////////////////////////////////////////////////////////////
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
	float offset = (aOffsetInPixels * (1.0 / ScreenDimensions.x));
	float4 ret = float4((hpos.x + offset), hpos.yzw);
	
	return ret;
}

float4 OffsetHuvV(float4 hpos, float aOffsetInPixels)
{
	float offset = (aOffsetInPixels * (1.0 / ScreenDimensions.y));
	float4 ret = float4(hpos.x, (hpos.y + offset), hpos.zw);
	
	return ret;
}

float4 OffsetHuv(float4 hpos, float2 aOffsetInPixels)
{
	float2 offset = (aOffsetInPixels * (1.0 / ScreenDimensions));
	float4 ret = float4(hpos.xy + offset, hpos.zw);
	
	return ret;
}

// normalizes depth to [0, 1]
float GetNormalizedDepth(float aViewDepth)
{
	float near = GetNear();
	float far = GetFar();
	
	float ret = (-aViewDepth - near) / (far - near);
	
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
