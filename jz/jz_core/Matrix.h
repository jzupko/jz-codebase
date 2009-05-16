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
#ifndef _JZ_MATRIX_H_
#define _JZ_MATRIX_H_

#include <jz_core/Math.h>
#include <jz_core/Memory.h>

namespace jz
{

    class Matrix
    {
    public:
        typedef MemoryBuffer<float>::const_iterator const_iterator;
        typedef MemoryBuffer<float>::const_pointer const_pointer;
        typedef MemoryBuffer<float>::const_reference const_reference;
        typedef MemoryBuffer<float>::iterator iterator;
        typedef MemoryBuffer<float>::pointer pointer;
        typedef MemoryBuffer<float>::reference reference;
        typedef MemoryBuffer<float>::size_type size_type;
        typedef MemoryBuffer<float>::value_type value_type;
                
        Matrix()
        {}
    
        Matrix(float m11, float m12, float m13,
               float m21, float m22, float m23,
               float m31, float m32, float m33)
            : mData(9), mRows(3), mCols(3)
        {
            Matrix& m = (*this);
            
            m(0, 0) = m11; m(0, 1) = m12; m(0, 2) = m13;
            m(1, 0) = m21; m(1, 1) = m22; m(1, 2) = m23;
            m(2, 0) = m31; m(2, 1) = m32; m(2, 2) = m33;
        }

        Matrix(natural aNumRows, natural aNumCols)
            : mData(aNumRows * aNumCols), mRows(aNumRows), mCols(aNumCols)
        {}
        
        Matrix(const Matrix& m)
            : mData(m.mData), mRows(m.mRows), mCols(m.mCols)
        {}
        
        Matrix& operator=(const Matrix& m)
        {
            mData = m.mData;
            mRows = m.mRows;
            mCols = m.mCols;
            
            return *this;
        }
    
        Matrix::~Matrix()
        {}

        const_iterator begin() const
        {
            return mData.begin();
        }

        iterator begin()
        {
            return mData.begin();
        }
        
        const_iterator end() const
        {
            return mData.end();
        }
        
        iterator end()
        {
            return mData.end();
        }

        float operator[](size_t i) const { return mData[i]; }
        float& operator[](size_t i) { return mData[i]; }

        float operator()(natural r, natural c) const
        {
            return mData[I(r, c)];
        }
        
        float& operator()(natural r, natural c)
        {
            return mData[I(r, c)];
        }
        
        Matrix operator*(const Matrix& m) const;

        natural GetCols() const
        {
            return mCols;
        }
        
        natural GetRows() const
        {
            return mRows;
        }
        
        size_type size() const
        {
            return mData.size();
        }

        void resize(natural aNumRows, natural aNumCols)
        {
            mRows = aNumRows;
            mCols = aNumCols;
            mData.resize(aNumRows * aNumCols);
        }

        bool ToInverse(Matrix& arOut) const;

        Matrix GetTranspose() const;           
    
        void MakeZero()
        {
            mData.Initialize();
        }
    
        void SwapRows(natural r1, natural r2);

        Matrix operator-() const
        {
            const Matrix& a = (*this);
            Matrix ret(a.mRows, a.mCols);
            
            size_t count = size();
            for (size_t i = 0u; i < count; i++)
            {
                ret.mData[i] = (-a.mData[i]);
            }

            return ret;
        }

        Matrix operator+(const Matrix& b) const
        {
            const Matrix& a = (*this);

            JZ_ASSERT(a.mRows == b.mRows && a.mCols == b.mCols);
            Matrix ret(a.mRows, a.mCols);
            
            size_t count = size();
            for (size_t i = 0u; i < count; i++)
            {
                ret.mData[i] = (a.mData[i] + b.mData[i]);
            }

            return ret;
        }

        Matrix operator-(const Matrix& b) const
        {
            const Matrix& a = (*this);

            JZ_ASSERT(a.mRows == b.mRows && a.mCols == b.mCols);
            Matrix ret(a.mRows, a.mCols);
            
            size_t count = size();
            for (size_t i = 0u; i < count; i++)
            {
                ret.mData[i] = (a.mData[i] - b.mData[i]);
            }

            return ret;
        }
    
    private:
        // column major storage
        int I(int aRow, int aColumn) const
        {
            return (aColumn * mRows) + aRow;
        }
        
        natural mRows;
        natural mCols;
        
        MemoryBuffer<float> mData;
    };

    static Matrix Abs(const Matrix& m)
    {
        Matrix ret(m.GetRows(), m.GetCols());
        
        size_t count = m.size();
        for (size_t i = 0u; i < count; i++)
        {
            ret[i] = ::jz::Abs(m[i]);
        }

        return ret;        
    }

    bool SolveLinearSystem(const Matrix& A, Matrix& BX);

}

#endif
