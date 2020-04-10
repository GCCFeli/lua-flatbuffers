
#include "fixed/fixed_vector2.hpp"

#include "fixed/lfixed.h"
#include "lua.h"
using namespace Fixed64;

constexpr lua_Number kDregeeToRadian = 74961319LL;

constexpr lua_Number kRadianToDegree = 246083500440LL;

static int vector2_new(lua_State* L) {
    lua_Number x = luaL_checknumber(L, 1);
    lua_Number y = luaL_checknumber(L, 2);
    lua_Vector2 v = lua_Vector2{ x, y };
    lua_pushvector2(L, v);

    return 1;
}

static lua_Number Square(lua_Number x) {
    return Mul(x, x);
}

static lua_Number SqrMagnitude(const lua_Vector2& vector) {
    return Add(Square(vector.x), Square(vector.y));
}

static lua_Number Magnitude(const lua_Vector2& vector) {
    return Sqrt(SqrMagnitude(vector));
}

static int vector2_sqrmagnitude(lua_State* L) {
    lua_Vector2 vector = lua_tovector2(L, 1);

    lua_Number result = SqrMagnitude(vector);
    lua_pushnumber(L, result);
    return 1;
}

static int vector2_magnitude(lua_State* L) {
    lua_Vector2 vector = lua_tovector2(L, 1);

    lua_Number result = Magnitude(vector);
    lua_pushnumber(L, result);
    return 1;
}

static int vector2_clone(lua_State* L) {
    lua_Vector2 vector = lua_tovector2(L, 1);

    lua_pushvector2(L, vector);

    return 1;
}

static int vector2_turnLeft(lua_State* L) {
    lua_Vector2 vector = lua_tovector2(L, 1);

    lua_Vector2 result{ -vector.y, vector.x };
    lua_pushvector2(L, result);
    return 1;
}

static int vector2_turnRight(lua_State* L) {
    lua_Vector2 vector = lua_tovector2(L, 1);

    lua_Vector2 result{ vector.y, -vector.x };
    lua_pushvector2(L, result);
    return 1;
}

static lua_Vector2 Rotate(lua_Vector2 vector, lua_Number angle) {
    auto ca = Cos(angle);
    auto sa = Sin(angle);

    return { Sub(Mul(vector.x, ca), Mul(vector.y, sa)), Add(Mul(vector.x, sa), Mul(vector.y, ca)) };
}

static int vector2_rotate(lua_State* L) {
    lua_Vector2 vector = lua_tovector2(L, 1);
    lua_Number angle = lua_tonumber(L, 2);

    lua_pushvector2(L, Rotate(vector, angle));
    return 1;
}

static int vector2_rotateByDegrees(lua_State* L) {
    lua_Vector2 vector = lua_tovector2(L, 1);
    lua_Number angle = lua_tonumber(L, 2);

    angle = Mul(angle, kDregeeToRadian);

    lua_pushvector2(L, Rotate(vector, angle));
    return 1;
}

static int vector2_vrotate(lua_State* L) {
    lua_Vector2 v = lua_tovector2(L, 1);
    lua_Vector2 dir = lua_tovector2(L, 2);

    lua_Vector2 result{ Sub(Mul(v.x, dir.x), Mul(v.y, dir.y)), Add(Mul(v.x, dir.y), Mul(v.y, dir.x)) };
    lua_pushvector2(L, result);
    return 1;
}

static int vector2_vyrotate(lua_State* L) {
    lua_Vector2 v = lua_tovector2(L, 1);
    lua_Vector2 dir = lua_tovector2(L, 2);

    lua_Vector2 result{ Add(Mul(v.x, dir.y), Mul(v.y, dir.x)), Add(-Mul(v.x, dir.x), Mul(v.y, dir.y)) };
    lua_pushvector2(L, result);
    return 1;
}

static lua_Number Dot(const lua_Vector2 a, const lua_Vector2 b) {
    return Add(Mul(a.x, b.x), Mul(a.y, b.y));
}

static int vector2_dot(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);

    lua_pushnumber(L, Dot(a, b));
    return 1;
}

static int vector2_dot3(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);
    lua_Vector2 c = lua_tovector2(L, 3);

    lua_Number result = Add(Mul(Sub(b.x, a.x), Sub(c.x, a.x)), Mul(Sub(b.y, a.y), Sub(c.y, a.y)));

    lua_pushnumber(L, result);
    return 1;
}

static lua_Number Cross(const lua_Vector2& a, const lua_Vector2& b) {
    return Sub(Mul(a.x, b.y), Mul(a.y, b.x));
}

static int vector2_cross(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);

    lua_Number result = Cross(a, b);

    lua_pushnumber(L, result);
    return 1;
}

static int vector2_cross3(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);
    lua_Vector2 c = lua_tovector2(L, 3);

    lua_Number result = Sub(Mul(Sub(b.x, a.x), Sub(c.y, a.y)), Mul(Sub(b.y, a.y), Sub(c.x, a.x)));

    lua_pushnumber(L, result);
    return 1;
}

static int vector2_norm(lua_State* L) {
    lua_Vector2 v = lua_tovector2(L, 1);
    lua_Number m2 = SqrMagnitude(v);

    if (m2 == Fixed64::One) {
        lua_pushvector2(L, v);
        return 1;
    }

    if (m2 < (1LL << 16)) {
        v.x <<= 16;
        v.y <<= 16;
        m2 = SqrMagnitude(v);
    }

    if (m2 == Fixed64::Zero) {
        lua_pushvector2(L, lua_Vector2{ 0, 0 });
        return 1;
    }

    lua_Number inv_len = RSqrt(m2);

    v.x = Mul(v.x, inv_len);
    v.y = Mul(v.y, inv_len);

    lua_pushvector2(L, v);

    return 1;
}

static lua_Number SqrDistance(const lua_Vector2& a, const lua_Vector2& b) {
    return Add(Square(Sub(a.x, b.x)), Square(Sub(a.y, b.y)));
}

static int vector2_sqrdist(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);

    lua_pushnumber(L, SqrDistance(a, b));

    return 1;
}

static int vector2_dist(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);

    lua_pushnumber(L, Sqrt(SqrDistance(a, b)));

    return 1;
}

static int vector2_lerp(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);
    lua_Number t = lua_tonumber(L, 3);

    lua_Vector2 r{ Add(a.x, Mul(Sub(b.x, a.x), t)), Add(a.y, Mul(Sub(b.y, a.y), t)) };

    lua_pushvector2(L, r);

    return 1;
}

static int vector2_project(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);
    lua_Number t = Div(Dot(a, b), SqrMagnitude(b));

    lua_Vector2 r{ Mul(b.x, t), Mul(b.y, t) };

    lua_pushvector2(L, r);

    return 1;
}

static lua_Number Angle(const lua_Vector2& a, const lua_Vector2& b) {
    constexpr lua_Number eps = FromDouble(1e-4);

    if ((Cmpf(Abs(a.x), eps) < 0 && Cmpf(Abs(a.y), eps) < 0) || (Cmpf(Abs(b.x), eps) < 0 && Cmpf(Abs(b.y), eps) < 0)) {
        return Zero;
    } else {
        auto t = Div(Dot(a, b), Sqrt(Mul(SqrMagnitude(a), SqrMagnitude(b))));

        return Acos(Clamp(t, -One, One));
    }
}

static lua_Number Angle(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);
    return Angle(a, b);
}

static int vector2_angle(lua_State* L) {
    const auto angle = Angle(L);
    lua_pushnumber(L, angle);

    return 1;
}

static int vector2_degree(lua_State* L) {
    const auto angle = Angle(L);
    lua_pushnumber(L, Mul(angle, kRadianToDegree));

    return 1;
}

static lua_Number SAngle(lua_State* L) {
    lua_Vector2 a = lua_tovector2(L, 1);
    lua_Vector2 b = lua_tovector2(L, 2);

    auto angle = Angle(a, b);
    auto cross = Cross(a, b);

    if (Sign(cross) < 0) {
        angle = -angle;
    }

    return angle;
}

static int vector2_sangle(lua_State* L) {
    const auto angle = SAngle(L);
    lua_pushnumber(L, angle);

    return 1;
}

static int vector2_sdegree(lua_State* L) {
    const auto angle = SAngle(L);
    lua_pushnumber(L, Mul(angle, kRadianToDegree));

    return 1;
}

static int AngleToVector(lua_State* L, lua_Number angle) {
    auto c = Cos(angle);
    auto s = Sin(angle);

    lua_Vector2 r{ c, s };
    lua_pushvector2(L, r);
    return 1;
}

static int vector2_angleToVector(lua_State* L) {
    lua_Number angle = lua_tonumber(L, 1);

    return AngleToVector(L, angle);
}

static int vector2_degreeToVector(lua_State* L) {
    lua_Number angle = lua_tonumber(L, 1);

    return AngleToVector(L, Mul(angle, kDregeeToRadian));
}

static lua_Number Angle2D(lua_State* L) {
    const lua_Vector2 from = { Fixed64::One, Fixed64::Zero };
    const lua_Vector2 a = lua_tovector2(L, 1);
    const auto sign64 = a.y;
    const auto signN = sign64 >= Fixed64::Zero ? Fixed64::One : Fixed64::Neg1;
    const auto angle = Angle(from, a);
    return Mul(signN, angle);
}

static int vector2_angle2D(lua_State* L) {
    const auto angle = Angle2D(L);
    lua_pushnumber(L, angle);
    return 1;
}

static int vector2_degree2D(lua_State* L) {
    const auto angle = Angle2D(L);
    lua_pushnumber(L, Mul(angle, kRadianToDegree));
    return 1;
}

static int vector2_clamp(lua_State* L) {
    lua_Vector2 v = lua_tovector2(L, 1);
    lua_Number a = lua_tonumber(L, 2);

    auto mv = SqrMagnitude(v);
    if (mv > a) {
        v.x = Mul(Div(v.x, mv), a);
        v.y = Mul(Div(v.y, mv), a);
    }
    lua_pushvector2(L, v);
    return 1;
}

constexpr lua_Number kNormalizationToleranceSq = lua_const_number(1e-4);

static int vector2_IsNormalized(lua_State* L) {
    lua_Vector2 v = lua_tovector2(L, 1);

    auto len2 = SqrMagnitude(v);

    bool ret = Abs(Sub(len2, One)) < kNormalizationToleranceSq;

    lua_pushboolean(L, ret);
    return 1;
}

static const struct luaL_Reg vector2_lib[] = {
    { "new", vector2_new },  // constructor
    { "sqrmagnitude", vector2_sqrmagnitude },
    { "magnitude", vector2_magnitude },
    { "clone", vector2_clone },
    { "turnLeft", vector2_turnLeft },
    { "turnRight", vector2_turnRight },
    { "rotate", vector2_rotate },
    { "rotateByDegrees", vector2_rotateByDegrees },
    { "vrotate", vector2_vrotate },
    { "vyrotate", vector2_vyrotate },
    { "dot", vector2_dot },
    { "dot3", vector2_dot3 },
    { "cross", vector2_cross },
    { "cross3", vector2_cross3 },
    { "norm", vector2_norm },
    { "sqrdist", vector2_sqrdist },
    { "dist", vector2_dist },
    { "lerp", vector2_lerp },
    { "project", vector2_project },
    { "angle", vector2_angle },
    { "degree", vector2_degree },
    { "sangle", vector2_sangle },
    { "sdegree", vector2_sdegree },
    { "angleToVector", vector2_angleToVector },
    { "degreeToVector", vector2_degreeToVector },
    { "clamp", vector2_clamp },
    { "angle2D", vector2_angle2D },
    { "degree2D", vector2_degree2D },
    { "normalized", vector2_IsNormalized },

    /* placeholders */
    { "zero", NULL },
    { "forward", NULL },
    { "back", NULL },
    { "left", NULL },
    { "right", NULL },
    { NULL, NULL },
};

LUAMOD_API int luaopen_vector2(lua_State* L) {
    luaL_newlib(L, vector2_lib);

    lua_pushvector2(L, lua_Vector2{ Zero, Zero });
    lua_setfield(L, -2, "zero");

    lua_pushvector2(L, lua_Vector2{ Zero, One });
    lua_setfield(L, -2, "forward");

    lua_pushvector2(L, lua_Vector2{ Zero, -One });
    lua_setfield(L, -2, "back");

    lua_pushvector2(L, lua_Vector2{ -One, Zero });
    lua_setfield(L, -2, "left");

    lua_pushvector2(L, lua_Vector2{ One, Zero });
    lua_setfield(L, -2, "right");

    // lua_setglobal(L, "vector2_native");
    return 1;
}