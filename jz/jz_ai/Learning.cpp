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

        bool MultipleLinearRegression(const Matrix& Y, const Matrix& X, Matrix& B)
        {
            Matrix Xt  = X.GetTranspose();
            Matrix XtX = Xt * X;
            Matrix XTinv;
            
            if (!XtX.ToInverse(XTinv))
            {
                return false;
            }
            
            B = (XTinv * Xt * Y);
            
            return true;
        }

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

        void Neighbor(const SaState& aIn, SaState& arOut, float aFactor)
        {
            for (size_t i = 0; i < aIn.size(); i++)
            {
                const float k = ((UniformRandomf() * 2.0f) - 1.0f) * aFactor;
                const float t = aIn[i] + k;
                
                if (LessThan(t, 0.0f) || GreaterThan(t, 1.0f))
                {
                    arOut[i] = aIn[i] - k;
                }
                else
                {
                    arOut[i] = t;
                }
            }
        }
        
        float Probability(float aErrorCurrent, float aErrorNext, float aTemperature)
        {
            if (aErrorNext < aErrorCurrent)
            {
                return 1.0f;
            }
            else
            {
                return exp((aErrorCurrent - aErrorNext) / aTemperature);
            }
        }

        float Gradient(const float* apTerms, natural aTermCount, float aTarget, float* apCoefficients)
        {
            float o = 0.0f;
            
            for (natural i = 0; i < aTermCount; i++)
            {
                o += apTerms[i] * apCoefficients[i];
            }
            
            const float kDelta = (aTarget - o);

            return kDelta;
        }

        float StochasticGradientDescent(const float* apTerms, natural aTermCount, float aTarget, float* apCoefficients, float aLearningRate)
        {
            const float kDelta = Gradient(apTerms, aTermCount, aTarget, apCoefficients);
            const float kStep  = aLearningRate * kDelta;
            
            for (natural i = 0; i < aTermCount; i++)
            {
                apCoefficients[i] += kStep * apTerms[i];
            }
            
            return (0.5f * (kDelta * kDelta));
        }

        float GradientDescent(float aDelta, float* apCoefficients, natural aCoeffCount, float aLearningRate)
        {
            const float kStep = aLearningRate * aDelta;

            for (natural i = 0; i < aCoeffCount; i++)
            {
                apCoefficients[i] += kStep;
            }

            return (0.5f * (aDelta * aDelta));
        }

    }
}
