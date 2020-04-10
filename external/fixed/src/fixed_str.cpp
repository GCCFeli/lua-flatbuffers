#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include <algorithm>

#include "fixed/Fixed32.h"
#ifdef LUA_FIXED48
#    include "fixed/Fixed48.h"
#endif

// https://github.com/PetteriAimonen/libfixmath/blob/master/libfixmath/fix16_str.c

static int set_buffer(char** pBuf, char value, int size, uint32_t n) {
    if ((uint32_t)size < n) {
        **pBuf = value;
        (*pBuf)++;
    }
    size++;
    return size;
}

static int itoa_loop(char** pBuf, uint64_t scale, uint64_t value, int size, uint32_t n, bool is_int_part) {
    bool skip_header = is_int_part;
    bool skip_tail = !is_int_part;
    while (scale) {
        unsigned digit = (unsigned)(value / scale);

        if (!skip_header || digit || scale == 1) {
            skip_header = false;
            size = set_buffer(pBuf, '0' + digit, size, n);
            value %= scale;
        }

        if (skip_tail && value == 0) {
            break;
        }

        scale /= 10;
    }
    return size;
}

static Fixed32::FP_INT mul(Fixed32::FP_INT a, Fixed32::FP_INT b) {
    return Fixed32::Mul(a, b);
}

static Fixed64::FP_LONG mul(Fixed64::FP_LONG a, Fixed64::FP_LONG b) {
    return Fixed64::Mul(a, b);
}

template <typename I, typename = typename std::enable_if_t<::std::is_signed_v<I>>>
static int to_string(char* buf, uint32_t n, I value) {
    using UI = std::make_unsigned_t<I>;
    constexpr bool is_fixed32 = std::is_same_v<I, Fixed32::FP_INT>;
    constexpr UI int_part_shift = sizeof(I) * 4;
    constexpr UI frac_part_mask = std::numeric_limits<UI>::max() >> int_part_shift;
    constexpr UI int_part_scale = is_fixed32 ? 10000 : 1000000000;

    int size = 0;

    UI uvalue = (value >= 0) ? value : -value;
    if (value < 0)
        size = set_buffer(&buf, '-', size, n);

    /* Separate the integer and decimal parts of the value */
    UI intpart = uvalue >> int_part_shift;
    I fracpart = uvalue & frac_part_mask;
    I scale = int_part_scale * 10;
    fracpart = mul(fracpart, scale);

    if (fracpart >= scale) {
        /* Handle carry from decimal part */
        intpart++;
        fracpart -= scale;
    }

    /* Format integer part */
    size = itoa_loop(&buf, int_part_scale, intpart, size, n, true);

    /* Format decimal part (if any) */
    if (fracpart > 0) {
        size = set_buffer(&buf, '.', size, n);
        size = itoa_loop(&buf, scale / 10, fracpart, size, n, false);
    }

    if (n > 0) {
        buf[0] = '\0';
    }
    return size;
}

namespace Fixed32 {
FP_INT FromString(const char* buf, char** endptr) {
    return FromDouble(strtod(buf, endptr));
}

int ToString(char* buf, uint32_t n, FP_INT value) {
    return to_string(buf, n, value);
}
}  // namespace Fixed32

namespace Fixed64 {
FP_LONG FromString(const char* buf, char** endptr) {
    return FromDouble(strtod(buf, endptr));
}

int ToString(char* buf, uint32_t n, FP_LONG value) {
    return to_string(buf, n, value);
}
}  // namespace Fixed64

#ifdef LUA_FIXED48

FP_LONG FromString_FP48(const char* buf, char** endptr) {
    return FromDouble_FP48(strtod(buf, endptr));
}

int ToString_FP48(char* buf, uint32_t n, FP_LONG value) {
    int size = 0;

    FP_LONG uvalue = (value >= 0) ? value : Neg_FP48(value);
    if (value < 0)
        size = set_buffer(&buf, '-', size, n);

    /* Separate the integer and decimal parts of the value */
    FP_LONG intpart = (uvalue >> 32) & 0x00007FFF;
    FP_LONG fracpart = uvalue & 0xFFFFFFFF;
    FP_LONG scale = 10000000000;
    fracpart = Mul(fracpart, scale);

    if (fracpart >= scale) {
        /* Handle carry from decimal part */
        intpart++;
        fracpart -= scale;
    }

    /* Format integer part */
    size = itoa_loop(&buf, 10000, intpart, size, n, true);

    /* Format decimal part (if any) */
    if (fracpart > 0) {
        size = set_buffer(&buf, '.', size, n);
        size = itoa_loop(&buf, scale / 10, fracpart, size, n, false);
    }

    if (n > 0) {
        buf[MIN((uint32_t)size, n)] = '\0';
    }
    return size;
}
#endif