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

#include <jz_core/Angle.h>

namespace jz
{

    const Degree Degree::kZero = Degree(0);
    const Degree Degree::k45 = Degree(45);
    const Degree Degree::k90 = Degree(90);
    const Degree Degree::k135 = Degree(135);
    const Degree Degree::k180 = Degree(180);
    const Degree Degree::k225 = Degree(225);
    const Degree Degree::k260 = Degree(260);
    const Degree Degree::k315 = Degree(315);
    const Degree Degree::k360 = Degree(360);

    const Radian Radian::kPi = Radian(Constants<float>::kPi);
    const Radian Radian::kPiOver2 = Radian(Constants<float>::kPiOver2);
    const Radian Radian::kPiOver4 = Radian(Constants<float>::kPiOver4);
    const Radian Radian::k3PiOver2 = Radian(Constants<float>::k3PiOver2);
    const Radian Radian::kTwoPi = Radian(Constants<float>::kTwoPi);
    const Radian Radian::kZero = Radian(0);

}