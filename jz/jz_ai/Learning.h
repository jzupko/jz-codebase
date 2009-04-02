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
#ifndef _JZ_AI_LEARNING_H_
#define _JZ_AI_LEARNING_H_

#include <jz_core/Prereqs.h>
#include <jz_core/Matrix.h>
#include <vector>

namespace jz
{
    namespace ai
    {

        template <natural I, natural H, natural O>
        class NeuralNetwork
        {
            public:
                typedef float (*LearningFunction)(float s);
                
                static float Linear(float s)
                {
                    return s;
                }
                
                static float Sigmoid(float s)
                {
                    return 1.0f / (1.0f + exp(-s));
                }

                struct NNData
                {
                    float In[I];
                    float Hid[H];
                    float Out[O];

                    float InHid[I][H];
                    float HidOut[H][O];
                } Data;

                float LearningRate;
                
                LearningFunction Function;
                
                NeuralNetwork(float aLearningRate = 0.2f, LearningFunction aFunction = Sigmoid)
                    : LearningRate(aLearningRate), Function(aFunction)
                {
                    Initialize();
                }

                ~NeuralNetwork()
                {}

                void Initialize()
                {
                    memset(reinterpret_cast<void*>(&Data), 0, sizeof(Data));

                    Randomize(reinterpret_cast<float*>(Data.InHid), I * H);
                    Randomize(reinterpret_cast<float*>(Data.HidOut), H * O);
                }

                float GetMeanSquareError(float* apIn, float* apOut)
                {
                    memcpy(Data.In, (u8*)apIn, I * sizeof(float));
                    Propagate();
                    
                    float mse = 0.0f;
                    
                    for (natural i = 0; i < O; i++)
                    {
                        const float t = Data.Out[i] - apOut[i];
                        
                        mse += (t * t);
                    }
                    
                    return (mse / 2.0f);
                }

                void Propagate()
                {
                    for (natural i = 0; i < H; i++)
                    {
                        float sum(0);

                        for (natural j = 0; j < I; j++)
                        {
                            sum += Data.In[j] * Data.InHid[j][i];
                        }
                        
                        Data.Hid[i] = Function(sum);
                    }

                    for (natural i = 0; i < O; i++)
                    {
                        float sum(0);

                        for (natural j = 0; j < H; j++)
                        {
                            sum += Data.Hid[j] * Data.HidOut[j][i];
                        }

                        Data.Out[i] = Function(sum);
                    }
                }

                void Train(float* apIn, float* apOut)
                {
                    memcpy(Data.In, (u8*)apIn, I * sizeof(float));

                    Propagate();
                    BackPropagate(apOut);
                }

            private:
                void BackPropagate(float* t)
                {
                    float hidError[H];
                    float outError[O];

                    for (natural i = 0; i < O; i++)
                    {
                        outError[i] = Data.Out[i] * (1.0f - Data.Out[i]) * (t[i] - Data.Out[i]);
                    }

                    for (natural i = 0; i < H; i++)
                    {
                        float sum(0);

                        for (natural j = 0; j < O; j++)
                        {
                            sum += outError[j] * Data.HidOut[i][j];
                        }

                        hidError[i] = Data.Hid[i] + (1.0f - Data.Hid[i]) * sum;
                    }

                    for (natural i = 0; i < H; i++)
                    {
                        for (natural j = 0; j < O; j++)
                        {
                            Data.HidOut[i][j] += LearningRate * outError[j] * Data.Out[j];
                        }
                    }

                    for (natural i = 0; i < I; i++)
                    {
                        for (natural j = 0; j < H; j++)
                        {
                            Data.InHid[i][j] += LearningRate * hidError[j] * Data.Hid[j];
                        }
                    }
                }

                void Randomize(float* p, natural s)
                {
                    for (natural i = 0; i < s; i++)
                    {
                        p[i] = UniformRandomf() - 0.5f;
                    }
                }
        };

        template <typename T, typename U>
        void SimpleCubicRegression(const T& aResponses, const U& aPredictions, float& arIntercept, float& arLinearSlope, float& arQuadraticSlope, float& arCubicSlope)
        {
            typedef typename T::const_iterator ConstItrA;
            typedef typename U::const_iterator ConstItrB;

            ConstItrA R = aResponses.begin();
            ConstItrB P = aPredictions.begin();

            float sumX2(0);
            float sumX3(0);
            float sumX4(0);
            float sumX5(0);
            float sumX6(0);
            float sumX3Y(0);
            float sumX2Y(0);

            size_t n = 0;

            for (; P != aPredictions.end(); P++, R++)
            {
                const float x2((*P) * (*P));
                const float x3((*P) * x2);
                const float x4(x2 * x2);
                const float x5(x2 * x3);
                const float x6(x3 * x3);
                const float x3y(x3 * (*R));
                const float x2y(x2 * (*R));

                sumX2 += x2;
                sumX3 += x3;
                sumX4 += x4;
                sumX5 += x5;
                sumX6 += x6;
                sumX3Y += x3y;
                sumX2Y += x2y;

                n++;
            }

            if (n > 1)
            {
                Matrix A(4, 4);
                Matrix X(4, 1);
                Matrix Ainv(4, 4);

                const float x0 = aPredictions.front();
                const float x02 = x0 * x0;
                const float x03 = x02 * x0;
                const float xn = aPredictions.back();
                const float xn2 = xn * xn;
                const float xn3 = xn2 * xn;
                const float y0 = aResponses.front();
                const float yn = aResponses.back();

                A(0,0) = sumX6; A(0,1) = sumX5; A(0,2) = sumX4; A(0,3) = sumX3;
                A(1,0) = sumX5; A(1,1) = sumX4; A(1,2) = sumX3; A(1,3) = sumX2;
                A(2,0) = x03;   A(2,1) = x02;   A(2,2) = x0;    A(2,3) = 1.0f;
                A(3,0) = xn3;   A(3,1) = xn2;   A(3,2) = xn;    A(3,3) = 1.0f;
                
                X(0,0) = sumX3Y;
                X(1,0) = sumX2Y;
                X(2,0) = y0;
                X(3,0) = yn;

                if (A.ToInverse(Ainv))
                {
                    Matrix B = Ainv * X;

                    arIntercept = B(3,0);
                    arLinearSlope = B(2,0);
                    arQuadraticSlope = B(1,0);
                    arCubicSlope = B(0,0);

                    return;
                }
            }

            arIntercept = 0;
            arLinearSlope = 0;
            arQuadraticSlope = 0;
            arCubicSlope = 0;
        }

        template <typename T, typename U>
        void SimpleLinearRegression(const T& aResponses, const U& aPredictions, float& arIntercept, float& arSlope)
        {
            typedef typename T::const_iterator ConstItrA;
            typedef typename U::const_iterator ConstItrB;
            
            ConstItrA R = aResponses.begin();
            ConstItrB P = aPredictions.begin();
            
            float sumX(0);
            float sumY(0);
            float sumXY(0);
            float sumX2(0);

            size_t n = 0;
            
            for (; P != aPredictions.end(); P++, R++)
            {
                sumX  += *P;
                sumY  += *R;
                sumXY += (*P * *R);
                sumX2 += (*P * *P);
                n++;
            }
            
            arSlope     = ((float(n) * sumXY) - (sumX * sumY)) / ((float(n) * sumX2) - (sumX * sumX));
            arIntercept = (sumY - (arSlope * sumX)) / float(n);
        }
        
        inline bool MultipleLinearRegression(const Matrix& Y, const Matrix& X, Matrix& B)
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
        
        template <typename T, typename U, typename V>
        bool Regression(const T& aResponses, const U& aPredictions, V& arCoefficients)
        {
            natural predictionCount = (natural)(aPredictions.size() / aResponses.size());        
            
            if (predictionCount == 1)
            {
                float intercept = 0;
                float slope     = 0;
                
                SimpleLinearRegression(aResponses, aPredictions, intercept, slope);
                
                arCoefficients[0] = intercept;
                arCoefficients[1] = slope;
            }
            else
            {
                Matrix Y((natural)aResponses.size(), 1);
                Matrix X((natural)aResponses.size(), predictionCount + 1);
                Matrix B(predictionCount + 1, 1);
                
                for (natural i = 0; i < Y.GetRows(); i++)
                {
                    Y(i, 0) = aResponses[i];
                    X(i, 0) = 1.0f;
                    
                    for (natural j = 1; j < predictionCount + 1; j++)
                    {
                        const natural kIndex = (i * predictionCount) + (j-1);
                        
                        X(i, j) = aPredictions[kIndex];
                    }
                }
                
                if (!MultipleLinearRegression(Y, X, B))
                {
                    return false;
                }
                
                for (natural i = 0; i < B.GetRows(); i++)
                {
                    arCoefficients[i] = B(i,0);
                }
            }
            
            return true;
        }

        template <typename T, typename U, typename V>
        float CoefficientOfDetermination(const T& aResponses, const U& aPredictions, const V& aCoefficients)
        {
            float totalSS(Variance(aResponses, Mean(aResponses)));
            
            MemoryBuffer<float> regressions(aResponses.size());

            for (natural i = 0; i < (natural)aResponses.size(); i++)
            {
                float entry(aCoefficients[0]);
                
                for (natural j = 1; j < (natural)aCoefficients.size(); j++)
                {
                    const natural kIndex = (i * (natural)(aCoefficients.size() - 1)) + (j-1);
                    
                    entry += aCoefficients[j] * aPredictions[kIndex];
                }
                
                regressions[i] = entry;
            }
            
            float regreSS(Variance(regressions, Mean(regressions)));
            
            return (regreSS / totalSS);
        }

        struct SaSettings
        {
            SaSettings(float aFactor = 0.1f, natural aMaxIterations = 100, float aThreshold = 0.001f)
                : Factor(aFactor), MaxIterations(aMaxIterations), Threshold(aThreshold)
            {}
            
            float   Factor;
            natural MaxIterations;
            float   Threshold;
        };
        
        typedef vector<float> SaState;        
        typedef float (*EvalFunction)(const SaState& aState);

        void Anneal(const SaState& aIn, SaState& arOut, EvalFunction aEvalFunc, const SaSettings& aSettings = SaSettings());
        
        inline void Neighbor(const SaState& aIn, SaState& arOut, float aFactor)
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
        
        inline float Probability(float aErrorCurrent, float aErrorNext, float aTemperature)
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
     
        inline float Temperature(natural aIteration, natural aMaxIterations)
        {
            return exp((float)aMaxIterations - float(aIteration) * 1.25f);
        }

        inline float Gradient(const float* apTerms, natural aTermCount, float aTarget, float* apCoefficients)
        {
            float o = 0.0f;
            
            for (natural i = 0; i < aTermCount; i++)
            {
                o += apTerms[i] * apCoefficients[i];
            }
            
            const float kDelta = (aTarget - o);

            return kDelta;
        }

        inline float StochasticGradientDescent(const float* apTerms, natural aTermCount, float aTarget, float* apCoefficients, float aLearningRate = 0.01f)
        {
            const float kDelta = Gradient(apTerms, aTermCount, aTarget, apCoefficients);
            const float kStep  = aLearningRate * kDelta;
            
            for (natural i = 0; i < aTermCount; i++)
            {
                apCoefficients[i] += kStep * apTerms[i];
            }
            
            return (0.5f * (kDelta * kDelta));
        }

        inline float GradientDescent(float aDelta, float* apCoefficients, natural aCoeffCount, float aLearningRate = 0.01f)
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

#endif
