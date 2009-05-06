#ifndef _LIGHTING_COMMON_FXH_
#define _LIGHTING_COMMON_FXH_

#include "../media/common.fxh"

//////////////////////////////////////////////////////////////////////////////////////////
// runtime constants
//////////////////////////////////////////////////////////////////////////////////////////
float3 LightAttenuation : jz_LightAttenuation;
float3 LightColor : jz_LightColor;
float3 LightV : jz_LightV;
float3 SpotDirection : jz_SpotDirection;
float SpotFalloffCosHalfAngle : jz_SpotFalloffCosHalfAngle;
float SpotFalloffExponent : jz_SpotFalloffExponent;

//////////////////////////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////////////////////////
float GetDiffuseFactor(float ndotl)
{
    return max(ndotl, 0.0);
}

float GetLightAttenuation(float3 att, float distance)
{
    float ret = 1.0 /
        (att.x + (att.y * distance) + (att.z * (distance * distance)));

    return ret;
}

float GetLightAttenuation(float3 att, float distance, float distanceSquare)
{
    float ret = 1.0 /
        (att.x + (att.y * distance) + (att.z * distanceSquare));

    return ret;
}

float3 GetLitDiffuseAtt(float3 lightColor, float3 matDiffuse, float diffFactor, float att)
{
    float3 ret = (lightColor * matDiffuse * diffFactor);
	ret *= att;
	
    return ret;
}

float3 GetLitDiffuse(float3 lightColor, float3 matDiffuse, float diffFactor)
{
    float3 ret = (lightColor * matDiffuse * diffFactor);

    return ret;
}

float3 GetLitSpecular(float3 lightColor, float3 matSpecular, float specFactor)
{
    float3 ret = (lightColor * matSpecular * specFactor);

    return ret;
}

float3 GetLitColor(
    float3 lightColor, 
    float3 matDiffuse, float diffFactor,
    float3 matSpecular, float specFactor)
{
    return (GetLitDiffuse(lightColor, matDiffuse, diffFactor) +
            GetLitSpecular(lightColor, matSpecular, specFactor));
}

float3 GetLitColorAtt(
    float3 lightColor, 
    float3 matDiffuse, float diffFactor,
    float3 matSpecular, float specFactor,
    float att)
{
    float3 ret = GetLitColor(lightColor, matDiffuse, diffFactor,
                             matSpecular, specFactor);

    ret *= att;

    return ret;
}

float GetNdotL(float3 n, float3 lv)
{
    return (dot(n, lv));
}

float3 GetReflectionVector(float3 n, float3 lv, float ndotl)
{
    return (2.0 * ndotl * n) - lv;
}

float GetSpecularFactor(float3 ev, float3 rv, 
                        float ndotl, float shininess)
{
    float ret =
        (ndotl > 0.0)
            ? pow(max(dot(ev, rv), 0.0), shininess)
            : 0.0;

    return ret;
}

float GetSpotFactor(float3 lv, float3 spotDirection, float spotFalloffExponent, float spotFalloffCosHalfAngle)
{
    float spotDot = -dot(lv, spotDirection);
    float ret = 
        (spotDot < spotFalloffCosHalfAngle) 
            ? 0.0
            : pow(max(spotDot, 0.0), spotFalloffExponent);

    return ret;
}

#endif
