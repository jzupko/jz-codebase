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
#ifndef _JZ_AI_FUZZY_LOGIC_H_
#define _JZ_AI_FUZZY_LOGIC_H_

#include <jz_core/Math.h>
#include <jz_core/Prereqs.h>
#include <vector>

namespace jz
{
    namespace ai
    {

        static float gsLastAggregation = 0.0f;

        class TriangleDefuzzify
        {
            public:
                TriangleDefuzzify(float p1 = 0, float p2 = 0, float p3 = 0)
                    : mP1(p1), mP2(p2), mP3(p3)
                {}

                ~TriangleDefuzzify()
                {}

                float operator()(float v) const;
                float GetCenterOfMass(float aAA) const;
                float GetMass(float aAA) const;

                void Set(float p1, float p2, float p3)
                {
                    mP1 = p1;
                    mP2 = p2;
                    mP3 = p3;
                }

            private:
                float mP1, mP2, mP3;
        };

        class FuzzyTerm
        {
            public:
                FuzzyTerm(float aMembership = 0.0f) 
                    : mMembership(aMembership)
                {}
                
                ~FuzzyTerm()
                {}

                // using MinMax
                FuzzyTerm operator&&(const FuzzyTerm& b) const
                {
                    return FuzzyTerm(Min(mMembership, b.mMembership));
                }

                operator bool() const;

                float Get() const
                {
                    return mMembership;
                }
                
                float GetCenterOfMass() const
                {
                    return mFunc.GetCenterOfMass(mMembership);
                }

                float GetMass() const
                {
                    return mFunc.GetMass(mMembership);
                }
                
                // using MinMax
                void Is()
                {
                    mMembership = Max(mMembership, gsLastAggregation);
                }
                
                FuzzyTerm operator!() const
                {
                    return FuzzyTerm(float(1) - mMembership);
                }

                // using MinMax
                FuzzyTerm operator||(const FuzzyTerm& b) const
                {
                    return FuzzyTerm(Max(mMembership, b.mMembership));
                }

                void Set(float v)
                {
                    mMembership = v;
                }

                void SetTriangle(float p1, float p2, float p3)
                {
                    mFunc.Set(p1, p2, p3);
                }

                void Update(const float& v)
                {
                    mMembership = mFunc(v);
                }

            private:
                TriangleDefuzzify mFunc;
                float             mMembership;
        };

        class FuzzyVariable
        {
            public:
                typedef vector<FuzzyTerm> FuzzyTerms;
                
                FuzzyVariable()
                    : mbDirty(true), mbValid(false)
                {}
                
                ~FuzzyVariable()
                {}

                const FuzzyTerm& operator[](natural i) const
                {
                    return mTerms[i];
                }

                FuzzyTerm& operator[](natural i)
                {
                    return mTerms[i];
                }

                float Get() const
                {
                    if (mbDirty)
                    {
                        const_cast<FuzzyVariable*>(this)->DeFuzzify();
                    }
                    
                    return mValue;
                }

                bool IsValid() const
                {
                    if (mbDirty)
                    {
                        const_cast<FuzzyVariable*>(this)->DeFuzzify();
                    }

                    return mbValid;
                }

                void Reset();
                void Set(float v);
                
                void SetTermsCount(natural i)
                {
                    mTerms.resize(i);
                }

            private:
                bool       mbDirty;
                bool       mbValid;
                FuzzyTerms mTerms;
                float      mValue;

                void DeFuzzify();
        };

    }
}

#endif
