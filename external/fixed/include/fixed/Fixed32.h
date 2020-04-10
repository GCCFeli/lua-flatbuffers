#pragma once

// Include numeric types
#include <stdint.h>
#include "fixed/Fixed64.h"
#include "fixed/FixedUtil.h"

namespace Fixed32 {
using FixedUtil::FP_INT;
using FixedUtil::FP_LONG;
using FixedUtil::FP_UINT;
using FixedUtil::FP_ULONG;

constexpr FP_INT Shift = 16;
constexpr FP_INT FractionMask = (1 << Shift) - 1;
constexpr FP_INT IntegerMask = ~FractionMask;

// Constants
constexpr FP_INT Zero = 0;
constexpr FP_INT Neg1 = 0xFFFFFFFF << Shift;
constexpr FP_INT One = 1 << Shift;
constexpr FP_INT Two = 2 << Shift;
constexpr FP_INT Three = 3 << Shift;
constexpr FP_INT Four = 4 << Shift;
constexpr FP_INT Half = One >> 1;
constexpr FP_INT Pi = (FP_INT)(13493037705L >> 16);  //(FP_INT)(Math.PI * 65536.0) << 16;
constexpr FP_INT Pi2 = (FP_INT)(26986075409L >> 16);
constexpr FP_INT PiHalf = (FP_INT)(6746518852L >> 16);
constexpr FP_INT E = (FP_INT)(11674931555L >> 16);

constexpr FP_INT MinValue = INT32_MIN;
constexpr FP_INT MaxValue = INT32_MAX;

// Private constants
constexpr FP_INT RCP_LN2 = (FP_INT)(0x171547652L >> 16);    // 1.0 / log(2.0) ~= 1.4426950408889634
constexpr FP_INT RCP_LOG2_E = (FP_INT)(2977044471L >> 16);  // 1.0 / log2(e) ~= 0.6931471805599453
constexpr FP_INT RCP_TWO_PI = 683565276;                    // 1.0 / (4.0 * 0.5 * pi);  -- the 4.0 factor converts directly to s2.30

// Converts an integer to a fixed-point value.
FP_INLINE FP_INT FromInt(FP_INT v) {
    return (FP_INT)v << Shift;
}

// Converts a double to a fixed-point value.
FP_INLINE FP_INT FromDouble(double v) {
    return (FP_INT)(v * 65536.0);
}

// Converts a float to a fixed-point value.
FP_INLINE FP_INT FromFloat(float v) {
    return (FP_INT)(v * 65536.0f);
}

// Converts a fixed-point value into an integer by rounding it up to nearest integer.
FP_INLINE FP_INT CeilToInt(FP_INT v) {
    return (FP_INT)((v + (One - 1)) >> Shift);
}

// Converts a fixed-point value into an integer by rounding it down to nearest integer.
FP_INLINE FP_INT FloorToInt(FP_INT v) {
    return (FP_INT)(v >> Shift);
}

// Converts a fixed-point value into an integer by rounding it to nearest integer.
FP_INLINE FP_INT RoundToInt(FP_INT v) {
    return (FP_INT)((v + Half) >> Shift);
}

// Converts a fixed-point value into a double.
FP_INLINE double ToDouble(FP_INT v) {
    return (double)v * (1.0 / 65536.0);
}

// Converts a FP value into a float.
FP_INLINE float ToFloat(FP_INT v) {
    return (float)v * (1.0f / 65536.0f);
}

// Returns the absolute (positive) value of x.
FP_INLINE FP_INT Abs(FP_INT x) {
    // \note fails with MinValue
    // \note for some reason this is twice as fast as (x > 0) ? x : -x
    return (x < 0) ? -x : x;
}

// Negative absolute value (returns -abs(x)).
FP_INLINE FP_INT Nabs(FP_INT x) {
    return (x > 0) ? -x : x;
}

// Round up to nearest integer.
FP_INLINE FP_INT Ceil(FP_INT x) {
    return (x + FractionMask) & IntegerMask;
}

// Round down to nearest integer.
FP_INLINE FP_INT Floor(FP_INT x) {
    return x & IntegerMask;
}

// Round to nearest integer.
FP_INLINE FP_INT Round(FP_INT x) {
    return (x + Half) & IntegerMask;
}

// Returns the fractional part of x. Equal to 'x - floor(x)'.
FP_INLINE FP_INT Fract(FP_INT x) {
    return x & FractionMask;
}

// Returns the minimum of the two values.
FP_INLINE FP_INT Min(FP_INT a, FP_INT b) {
    return (a < b) ? a : b;
}

// Returns the maximum of the two values.
FP_INLINE FP_INT Max(FP_INT a, FP_INT b) {
    return (a > b) ? a : b;
}

// Returns the sign of the value (-1 if negative, 0 if zero, 1 if positive).
FP_INLINE FP_INT Sign(FP_INT x) {
    if (x == 0)
        return 0;
    return (x < 0) ? Neg1 : One;
}

// Adds the two FP numbers together.
FP_INLINE FP_INT Add(FP_INT a, FP_INT b) {
    return a + b;
}

// Subtracts the two FP numbers from each other.
FP_INLINE FP_INT Sub(FP_INT a, FP_INT b) {
    return a - b;
}

// Multiplies two FP values together.
FP_INLINE FP_INT Mul(FP_INT a, FP_INT b) {
    return (FP_INT)(((FP_LONG)a * (FP_LONG)b) >> Shift);
}

FP_INLINE FP_INT Nlz(FP_UINT x) {
    FP_INT n = 0;
    if (x <= 0x0000FFFF) {
        n = n + 16;
        x = x << 16;
    }
    if (x <= 0x00FFFFFF) {
        n = n + 8;
        x = x << 8;
    }
    if (x <= 0x0FFFFFFF) {
        n = n + 4;
        x = x << 4;
    }
    if (x <= 0x3FFFFFFF) {
        n = n + 2;
        x = x << 2;
    }
    if (x <= 0x7FFFFFFF) {
        n = n + 1;
    }
    if (x == 0)
        return 32;
    return n;
}

// Divides two FP values.
FP_INLINE FP_INT DivPrecise(FP_INT a, FP_INT b) {
    if (b == MinValue || b == 0)
        return 0;

    FP_INT res = (FP_INT)(((FP_LONG)a << Shift) / (FP_LONG)b);
    return res;
}

// Calculates division approximation.
FP_INLINE FP_INT Div(FP_INT a, FP_INT b) {
    if (b == MinValue || b == 0)
        return 0;

    return (FP_INT)(((FP_LONG)a << 16) / b);
}

// Calculates division approximation.
FP_INLINE FP_INT DivFast(FP_INT a, FP_INT b) {
    if (b == MinValue || b == 0)
        return 0;

    // Handle negative values.
    FP_INT sign = (b < 0) ? -1 : 1;
    b *= sign;

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    FP_INT offset = 29 - Nlz((FP_UINT)b);
    FP_INT n = FixedUtil::ShiftRight(b, offset - 28);
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);

    // Polynomial approximation.
    FP_INT res = FixedUtil::RcpPoly6(n - ONE);

    // Multiply by reciprocal, apply exponent, convert back to s16.16.
    FP_INT y = FixedUtil::Qmul30(res, a);
    return FixedUtil::ShiftRight(sign * y, offset - 14);
}

// Calculates division approximation.
FP_INLINE FP_INT DivFastest(FP_INT a, FP_INT b) {
    if (b == MinValue || b == 0)
        return 0;

    // Handle negative values.
    FP_INT sign = (b < 0) ? -1 : 1;
    b *= sign;

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    FP_INT offset = 29 - Nlz((FP_UINT)b);
    FP_INT n = FixedUtil::ShiftRight(b, offset - 28);
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);

    // Polynomial approximation.
    FP_INT res = FixedUtil::RcpPoly4(n - ONE);

    // Multiply by reciprocal, apply exponent, convert back to s16.16.
    FP_INT y = FixedUtil::Qmul30(res, a);
    return FixedUtil::ShiftRight(sign * y, offset - 14);
}

// Divides two FP values and returns the modulus.
FP_INLINE FP_INT Mod(FP_INT a, FP_INT b) {
    FP_INT di = a / b;
    FP_INT ret = a - (di * b);
    return ret;
}

// Calculates the square root of the given number.
FP_INLINE FP_INT SqrtPrecise(FP_INT a) {
    // Adapted from https://github.com/chmike/fpsqrt
    if (a < 0)
        return -1;

    FP_UINT r = (FP_UINT)a;
    FP_UINT b = 0x40000000;
    FP_UINT q = 0;
    while (b > 0x40) {
        FP_UINT t = q + b;
        if (r >= t) {
            r -= t;
            q = t + b;
        }
        r <<= 1;
        b >>= 1;
    }
    q >>= 8;
    return (FP_INT)q;
}

// Calculates the square root of the given number.
FP_INLINE FP_INT Sqrt(FP_INT x) {
    // Return 0 for all non-positive values.
    if (x <= 0)
        return 0;

    // Constants (s2.30).
    constexpr FP_INT ONE = (1 << 30);
    constexpr FP_INT SQRT2 = 1518500249;  // sqrt(2.0)

    // Normalize input into [1.0, 2.0( range (as s2.30).
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::SqrtPoly3Lut8(n - ONE);

    // Divide offset by 2 (to get sqrt), compute adjust value for odd exponents.
    FP_INT adjust = ((offset & 1) != 0) ? SQRT2 : ONE;
    offset = offset >> 1;

    // Apply exponent, convert back to s16.16.
    FP_INT yr = FixedUtil::Qmul30(adjust, y);
    return FixedUtil::ShiftRight(yr, 14 - offset);
}

// Calculates the square root of the given number.
FP_INLINE FP_INT SqrtFast(FP_INT x) {
    // Return 0 for all non-positive values.
    if (x <= 0)
        return 0;

    // Constants (s2.30).
    constexpr FP_INT ONE = (1 << 30);
    constexpr FP_INT SQRT2 = 1518500249;  // sqrt(2.0)

    // Normalize input into [1.0, 2.0( range (as s2.30).
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::SqrtPoly4(n - ONE);

    // Divide offset by 2 (to get sqrt), compute adjust value for odd exponents.
    FP_INT adjust = ((offset & 1) != 0) ? SQRT2 : ONE;
    offset = offset >> 1;

    // Apply exponent, convert back to s16.16.
    FP_INT yr = FixedUtil::Qmul30(adjust, y);
    return FixedUtil::ShiftRight(yr, 14 - offset);
}

// Calculates the square root of the given number.
FP_INLINE FP_INT SqrtFastest(FP_INT x) {
    // Return 0 for all non-positive values.
    if (x <= 0)
        return 0;

    // Constants (s2.30).
    constexpr FP_INT ONE = (1 << 30);
    constexpr FP_INT SQRT2 = 1518500249;  // sqrt(2.0)

    // Normalize input into [1.0, 2.0( range (as s2.30).
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::SqrtPoly3(n - ONE);

    // Divide offset by 2 (to get sqrt), compute adjust value for odd exponents.
    FP_INT adjust = ((offset & 1) != 0) ? SQRT2 : ONE;
    offset = offset >> 1;

    // Apply exponent, convert back to s16.16.
    FP_INT yr = FixedUtil::Qmul30(adjust, y);
    return FixedUtil::ShiftRight(yr, 14 - offset);
}

// Calculates the reciprocal square root.
FP_INLINE FP_INT RSqrt(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Constants (s2.30).
    constexpr FP_INT ONE = (1 << 30);
    constexpr FP_INT HALF_SQRT2 = 759250125;  // 0.5 * sqrt(2.0)

    // Normalize input into [1.0, 2.0( range (as s2.30).
    FP_INT offset = 1 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::RSqrtPoly3Lut16(n - ONE);

    // Divide offset by 2 (to get sqrt), compute adjust value for odd exponents.
    FP_INT adjust = ((offset & 1) != 0) ? HALF_SQRT2 : ONE;
    offset = offset >> 1;

    // Apply exponent, convert back to s16.16.
    FP_INT yr = FixedUtil::Qmul30(adjust, y);
    return FixedUtil::ShiftRight(yr, offset + 21);
}

// Calculates the reciprocal square root.
FP_INLINE FP_INT RSqrtFast(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Constants (s2.30).
    constexpr FP_INT ONE = (1 << 30);
    constexpr FP_INT HALF_SQRT2 = 759250125;  // 0.5 * sqrt(2.0)

    // Normalize input into [1.0, 2.0( range (as s2.30).
    FP_INT offset = 1 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::RSqrtPoly5(n - ONE);

    // Divide offset by 2 (to get sqrt), compute adjust value for odd exponents.
    FP_INT adjust = ((offset & 1) != 0) ? HALF_SQRT2 : ONE;
    offset = offset >> 1;

    // Apply exponent, convert back to s16.16.
    FP_INT yr = FixedUtil::Qmul30(adjust, y);
    return FixedUtil::ShiftRight(yr, offset + 21);
}

// Calculates the reciprocal square root.
FP_INLINE FP_INT RSqrtFastest(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Constants (s2.30).
    constexpr FP_INT ONE = (1 << 30);
    constexpr FP_INT HALF_SQRT2 = 759250125;  // 0.5 * sqrt(2.0)

    // Normalize input into [1.0, 2.0( range (as s2.30).
    FP_INT offset = 1 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::RSqrtPoly3(n - ONE);

    // Divide offset by 2 (to get sqrt), compute adjust value for odd exponents.
    FP_INT adjust = ((offset & 1) != 0) ? HALF_SQRT2 : ONE;
    offset = offset >> 1;

    // Apply exponent, convert back to s16.16.
    FP_INT yr = FixedUtil::Qmul30(adjust, y);
    return FixedUtil::ShiftRight(yr, offset + 21);
}

// Calculates reciprocal approximation.
FP_INLINE FP_INT Rcp(FP_INT x) {
    if (x == MinValue || x == 0)
        return 0;

    // Handle negative values.
    FP_INT sign = (x < 0) ? -1 : 1;
    x *= sign;

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    FP_INT offset = 29 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 28);
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);

    // Polynomial approximation.
    FP_INT res = FixedUtil::RcpPoly4Lut8(n - ONE);

    // Apply exponent, convert back to s16.16.
    return FixedUtil::ShiftRight(sign * res, offset);
}

// Calculates reciprocal approximation.
FP_INLINE FP_INT RcpFast(FP_INT x) {
    if (x == MinValue || x == 0)
        return 0;

    // Handle negative values.
    FP_INT sign = (x < 0) ? -1 : 1;
    x *= sign;

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    FP_INT offset = 29 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 28);
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);

    // Polynomial approximation.
    FP_INT res = FixedUtil::RcpPoly6(n - ONE);
    // FP_INT res = Util.RcpPoly3Lut8(n - ONE);

    // Apply exponent, convert back to s16.16.
    return FixedUtil::ShiftRight(sign * res, offset);
}

// Calculates reciprocal approximation.
FP_INLINE FP_INT RcpFastest(FP_INT x) {
    if (x == MinValue || x == 0)
        return 0;

    // Handle negative values.
    FP_INT sign = (x < 0) ? -1 : 1;
    x *= sign;

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    FP_INT offset = 29 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 28);
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);

    // Polynomial approximation.
    FP_INT res = FixedUtil::RcpPoly4(n - ONE);
    // FP_INT res = Util.RcpPoly3Lut4(n - ONE);

    // Apply exponent, convert back to s16.16.
    return FixedUtil::ShiftRight(sign * res, offset);
}

// Calculates the base 2 exponent.
FP_INLINE FP_INT Exp2(FP_INT x) {
    // Handle values that would under or overflow.
    if (x >= 15 * One)
        return MaxValue;
    if (x <= -16 * One)
        return 0;

    // Compute exp2 for fractional part.
    FP_INT k = (x & FractionMask) << 14;
    FP_INT y = FixedUtil::Exp2Poly5(k);

    // Combine integer and fractional result, and convert back to s16.16.
    FP_INT intPart = x >> Shift;
    return FixedUtil::ShiftRight(y, 14 - intPart);
}

// Calculates the base 2 exponent.
FP_INLINE FP_INT Exp2Fast(FP_INT x) {
    // Handle values that would under or overflow.
    if (x >= 15 * One)
        return MaxValue;
    if (x <= -16 * One)
        return 0;

    // Compute exp2 for fractional part.
    FP_INT k = (x & FractionMask) << 14;
    FP_INT y = FixedUtil::Exp2Poly4(k);

    // Combine integer and fractional result, and convert back to s16.16.
    FP_INT intPart = x >> Shift;
    return FixedUtil::ShiftRight(y, 14 - intPart);
}

// Calculates the base 2 exponent.
FP_INLINE FP_INT Exp2Fastest(FP_INT x) {
    // Handle values that would under or overflow.
    if (x >= 15 * One)
        return MaxValue;
    if (x <= -16 * One)
        return 0;

    // Compute exp2 for fractional part.
    FP_INT k = (x & FractionMask) << 14;
    FP_INT y = FixedUtil::Exp2Poly3(k);

    // Combine integer and fractional result, and convert back to s16.16.
    FP_INT intPart = x >> Shift;
    return FixedUtil::ShiftRight(y, 14 - intPart);
}

FP_INLINE FP_INT Exp(FP_INT x) {
    // e^x == 2^(x / ln(2))
    return Exp2(Mul(x, RCP_LN2));
}

FP_INLINE FP_INT ExpFast(FP_INT x) {
    // e^x == 2^(x / ln(2))
    return Exp2Fast(Mul(x, RCP_LN2));
}

FP_INLINE FP_INT ExpFastest(FP_INT x) {
    // e^x == 2^(x / ln(2))
    return Exp2Fastest(Mul(x, RCP_LN2));
}

FP_INLINE FP_INT Log(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Normalize value to range [1.0, 2.0( as s2.30 and extract exponent.
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);

    // Polynomial approximation.
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::LogPoly5Lut8(n - ONE);

    // Combine integer and fractional parts (into s16.16).
    return offset * RCP_LOG2_E + (y >> 14);
}

FP_INLINE FP_INT LogFast(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Normalize value to range [1.0, 2.0( as s2.30 and extract exponent.
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);

    // Polynomial approximation.
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::LogPoly3Lut8(n - ONE);

    // Combine integer and fractional parts (into s16.16).
    return offset * RCP_LOG2_E + (y >> 14);
}

FP_INLINE FP_INT LogFastest(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Normalize value to range [1.0, 2.0( as s2.30 and extract exponent.
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);

    // Polynomial approximation.
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::LogPoly5(n - ONE);

    // Combine integer and fractional parts (into s16.16).
    return offset * RCP_LOG2_E + (y >> 14);
}

FP_INLINE FP_INT Log2(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Normalize value to range [1.0, 2.0( as s2.30 and extract exponent.
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);

    // Polynomial approximation of mantissa.
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::Log2Poly4Lut16(n - ONE);

    // Combine integer and fractional parts (into s16.16).
    return (offset << Shift) + (y >> 14);
}

FP_INLINE FP_INT Log2Fast(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Normalize value to range [1.0, 2.0( as s2.30 and extract exponent.
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);

    // Polynomial approximation of mantissa.
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::Log2Poly3Lut16(n - ONE);

    // Combine integer and fractional parts (into s16.16).
    return (offset << Shift) + (y >> 14);
}

FP_INLINE FP_INT Log2Fastest(FP_INT x) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    // Normalize value to range [1.0, 2.0( as s2.30 and extract exponent.
    FP_INT offset = 15 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset - 14);

    // Polynomial approximation of mantissa.
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT(n >= ONE);
    FP_INT y = FixedUtil::Log2Poly5(n - ONE);

    // Combine integer and fractional parts (into s16.16).
    return (offset << Shift) + (y >> 14);
}

// Calculates x to the power of the exponent.
FP_INLINE FP_INT Pow(FP_INT x, FP_INT exponent) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    return Exp(Mul(exponent, Log(x)));
}

// Calculates x to the power of the exponent.
FP_INLINE FP_INT PowFast(FP_INT x, FP_INT exponent) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    return ExpFast(Mul(exponent, LogFast(x)));
}

// Calculates x to the power of the exponent.
FP_INLINE FP_INT PowFastest(FP_INT x, FP_INT exponent) {
    // Return 0 for invalid values
    if (x <= 0)
        return 0;

    return ExpFastest(Mul(exponent, LogFastest(x)));
}

FP_INLINE FP_INT UnitSin(FP_INT z) {
    // See: http://www.coranac.com/2009/07/sines/

    // Handle quadrants 1 and 2 by mirroring the [1, 3] range to [-1, 1] (by calculating 2 - z).
    // The if condition uses the fact that for the quadrants of interest are 0b01 and 0b10 (top two bits are different).
    if ((z ^ (z << 1)) < 0)
        z = (1 << 31) - z;

    // Now z is in range [-1, 1].
    FP_ASSERT_ONLY(constexpr FP_INT ONE = (1 << 30));
    FP_ASSERT((z >= -ONE) && (z <= ONE));

    // Polynomial approximation.
    FP_INT zz = FixedUtil::Qmul30(z, z);
    FP_INT res = FixedUtil::Qmul30(FixedUtil::SinPoly4(zz), z);

    // Return as s2.30.
    return res;
}

FP_INLINE FP_INT UnitSinFast(FP_INT z) {
    // See: http://www.coranac.com/2009/07/sines/

    // Handle quadrants 1 and 2 by mirroring the [1, 3] range to [-1, 1] (by calculating 2 - z).
    // The if condition uses the fact that for the quadrants of interest are 0b01 and 0b10 (top two bits are different).
    if ((z ^ (z << 1)) < 0)
        z = (1 << 31) - z;

    // Now z is in range [-1, 1].
    FP_ASSERT_ONLY(constexpr FP_INT ONE = (1 << 30));
    FP_ASSERT((z >= -ONE) && (z <= ONE));

    // Polynomial approximation.
    FP_INT zz = FixedUtil::Qmul30(z, z);
    FP_INT res = FixedUtil::Qmul30(FixedUtil::SinPoly3(zz), z);

    // Return as s2.30.
    return res;
}

FP_INLINE FP_INT UnitSinFastest(FP_INT z) {
    // See: http://www.coranac.com/2009/07/sines/

    // Handle quadrants 1 and 2 by mirroring the [1, 3] range to [-1, 1] (by calculating 2 - z).
    // The if condition uses the fact that for the quadrants of interest are 0b01 and 0b10 (top two bits are different).
    if ((z ^ (z << 1)) < 0)
        z = (1 << 31) - z;

    // Now z is in range [-1, 1].
    FP_ASSERT_ONLY(constexpr FP_INT ONE = (1 << 30));
    FP_ASSERT((z >= -ONE) && (z <= ONE));

    // Polynomial approximation.
    FP_INT zz = FixedUtil::Qmul30(z, z);
    FP_INT res = FixedUtil::Qmul30(FixedUtil::SinPoly2(zz), z);

    // Return as s2.30.
    return res;
}

FP_INLINE FP_INT Sin(FP_INT x) {
    // Map [0, 2pi] to [0, 4] (as s2.30).
    // This also wraps the values into one period.
    FP_INT z = Mul(RCP_TWO_PI, x);

    // Compute sin from s2.30 and convert back to s16.16.
    return UnitSin(z) >> 14;
}

FP_INLINE FP_INT SinFast(FP_INT x) {
    // Map [0, 2pi] to [0, 4] (as s2.30).
    // This also wraps the values into one period.
    FP_INT z = Mul(RCP_TWO_PI, x);

    // Compute sin from s2.30 and convert back to s16.16.
    return UnitSinFast(z) >> 14;
}

FP_INLINE FP_INT SinFastest(FP_INT x) {
    // Map [0, 2pi] to [0, 4] (as s2.30).
    // This also wraps the values into one period.
    FP_INT z = Mul(RCP_TWO_PI, x);

    // Compute sin from s2.30 and convert back to s16.16.
    return UnitSinFastest(z) >> 14;
}

FP_INLINE FP_INT Cos(FP_INT x) {
    return Sin(x + PiHalf);
}

FP_INLINE FP_INT CosFast(FP_INT x) {
    return SinFast(x + PiHalf);
}

FP_INLINE FP_INT CosFastest(FP_INT x) {
    return SinFastest(x + PiHalf);
}

FP_INLINE FP_INT Tan(FP_INT x) {
    FP_INT z = Mul(RCP_TWO_PI, x);
    FP_INT sinX = UnitSin(z);
    FP_INT cosX = UnitSin(z + (1 << 30));
    return Div(sinX, cosX);
}

FP_INLINE FP_INT TanFast(FP_INT x) {
    FP_INT z = Mul(RCP_TWO_PI, x);
    FP_INT sinX = UnitSinFast(z);
    FP_INT cosX = UnitSinFast(z + (1 << 30));
    return DivFast(sinX, cosX);
}

FP_INLINE FP_INT TanFastest(FP_INT x) {
    FP_INT z = Mul(RCP_TWO_PI, x);
    FP_INT sinX = UnitSinFastest(z);
    FP_INT cosX = UnitSinFastest(z + (1 << 30));
    return DivFastest(sinX, cosX);
}

FP_INLINE FP_INT Atan2Div(FP_INT y, FP_INT x) {
    FP_ASSERT(y >= 0 && x > 0 && x >= y);

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT_ONLY(constexpr FP_INT HALF = (1 << 29));
    FP_INT offset = 1 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset);
    FP_ASSERT(n >= ONE);

    // Polynomial approximation of reciprocal.
    FP_INT oox = FixedUtil::RcpPoly4Lut8(n - ONE);
    FP_ASSERT(oox >= HALF && oox <= ONE);

    // Apply exponent and multiply.
    FP_INT yr = FixedUtil::ShiftRight(y, offset);
    return FixedUtil::Qmul30(yr, oox);
}

FP_INLINE FP_INT Atan2(FP_INT y, FP_INT x) {
    // See: https://www.dsprelated.com/showarticle/1052.php

    if (x == 0) {
        if (y > 0)
            return PiHalf;
        if (y < 0)
            return -PiHalf;
        return 0;
    }

    FP_INT nx = Abs(x);
    FP_INT ny = Abs(y);
    FP_INT negMask = ((x ^ y) >> 31);

    if (nx >= ny) {
        FP_INT k = Atan2Div(ny, nx);
        FP_INT z = FixedUtil::AtanPoly5Lut8(k);
        FP_INT angle = (negMask ^ (z >> 14)) - negMask;
        if (x > 0)
            return angle;
        if (y >= 0)
            return angle + Pi;
        return angle - Pi;
    } else {
        FP_INT k = Atan2Div(nx, ny);
        FP_INT z = FixedUtil::AtanPoly5Lut8(k);
        FP_INT angle = negMask ^ (z >> 14);
        return ((y > 0) ? PiHalf : -PiHalf) - angle;
    }
}

FP_INLINE FP_INT Atan2DivFast(FP_INT y, FP_INT x) {
    FP_ASSERT(y >= 0 && x > 0 && x >= y);

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT_ONLY(constexpr FP_INT HALF = (1 << 29));
    FP_INT offset = 1 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset);

    // Polynomial approximation.
    FP_INT oox = FixedUtil::RcpPoly6(n - ONE);
    FP_ASSERT(oox >= HALF && oox <= ONE);

    // Apply exponent and multiply.
    FP_INT yr = FixedUtil::ShiftRight(y, offset);
    return FixedUtil::Qmul30(yr, oox);
}

FP_INLINE FP_INT Atan2Fast(FP_INT y, FP_INT x) {
    // See: https://www.dsprelated.com/showarticle/1052.php

    if (x == 0) {
        if (y > 0)
            return PiHalf;
        if (y < 0)
            return -PiHalf;
        return 0;
    }

    FP_INT nx = Abs(x);
    FP_INT ny = Abs(y);
    FP_INT negMask = ((x ^ y) >> 31);

    if (nx >= ny) {
        FP_INT k = Atan2DivFast(ny, nx);
        FP_INT z = FixedUtil::AtanPoly3Lut8(k);
        FP_INT angle = negMask ^ (z >> 14);
        if (x > 0)
            return angle;
        if (y >= 0)
            return angle + Pi;
        return angle - Pi;
    } else {
        FP_INT k = Atan2DivFast(nx, ny);
        FP_INT z = FixedUtil::AtanPoly3Lut8(k);
        FP_INT angle = negMask ^ (z >> 14);
        return ((y > 0) ? PiHalf : -PiHalf) - angle;
    }
}

FP_INLINE FP_INT Atan2DivFastest(FP_INT y, FP_INT x) {
    FP_ASSERT(y >= 0 && x > 0 && x >= y);

    // Normalize input into [1.0, 2.0( range (convert to s2.30).
    constexpr FP_INT ONE = (1 << 30);
    FP_ASSERT_ONLY(constexpr FP_INT HALF = (1 << 29));
    FP_INT offset = 1 - Nlz((FP_UINT)x);
    FP_INT n = FixedUtil::ShiftRight(x, offset);

    // Polynomial approximation.
    FP_INT oox = FixedUtil::RcpPoly4(n - ONE);
    FP_ASSERT(oox >= HALF && oox <= ONE);

    // Apply exponent and multiply.
    FP_INT yr = FixedUtil::ShiftRight(y, offset);
    return FixedUtil::Qmul30(yr, oox);
}

FP_INLINE FP_INT Atan2Fastest(FP_INT y, FP_INT x) {
    // See: https://www.dsprelated.com/showarticle/1052.php

    if (x == 0) {
        if (y > 0)
            return PiHalf;
        if (y < 0)
            return -PiHalf;
        return 0;
    }

    FP_INT nx = Abs(x);
    FP_INT ny = Abs(y);
    FP_INT negMask = ((x ^ y) >> 31);

    if (nx >= ny) {
        FP_INT k = Atan2DivFastest(ny, nx);
        FP_INT z = FixedUtil::AtanPoly4(k);
        FP_INT angle = negMask ^ (z >> 14);
        if (x > 0)
            return angle;
        if (y >= 0)
            return angle + Pi;
        return angle - Pi;
    } else {
        FP_INT k = Atan2DivFastest(nx, ny);
        FP_INT z = FixedUtil::AtanPoly4(k);
        FP_INT angle = negMask ^ (z >> 14);
        return ((y > 0) ? PiHalf : -PiHalf) - angle;
    }
}

FP_INLINE FP_INT Asin(FP_INT x) {
    // Return 0 for invalid values
    if (x < -One || x > One)
        return 0;

    // Compute Atan2(x, Sqrt((1+x) * (1-x))), using s32.32.
    FP_LONG xx = (FP_LONG)(One + x) * (FP_LONG)(One - x);
    FP_LONG y = Fixed64::Sqrt(xx);
    return (FP_INT)(Fixed64::Atan2((FP_LONG)x << 16, y) >> 16);
}

FP_INLINE FP_INT AsinFast(FP_INT x) {
    // Return 0 for invalid values
    if (x < -One || x > One)
        return 0;

    // Compute Atan2(x, Sqrt((1+x) * (1-x))), using s32.32.
    FP_LONG xx = (FP_LONG)(One + x) * (FP_LONG)(One - x);
    FP_LONG y = Fixed64::SqrtFast(xx);
    return (FP_INT)(Fixed64::Atan2Fast((FP_LONG)x << 16, y) >> 16);
}

FP_INLINE FP_INT AsinFastest(FP_INT x) {
    // Return 0 for invalid values
    if (x < -One || x > One)
        return 0;

    // Compute Atan2(x, Sqrt((1+x) * (1-x))), using s32.32.
    FP_LONG xx = (FP_LONG)(One + x) * (FP_LONG)(One - x);
    FP_LONG y = Fixed64::SqrtFastest(xx);
    return (FP_INT)(Fixed64::Atan2Fastest((FP_LONG)x << 16, y) >> 16);
}

FP_INLINE FP_INT Acos(FP_INT x) {
    // Return 0 for invalid values
    if (x < -One || x > One)
        return 0;

    // Compute Atan2(Sqrt((1+x) * (1-x)), x), using s32.32.
    FP_LONG xx = (FP_LONG)(One + x) * (FP_LONG)(One - x);
    FP_LONG y = Fixed64::Sqrt(xx);
    return (FP_INT)(Fixed64::Atan2(y, (FP_LONG)x << 16) >> 16);
}

FP_INLINE FP_INT AcosFast(FP_INT x) {
    // Return 0 for invalid values
    if (x < -One || x > One)
        return 0;

    // Compute Atan2(Sqrt((1+x) * (1-x)), x), using s32.32.
    FP_LONG xx = (FP_LONG)(One + x) * (FP_LONG)(One - x);
    FP_LONG y = Fixed64::SqrtFast(xx);
    return (FP_INT)(Fixed64::Atan2Fast(y, (FP_LONG)x << 16) >> 16);
}

FP_INLINE FP_INT AcosFastest(FP_INT x) {
    // Return 0 for invalid values
    if (x < -One || x > One)
        return 0;

    // Compute Atan2(Sqrt((1+x) * (1-x)), x), using s32.32.
    FP_LONG xx = (FP_LONG)(One + x) * (FP_LONG)(One - x);
    FP_LONG y = Fixed64::SqrtFastest(xx);
    return (FP_INT)(Fixed64::Atan2Fastest(y, (FP_LONG)x << 16) >> 16);
}

FP_INLINE FP_INT Atan(FP_INT x) {
    return Atan2(x, One);
}

FP_INLINE FP_INT AtanFast(FP_INT x) {
    return Atan2Fast(x, One);
}

FP_INLINE FP_INT AtanFastest(FP_INT x) {
    return Atan2Fastest(x, One);
}

FP_INT FromString(const char* buf, char** endptr);

int ToString(char* buf, uint32_t n, FP_INT value);
#undef FP_ASSERT
#undef FP_ASSERT_ONLY
};  // namespace Fixed32
