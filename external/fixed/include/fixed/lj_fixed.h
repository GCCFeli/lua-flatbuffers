#ifndef LJ_FIXED_H
#define LJ_FIXED_H

#include "fixed/Fixed32.h"
#include "fixed/Fixed48.h"
#include "fixed/Fixed64.h"

#ifdef LUA_FIXED32
#    define LUA_NUMBER FP_INT
#    define l_mathop(op) op##_FP32
#elif defined(LUA_FIXED48)
#    define LUA_NUMBER FP_LONG
#    define l_mathop(op) op##_FP48
#elif defined(LUA_FIXED64)
#    define LUA_NUMBER FP_LONG
#    define l_mathop(op) op##_FP64
#else
#    error WTF
#endif

#define l_mathlim(n) (DBL_##n)  // TODO

#define LUAI_UACNUMBER double  // TODO

#define LUA_NUMBER_FRMLEN ""    // TODO
#define LUA_NUMBER_FMT "%.14g"  // TODO

//#define lua_str2number(s, p) (l_mathop(FromString)((s), (p)))
//
//#undef lua_number2str
//
//#define lua_number2str(s, sz, n) (l_mathop(ToString)((s), (sz), (n)))
//
//#undef lua_numbertointeger
//#define lua_numbertointeger(n, p) ((*(p) = (LUA_INTEGER)(l_mathop(FloorToInt)(n)), 1))
//
//#define lua_integertonumber(n) (l_mathop(FromInt)((n)))
//
//#define luai_numadd(L, a, b) (l_mathop(Add)((a), (b)))
//#define luai_numsub(L, a, b) (l_mathop(Sub)((a), (b)))
//#define luai_nummul(L, a, b) (l_mathop(Mul)((a), (b)))
//#define luai_numunm(L, a) (-(a))
//#define luai_numeq(a, b) ((a) == (b))
//#define luai_numlt(a, b) ((a) < (b))
//#define luai_numle(a, b) ((a) <= (b))
//#define luai_numisnan(a) (!luai_numeq((a), (a)))
//
//#define LUA_MATHOP_FUNC1(op, func) \
//    FP_INLINE LUA_NUMBER l_mathop(op)(LUA_NUMBER a) { return l_mathop(func)(a); }
//
//#define LUA_MATHOP_FUNC2(op, func) \
//    FP_INLINE LUA_NUMBER l_mathop(op)(LUA_NUMBER a, LUA_NUMBER b) { return l_mathop(func)(a, b); }
//
//LUA_MATHOP_FUNC2(pow, Pow);
//LUA_MATHOP_FUNC1(floor, Floor);
//LUA_MATHOP_FUNC1(ceil, Ceil);
//LUA_MATHOP_FUNC2(fmod, Mod);
//LUA_MATHOP_FUNC1(fabs, Abs);
//LUA_MATHOP_FUNC1(sin, Sin);
//LUA_MATHOP_FUNC1(cos, Cos);
//LUA_MATHOP_FUNC1(tan, Tan);
//LUA_MATHOP_FUNC1(asin, Asin);
//LUA_MATHOP_FUNC1(acos, Acos);
//LUA_MATHOP_FUNC2(atan2, Atan2);
//LUA_MATHOP_FUNC1(sqrt, Sqrt);
//LUA_MATHOP_FUNC1(log, Log);
//LUA_MATHOP_FUNC1(log2, Log2);
//LUA_MATHOP_FUNC1(exp, Exp);
//
//FP_INLINE LUA_NUMBER l_mathop(log10)(LUA_NUMBER a) {
//    return l_mathop(Div)(l_mathop(Log)(a), l_mathop(Log)(l_mathop(FromInt)(10)));
//}

#undef lua_const_number
#define lua_const_number(i) (FromDouble_FP64(i))

#define LUA_NUMBER_SCAN "%lf"

//
//#define luai_numidiv(L, a, b) ((void)L, l_floor(luai_numdiv(L, a, b)))
//
///* float division */
//#define luai_numdiv(L, a, b) (l_mathop(Div)((a), (b)))
//
///*
//** 'l_intfitsf' checks whether a given integer can be converted to a
//** float without rounding. Used in comparisons. Left undefined if
//** all integers fit in a float precisely.
//*/
//#define l_intfitsf(i) (((i) <= l_mathop(MaxIntValue)) && ((i) >= l_mathop(MinIntValue)))
//
//#define l_rand() Rand_FP()
//#define l_srand(x) Srand_FP(x)
//
//#define l_hashfloat(n) (n)
//
//typedef struct lua_Vector2 {
//    LUA_NUMBER x;
//    LUA_NUMBER y;
//} lua_Vector2;
//
//#define lua_assert(c) assert(c)
#endif