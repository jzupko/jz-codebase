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

#include <jz_core/Matrix.h>
#include <stdexcept>

namespace jz
{

    bool Matrix::ToInverse(Matrix& arOut) const
    {
        // matrix must be square to invert.
        if (!(mRows > 0) || (mRows != mCols))
        {
            return false;
        }

        const natural kSize = mRows;
        arOut = *this;

        MemoryBuffer<bool> bPivoted(kSize);
        bPivoted.Initialize();
        
        MemoryBuffer<natural> cols(kSize);
        MemoryBuffer<natural> rows(kSize);

        for (natural i = 0; i < kSize; i++)
        {
            float max(Constants<float>::kMin);
            natural r(0);
            natural c(0);

            for (natural j = 0; j < kSize; j++)
            {
                if (!bPivoted[j])
                {
                    for (natural k = 0; k < kSize; k++)
                    {
                        if (!bPivoted[k])
                        {
                            float abs = Abs(arOut(j, k));

                            if (GreaterThan(abs, max))
                            {
                                max = abs;
                                r   = j;
                                c   = k;
                            }
                        }
                    }
                }
            }
            
            if (AboutZero(max))
            {
                // singular.
                return false;
            }
            
            bPivoted[c] = true;

            if (r != c)
            {
                arOut.SwapRows(r, c);
            }

            rows[i] = r;
            cols[i] = c;

            float inv   = 1.0f / arOut(c, c);
            arOut(c, c) = 1.0f;
            
            for (natural j = 0; j < kSize; j++)
            {
                arOut(c, j) *= inv;
            }

            for (natural j = 0; j < kSize; j++)
            {
                if (j != c)
                {
                    float a     = arOut(j, c);
                    arOut(j, c) = 0.0f;
                    
                    for (natural k = 0; k < kSize; k++)
                    {
                        arOut(j, k) -= (arOut(c, k) * a);
                    }
                }
            }
        }

        for (natural i = kSize - 1; i >= 0; i--)
        {
            if (rows[i] != cols[i])
            {
                for (natural j = 0; j < kSize; j++)
                {
                    float a = arOut(j, rows[i]);

                    arOut(j, rows[i]) = arOut(j, cols[i]);
                    arOut(j, cols[i]) = a;
                }
            }
        }

        return true;
    }

    Matrix Matrix::GetTranspose() const
    {
        Matrix ret(mCols, mRows);
        
        for (natural r = 0; r < mRows; r++)
        {
            for (natural c = 0; c < mCols; c++)
            {
                ret(c, r) = mData[I(r, c)];
            }
        }

        return ret;
    }

    Matrix Matrix::operator*(const Matrix& m) const
    {
        if (mCols != m.mRows)
        {
            throw std::exception(__FUNCTION__ ": incompatible matrix dimensions.");
        }

        Matrix ret(mRows, m.mCols);
        ret.MakeZero();
        
        for (natural c2 = 0; c2 < m.mCols; c2++)
        {
            for (natural r = 0; r < mRows; r++)
            {
                for (natural c = 0; c < mCols; c++)
                {
                    ret(r, c2) += mData[I(r, c)] * m(c, c2);
                }
            }
        }

        return ret;
    }

    void Matrix::SwapRows(natural r1, natural r2)
    {
        if (r1 >= mRows || r2 >= mRows)
        {
            throw std::out_of_range(__FUNCTION__);
        }
        
        for (natural c = 0; c < mCols; c++)
        {
            Swap(mData[I(r1, c)], mData[I(r2, c)]);
        }
    }

    bool SolveLinearSystem(const Matrix& A, Matrix& BX)
    {
        if (A.GetRows() != A.GetCols() || A.GetRows() <= 0)
        {
            return false;
        }
        
        const natural kSize = A.GetCols();
        
        Matrix M(A);
        Matrix N(kSize, 1);

        for (natural i = 0; i < kSize; i++)
        {
            float max = 0.0f;
            
            for (natural j = 0; j < kSize; j++)
            {
                float value = Abs(M(i,j));
                
                if (GreaterThan(value, max))
                {
                    max = value;
                }
            }
            
            if (AboutZero(max))
            {
                return false;
            }
            else
            {
                N(i, 0) = 1.0f / max;
            }
        }
        
        for (natural j = 0; j < kSize - 1; j++)
        {
            natural row = -1;
            float   max = 0.0f;

            for (natural i = j; i < kSize; i++)
            {
                float value = Abs(M(i, j)) * N(i, 0);

                if (GreaterThan(value, max))
                {
                    max = value;
                    row = i;
                }
            }

            if (row != j)
            {            
                if (row == -1)
                {
                    return false;
                }

                for (natural k = 0; k < kSize; k++)
                {
                    Swap(M(j, k), M(row, k));
                }
                
                Swap(BX(j, 0), BX(row, 0));
                Swap(N(j,0), N(row,0));
            }
            
            const float kDenominator = 1.0f / M(j, j);

            for (natural i = j+1; i < kSize; i++)
            {
                const float kFactor = M(i, j) * kDenominator;

                BX(i,0) -= BX(j,0) * kFactor;

                for (natural k = 0; k < kSize; k++)
                {
                    M(i, k) -= M(j, k) * kFactor;
                }
            }
        }

        for (natural i = kSize - 1; i >= 0; i--)
        {        
            float sum = BX(i, 0);
            
            for (natural j = i+1; j < kSize; j++)
            {
                sum -= M(i, j) * BX(j, 0);
            }
            
            BX(i,0) = sum / M(i, i);
        }

        return true;
    }

}
