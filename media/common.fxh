#ifndef _COMMON_FXH_
#define _COMMON_FXH_

//////////////////////////////////////////////////////////////////////////////////////////
// static constants and helpers
//////////////////////////////////////////////////////////////////////////////////////////
#define NDEBUG 0

#define JZ_CAT_IMPLC(a) a
#define JZ_CAT_IMPLB(a,b) JZ_CAT_IMPLC(a##b)
#define JZ_CAT_IMPL(a) JZ_CAT_IMPLB##a
#define JZ_CAT(a,b) JZ_CAT_IMPL((a,b))

#define JZ_EQL(a,b) 

#define ALPHA_IS_OPAQUE      127
#define ALPHA_IS_OPAQUE_F    (127.0 / 255.0)

#define BACK_FACE_IS_CCW 1

#if BACK_FACE_IS_CCW
#	define BACK_FACE_CULLING Ccw
#	define FRONT_FACE_CULLING Cw
#else
#	define BACK_FACE_CULLING Cw
#	define FRONT_FACE_CULLING Ccw
#endif

#define DISABLE_COLOR_WRITE \
	AlphaBlendEnable = true; \
	DestBlend = One; \
	SrcBlend = Zero; \
	ColorWriteEnable = 0; \
	ColorWriteEnable1 = 0; \
	ColorWriteEnable2 = 0; \
	ColorWriteEnable3 = 0;
	
#define MASK_TO_EXCLUDE_FROM_DEFERRED_LIGHTING \
		TwoSidedStencilMode = false; \
        StencilRef = 0xFF; \
        StencilEnable = true; \
        StencilFail = Keep; \
        StencilFunc = Always; \
		StencilPass = Replace; \
		StencilZFail = Keep; \
        StencilMask = 0xFF; \
        StencilWriteMask = STENCIL_BIT_NO_DEFERRED; 
        
#define DISABLE_STENCIL \
		StencilEnable = false; \
		TwoSidedStencilMode = false;

static const float3 kBlack3 = float3(0, 0, 0);
static const float4 kBlack4 = float4(0, 0, 0, 1);

static const float kInfiniteProjEpsilon = (2.4e-4 - 1.0);

static const float3 kZero3 = float3(0, 0, 0);
static const float3 kOne3 = float3(1.0, 1.0, 1.0);

static const float kLooseTolerance = 1e-3;
static const float kZeroTolerance = 1e-6;
static const float3 kZeroTolerance3 = float3(1e-6, 1e-6, 1e-6);

static const float4x4 kProjectionAdjust = float4x4(0.5, 0,   0, 0,
                                                   0,  -0.5, 0, 0,
                                                   0,   0,   1, 0,
                                                   0.5, 0.5, 0, 1);

static const int kSkinningMatricesCount = 72;
static const int kSkinningMatricesSize = (kSkinningMatricesCount * 3);

static const float4x4 kStdProj = float4x4(0.75, 0, 0, 0,
										  0,    1, 0, 0,
										  0, 0, -1.0043993, -1.0,
										  0, 0, -0.09957182, 0);

static const float4x4 kIdentity = float4x4(1, 0, 0, 0,
										   0, 1, 0, 0,
										   0, 0, 1, 0,
										   0, 0, 0, 1);
						
	  
#define TRANSPARENT_TEXTURE_1_BIT 1

#define STENCIL_BIT_NO_DEFERRED   1
#define STENCIL_BIT_DEFERRED_MASK 2

//////////////////////////////////////////////////////////////////////////////////////////
// runtime constants and core functions
//////////////////////////////////////////////////////////////////////////////////////////
shared float Gamma : jz_Gamma = 1.0;
shared float2 CameraFocalLength : jz_CameraFocalLength = float2(1, 1);
shared float4x4 Projection : jz_Projection = kStdProj;
shared float2 ScreenDimensions : jz_ScreenDimensions = float2(1, 1);
shared float Time : jz_Time = 0.0f;
shared float4x4 View : jz_View = kIdentity;

float4 ConstantColor : jz_ConstantColor = kBlack4;
# if defined(ANIMATED)
    float4 Skinning[kSkinningMatricesSize] : jz_Skinning;
# endif
float4x4 Wit : jz_Wit = kIdentity;
float4x4 World : jz_World = kIdentity;

float3 GammaColor(uniform float3 aColor)
{
	float3 ret;
	ret.r = (aColor.r >= kLooseTolerance) ? pow(aColor.r, Gamma) : 0.0;
	ret.g = (aColor.g >= kLooseTolerance) ? pow(aColor.g, Gamma) : 0.0;
	ret.b = (aColor.b >= kLooseTolerance) ? pow(aColor.b, Gamma) : 0.0;
	
	return ret;
}

float4 LinearToNonLinear(float4 aColor)
{
	return float4(pow(aColor.rgb, (1.0 / Gamma)), aColor.a);
}

float4 GammaColor(uniform float4 aColor)
{
	float4 ret = float4(GammaColor(aColor.rgb), aColor.a);
	
	return ret;
}

float4 GammaTex2D(sampler aSampler, float2 aTexCoords)
{
	float4 col = tex2D(aSampler, aTexCoords);
	
	return float4(pow(col.rgb, Gamma), col.a);
}

// inversion by parts
float3x3 GetInverse(float3x3 m)
{
    float n11 = m._11; float n12 = m._12; float n13 = m._13;
    float n21 = m._21; float n22 = m._22; float n23 = m._23;
    float n31 = m._31; float n32 = m._32; float n33 = m._33;

    // entries in new matrix p to be multiplied by (1 / |M|)
    float p11 = (n22 * n33) - (n23 * n32);
    float p21 = (n23 * n31) - (n21 * n33);
    float p31 = (n21 * n32) - (n22 * n31);

    // 1 / |M|
    float t = 1.0f / ((n11 * p11) + (n12 * p21) + (n13 * p31));

	float3x3 n;
    n._11 = p11 * t; n._12 = ((n13 * n32) - (n12 * n33)) * t; n._13 = ((n12 * n23) - (n13 * n22)) * t;
    n._21 = p21 * t; n._22 = ((n11 * n33) - (n13 * n31)) * t; n._23 = ((n13 * n21) - (n11 * n23)) * t;
    n._31 = p31 * t; n._32 = ((n12 * n31) - (n11 * n32)) * t; n._33 = ((n11 * n22) - (n12 * n21)) * t;
    
    return n;
}

float4x4 GetInverse(float4x4 m)
{
    float m00 = m._11, m01 = m._12, m02 = m._13, m03 = m._14;
    float m10 = m._21, m11 = m._22, m12 = m._23, m13 = m._24;
    float m20 = m._31, m21 = m._32, m22 = m._33, m23 = m._34;
    float m30 = m._41, m31 = m._42, m32 = m._43, m33 = m._44;

    float v0 = (m20 * m31) - (m21 * m30);
    float v1 = (m20 * m32) - (m22 * m30);
    float v2 = (m20 * m33) - (m23 * m30);
    float v3 = (m21 * m32) - (m22 * m31);
    float v4 = (m21 * m33) - (m23 * m31);
    float v5 = (m22 * m33) - (m23 * m32);

    float t00 =  (v5 * m11 - v4 * m12 + v3 * m13);
    float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
    float t20 =  (v4 * m10 - v2 * m11 + v0 * m13);
    float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

    float t = 1.0 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

    float d00 = (t00 * t);
    float d10 = (t10 * t);
    float d20 = (t20 * t);
    float d30 = (t30 * t);

    float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * t;
    float d11 =  (v5 * m00 - v2 * m02 + v1 * m03) * t;
    float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * t;
    float d31 =  (v3 * m00 - v1 * m01 + v0 * m02) * t;

    v0 = (m10 * m31) - (m11 * m30);
    v1 = (m10 * m32) - (m12 * m30);
    v2 = (m10 * m33) - (m13 * m30);
    v3 = (m11 * m32) - (m12 * m31);
    v4 = (m11 * m33) - (m13 * m31);
    v5 = (m12 * m33) - (m13 * m32);

    float d02 =  (v5 * m01 - v4 * m02 + v3 * m03) * t;
    float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * t;
    float d22 =  (v4 * m00 - v2 * m01 + v0 * m03) * t;
    float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * t;

    v0 = (m21 * m10) - (m20 * m11);
    v1 = (m22 * m10) - (m20 * m12);
    v2 = (m23 * m10) - (m20 * m13);
    v3 = (m22 * m11) - (m21 * m12);
    v4 = (m23 * m11) - (m21 * m13);
    v5 = (m23 * m12) - (m22 * m13);

    float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * t;
    float d13 =  (v5 * m00 - v2 * m02 + v1 * m03) * t;
    float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * t;
    float d33 =  (v3 * m00 - v1 * m01 + v0 * m02) * t;

    return float4x4(
        d00, d01, d02, d03,
        d10, d11, d12, d13,
        d20, d21, d22, d23,
        d30, d31, d32, d33);
}

float4 GetPlane(float3 aNormal, float3 aPosition)
{
	float4 ret = float4(aNormal, dot(-aNormal, aPosition));
	
	return ret;
}

float DotCoordinate(float4 aPlane, float3 aPoint)
{
	return (aPlane.w + dot(aPlane.xyz, aPoint));
}

float2 Reflect(float2 u, float2 v)
{
	return (u - (2.0 * dot(u, v) * v));
}

float3 Reflect(float3 u, float3 v)
{
	return (u - (2.0 * dot(u, v) * v));
}

float3x3 GetInverseTranspose(float3x3 m)
{
	float3x3 ret = transpose(GetInverse(m));
	
	return ret;
}

float4x4 GetOrthoInverse(float4x4 m)
{
	float4x4 ret = float4x4(
	    m._11,  m._21,  m._31, 0,
	    m._12,  m._22,  m._32, 0,
	    m._13,  m._23,  m._33, 0,
	   -m._41, -m._42, -m._43, 1);
	   
	return ret;
}

# if defined(ANIMATED)
	float3x3 GetSkinning3x3(int index)
	{
		int i = (index * 3);
		int i0 = (i + 0);
		int i1 = (i + 1);
		int i2 = (i + 2);
		
		float3x3 ret = float3x3(
			Skinning[i0].x, Skinning[i1].x, Skinning[i2].x,
			Skinning[i0].y, Skinning[i1].y, Skinning[i2].y,
			Skinning[i0].z, Skinning[i1].z, Skinning[i2].z);
		    
		return ret;
	}

	float3x3 GetSkinning3x3(float4 weights, float4 indices)
	{
		float3x3 ret;
		ret  = weights.x * GetSkinning3x3(indices.x);
		ret += weights.y * GetSkinning3x3(indices.y);
		ret += weights.z * GetSkinning3x3(indices.z);
		ret += weights.w * GetSkinning3x3(indices.w);
		
		return ret;
	}

	float4x4 GetSkinning4x4(int index)
	{
		int i = (index * 3);
		int i0 = (i + 0);
		int i1 = (i + 1);
		int i2 = (i + 2);
		
		float4x4 ret = float4x4(
			Skinning[i0].x, Skinning[i1].x, Skinning[i2].x, 0,
			Skinning[i0].y, Skinning[i1].y, Skinning[i2].y, 0,
			Skinning[i0].z, Skinning[i1].z, Skinning[i2].z, 0,
			Skinning[i0].w, Skinning[i1].w, Skinning[i2].w, 1);
		    
		return ret;
	}

	float4x4 GetSkinning4x4(float4 weights, float4 indices)
	{
		float4x4 ret;
		ret  = weights.x * GetSkinning4x4(indices.x);
		ret += weights.y * GetSkinning4x4(indices.y);
		ret += weights.z * GetSkinning4x4(indices.z);
		ret += weights.w * GetSkinning4x4(indices.w);
		
		return ret;
	}
# endif

float4x4 GetProjection()
{
	return Projection;
}

float4x4 GetView()
{
	return View;
}

float GetFar()
{
	float ret = (GetProjection()._43 / (1.0f + GetProjection()._33));
	
	return ret;
}

float GetNear()
{
	float ret = (GetProjection()._43 / GetProjection()._33);
	
	return ret;	
}

float3 GetEyePositionWorld()
{
	float4x4 view = GetView();
	
	float3 ret = float3(-view._41, -view._42, -view._43);
	
	return ret;
}

float4x4 GetWorld()
{
	return World;
}

float3x3 GetInfiniteProjection()
{
	float3x3 ret = (float3x3)GetProjection();
	ret._33 = kInfiniteProjEpsilon;	

	return ret; 
}

float4x4 GetInverseView()
{
	return GetOrthoInverse(GetView());
}

float3x3 GetWit()
{
	return (float3x3)Wit;
}

float4x4 GetVP()
{
	return mul(GetView(), GetProjection());
}

float4x4 GetWV()
{
	return mul(GetWorld(), GetView());
}

float3x3 GetWitVP()
{
	return mul(GetWit(), (float3x3)GetVP());
}

float3x3 GetWitV()
{
	return mul(GetWit(), (float3x3)GetView());
}

float4x4 GetWVP()
{
	return mul(GetWorld(), GetVP());
}

float4 GetInfiniteHPosFromView(float4 v)
{
	float3x3 infProj = GetInfiniteProjection();
	
	float4 ret = float4(mul(v.xyz, infProj), -v.z);
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// in/out
///////////////////////////////////////////////////////////////////////////////
struct vsInSimple { float4 Position : POSITION; };
struct vsOutSimple { float4 Position : POSITION; };

///////////////////////////////////////////////////////////////////////////////
// vertex shaders
///////////////////////////////////////////////////////////////////////////////
vsOutSimple VertexSimple(vsInSimple aIn)
{
	vsOutSimple ret;
	ret.Position = mul(aIn.Position, GetWVP());
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// fragment shaders
///////////////////////////////////////////////////////////////////////////////
float4 FragmentSimple(vsOutSimple aIn) : COLOR
{
	return ConstantColor;
}

#endif
