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

#ifndef JZ_ANGLE_NAME
#   error "Must define JZ_ANGLE_NAME before including this file."
#endif

public:
    JZ_ANGLE_NAME()
        : mValue(0)
    {}

    explicit JZ_ANGLE_NAME(float a)
        : mValue(a)
    {}

    float GetValue() const
    {
        return mValue;
    }

    void SetValue(float a)
    {
        mValue = a;
    }

    JZ_ANGLE_NAME& operator=(const JZ_ANGLE_NAME& a)
    {
        mValue = a.mValue;
        
        return *this;
    }

    bool operator==(JZ_ANGLE_NAME a) const
    {
        return (mValue == a.mValue);
    }

    bool operator!=(JZ_ANGLE_NAME a) const
    {
        return (mValue != a.mValue);
    }

    bool operator<(JZ_ANGLE_NAME a) const
    {
        return (mValue < a.mValue);
    }

    bool operator<=(JZ_ANGLE_NAME a) const
    {
        return (mValue <= a.mValue);
    }

    bool operator>(JZ_ANGLE_NAME a) const
    {
        return (mValue > a.mValue);
    }

    bool operator>=(JZ_ANGLE_NAME a) const
    {
        return (mValue >= a.mValue);
    }

    JZ_ANGLE_NAME operator+(JZ_ANGLE_NAME a) const
    {
        return JZ_ANGLE_NAME(mValue + a.mValue);
    }

    JZ_ANGLE_NAME operator-(JZ_ANGLE_NAME a) const
    {
        return JZ_ANGLE_NAME(mValue - a.mValue);
    }

    JZ_ANGLE_NAME operator*(JZ_ANGLE_NAME a) const
    {
        return JZ_ANGLE_NAME(mValue * a.mValue);
    }

    JZ_ANGLE_NAME operator*(float s) const
    {
        return JZ_ANGLE_NAME(mValue * s);
    }

    JZ_ANGLE_NAME operator/(JZ_ANGLE_NAME a) const
    {
        return JZ_ANGLE_NAME(mValue / a.mValue);
    }

    JZ_ANGLE_NAME operator/(float s) const
    {
        return JZ_ANGLE_NAME(mValue / s);
    }

    JZ_ANGLE_NAME operator-() const
    {
        return JZ_ANGLE_NAME(-mValue);
    }

    JZ_ANGLE_NAME& operator+=(JZ_ANGLE_NAME a)
    {
        mValue += a.mValue;
        
        return *this;
    }

    JZ_ANGLE_NAME& operator-=(JZ_ANGLE_NAME a)
    {
        mValue -= a.mValue;
        
        return *this;
    }

    JZ_ANGLE_NAME& operator*=(JZ_ANGLE_NAME a)
    {
        mValue *= a.mValue;
        
        return *this;
    }
    
    JZ_ANGLE_NAME& operator*=(float s)
    {
        mValue *= s;
        
        return *this;
    }

    JZ_ANGLE_NAME& operator/=(JZ_ANGLE_NAME a)
    {
        mValue /= a.mValue;
        
        return *this;
    }

    JZ_ANGLE_NAME& operator/=(float s)
    {
        mValue /= s;
        
        return *this;
    }

    JZ_ANGLE_NAME operator++(int)
    {
        JZ_ANGLE_NAME t(mValue);
        
        mValue++;

        return t;
    }

    JZ_ANGLE_NAME& operator++()
    {
        ++mValue;
        
        return *this;
    }

    JZ_ANGLE_NAME operator--(int)
    {
        JZ_ANGLE_NAME t(mValue);
        
        mValue--;

        return t;
    }

    JZ_ANGLE_NAME& operator--()
    {
        --mValue;
        
        return *this;
    }

private:
    float mValue;

#undef JZ_ANGLE_NAME
