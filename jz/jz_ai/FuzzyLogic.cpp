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

#include <jz_ai/FuzzyLogic.h>

namespace jz
{
    namespace ai
    {

        float TriangleDefuzzify::operator()(float v) const
        {
            if      (v < mP1)            return 0.0f;
            else if (v > mP3)            return 0.0f;
            else if (AboutEqual(v, mP2)) return 1.0f;
            else if (AboutEqual(v, mP1)) return 0.0f;
            else if (AboutEqual(v, mP3)) return 0.0f;
            else if (v < mP2)            return (v - mP1) / (mP2 - mP1);
            else                         return (mP3 - v) / (mP3 - mP2);        
        }
        
        float TriangleDefuzzify::GetCenterOfMass(float aAA) const
        {
            if (AboutEqual(aAA, 1.0f) || aAA > 1.0f)
            {
                return (mP1 + mP2 + mP3) / 3.0f;
            }
            else if (AboutZero(aAA) || aAA < 0.0f)
            {
                return Constants<float>::kNaN;
            }
            else
            {
                // calculate two new points of the trapezoid.
                const float kp4(mP1 + (aAA * (mP2 - mP1)));
                const float kp5(mP2 + ((1.0f - aAA) * (mP3 - mP2)));

                float a(kp5 - kp4);
                float b(mP3 - mP1);
                float c(kp4 - mP1);

                float num = (2.0f * a * c) + (a * a) + (c * b) + (a * b) + (b * b);
                float den = 3.0f * (a + b);
                
                return mP1 + (num / den);
            }
        }

        float TriangleDefuzzify::GetMass(float aAA) const
        {
            if (AboutEqual(aAA, 1.0f) || aAA > 1.0f)
            {
                return (mP3 - mP1) / 2.0f;
            }
            else if (AboutZero(aAA) || aAA < 0.0f)
            {
                return 0.0f;
            }
            else
            {
                // calculate two new points of the trapezoid.
                const float kp4(mP1 + (aAA * (mP2 - mP1)));
                const float kp5(mP2 + ((1.0f - aAA) * (mP3 - mP2)));

                return (((kp5 - kp4) + (mP3 - mP1)) / 2.0f) * aAA;
            }
        }

        FuzzyTerm::operator bool() const
        {
            if (mMembership > Constants<float>::kZeroTolerance)
            {
                gsLastAggregation = mMembership;
            }
            else
            {
                return false;
            }

            return true;
        }   

        void FuzzyVariable::Reset()
        {
            for (FuzzyTerms::iterator I = mTerms.begin(); I != mTerms.end(); I++)
            {
                I->Set(0);
            }
            
            mbDirty = true;
            mbValid = false;
            mValue  = Constants<float>::kNaN;
        }        

        void FuzzyVariable::Set(float v)
        {
            mValue  = v;
            mbDirty = false;

            for (FuzzyTerms::iterator I = mTerms.begin(); I != mTerms.end(); I++)
            {
                I->Update(v);
            }
        }

        // using CoG    
        void FuzzyVariable::DeFuzzify()
        {
            float totalMass(0);
            float firstMoment(0);

            for (FuzzyTerms::const_iterator I = mTerms.begin(); I != mTerms.end(); I++)
            {
                if (!AboutZero(I->Get()))
                {
                    float mass = I->GetMass();

                    firstMoment += I->GetCenterOfMass() * mass;
                    totalMass   += mass;
                }
            }

            mbDirty = false;

            if (!AboutZero(totalMass))
            {
                mbValid = true;
                mValue = firstMoment / totalMass;
            }
            else
            {
                mbValid = false;
                mValue  = Constants<float>::kNaN;               
            } 
        }

    }
}
