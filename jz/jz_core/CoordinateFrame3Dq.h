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

#pragma once
#ifndef _JZ_COORDINATE_FRAME_3DQ_H_
#define _JZ_COORDINATE_FRAME_3DQ_H_

#include <jz_core/Quaternion.h>
#include <jz_core/Vector3.h>

namespace jz
{
    struct Matrix4;
    struct CoordinateFrame3Dq
    {
        static const CoordinateFrame3Dq kIdentity;
        static const CoordinateFrame3Dq kZero;

        Quaternion Orientation;
        Vector3 Translation;

        CoordinateFrame3Dq()
            : Orientation(Quaternion::kIdentity), Translation(Vector3::kZero)
        {}

        CoordinateFrame3Dq(const Quaternion& aOrientation, const Vector3& aTranslation)
            : Orientation(aOrientation), Translation(aTranslation)
        { }

        static CoordinateFrame3Dq Invert(const CoordinateFrame3Dq& aFrame)
        {
            CoordinateFrame3Dq ret;
            ret.Orientation = Quaternion::Invert(aFrame.Orientation);
            ret.Translation = Vector3::Transform(ret.Orientation, -aFrame.Translation);

            return ret;
        }

        static CoordinateFrame3Dq Lerp(const CoordinateFrame3Dq& a, const CoordinateFrame3Dq& b, float aWeightOfB)
        {
            CoordinateFrame3Dq ret;
#           if JZ_USE_SLERP
                ret.Orientation = Quaternion::Slerp(a.Orientation, b.Orientation, aWeightOfB);
#           else
                ret.Orientation = Quaternion::Nlerp(a.Orientation, b.Orientation, aWeightOfB);
#           endif
            ret.Translation = Vector3::Lerp(a.Translation, b.Translation, aWeightOfB);

            return ret;
        }

        static CoordinateFrame3Dq CreateFromMatrix4(const Matrix4& m);
    };

    __inline CoordinateFrame3Dq operator*(const CoordinateFrame3Dq& a, const CoordinateFrame3Dq& b)
    {
        CoordinateFrame3Dq ret;
        // Note: quaternions need to be multiplied right-to-left when the expected order would be
        //       left-to-right in this codebase.
        ret.Orientation = (b.Orientation * a.Orientation);
        ret.Translation = Vector3::Transform(b.Orientation, a.Translation) + b.Translation;

        return ret;
    }

    void FromTransform(const Matrix4& a, CoordinateFrame3Dq& b);
    void ToTransform(const CoordinateFrame3Dq& a, Matrix4& b);

}

#endif
