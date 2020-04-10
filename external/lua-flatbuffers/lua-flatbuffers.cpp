#include <vector>
#include "lua-flatbuffers.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

struct SizedString {
    size_t size;
    const char* string;
};

class BinaryArray {
public:
    BinaryArray(size_t size) { data.resize(size); memset(data.data(), 0, size); }
    BinaryArray(SizedString str) { data.resize(str.size); memcpy(data.data(), str.string, str.size); }

    SizedString Slice(size_t startPos, size_t endPos) {
        if (endPos > data.size()) endPos = data.size();
        return SizedString{ endPos - startPos, data.data() + startPos }; 
    }

    void Grow(size_t newSize) {
        size_t oldSize = data.size();
        size_t deltaSize = newSize - oldSize;
        data.resize(newSize);
        memmove(data.data() + deltaSize, data.data(), oldSize);
    }

    void Pad(size_t n, size_t startPos) {
        memset(data.data() + startPos, '\0', n);
    }

    void Set(SizedString value, size_t position) {
        memcpy(data.data() + position, value.string, value.size);
    }

    size_t Size() {
        return data.size();
    }

private:
    std::vector<char> data;
};

static int ba_slice(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 3) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2) && lua_isinteger(L, 3)) {
            BinaryArray* ba = (BinaryArray*)lua_touserdata(L, 1);
            size_t startPos = lua_tointeger(L, 2);
            size_t endPos = lua_tointeger(L, 3);
            SizedString ret = ba->Slice(startPos, endPos);
            lua_pushlstring(L, ret.string, ret.size);
            return 1;
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int ba_grow(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2)) {
            BinaryArray* ba = (BinaryArray*)lua_touserdata(L, 1);
            size_t newSize = lua_tointeger(L, 2);
            ba->Grow(newSize);
            return 0;
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int ba_pad(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 3) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2) && lua_isinteger(L, 3)) {
            BinaryArray* ba = (BinaryArray*)lua_touserdata(L, 1);
            size_t n = lua_tointeger(L, 2);
            size_t startPos = lua_tointeger(L, 3);
            ba->Pad(n, startPos);
            return 0;
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int ba_set(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 3) {
        if (lua_isuserdata(L, 1) && lua_isstring(L, 2) && lua_isinteger(L, 3)) {
            BinaryArray* ba = (BinaryArray*)lua_touserdata(L, 1);
            SizedString value;
            value.string = lua_tolstring(L, 2, &value.size);
            size_t position = lua_tointeger(L, 3);
            ba->Set(value, position);
            return 0;
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int ba_size(lua_State* L) {
    int n = lua_gettop(L);
    if (n >= 1) {
        if (lua_isuserdata(L, 1)) {
            BinaryArray* ba = (BinaryArray*)lua_touserdata(L, 1);
            size_t size = ba->Size();
            lua_pushinteger(L, size);
            return 1;
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

void set_binaryarray_mt(lua_State* L) {
    // assume stack top is a binaryarray
    if (luaL_newmetatable(L, "ba_mt")) {
        lua_newtable(L); // [ba, ba_mt, mt]

        lua_pushcfunction(L, ba_slice); // [ba, ba_mt, mt, ba_slice]
        lua_setfield(L, -2, "Slice"); // [ba, ba_mt, mt]

        lua_pushcfunction(L, ba_grow); // [ba, ba_mt, mt, ba_slice]
        lua_setfield(L, -2, "Grow"); // [ba, ba_mt, mt]

        lua_pushcfunction(L, ba_pad); // [ba, ba_mt, mt, ba_slice]
        lua_setfield(L, -2, "Pad"); // [ba, ba_mt, mt]

        lua_pushcfunction(L, ba_set); // [ba, ba_mt, mt, ba_slice]
        lua_setfield(L, -2, "Set"); // [ba, ba_mt, mt]

        lua_setfield(L, -2, "__index"); // [ba, ba_mt]

        lua_pushcfunction(L, ba_size); // [ba, ba_mt, ba_size]
        lua_setfield(L, -2, "__len"); // [ba, ba_mt]
    }

    lua_setmetatable(L, -2); // [ba]
}

static int new_binaryarray(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 1) {
        if (lua_isnumber(L, 1)) {
            size_t size = lua_tointeger(L, 1);
            void* ptr = lua_newuserdata(L, sizeof(BinaryArray));
            ptr = new(ptr) BinaryArray(size);
            set_binaryarray_mt(L);
            return 1;
        } else if (lua_isstring(L, 1)) {
            SizedString str;
            str.string = lua_tolstring(L, 1, &str.size);
            void* ptr = lua_newuserdata(L, sizeof(BinaryArray));
            ptr = new(ptr) BinaryArray(str);
            set_binaryarray_mt(L);
            return 1;
        }
    } 
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

extern "C" {

LUALIB_API int luaopen_flatbuffers(lua_State* L)
{
	lua_newtable(L); // [flatbuffers]

	lua_pushliteral(L, "flatbuffers"); // [flatbuffers, name]
	lua_setfield(L, -2, "_NAME"); // [flatbuffers]

	lua_pushliteral(L, "v0.1"); // [flatbuffers, version]
	lua_setfield(L, -2, "_VERSION"); // [flatbuffers]

	lua_pushcfunction(L, new_binaryarray); // [flatbuffers, new_binaryarray]
	lua_setfield(L, -2, "new_binaryarray"); // [flatbuffers]

	return 1;
}

}