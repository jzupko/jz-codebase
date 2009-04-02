///////////////////////////////////////////////////////////////////////////////
// Improved Noise by Ken Perlin - http://mrl.nyu.edu/~perlin/noise/
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMPROVED_NOISE_H_
#define _IMPROVED_NOISE_H_

#include "../media/common.fxh"

///////////////////////////////////////////////////////////////////////////////
// runtime constants
///////////////////////////////////////////////////////////////////////////////
texture inPermutations2D : jz_inPermutations2D;
texture inPermGradients1D : jz_inPermGradients1D;

sampler Permutations = sampler_state { texture = <inPermutations2D>; AddressU = Wrap; AddressV = Wrap; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = NONE; };
sampler PermGradients = sampler_state { texture = <inPermGradients1D>; AddressU = Wrap; AddressV = Clamp; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = NONE; };

///////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////
float3 Fade(float3 t)
{
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); 
}

float GradPerm(float u, float3 p)
{
	return dot(tex1D(PermGradients, u).xyz, p);
}

float4 Perm(float2 uv)
{
	return tex2D(Permutations, uv);
}

float ImprovedNoise(float3 p)
{
	float kOne = (1.0 / 256.0);
	
	float3 P = fmod(floor(p), 256.0);
  	p -= floor(p);
	float3 f = Fade(p);

	P = (P / 256.0);
	
	float4 AA = Perm(P.xy) + P.z;
 
  	return lerp(lerp(lerp(GradPerm(AA.x, p ),  
                          GradPerm(AA.z, p + float3(-1,  0, 0)), f.x),
                     lerp(GradPerm(AA.y, p + float3( 0, -1, 0)),
                          GradPerm(AA.w, p + float3(-1, -1, 0)), f.x), f.y),
                             
                lerp(lerp(GradPerm(AA.x + kOne, p + float3( 0,  0, -1)),
                          GradPerm(AA.z + kOne, p + float3(-1,  0, -1)), f.x),
                     lerp(GradPerm(AA.y + kOne, p + float3( 0, -1, -1)),
                          GradPerm(AA.w + kOne, p + float3(-1, -1, -1)), f.x), f.y), f.z);
}

float Turbulence(float3 p, int octaves, float lacunarity = 2.0, float gain = 0.5)
{
	float sum = 0;
	float freq = 1.0;
	float amp = 1.0;
	
	for (int i = 0; i < octaves; i++)
	{
		sum += abs(ImprovedNoise(p * freq)) * amp;
		freq *= lacunarity;
		amp *= gain;
	}

	return sum;
}

#endif
