#ifndef LFIXED_H
#    define LFIXED_H

#    include "fixed/Fixed32.h"

#    ifdef LUA_FIXED32
#        define LUA_NUMBER FP_INT
#        define l_mathop(op) op
#        define MAXNUMBER2STR 13
using namespace Fixed32;
#    elif defined(LUA_FIXED64)
#        define LUA_NUMBER Fixed64::FP_LONG
#        define l_mathop(op) op
#        define l_fixed_mathop(op) Fixed64::op
#        define MAXNUMBER2STR 23 // "-9999999999.9999999999\0"
#    else
#        error WTF
#    endif

#    define l_mathlim(n) (DBL_##n)  // TODO

#    define LUAI_UACNUMBER double  // TODO

#    define LUA_NUMBER_FRMLEN ""    // TODO
#    define LUA_NUMBER_FMT "%.14g"  // TODO

#    define lua_str2number(s, p) (l_fixed_mathop(FromString)((s), (p)))

#    undef lua_number2str

#    define lua_number2str(s, sz, n) (l_fixed_mathop(ToString)((s), (sz), (n)))

#    undef lua_numbertointeger
#    define lua_numbertointeger(n, p) ((*(p) = (LUA_INTEGER)(l_fixed_mathop(FloorToInt)(n)), 1))

#    define lua_integertonumber(n) (l_fixed_mathop(FromInt)((n)))

#    define luai_numadd(L, a, b) (l_fixed_mathop(Add)((a), (b)))
#    define luai_numsub(L, a, b) (l_fixed_mathop(Sub)((a), (b)))
#    define luai_nummul(L, a, b) (l_fixed_mathop(Mul)((a), (b)))
#    define luai_numunm(L, a) (-(a))
#    define luai_numeq(a, b) ((a) == (b))
#    define luai_numlt(a, b) ((a) < (b))
#    define luai_numle(a, b) ((a) <= (b))
#    define luai_numisnan(a) (!luai_numeq((a), (a)))

#    define LUA_MATHOP_FUNC1(op, func) \
        FP_INLINE LUA_NUMBER l_mathop(op)(LUA_NUMBER a) { return l_fixed_mathop(func)(a); }

#    define LUA_MATHOP_FUNC2(op, func) \
        FP_INLINE LUA_NUMBER l_mathop(op)(LUA_NUMBER a, LUA_NUMBER b) { return l_fixed_mathop(func)(a, b); }

LUA_MATHOP_FUNC2(pow, Pow);
LUA_MATHOP_FUNC1(floor, Floor);
LUA_MATHOP_FUNC1(ceil, Ceil);
LUA_MATHOP_FUNC2(fmod, Mod);
LUA_MATHOP_FUNC1(fabs, Abs);
LUA_MATHOP_FUNC1(sin, Sin);
LUA_MATHOP_FUNC1(cos, Cos);
LUA_MATHOP_FUNC1(tan, Tan);
LUA_MATHOP_FUNC1(asin, Asin);
LUA_MATHOP_FUNC1(acos, Acos);
LUA_MATHOP_FUNC2(atan2, Atan2);
LUA_MATHOP_FUNC1(sqrt, Sqrt);
LUA_MATHOP_FUNC1(log, Log);
LUA_MATHOP_FUNC1(log2, Log2);
LUA_MATHOP_FUNC1(exp, Exp);

FP_INLINE LUA_NUMBER l_mathop(log10)(LUA_NUMBER a) {
    return l_fixed_mathop(Div)(l_fixed_mathop(Log)(a), l_fixed_mathop(Log)(l_fixed_mathop(FromInt)(10)));
}

#    undef lua_const_number
#    define lua_const_number(i) (l_fixed_mathop(FromDouble)(i))

#    define luai_numidiv(L, a, b) ((void)L, l_floor(luai_numdiv(L, a, b)))

/* float division */
#    define luai_numdiv(L, a, b) (l_fixed_mathop(Div)((a), (b)))

/*
** 'l_intfitsf' checks whether a given integer can be converted to a
** float without rounding. Used in comparisons. Left undefined if
** all integers fit in a float precisely.
*/
#    define l_intfitsf(i) (((i) <= l_fixed_mathop(MaxIntValue)) && ((i) >= l_fixed_mathop(MinIntValue)))

#    define l_rand(L) Rand_FP((L))
#    define l_srand(L, x) Srand_FP((L), (x))

#    define l_hashfloat(n) (n)

typedef struct lua_Vector2 {
    LUA_NUMBER x;
    LUA_NUMBER y;
} lua_Vector2;

#    define lua_assert(c) assert(c)
#endif

// ceil
#define LUA_FLOORN2I (2)