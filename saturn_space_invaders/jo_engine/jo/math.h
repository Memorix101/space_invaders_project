/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2016, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/** @file math.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine Math tools
 *  @bug No known bugs.
 */

#ifndef __JO_MATH_H__
# define __JO_MATH_H__

/** @brief Sinus lookup table (internal engine usage) use jo_sin() instead
 *  @warning MC Hammer: don't touch this
 */
extern int JoSinLookupTable[360];

/** @brief Cosinus lookup table (internal engine usage) use jo_cos() instead
 *  @warning MC Hammer: don't touch this
 */
extern int JoCosLookupTable[360];

/** @brief Set a variable to zero
 *  @remark faster than X = 0
 *  @param X Variable name
 */
# define JO_ZERO(X)					X ^= X
/** @brief Fixed floating point value for 0 */
# define JO_FIXED_0					(0)
/** @brief Fixed floating point value for 1 */
# define JO_FIXED_1					(65536)
/** @brief Fixed floating point value for 120 */
# define JO_FIXED_120				(7864320)
/** @brief Fixed floating point value for 150 */
# define JO_FIXED_150				(9830400)

/** @brief Float minimum positive value */
# define JO_FLOAT_EPSILON           (0.00001f)

/** @brief PI value */
# define JO_PI                      (3.1415927)

/** @brief PI/2 value */
# define JO_PI_2                    (1.5707963)

/** @brief Multiply a variable by 2
 *  @remark faster than X * 2
 *  @param X Variable name
 */
# define JO_MULT_BY_2(X)			((X) << 1)
/** @brief Multiply a variable by 4
 *  @remark faster than X * 4
 *  @param X Variable name
 */
# define JO_MULT_BY_4(X)			((X) << 2)
/** @brief Multiply a variable by 8
 *  @remark faster than X * 8
 *  @param X Variable name
 */
# define JO_MULT_BY_8(X)			((X) << 3)
/** @brief Multiply a variable by 16
 *  @remark faster than X * 16
 *  @param X Variable name
 */
# define JO_MULT_BY_16(X)			((X) << 4)
/** @brief Multiply a variable by 32
 *  @remark faster than X * 32
 *  @param X Variable name
 */
# define JO_MULT_BY_32(X)			((X) << 5)

/** @brief Multiply a variable by 256
 *  @remark faster than X * 256
 *  @param X Variable name
 */
# define JO_MULT_BY_256(X)			((X) << 8)

/** @brief Multiply a variable by 1024
 *  @remark faster than X * 1024
 *  @param X Variable name
 */
# define JO_MULT_BY_1024(X)			((X) << 10)

/** @brief Multiply a variable by 32768
 *  @remark faster than X * 32768
 *  @param X Variable name
 */
# define JO_MULT_BY_32768(X)			((X) << 15)

/** @brief Multiply a variable by 65536
 *  @remark faster than X * 65536
 *  @param X Variable name
 */
# define JO_MULT_BY_65536(X)			((X) << 16)

/** @brief Devide a variable by 2
 *  @remark faster than X / 2
 *  @param X Variable name
 */
# define JO_DIV_BY_2(X)				((X) >> 1)
/** @brief Devide a variable by 4
 *  @remark faster than X / 4
 *  @param X Variable name
 */
# define JO_DIV_BY_4(X)				((X) >> 2)
/** @brief Devide a variable by 8
 *  @remark faster than X / 8
 *  @param X Variable name
 */
# define JO_DIV_BY_8(X)				((X) >> 3)
/** @brief Devide a variable by 16
 *  @remark faster than X / 16
 *  @param X Variable name
 */
# define JO_DIV_BY_16(X)			((X) >> 4)
/** @brief Devide a variable by 32
 *  @remark faster than X / 32
 *  @param X Variable name
 */
# define JO_DIV_BY_32(X)			((X) >> 5)
/** @brief Devide a variable by 1024
 *  @remark faster than X / 1024
 *  @param X Variable name
 */
# define JO_DIV_BY_1024(X)			((X) >> 10)

/** @brief Devide a variable by 32768
 *  @remark faster than X / 32768
 *  @param X Variable name
 */
# define JO_DIV_BY_32768(X)			((X) >> 15)

/** @brief Devide a variable by 65536
 *  @remark faster than X / 65536
 *  @param X Variable name
 */
# define JO_DIV_BY_65536(X)			((X) >> 16)

/** @brief Get the absolute value of X
 *  @param X Variable name
 */
# define JO_ABS(X)                  ((X) < 0 ? -(X) : (X))

/** @brief Get the absolute value of X
 *  @param X Variable name
 */
# define JO_FABS(X)                  ((X) < 0.0f ? -(X) : (X))

/** @brief Get the smaller value between A and B
 *  @param A First operand
 *  @param B Second operand
 */
# define JO_MIN(A, B)               (((A) < (B)) ? (A) : (B))

/** @brief Get the bigger value between A and B
 *  @param A First operand
 *  @param B Second operand
 */
# define JO_MAX(A, B)               (((A) > (B)) ? (A) : (B))

/** @brief Change the sign of X
 *  @param X Variable name
 */
# define JO_CHANGE_SIGN(X)          (-(X))

/** @brief Check if A and B is nearly the same value
 *  @param A First operand
 *  @param B Second operand
 */
# define JO_FLOAT_NEARLY_EQUALS(A, B)     ((A) <= ((B) + JO_FLOAT_EPSILON) && (A) >= ((B) - JO_FLOAT_EPSILON))

/** @brief Check if A nearly equals zero
 *  @param A operand
 */
# define JO_IS_FLOAT_NULL(A)              ((A) <= JO_FLOAT_EPSILON && (A) >= (-JO_FLOAT_EPSILON))

/** @brief Convert radians to degrees
 *  @param A operand
 */
# define JO_RAD_TO_DEG(A)                  ((A) * (180.0f / JO_PI))

/** @brief Convert degrees to radians
 *  @param A operand
 */
# define JO_DEG_TO_RAD(A)                  ((A) / 180.0f * JO_PI)

/** @brief Check if A is odd
 *  @param A operand
 */
# define JO_IS_ODD(A)              ((A) & 1)

/** @brief Check if float almost equals 0;
 *  @param f floating point number
 *  @return true if the float almost equals 0 otherwise false
 */
static  __jo_force_inline bool	jo_is_float_equals_zero(float f) { return (JO_ABS(f) < 0.00000001f); }

/** @brief Fast method to get if two square intersects (HitBox processing)
 *  @param x1 Horizontal position of the first square
 *  @param y1 Vertical position of the first square
 *  @param w1 Width of the first square
 *  @param h1 Height of the first square
 *  @param x2 Horizontal position of the second square
 *  @param y2 Vertical position of the second square
 *  @param w2 Width of the second square
 *  @param h2 Height of the second square
 *  @return true if squares collides otherwise false
 */
static  __jo_force_inline bool	jo_square_intersect(const int x1, const int y1, const int w1, const int h1,
                                                    const int x2, const int y2, const int w2, const int h2)
{
    return ((x1 < x2 + w2) && (x2 < x1 + w1)) && ((y1 < y2 + h2) && (y2 < y1 + h1));
}

/** @brief Get a random number
 *  @param max maximum value
 *  @return 1 to max
 */
static  __jo_force_inline int	jo_random(int max) { return slRandom() % max; }

/** @brief Get a random number with a specific multiple
 *  @param max maximum value
 *  @param multiple multiple
 *  @return A multiple between 1 to max
 */
static  __jo_force_inline int	jo_random_using_multiple(int max, int multiple) { return ((slRandom() % max) / multiple) * multiple; }

/** @brief Fast square root
 *  @param value Value
 *  @return Sqrt(value)
 */
unsigned int jo_sqrt(unsigned int value);

/** @brief Fast Square root using floating number
 *  @param value Value
 *  @return Sqrt(value)
 *  @warning Not precise
 */
static  __jo_force_inline float jo_sqrtf(float value) { unsigned int i = *(unsigned int*)(void *)&value; i += 127 << 23; i >>= 1; return *(float*)(void *)&i; }

/*
** SINUS COMPUTATION
*/

/** @brief Fast sinus computation
 *  @param angle Angle in degree
 *  @return Sin(angle) * 32768
 */
static  __jo_force_inline int	jo_sin(const int angle) { return (angle >= 0 ? JoSinLookupTable[JO_ABS(angle) % 360] : -JoSinLookupTable[JO_ABS(angle) % 360]); }

/** @brief Sinus computation
 *  @param angle Angle in degree
 *  @return Sin(angle) using floating number (slow)
 *  @warning slower than jo_sin() because it use floating point
 */
static  __jo_force_inline float	jo_sinf(const int angle) { return (jo_sin(angle) / 32768.0f);  }

/** @brief Sinus computation
 *  @param angle Angle in radian
 *  @return Sin(angle) * 32768
 *  @warning slower than jo_sin() because it use floating point
 */
static  __jo_force_inline int	jo_sin_rad(const float angle) { return jo_sin((int)(JO_RAD_TO_DEG(angle))); }

/** @brief Sinus computation
 *  @param angle Angle in radian
 *  @return Sin(angle) using floating number (slow)
 *  @warning slower than jo_sin_rad() because it use floating point
 */
static  __jo_force_inline float	jo_sin_radf(const float angle) {  return (jo_sin_rad(angle) / 32768.0f); }

/** @brief Fast sinus multiplication
 *  @param angle Angle in degree
 *  @param value Value
 *  @return value * Sin(angle)
 */
static  __jo_force_inline int	jo_sin_mult(const int value, const int angle) { return JO_DIV_BY_32768(value * jo_sin(angle)); }

/** @brief Fast sinus multiplication
 *  @param angle Angle in degree
 *  @param value Value
 *  @return value * Sin(angle) using floating number (slow)
 */
static  __jo_force_inline float	jo_sinf_mult(const float value, const int angle) { return value * jo_sinf(angle); }

/*
** COSINUS COMPUTATION
*/

/** @brief Fast cosinus computation
 *  @param angle Angle in degree
 *  @return Cos(angle) * 32768
 */
static  __jo_force_inline int	jo_cos(const int angle) { return JoCosLookupTable[JO_ABS(angle) % 360]; }

/** @brief Cosinus computation
 *  @param angle Angle in degree
 *  @return Cos(angle) using floating number (slow)
 *  @warning slower than jo_cos() because it use floating point
 */
static  __jo_force_inline float	jo_cosf(const int angle) { return (jo_cos(angle) / 32768.0f);  }

/** @brief Cosinus computation
 *  @param angle Angle in radian
 *  @return Cos(angle) * 32768
 *  @warning slower than jo_cos() because it use floating point
 */
static  __jo_force_inline int	jo_cos_rad(const float angle) { return jo_cos((int)(JO_RAD_TO_DEG(angle))); }

/** @brief Cosinus computation
 *  @param angle Angle in radian
 *  @return Cos(angle) using floating number (slow)
 *  @warning slower than jo_cos_rad() because it use floating point
 */
static  __jo_force_inline float	jo_cos_radf(const float angle) { return jo_cos_rad(angle) / 32768.0f; }

/** @brief Fast cosinus multiplication
 *  @param angle Angle in degree
 *  @param value Value
 *  @return value * Cos(angle)
 */
static  __jo_force_inline int	jo_cos_mult(const int value, const int angle) { return JO_DIV_BY_32768(value * jo_cos(angle)); }

/** @brief Fast cosinus multiplication
 *  @param angle Angle in degree
 *  @param value Value
 *  @return value * Cos(angle) using floating number (slow)
 */
static  __jo_force_inline float	jo_cosf_mult(const float value, const int angle) { return value * jo_cosf(angle); }

/*
** ATAN2
*/

/** @brief Fast ATAN2 computation in radian
 *  @param y First value
 *  @param x Second value
 *  @warning Y is the first operand (like Math.Atan2() in C#)
 *  @return ATAN2(Y, X)
 */
float                           jo_atan2f_rad(const float y, const float x);

/** @brief Fast ATAN2 computation in degree
 *  @param y First value
 *  @param x Second value
 *  @warning Y is the first operand (like Math.Atan2() in C#)
 *  @return ATAN2(Y, X)
 */
static __jo_force_inline int  jo_atan2f(const float y, const float x) { return (int)JO_RAD_TO_DEG(jo_atan2f_rad(y, x)); }

/*
** MISC
*/

/** @brief Get the greatest common divisor
 *  @remarks PGCD in French
 *  @param a First value (> 0)
 *  @param b Second value (> 0)
 *  @return GCD(a, b)
 */
int     jo_gcd(int a, int b);

#endif /* !__JO_MATH_H__ */

/*
** END OF FILE
*/

