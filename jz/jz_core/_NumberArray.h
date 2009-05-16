//
// Copyright (c) 2009 Joseph A. Zupko
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#if !defined(JZ_NUMBER_ARRAY_NAME) || !defined(JZ_NUMBER_ARRAY_DIM)
#   error "Please define the necessary macros before including this file."
#endif

JZ_STATIC_ASSERT(JZ_NUMBER_ARRAY_DIM > 1 && JZ_NUMBER_ARRAY_DIM < 5);

static const int N = JZ_NUMBER_ARRAY_DIM;
static const size_t kSizeInBytes = sizeof(float) * N;

JZ_NUMBER_ARRAY_NAME()
{
    pData[0] = 0;
    pData[1] = 0;

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] = 0;
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] = 0;
#   endif
}

JZ_NUMBER_ARRAY_NAME(const JZ_NUMBER_ARRAY_NAME& a)
{
    pData[0] = a.pData[0];
    pData[1] = a.pData[1];

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] = a.pData[2];
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] = a.pData[3];
#   endif
}

explicit JZ_NUMBER_ARRAY_NAME(float s)
{
    pData[0] = s;
    pData[1] = s;

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] = s;
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] = s;
#   endif
}

#if (JZ_NUMBER_ARRAY_DIM == 2)
    JZ_NUMBER_ARRAY_NAME(float a0, float a1)
    {
        pData[0] = a0;
        pData[1] = a1;
    }
#elif (JZ_NUMBER_ARRAY_DIM == 3)
    JZ_NUMBER_ARRAY_NAME(float a0, float a1, float a2)
    {
        pData[0] = a0;
        pData[1] = a1;
        pData[2] = a2;
    }
#elif (JZ_NUMBER_ARRAY_DIM == 4)
    JZ_NUMBER_ARRAY_NAME(float a0, float a1, float a2, float a3)
    {
        pData[0] = a0;
        pData[1] = a1;
        pData[2] = a2;
        pData[3] = a3;
    }
#endif

JZ_NUMBER_ARRAY_NAME& operator=(const JZ_NUMBER_ARRAY_NAME& a)
{
    pData[0] = a.pData[0];
    pData[1] = a.pData[1];

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] = a.pData[2];
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] = a.pData[3];
#   endif

    return *this;
}

float& operator[](int i)
{
    return pData[i];
}

float operator[](int i) const
{
    return pData[i];
}

bool operator==(const JZ_NUMBER_ARRAY_NAME& a) const
{
    return (pData[0] == a.pData[0]) &&
           (pData[1] == a.pData[1])

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        && (pData[2] == a.pData[2])
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        && (pData[3] == a.pData[3])
#   endif

    ;
}

bool operator!=(const JZ_NUMBER_ARRAY_NAME& a) const
{
    return !(*this == a);
}

JZ_NUMBER_ARRAY_NAME operator-() const
{
    return JZ_NUMBER_ARRAY_NAME(-pData[0], -pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , -pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , -pData[3]
#   endif

    );
}

JZ_NUMBER_ARRAY_NAME operator+(const JZ_NUMBER_ARRAY_NAME& a) const
{
    return JZ_NUMBER_ARRAY_NAME(pData[0] + a.pData[0], pData[1] + a.pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2] + a.pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3] + a.pData[3]
#   endif

    );
}

JZ_NUMBER_ARRAY_NAME operator-(const JZ_NUMBER_ARRAY_NAME& a) const
{
    return JZ_NUMBER_ARRAY_NAME(pData[0] - a.pData[0], pData[1] - a.pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2] - a.pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3] - a.pData[3]
#   endif

    );
}

JZ_NUMBER_ARRAY_NAME operator*(const JZ_NUMBER_ARRAY_NAME& a) const
{
    return JZ_NUMBER_ARRAY_NAME(pData[0] * a.pData[0], pData[1] * a.pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2] * a.pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3] * a.pData[3]
#   endif

    );
}

JZ_NUMBER_ARRAY_NAME operator/(const JZ_NUMBER_ARRAY_NAME& a) const
{
    return JZ_NUMBER_ARRAY_NAME(pData[0] / a.pData[0], pData[1] / a.pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2] / a.pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3] / a.pData[3]
#   endif

    );
}

JZ_NUMBER_ARRAY_NAME& operator+=(const JZ_NUMBER_ARRAY_NAME& a)
{
    pData[0] += a.pData[0];
    pData[1] += a.pData[1];

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] += a.pData[2];
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] += a.pData[3];
#   endif

    return *this;
}

JZ_NUMBER_ARRAY_NAME& operator-=(const JZ_NUMBER_ARRAY_NAME& a)
{
    pData[0] -= a.pData[0];
    pData[1] -= a.pData[1];

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] -= a.pData[2];
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] -= a.pData[3];
#   endif

    return *this;
}

JZ_NUMBER_ARRAY_NAME& operator*=(const JZ_NUMBER_ARRAY_NAME& a)
{
    pData[0] *= a.pData[0];
    pData[1] *= a.pData[1];

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] *= a.pData[2];
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] *= a.pData[3];
#   endif

    return *this;
}

JZ_NUMBER_ARRAY_NAME& operator/=(const JZ_NUMBER_ARRAY_NAME& a)
{
    pData[0] /= a.pData[0];
    pData[1] /= a.pData[1];

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] /= a.pData[2];
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] /= a.pData[3];
#   endif

    return *this;
}

JZ_NUMBER_ARRAY_NAME operator*(float s) const
{
    return JZ_NUMBER_ARRAY_NAME(pData[0] * s, pData[1] * s

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2] * s
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3] * s
#   endif

    );
}

JZ_NUMBER_ARRAY_NAME operator/(float s) const
{
    return JZ_NUMBER_ARRAY_NAME(pData[0] / s, pData[1] / s

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2] / s
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3] / s
#   endif

    );
}

JZ_NUMBER_ARRAY_NAME& operator*=(float s)
{
    pData[0] *= s;
    pData[1] *= s;

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] *= s;
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] *= s;
#   endif

    return *this;
}

JZ_NUMBER_ARRAY_NAME& operator/=(float s)
{
    pData[0] /= s;
    pData[1] /= s;

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] /= s;
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] /= s;
#   endif

    return *this;
}

Axis::Id ClosestAxis() const
{
    return (Abs(*this).MaxAxis());
}

Axis::Id MaxAxis() const
{
    Axis::Id ret = Axis::kX;
    float maxF = Constants<float>::kMin;

    if (pData[0] > maxF) { ret = Axis::kX; maxF = pData[0]; }
    if (pData[1] > maxF) { ret = Axis::kY; maxF = pData[1]; }

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        if (pData[2] > maxF) { ret = Axis::kZ; maxF = pData[2]; }
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        if (pData[3] > maxF) { ret = Axis::kW; }
#   endif

    return ret;
}

void Assign(float s)
{
    pData[0] = s;
    pData[1] = s;

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        pData[2] = s;
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        pData[3] = s;
#   endif
}

float Length() const
{
    return Sqrt(LengthSquared());
}

float LengthSquared() const
{
    return Dot(*this, *this);
}

float Max() const
{
    return jz::Max(pData[0], pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3]
#   endif

    );
}

float Min() const
{
    return jz::Min(pData[0], pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , pData[3]
#   endif

    );
}

float Sum() const
{
    return pData[0] + pData[1]

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        + pData[2]
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        + pData[3]
#   endif

    ;
}


#if (JZ_NUMBER_ARRAY_DIM == 2)
    void Set(float a0, float a1)
    {
        pData[0] = a0;
        pData[1] = a1;
    }
#elif (JZ_NUMBER_ARRAY_DIM == 3)
    void Set(float a0, float a1, float a2)
    {
        pData[0] = a0;
        pData[1] = a1;
        pData[2] = a2;
    }
#elif (JZ_NUMBER_ARRAY_DIM == 4)
    void Set(float a0, float a1, float a2, float a3)
    {
        pData[0] = a0;
        pData[1] = a1;
        pData[2] = a2;
        pData[3] = a3;
    }
#endif

static bool AboutEqual(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b, float aTolerance = Constants<float>::kZeroTolerance)
{
    return jz::AboutEqual(a.pData[0], b.pData[0], aTolerance) &&
           jz::AboutEqual(a.pData[1], b.pData[1], aTolerance)

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        && jz::AboutEqual(a.pData[2], b.pData[2], aTolerance)
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        && jz::AboutEqual(a.pData[3], b.pData[3], aTolerance)
#   endif

    ;
}

static bool AboutZero(const JZ_NUMBER_ARRAY_NAME& a, float aTolerance = Constants<float>::kZeroTolerance)
{
    return AboutEqual(a, JZ_NUMBER_ARRAY_NAME(0), aTolerance);
}

static JZ_NUMBER_ARRAY_NAME Abs(const JZ_NUMBER_ARRAY_NAME& a)
{
    JZ_NUMBER_ARRAY_NAME ret;

    ret.pData[0] = jz::Abs(a.pData[0]);
    ret.pData[1] = jz::Abs(a.pData[1]);

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        ret.pData[2] = jz::Abs(a.pData[2]);
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        ret.pData[3] = jz::Abs(a.pData[3]);
#   endif

    return ret;
}

static JZ_NUMBER_ARRAY_NAME Clamp(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& aMin, const JZ_NUMBER_ARRAY_NAME& aMax)
{
    JZ_NUMBER_ARRAY_NAME ret;
    ret.pData[0] = jz::Clamp(a.pData[0], aMin.pData[0], aMax.pData[0]);
    ret.pData[1] = jz::Clamp(a.pData[1], aMin.pData[1], aMax.pData[1]);

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        ret.pData[2] = jz::Clamp(a.pData[2], aMin.pData[2], aMax.pData[2]);
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        ret.pData[3] = jz::Clamp(a.pData[3], aMin.pData[3], aMax.pData[3]);
#   endif

    return ret;
}

static float Distance(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b)
{
    return ((a - b).Length());
}

static float DistanceSquared(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b)
{
    return ((a - b).LengthSquared());
}

__inline static float Dot(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b)
{
    float ret = (a.pData[0] * b.pData[0]) + (a.pData[1] * b.pData[1])
#       if (JZ_NUMBER_ARRAY_DIM > 2)
            + (a.pData[2] * b.pData[2])
#       endif
#       if (JZ_NUMBER_ARRAY_DIM > 3)
            + (a.pData[3] * b.pData[3])
#       endif
            ;

    return ret;
}

static JZ_NUMBER_ARRAY_NAME Max(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b)
{
    return JZ_NUMBER_ARRAY_NAME(jz::Max(a.pData[0], b.pData[0]), jz::Max(a.pData[1], b.pData[1])
    
#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , jz::Max(a.pData[2], b.pData[2])
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , jz::Max(a.pData[3], b.pData[3])
#   endif

    );
}

static JZ_NUMBER_ARRAY_NAME Max(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b, const JZ_NUMBER_ARRAY_NAME& c)
{
    return JZ_NUMBER_ARRAY_NAME(jz::Max(a.pData[0], b.pData[0], c.pData[0]), jz::Max(a.pData[1], b.pData[1], c.pData[1])
    
#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , jz::Max(a.pData[2], b.pData[2], c.pData[2])
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , jz::Max(a.pData[3], b.pData[3], c.pData[3])
#   endif

    );
}

static JZ_NUMBER_ARRAY_NAME Min(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b, const JZ_NUMBER_ARRAY_NAME& c)
{
    return JZ_NUMBER_ARRAY_NAME(jz::Min(a.pData[0], b.pData[0], c.pData[0]), jz::Min(a.pData[1], b.pData[1], c.pData[1])
    
#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , jz::Min(a.pData[2], b.pData[2], c.pData[2])
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , jz::Min(a.pData[3], b.pData[3], c.pData[3])
#   endif

    );
}

static JZ_NUMBER_ARRAY_NAME Min(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b)
{
    return JZ_NUMBER_ARRAY_NAME(jz::Min(a.pData[0], b.pData[0]), jz::Min(a.pData[1], b.pData[1])
    
#   if (JZ_NUMBER_ARRAY_DIM > 2)
        , jz::Min(a.pData[2], b.pData[2])
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        , jz::Min(a.pData[3], b.pData[3])
#   endif

    );
}

static JZ_NUMBER_ARRAY_NAME Lerp(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b, float aWeightOfB)
{
    JZ_NUMBER_ARRAY_NAME ret;
    ret.pData[0] = jz::Lerp(a.pData[0], b.pData[0], aWeightOfB);
    ret.pData[1] = jz::Lerp(a.pData[1], b.pData[1], aWeightOfB);

#   if (JZ_NUMBER_ARRAY_DIM > 2)
        ret.pData[2] = jz::Lerp(a.pData[2], b.pData[2], aWeightOfB);
#   endif

#   if (JZ_NUMBER_ARRAY_DIM > 3)
        ret.pData[3] = jz::Lerp(a.pData[3], b.pData[3], aWeightOfB);
#   endif

    return ret;
}

static JZ_NUMBER_ARRAY_NAME Normalize(const JZ_NUMBER_ARRAY_NAME& a)
{
    JZ_NUMBER_ARRAY_NAME ret(a);
    float len = ret.Length();

    if (len > jz::Constants<float>::kZeroTolerance) { ret /= len; }

    return ret;
}

static JZ_NUMBER_ARRAY_NAME Nlerp(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b, float aWeightOfB)
{
    JZ_NUMBER_ARRAY_NAME ret(a);

    if (jz::LessThan(Dot(a, b), 0.0f)) { ret += ((-b - ret) * aWeightOfB); }
    else { ret += ((b - ret) * aWeightOfB); }

    ret = Normalize(ret);

    return ret;
}

static JZ_NUMBER_ARRAY_NAME Slerp(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b, float aWeightOfB)
{
    float dot = Dot(a, b);

    if (jz::AboutEqual(dot,  1.0f, Constants<float>::kSlerpThreshold) ||
        jz::AboutEqual(dot, -1.0f, Constants<float>::kSlerpThreshold))
    {
        return Nlerp(a, b, aWeightOfB);
    }
    else
    {
        dot = jz::Clamp(dot, -1.0f, 1.0f);

        Radian theta0 = ACos(dot);
        Radian theta  = theta0 * aWeightOfB;
        
        JZ_NUMBER_ARRAY_NAME v0 = Normalize(b - (a * dot));
        JZ_NUMBER_ARRAY_NAME ret = ((a * Cos(theta)) + (v0 * Sin(theta)));

        return ret;
    }
}

static Radian SmallestAngle(const JZ_NUMBER_ARRAY_NAME& a, const JZ_NUMBER_ARRAY_NAME& b)
{
    JZ_NUMBER_ARRAY_NAME normA(Normalize(a));
    JZ_NUMBER_ARRAY_NAME normB(Normalize(b));

    Radian ret = jz::ACos(jz::Clamp(Dot(normA, normB), -1.0f, 1.0f));

    return ret;
}

#undef JZ_NUMBER_ARRAY_DIM
#undef JZ_NUMBER_ARRAY_NAME
