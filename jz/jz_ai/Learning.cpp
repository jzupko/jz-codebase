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

#include <jz_ai/Learning.h>
#include <jz_core/Color.h>
#include <jz_core/Math.h>
#include <jz_core/Utility.h>

#include <fstream>

namespace jz
{
    namespace ai
    {

        void Anneal(const SaState& aIn, SaState& arOut, EvalFunction aEvalFunc, const SaSettings& aSettings)
        {
            SaState current = aIn;
            SaState best    = aIn;
            SaState next(current.size());
                    
            float errorCurrent = aEvalFunc(current);
            float errorBest    = aEvalFunc(best);
            float errorNext    = 0;

            for (natural i = 0; i < aSettings.MaxIterations; i++)
            {
                Neighbor(current, next, aSettings.Factor);
                errorNext = aEvalFunc(next);

                if (errorNext < errorBest)
                {
                    errorBest = errorNext;
                    best      = next;
                }                

                if (errorBest < aSettings.Threshold)
                {
                    break;
                }

                if (UniformRandomf() < Probability(errorCurrent, errorNext, Temperature(i, aSettings.MaxIterations)))                
                {
                    errorCurrent = errorNext;
                    current      = next;
                }
            }

            arOut = best;
        }

    }
}
