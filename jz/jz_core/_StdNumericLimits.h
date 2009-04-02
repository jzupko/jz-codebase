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

#if !defined(JZ_HELPER_TYPE) || !defined(JZ_HELPER_BASETYPE)
#   error "Please define all necessary macros before including this file."
#endif

#if defined(JZ_HELPER) || defined(JZ_HELPER2)
#   error "Naming conflict. Please resolve."
#endif

#define JZ_HELPER(type, name) static const type name = std::numeric_limits<JZ_HELPER_BASETYPE>::name
#define JZ_HELPER2(function) static JZ_HELPER_TYPE function() { return JZ_HELPER_TYPE(numeric_limits<JZ_HELPER_BASETYPE>::function()); }

namespace std
{

    template <>
    class numeric_limits<JZ_HELPER_TYPE>
    {
        public:
	        JZ_HELPER(float_denorm_style, has_denorm);
	        JZ_HELPER(bool, has_denorm_loss);
	        JZ_HELPER(bool, has_infinity);
	        JZ_HELPER(bool, has_quiet_NaN);
	        JZ_HELPER(bool, has_signaling_NaN);
	        JZ_HELPER(bool, is_bounded);
	        JZ_HELPER(bool, is_exact);
	        JZ_HELPER(bool, is_iec559);
	        JZ_HELPER(bool, is_integer);
	        JZ_HELPER(bool, is_modulo);
	        JZ_HELPER(bool, is_signed);
	        JZ_HELPER(bool, is_specialized);
	        JZ_HELPER(bool, tinyness_before);
	        JZ_HELPER(bool, traps);
	        JZ_HELPER(float_round_style, round_style);
	        JZ_HELPER(int, radix);
	        JZ_HELPER(int, digits);
	        JZ_HELPER(int, digits10);
	        JZ_HELPER(int, max_exponent);
	        JZ_HELPER(int, max_exponent10);
	        JZ_HELPER(int, min_exponent);
	        JZ_HELPER(int, min_exponent10);
            JZ_HELPER2(min);
            JZ_HELPER2(max);
            JZ_HELPER2(epsilon);
            JZ_HELPER2(round_error);
            JZ_HELPER2(denorm_min);
            JZ_HELPER2(infinity);
            JZ_HELPER2(quiet_NaN);
            JZ_HELPER2(signaling_NaN);
    };
    
}

#undef JZ_HELPER2
#undef JZ_HELPER
#undef JZ_HELPER_BASETYPE
#undef JZ_HELPER_TYPE
