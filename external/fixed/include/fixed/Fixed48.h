#ifndef FIXED_48_H
#define FIXED_48_H

#include "Fixed32.h"
#include "Fixed64.h"

#ifdef __cplusplus
extern "C" {
#endif
// Q16.32 Fixed number
// 1 sign bit, 16 unused bits (zero for normal numbers), 15 integer bits and 32 fractional  bit

#define POSITIVE_INFINITY_FP48 (0x7FF0'0000'0000'0000LL)
#define NEGATIVE_INFINITY_FP48 (0xFFF0'0000'0000'0000LL)

#define NAN_FP48 (0xFFF8'0000'0000'0000LL)

#define Shift_FP48 (Shift_FP64)
#define FractionMask_FP48 (FractionMask_FP64)
#define IntegerMask_FP48 (0x8000'7FFF'0000'0000LL)
#define PositiveMask_FP48 (0x8000'7FFF'FFFF'FFFFLL)

#define Zero_FP48 (Zero_FP64)
#define Neg1_FP48 (Neg1_FP64 & PositiveMask_FP48)
#define One_FP48 (One_FP64)
#define Two_FP48 (Two_FP64)
#define Three_FP48 (Three_FP64)
#define Four_FP48 (Four_FP64)
#define Half_FP48 (Half_FP64)
#define Pi_FP48 (Pi_FP64)
#define Pi2_FP48 (Pi2_FP64)
#define PiHalf_FP48 (PiHalf_FP64)
#define E_FP48 (E_FP64)

#define MinValue_FP48 (MinValue_FP64 & PositiveMask_FP48)
#define MaxValue_FP48 (MaxValue_FP64 & PositiveMask_FP48)

FP_INLINE FP_LONG FromFixed64_FP48(FP_LONG v) {
    return v & PositiveMask_FP48;
}

FP_INLINE FP_LONG ToFixed64_FP48(FP_LONG v) {
    return ((v << 16) | (v & (1LL << 63))) >> 16;
}

FP_INLINE FP_LONG FromFixed32_FP48(FP_INT v) {
    return FromFixed64_FP48((FP_LONG)v << 16);
}

FP_INLINE FP_INT ToFixed32_FP48(FP_LONG v) {
    return ((FP_INT)(v << 16));
}

// Converts an integer to a fixed-point value.
FP_INLINE FP_LONG FromInt_FP48(FP_INT v) {
    return FromFixed64_FP48(FromInt_FP64(v));
}

// Converts a double to a fixed-point value.
FP_INLINE FP_LONG FromDouble_FP48(double v) {
    return FromFixed64_FP48(FromDouble_FP64(v));
}

// Converts a float to a fixed-point value.
FP_INLINE FP_LONG FromFloat_FP48(float v) {
    return FromFixed64_FP48(FromFloat_FP64(v));
}

// Converts a fixed-point value into an integer by rounding it up to nearest integer.
FP_INLINE FP_INT CeilToInt_FP48(FP_LONG v) {
    return CeilToInt_FP64(ToFixed64_FP48(v));
}

// Converts a fixed-point value into an integer by rounding it down to nearest integer.
FP_INLINE FP_INT FloorToInt_FP48(FP_LONG v) {
    return FloorToInt_FP64(ToFixed64_FP48(v));
}

// Converts a fixed-point value into an integer by rounding it to nearest integer.
FP_INLINE FP_INT RoundToInt_FP48(FP_LONG v) {
    return RoundToInt_FP64(ToFixed64_FP48(v));
}

// Converts a fixed-point value into a double.
FP_INLINE double ToDouble_FP48(FP_LONG v) {
    return ToDouble_FP64(ToFixed64_FP48(v));
}

// Converts a FP value into a float.
FP_INLINE float ToFloat_FP48(FP_LONG v) {
    return ToFloat_FP64(ToFixed64_FP48(v));
}

FP_INLINE int Sign_FP48(FP_LONG v) {
    return Sign_FP64(v);
}

FP_INLINE int Cmpf_FP48(FP_LONG a, FP_LONG b) {
    return Cmpf_FP64(a, b);
}

FP_INLINE int SignEps_FP48(FP_LONG v) {
    return SignEps_FP64(v);
}

FP_INLINE FP_LONG Neg_FP48(FP_LONG v) {
    return FromFixed64_FP48(-v);
}

FP_LONG FromString_FP48(const char* nptr, char** endptr);

int ToString_FP48(char* s, uint32_t n, FP_LONG num);

// Functions take 1 FP48 as argument and returns 1 FP48
#define FP48_FUNC_1(func) \
    FP_INLINE FP_LONG func##_FP48(FP_LONG v) { return FromFixed64_FP48(func##_FP64(ToFixed64_FP48(v))); }

// Functions take 2 FP48s as arguments and returns 1 FP48
#define FP48_FUNC_2(func) \
    FP_INLINE FP_LONG func##_FP48(FP_LONG a, FP_LONG b) { return FromFixed64_FP48(func##_FP64(ToFixed64_FP48(a), ToFixed64_FP48(b))); }

// Functions take 3 FP48s as arguments and returns 1 FP48
#define FP48_FUNC_3(func)                                                                              \
    FP_INLINE FP_LONG func##_FP48(FP_LONG a, FP_LONG b, FP_LONG c) {                                   \
        return FromFixed64_FP48(func##_FP64(ToFixed64_FP48(a), ToFixed64_FP48(b), ToFixed64_FP48(c))); \
    }

FP48_FUNC_2(Atan2Div)
FP48_FUNC_2(Atan2DivFast)
FP48_FUNC_2(Atan2DivFastest)
FP48_FUNC_1(Abs)
FP48_FUNC_1(Acos)
FP48_FUNC_1(AcosFast)
FP48_FUNC_1(AcosFastest)
FP48_FUNC_2(Add)
FP48_FUNC_1(Asin)
FP48_FUNC_1(AsinFast)
FP48_FUNC_1(AsinFastest)
FP48_FUNC_1(Atan)
FP48_FUNC_2(Atan2)
FP48_FUNC_2(Atan2Fast)
FP48_FUNC_2(Atan2Fastest)
FP48_FUNC_1(AtanFast)
FP48_FUNC_1(AtanFastest)
FP48_FUNC_1(Ceil)
FP48_FUNC_3(Clamp)
FP48_FUNC_1(Cos)
FP48_FUNC_1(CosFast)
FP48_FUNC_1(CosFastest)
FP48_FUNC_2(Div)
FP48_FUNC_2(DivFast)
FP48_FUNC_2(DivFastest)
FP48_FUNC_2(DivPrecise)
FP48_FUNC_1(Exp)
FP48_FUNC_1(Exp2)
FP48_FUNC_1(Exp2Fast)
FP48_FUNC_1(Exp2Fastest)
FP48_FUNC_1(ExpFast)
FP48_FUNC_1(ExpFastest)
FP48_FUNC_1(Floor)
FP48_FUNC_1(Fract)
FP48_FUNC_1(Log)
FP48_FUNC_1(Log2)
FP48_FUNC_1(Log2Fast)
FP48_FUNC_1(Log2Fastest)
FP48_FUNC_1(LogFast)
FP48_FUNC_1(LogFastest)
FP48_FUNC_2(Max)
FP48_FUNC_2(Min)
FP48_FUNC_2(Mod)
FP48_FUNC_2(Mul)
FP48_FUNC_1(Nabs)
FP48_FUNC_2(Pow)
FP48_FUNC_2(PowFast)
FP48_FUNC_2(PowFastest)
FP48_FUNC_1(Rcp)
FP48_FUNC_1(RcpFast)
FP48_FUNC_1(RcpFastest)
FP48_FUNC_1(Round)
FP48_FUNC_1(RSqrt)
FP48_FUNC_1(RSqrtFast)
FP48_FUNC_1(RSqrtFastest)
FP48_FUNC_1(Sin)
FP48_FUNC_1(SinFast)
FP48_FUNC_1(SinFastest)
FP48_FUNC_1(Sqrt)
FP48_FUNC_1(SqrtFast)
FP48_FUNC_1(SqrtFastest)
FP48_FUNC_1(SqrtPrecise)
FP48_FUNC_2(Sub)
FP48_FUNC_1(Tan)
FP48_FUNC_1(TanFast)
FP48_FUNC_1(TanFastest)

#ifdef __cplusplus
}
#endif
#endif