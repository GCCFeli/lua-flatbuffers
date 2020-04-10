/*
** $Id: lmathlib.c,v 1.119.1.1 2017/04/19 17:20:42 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/

#define lmathlib_c
#define LUA_LIB

#include "lprefix.h"


#include <stdlib.h>
#include <math.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#undef PI
#define PI	(lua_const_number(3.141592653589793238462643383279502884))


#if !defined(l_rand)		/* { */
#if defined(LUA_USE_POSIX)
#define l_rand()	random()
#define l_srand(x)	srandom(x)
#define L_RANDMAX	2147483647	/* (2^31 - 1), following POSIX */
#else
#define l_rand()	rand()
#define l_srand(x)	srand(x)
#define L_RANDMAX	RAND_MAX
#endif
#endif				/* } */


static int math_abs (lua_State *L) {
  if (lua_isinteger(L, 1)) {
    lua_Integer n = lua_tointeger(L, 1);
    if (n < 0) n = (lua_Integer)(0u - (lua_Unsigned)n);
    lua_pushinteger(L, n);
  }
  else
    lua_pushnumber(L, l_mathop(fabs)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sin (lua_State *L) {
  lua_pushnumber(L, l_mathop(sin)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_cos (lua_State *L) {
  lua_pushnumber(L, l_mathop(cos)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tan (lua_State *L) {
  lua_pushnumber(L, l_mathop(tan)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_asin (lua_State *L) {
  lua_pushnumber(L, l_mathop(asin)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_acos (lua_State *L) {
  lua_pushnumber(L, l_mathop(acos)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_atan (lua_State *L) {
  lua_Number y = luaL_checknumber(L, 1);
  lua_Number x = luaL_optnumber(L, 2, lua_const_number(1));
  lua_pushnumber(L, l_mathop(atan2)(y, x));
  return 1;
}


static int math_toint (lua_State *L) {
  int valid;
  lua_Integer n = lua_tointegerx(L, 1, &valid);
  if (valid)
    lua_pushinteger(L, n);
  else {
    luaL_checkany(L, 1);
    lua_pushnil(L);  /* value is not convertible to integer */
  }
  return 1;
}


static void pushnumint (lua_State *L, lua_Number d) {
  lua_Integer n;
  if (lua_numbertointeger(d, &n))  /* does 'd' fit in an integer? */
    lua_pushinteger(L, n);  /* result is integer */
  else
    lua_pushnumber(L, d);  /* result is float */
}


static int math_floor (lua_State *L) {
  if (lua_isinteger(L, 1))
    lua_settop(L, 1);  /* integer is its own floor */
  else {
    lua_Number d = l_mathop(floor)(luaL_checknumber(L, 1));
    pushnumint(L, d);
  }
  return 1;
}


static int math_ceil (lua_State *L) {
  if (lua_isinteger(L, 1))
    lua_settop(L, 1);  /* integer is its own ceil */
  else {
    lua_Number d = l_mathop(ceil)(luaL_checknumber(L, 1));
    pushnumint(L, d);
  }
  return 1;
}


static int math_fmod (lua_State *L) {
  if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
    lua_Integer d = lua_tointeger(L, 2);
    if ((lua_Unsigned)d + 1u <= 1u) {  /* special cases: -1 or 0 */
      luaL_argcheck(L, d != 0, 2, "zero");
      lua_pushinteger(L, 0);  /* avoid overflow with 0x80000... / -1 */
    }
    else
      lua_pushinteger(L, lua_tointeger(L, 1) % d);
  }
  else
    lua_pushnumber(L, l_mathop(fmod)(luaL_checknumber(L, 1),
                                     luaL_checknumber(L, 2)));
  return 1;
}


/*
** next function does not use 'modf', avoiding problems with 'double*'
** (which is not compatible with 'float*') when lua_Number is not
** 'double'.
*/
static int math_modf (lua_State *L) {
  if (lua_isinteger(L ,1)) {
    lua_settop(L, 1);  /* number is its own integer part */
    lua_pushnumber(L, 0);  /* no fractional part */
  }
  else {
    lua_Number n = luaL_checknumber(L, 1);
    /* integer part (rounds toward zero) */
    lua_Number ip = (n < 0) ? l_mathop(ceil)(n) : l_mathop(floor)(n);
    pushnumint(L, ip);
    /* fractional part (test needed for inf/-inf) */
    lua_pushnumber(L, (n == ip) ? lua_const_number(0.0) : (n - ip));
  }
  return 2;
}


static int math_sqrt (lua_State *L) {
  lua_pushnumber(L, l_mathop(sqrt)(luaL_checknumber(L, 1)));
  return 1;
}


static int math_ult (lua_State *L) {
  lua_Integer a = luaL_checkinteger(L, 1);
  lua_Integer b = luaL_checkinteger(L, 2);
  lua_pushboolean(L, (lua_Unsigned)a < (lua_Unsigned)b);
  return 1;
}

static int math_log (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  lua_Number res;
  if (lua_isnoneornil(L, 2))
    res = l_mathop(log)(x);
  else {
    lua_Number base = luaL_checknumber(L, 2);
#if !defined(LUA_USE_C89)
    if (base == lua_const_number(2.0))
      res = l_mathop(log2)(x); else
#endif
    if (base == lua_const_number(10.0))
      res = l_mathop(log10)(x);
    else
      res = l_fixed_mathop(Div)(l_mathop(log)(x), l_mathop(log)(base));
  }
  lua_pushnumber(L, res);
  return 1;
}

static int math_exp (lua_State *L) {
  lua_pushnumber(L, l_mathop(exp)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_deg (lua_State *L) {
#ifdef LUA_FIXED32
  FP_LONG N = ((FP_LONG)luaL_checknumber(L, 1)) << 16;
  N = Mul(N, Div(FromInt(180), Pi));
  lua_pushnumber(L, (FP_INT)(N >> 16));
#elif defined(LUA_FIXED64)
  lua_Number N = (luaL_checknumber(L, 1));
  N = l_fixed_mathop(Mul)(N, l_fixed_mathop(Div)(l_fixed_mathop(FromInt(180)), l_fixed_mathop(Pi)));
  lua_pushnumber(L, N);
#else
  lua_pushnumber(L, luaL_checknumber(L, 1) * (lua_const_number(180.0) / PI));
#endif
  return 1;
}

static int math_rad (lua_State *L) {
#ifdef LUA_FIXED32
  FP_LONG N = ((FP_LONG)luaL_checknumber(L, 1)) << 16;
  N = Mul(N, Div(Pi, FromInt(180)));
  lua_pushnumber(L, (FP_INT)(N >> 16));
#elif defined(LUA_FIXED64)
  lua_Number N = (luaL_checknumber(L, 1));
  N = l_fixed_mathop(Mul)(N, l_fixed_mathop(Div)(l_fixed_mathop(Pi), l_fixed_mathop(FromInt)(180)));
  lua_pushnumber(L, N);
#else
  lua_pushnumber(L, luaL_checknumber(L, 1) * (PI / lua_const_number(180.0)));
#endif
  return 1;
}


static int math_min (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int imin = 1;  /* index of current minimum value */
  int i;
  luaL_argcheck(L, n >= 1, 1, "value expected");
  for (i = 2; i <= n; i++) {
    if (lua_compare(L, i, imin, LUA_OPLT))
      imin = i;
  }
  lua_pushvalue(L, imin);
  return 1;
}


static int math_max (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int imax = 1;  /* index of current maximum value */
  int i;
  luaL_argcheck(L, n >= 1, 1, "value expected");
  for (i = 2; i <= n; i++) {
    if (lua_compare(L, imax, i, LUA_OPLT))
      imax = i;
  }
  lua_pushvalue(L, imax);
  return 1;
}

#ifdef LUA_FIXED
static int math_random (lua_State *L) {
  return luaL_error(L, "use lucifer::SceneManager::Random() instead");
}

static int math_randomseed (lua_State *L) {
  return luaL_error(L, "use lucifer::SceneManager::RandomSeed() instead");
}
#else

/*
** This function uses 'double' (instead of 'lua_Number') to ensure that
** all bits from 'l_rand' can be represented, and that 'RANDMAX + 1.0'
** will keep full precision (ensuring that 'r' is always less than 1.0.)
*/
static int math_random (lua_State *L) {
  lua_Integer low, up;
  double r = (double)l_rand() * (1.0 / ((double)L_RANDMAX + 1.0));
  switch (lua_gettop(L)) {  /* check number of arguments */
    case 0: {  /* no arguments */
      lua_pushnumber(L, (lua_Number)r);  /* Number between 0 and 1 */
      return 1;
    }
    case 1: {  /* only upper limit */
      low = 1;
      up = luaL_checkinteger(L, 1);
      break;
    }
    case 2: {  /* lower and upper limits */
      low = luaL_checkinteger(L, 1);
      up = luaL_checkinteger(L, 2);
      break;
    }
    default: return luaL_error(L, "wrong number of arguments");
  }
  /* random integer in the interval [low, up] */
  luaL_argcheck(L, low <= up, 1, "interval is empty");
  luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1,
                   "interval too large");
  r *= (double)(up - low) + 1.0;
  lua_pushinteger(L, (lua_Integer)r + low);
  return 1;
}

static int math_randomseed (lua_State *L) {
  l_srand((unsigned int)(lua_Integer)luaL_checknumber(L, 1));
  (void)l_rand(); /* discard first value to avoid undesirable correlations */
  return 0;
}
#endif


static int math_type (lua_State *L) {
  if (lua_type(L, 1) == LUA_TNUMBER) {
      if (lua_isinteger(L, 1))
        lua_pushliteral(L, "integer");
      else
        lua_pushliteral(L, "float");
  }
  else {
    luaL_checkany(L, 1);
    lua_pushnil(L);
  }
  return 1;
}

static int math_fixed (lua_State* L) {
  lua_Integer i = lua_tointeger(L, 1);
  lua_pushnumber(L, (lua_Number)i);
  return 1;
}

static int math_fixed32 (lua_State* L) {
  lua_Integer i = lua_tointeger(L, 1);
#ifdef LUA_FIXED32
  lua_pushnumber(L, (lua_Number)(((FP_LONG)(FP_INT)i) << 16));
#elif defined LUA_FIXED64
  lua_pushnumber(L, (lua_Number)(((FixedUtil::FP_LONG)(FixedUtil::FP_INT)i) << 16));
#else
  lua_pushnumber(L, (lua_Number)i);
#endif
  return 1;
}

static int math_fixed64 (lua_State* L) {
  lua_Integer i = lua_tointeger(L, 1);
#ifdef LUA_FIXED32
  lua_pushnumber(L, (lua_Number)i >> 16L);
#elif defined LUA_FIXED64
  lua_pushnumber(L, (lua_Number)i);
#else
  lua_pushnumber(L, (lua_Number)i);
#endif
  return 1;
}

static int math_value32 (lua_State* L) {
  lua_Number n = luaL_checknumber(L, 1);
#ifdef LUA_FIXED32
  lua_pushinteger(L, (lua_Integer)n);
#elif defined LUA_FIXED64
  lua_pushinteger(L, (lua_Integer)(n >> 16));
#else
  lua_pushinteger(L, (lua_Integer)n);
#endif
  return 1;
}

static int math_value64 (lua_State* L) {
  lua_Number n = luaL_checknumber(L, 1);
#ifdef LUA_FIXED32
  lua_pushinteger(L, (lua_Integer)n << 16L);
#elif defined LUA_FIXED64
  lua_pushinteger(L, (lua_Integer)n);
#else
  lua_pushinteger(L, (lua_Integer)n);
#endif
  return 1;
}

static int math_clamp(lua_State* L) {
  int n = lua_gettop(L);  /* number of arguments */
  luaL_argcheck(L, n == 3, 1, "3 numbers expected");

  constexpr int inum = 1;
  constexpr int imin = 2;
  constexpr int imax = 3;

  int i = lua_compare(L, inum, imin, LUA_OPLT) ? imin : inum;
  i = lua_compare(L, imax, i, LUA_OPLT) ? imax : i;

  lua_pushvalue(L, i);
  return 1;
}

constexpr auto Zero = lua_const_number(0);
constexpr auto One = lua_const_number(1);

static lua_Number Clamp01(lua_Number num) {
  return (num < Zero ? Zero : (One < num ? One : num));
}

static int math_clamp01(lua_State* L) {
  int n = lua_gettop(L); /* number of arguments */
  luaL_argcheck(L, n == 1, 1, "1 number expected");

  lua_Number num = luaL_checknumber(L, 1);

  num = Clamp01(num);

  lua_pushnumber(L, num);
  return 1;
}

static int math_sign(lua_State* L) {
  lua_Number num = luaL_checknumber(L, 1);
  constexpr auto zero = lua_const_number(0);
  const lua_Integer sign = (num > zero ? 1 : (num < zero ? -1 : 0));

  lua_pushinteger(L, sign);
  return 1;
}

static int math_lerp(lua_State* L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Number t = luaL_checknumber(L, 3);

  const auto num = luai_numadd(L, a, luai_nummul(L, t, luai_numsub(L, b, a)));
  
  lua_pushnumber(L, num);
  return 1;
}

static int math_inverseLerp(lua_State* L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Number value = luaL_checknumber(L, 3);

  constexpr auto zero = lua_const_number(0);
  constexpr auto one = lua_const_number(1);

  const auto bp = luai_numsub(L, b, a);
  const auto valuep = luai_numsub(L, value, a);

  const auto t = (bp == zero) ? one : luai_numdiv(L, valuep, bp);

  lua_pushnumber(L, t);
  return 1;
}

static int math_hasAnyFlag(lua_State* L) {
  const lua_Unsigned lhs = (lua_Unsigned)(luaL_checkinteger(L, 1));
  const lua_Unsigned rhs = (lua_Unsigned)(luaL_checkinteger(L, 2));

  const auto r = (lhs & rhs) != 0u;

  lua_pushboolean(L, r);
  return 1;
}

static int math_hasFlags(lua_State* L) {
  const lua_Unsigned lhs = (lua_Unsigned)(luaL_checkinteger(L, 1));
  const lua_Unsigned rhs = (lua_Unsigned)(luaL_checkinteger(L, 2));

  const auto r = (lhs & rhs) == rhs;

  lua_pushboolean(L, r);
  return 1;
}

static int math_addFlags(lua_State* L) {
  const lua_Unsigned lhs = (lua_Unsigned)(luaL_checkinteger(L, 1));
  const lua_Unsigned rhs = (lua_Unsigned)(luaL_checkinteger(L, 2));

  const auto r = (lua_Integer)(lhs | rhs);

  lua_pushinteger(L, r);
  return 1;
}

static int math_removeFlags(lua_State* L) {
  const lua_Unsigned lhs = (lua_Unsigned)(luaL_checkinteger(L, 1));
  const lua_Unsigned rhs = (lua_Unsigned)(luaL_checkinteger(L, 2));

  const auto r = (lua_Integer)(lhs & ~rhs);

  lua_pushinteger(L, r);
  return 1;
}

constexpr lua_Number OneEighty = lua_const_number(180);
constexpr lua_Number NOneEighty = lua_const_number(-180);
constexpr lua_Number ThreeSixty = lua_const_number(360);

constexpr lua_Number Add(lua_Number a, lua_Number b) {
  return luai_numadd(0, a, b);
}

constexpr lua_Number Sub(lua_Number a, lua_Number b) {
  return luai_numsub(0, a, b);
}

constexpr lua_Number Mul(lua_Number a, lua_Number b) {
  return luai_nummul(0, a, b);
}

constexpr lua_Number Div(lua_Number a, lua_Number b) {
  return luai_numdiv(0, a, b);
}

constexpr lua_Number Floor(lua_Number a) {
  return l_mathop(floor)(a);
}

constexpr lua_Number Abs(lua_Number a) {
  return l_mathop(fabs)(a);
}

static lua_Number Repeat(lua_Number t, lua_Number length) {
  // return t - Floor(t / length) * length;
  return Sub(t, Mul(Floor(Div(t, length)), length));
}

static lua_Number NormalizeAngle(lua_Number angle) {
  lua_Number num = Repeat(angle, ThreeSixty);
  if (num > OneEighty) {
    num = Sub(num, ThreeSixty);
  } else if (num < NOneEighty) {
    num = Sub(num, ThreeSixty);
  }
  return num;
}

static int math_normalizeAngle(lua_State* L) {
  lua_Number angle = luaL_checknumber(L, 1);

  lua_Number num = NormalizeAngle(angle);

  lua_pushnumber(L, num);
  return 1;
}

static lua_Number DeltaAngle(lua_Number current, lua_Number target) {
  return NormalizeAngle(Sub(target, current));
}

static int math_deltaAngle(lua_State* L) {
  lua_Number current = luaL_checknumber(L, 1);
  lua_Number target = luaL_checknumber(L, 2);
  lua_Number num = DeltaAngle(current, target);

  lua_pushnumber(L, num);
  return 1;
}

static lua_Number UnsignedDeltaAngle(lua_Number current, lua_Number target) {
  return Abs(DeltaAngle(current, target));
}

static int math_UnsignedDeltaAngle(lua_State* L) {
  lua_Number current = luaL_checknumber(L, 1);
  lua_Number target = luaL_checknumber(L, 2);
  lua_Number num = UnsignedDeltaAngle(current, target);

  lua_pushnumber(L, num);
  return 1;
}

static lua_Number LerpAngle(lua_Number a, lua_Number b, lua_Number t) {
  lua_Number num = DeltaAngle(b, a);
  return Add( a, Mul(num, Clamp01(t)));
}

static int math_LerpAngle(lua_State* L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Number t = luaL_checknumber(L, 3);

  lua_Number num = LerpAngle(a, b, t);

  lua_pushnumber(L, num);
  return 1;
}

static bool BetweenAngles(lua_Number start, lua_Number end, lua_Number target) {
  auto delta_angle = Repeat(Sub(end, start), ThreeSixty);

  if (delta_angle == Zero) {
    return true;
  }

  auto start_to_target_delta_angle = Repeat(Sub(target, start), ThreeSixty);
  
  if (start_to_target_delta_angle < Zero) {
    start_to_target_delta_angle = Add(start_to_target_delta_angle, ThreeSixty);
  }

  if (delta_angle < Zero) {
    delta_angle = Add(delta_angle, ThreeSixty);
  }

  return delta_angle >= start_to_target_delta_angle;
}

static int math_BetweenAngles(lua_State* L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Number t = luaL_checknumber(L, 3);

  auto num = BetweenAngles(a, b, t);

  lua_pushboolean(L, num);
  return 1;
}

/*
** {==================================================================
** Deprecated functions (for compatibility only)
** ===================================================================
*/
#if defined(LUA_COMPAT_MATHLIB)

static int math_cosh (lua_State *L) {
  lua_pushnumber(L, l_mathop(cosh)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sinh (lua_State *L) {
  lua_pushnumber(L, l_mathop(sinh)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tanh (lua_State *L) {
  lua_pushnumber(L, l_mathop(tanh)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_pow (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  lua_Number y = luaL_checknumber(L, 2);
  lua_pushnumber(L, l_mathop(pow)(x, y));
  return 1;
}

static int math_frexp (lua_State *L) {
  int e;
  lua_pushnumber(L, l_mathop(frexp)(luaL_checknumber(L, 1), &e));
  lua_pushinteger(L, e);
  return 2;
}

static int math_ldexp (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  int ep = (int)luaL_checkinteger(L, 2);
  lua_pushnumber(L, l_mathop(ldexp)(x, ep));
  return 1;
}

static int math_log10 (lua_State *L) {
  lua_pushnumber(L, l_mathop(log10)(luaL_checknumber(L, 1)));
  return 1;
}

#endif
/* }================================================================== */



static const luaL_Reg mathlib[] = {
  {"abs",   math_abs},
  {"acos",  math_acos},
  {"asin",  math_asin},
  {"atan",  math_atan},
  {"ceil",  math_ceil},
  {"cos",   math_cos},
  {"deg",   math_deg},
  {"exp",   math_exp},
  {"tointeger", math_toint},
  {"floor", math_floor},
  {"fmod",   math_fmod},
  {"ult",   math_ult},
  {"log",   math_log},
  {"max",   math_max},
  {"min",   math_min},
  {"modf",   math_modf},
  {"rad",   math_rad},
  //{"random",     math_random},
  //{"randomseed", math_randomseed},
  {"sin",   math_sin},
  {"sqrt",  math_sqrt},
  {"tan",   math_tan},
  {"type", math_type},
  {"fixed", math_fixed},
  {"fixed32", math_fixed32},
  {"fixed64", math_fixed64},
  {"value32", math_value32},
  {"value64", math_value64},
  {"clamp", math_clamp},
  {"clamp01", math_clamp01},
  {"sign", math_sign},
  {"lerp", math_lerp},
  {"inverseLerp", math_inverseLerp},
  {"hasAnyFlag", math_hasAnyFlag},
  {"hasFlags", math_hasFlags},
  {"addFlags", math_addFlags},
  {"removeFlags", math_removeFlags},
  {"normalizeDegree", math_normalizeAngle},
  {"deltaDegree", math_deltaAngle},
  {"unsignedDeltaDegree", math_UnsignedDeltaAngle},
  {"lerpDegree", math_LerpAngle},
  {"betweenDegrees", math_BetweenAngles},
#if defined(LUA_COMPAT_MATHLIB)
  {"atan2", math_atan},
  {"cosh",   math_cosh},
  {"sinh",   math_sinh},
  {"tanh",   math_tanh},
  {"pow",   math_pow},
  {"frexp", math_frexp},
  {"ldexp", math_ldexp},
  {"log10", math_log10},
#endif
  /* placeholders */
  {"pi", NULL},
  {"huge", NULL},
  {"maxinteger", NULL},
  {"mininteger", NULL},
  {NULL, NULL}
};


/*
** Open math library
*/
LUAMOD_API int luaopen_math (lua_State *L) {
  luaL_newlib(L, mathlib);
  lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
#ifdef LUA_FIXED
  lua_pushnumber(L, l_fixed_mathop(MaxValue));
#else
  lua_pushnumber(L, (lua_Number)HUGE_VAL);
#endif
  lua_setfield(L, -2, "huge");
  lua_pushinteger(L, LUA_MAXINTEGER);
  lua_setfield(L, -2, "maxinteger");
  lua_pushinteger(L, LUA_MININTEGER);
  lua_setfield(L, -2, "mininteger");
  return 1;
}

