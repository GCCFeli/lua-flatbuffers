#include <vector>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

struct SizedString {
    size_t size;
    const char* string;
};

class BinaryArray {
public:
    BinaryArray(uint32_t size) { data.resize(size); memset(data.data(), 0, size); }
    BinaryArray(SizedString str) { data.resize(str.size); memcpy(data.data(), str.string, str.size); }

    SizedString Slice(uint32_t startPos, uint32_t endPos) {
        if (startPos < 0) startPos = 0;
        if (startPos > data.size()) startPos = (uint32_t)data.size();
        if (endPos < startPos) endPos = startPos;
        if (endPos > data.size()) endPos = (uint32_t)data.size();
        return SizedString{ endPos - startPos, data.data() + startPos }; 
    }

    void Grow(uint32_t newSize) {
        if (newSize < data.size()) return; // invalid args
        uint32_t oldSize = (uint32_t)data.size();
        uint32_t deltaSize = newSize - oldSize;
        data.resize(newSize);
        memmove(data.data() + deltaSize, data.data(), oldSize);
    }

    void Pad(uint32_t n, uint32_t startPos) {
        if (startPos < 0 || startPos > data.size()) return; // invalid args
        if (startPos + n > data.size()) n = (uint32_t)data.size() - startPos;
        memset(data.data() + startPos, '\0', n);
    }

    void Set(SizedString value, uint32_t position) {
        if (position < 0 || position > data.size()) return; // invalid args
        if (position + value.size > data.size()) return; // invalid args
        memcpy(data.data() + position, value.string, value.size);
    }

    uint32_t Size() {
        return (uint32_t)data.size();
    }

    char* Data(uint32_t position) {
        if (position < 0 || position + 1 > data.size()) return nullptr; // invalid args
        return data.data() + position;
    }

    std::vector<char> data;
};

class View {
public:
    View(BinaryArray* binaryArray, uint32_t position) : binaryArray(binaryArray), position(position) {}

    uint32_t Offset(uint32_t vtableOffset) {
        uint32_t vtable = position - Get<int32_t>(position);
        uint32_t vtableEnd = Get<uint16_t>(vtable);
        return vtableOffset < vtableEnd ? Get<uint16_t>(vtable + vtableOffset) : 0;
    }

    uint32_t Indirect(uint32_t offset) {
        return offset + UnpackUInt32(offset);
    }

    SizedString String(uint32_t offset) {
        offset = Indirect(offset);
        uint32_t start = offset + sizeof(uint32_t);
        uint32_t length = UnpackUInt32(offset);
        return binaryArray->Slice(start, start + length);
    }

    uint32_t VectorLen(uint32_t offset) {
        return UnpackUInt32(Indirect(offset + position));
    }

    uint32_t Vector(uint32_t offset) {
        offset += position;
        uint32_t x = offset + Get<uint32_t>(offset);
        x += sizeof(uint32_t);
        return x;
    }
    
    void Union(View* v2, uint32_t offset) {
        offset += position;
        v2->position = offset + Get<uint32_t>(offset);
        v2->binaryArray = binaryArray;
    }

    template<typename T>
    T Get(uint32_t offset) {
        return *((T*)binaryArray->Data(offset));
    }

    uint32_t UnpackUInt32(uint32_t offset) {
        return *((uint32_t*)binaryArray->Data(offset));
    }

    BinaryArray* binaryArray;
    uint32_t position;
};

class NumType {
public:
    NumType(const char* name, int bytewidth, const char* packFmt) : name(name), bytewidth(bytewidth), packFmt(packFmt) {}
    const char* name;
    int bytewidth;
    const char* packFmt;
};

struct BinaryArrayRef {
    BinaryArray* ptr;
    bool is_owner;
};

BinaryArray* check_binaryarray(lua_State* L, int n) {
    return ((BinaryArrayRef*)luaL_checkudata(L, n, "ba_mt"))->ptr;
}

View* check_view(lua_State* L, int n) {
    return *(View**)luaL_checkudata(L, n, "view_mt");
}

NumType* check_num_type(lua_State* L, int n, const char* name) {
    return *(NumType**)luaL_checkudata(L, n, name);
}

NumType* test_num_type(lua_State* L, int n, const char* name) {
    void* userdata = luaL_testudata(L, n, name);
    if (userdata != nullptr) return *(NumType**)userdata;
    return nullptr;
}

static int num_type_unpack(lua_State* L) {
    BinaryArray* ba = check_binaryarray(L, 2);
    uint32_t offset = (uint32_t)luaL_checkinteger(L, 3);
    void* ptr = ba->Data(offset);

    NumType* num_type = nullptr;
    if (num_type = test_num_type(L, 1, "Bool")) {
        lua_pushinteger(L, *((uint8_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Uint8")) {
        lua_pushinteger(L, *((uint8_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Uint16")) {
        lua_pushinteger(L, *((uint16_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Uint32")) {
        lua_pushinteger(L, *((uint32_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Uint64")) {
        lua_pushinteger(L, *((uint64_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Int8")) {
        lua_pushinteger(L, *((int8_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Int16")) {
        lua_pushinteger(L, *((int16_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Int32")) {
        lua_pushinteger(L, *((int32_t*)ptr));
        return 1;
    } else if (num_type = test_num_type(L, 1, "Int64")) {
        lua_pushinteger(L, *((int64_t*)ptr));
        return 1;
    }

    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int num_type_gc(lua_State* L) {
    NumType* num_type = nullptr;
    if ((num_type = test_num_type(L, 1, "Bool")) ||
        (num_type = test_num_type(L, 1, "Uint8")) ||
        (num_type = test_num_type(L, 1, "Uint16")) ||
        (num_type = test_num_type(L, 1, "Uint32")) ||
        (num_type = test_num_type(L, 1, "Uint64")) ||
        (num_type = test_num_type(L, 1, "Int8")) ||
        (num_type = test_num_type(L, 1, "Int16")) ||
        (num_type = test_num_type(L, 1, "Int32")) ||
        (num_type = test_num_type(L, 1, "Int64"))) {
        delete num_type;
    }
    return 0;
}

static void register_num_type(lua_State* L, const char* name, NumType* num_type) {
    luaL_Reg num_type_reg[] = {
        { "Unpack", num_type_unpack },
        { "__gc", num_type_gc },
        { nullptr, nullptr }
    };

    luaL_newmetatable(L, name); // [mt]
    luaL_setfuncs(L, num_type_reg, 0); // [mt]

    lua_pushinteger(L, num_type->bytewidth); // [mt, bytewidth]
    lua_setfield(L, -2, "bytewidth"); // [mt]

    lua_pushstring(L, num_type->packFmt); // [mt, packFmt]
    lua_setfield(L, -2, "packFmt"); // [mt]

    lua_setfield(L, -1, "__index"); // []
}

static void num_type_new(lua_State* L, const char* name, int bytewidth, const char* packFmt) {
    // assume stack top is a table of num_type [num_type_table]
    NumType** udata = (NumType**)lua_newuserdata(L, sizeof(NumType*));
    *udata = new NumType(name, bytewidth, packFmt);
    register_num_type(L, name, *udata);
    luaL_getmetatable(L, name);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, name); // [num_type_table]
}

static void new_num_types_table(lua_State* L) {
    lua_newtable(L);

    num_type_new(L, "Bool", 1, "<b");
    num_type_new(L, "Uint8", 1, "<I1");
    num_type_new(L, "Uint16", 2, "<I2");
    num_type_new(L, "Uint32", 4, "<I4");
    num_type_new(L, "Uint64", 8, "<I8");
    num_type_new(L, "Int8", 1, "<i1");
    num_type_new(L, "Int16", 2, "<i2");
    num_type_new(L, "Int32", 4, "<i4");
    num_type_new(L, "Int64", 8, "<i8");

    lua_pushliteral(L, "Uint32");
    lua_gettable(L, -2);
    lua_setfield(L, -2, "UOffsetT");

    lua_pushliteral(L, "Uint16");
    lua_gettable(L, -2);
    lua_setfield(L, -2, "VOffsetT");

    lua_pushliteral(L, "Int32");
    lua_gettable(L, -2);
    lua_setfield(L, -2, "SOffsetT");
}

static int ba_new(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t size = (uint32_t)lua_tointeger(L, 1);
        BinaryArrayRef* ba_ref = (BinaryArrayRef*)lua_newuserdata(L, sizeof(BinaryArrayRef));
        ba_ref->ptr = new BinaryArray(size);
        ba_ref->is_owner = true;
        luaL_getmetatable(L, "ba_mt");
        lua_setmetatable(L, -2);
        return 1;
    } else if (lua_isstring(L, 1)) {
        SizedString str;
        str.string = lua_tolstring(L, 1, &str.size);
        BinaryArrayRef* ba_ref = (BinaryArrayRef*)lua_newuserdata(L, sizeof(BinaryArrayRef));
        ba_ref->ptr = new BinaryArray(str);
        ba_ref->is_owner = true;
        luaL_getmetatable(L, "ba_mt");
        lua_setmetatable(L, -2);
        return 1;
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int ba_slice(lua_State* L) {
    BinaryArray* ba = check_binaryarray(L, 1);
    uint32_t startPos = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t endPos = (uint32_t)luaL_checkinteger(L, 3);
    SizedString ret = ba->Slice(startPos, endPos);
    lua_pushlstring(L, ret.string, ret.size);
    return 1;
}

static int ba_grow(lua_State* L) {
    BinaryArray* ba = check_binaryarray(L, 1);
    uint32_t newSize = (uint32_t)luaL_checkinteger(L, 2);
    ba->Grow(newSize);
    return 0;
}

static int ba_pad(lua_State* L) {
    BinaryArray* ba = check_binaryarray(L, 1);
    uint32_t n = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t startPos = (uint32_t)luaL_checkinteger(L, 3);
    ba->Pad(n, startPos);
    return 0;
}

static int ba_set(lua_State* L) {
    BinaryArray* ba = check_binaryarray(L, 1);
    SizedString value;
    value.string = luaL_checklstring(L, 2, &value.size);
    uint32_t position = (uint32_t)luaL_checkinteger(L, 3);
    ba->Set(value, position);
    return 0;
}

static int ba_size(lua_State* L) {
    BinaryArray* ba = check_binaryarray(L, 1);
    uint32_t size = ba->Size();
    lua_pushinteger(L, size);
    return 1;
}

static int ba_gc(lua_State* L) {
    BinaryArrayRef* ba_ref = ((BinaryArrayRef*)luaL_checkudata(L, 1, "ba_mt"));
    if (ba_ref->is_owner) delete ba_ref->ptr;
    return 0;
}

static void register_binaryarray(lua_State* L) {
    luaL_Reg binaryarray_reg[] = {
        { "Slice", ba_slice },
        { "Grow", ba_grow },
        { "Pad", ba_pad },
        { "Set", ba_set },
        { "__len", ba_size },
        { "__gc", ba_gc },
        { nullptr, nullptr }
    };

    luaL_newmetatable(L, "ba_mt");
    luaL_setfuncs(L, binaryarray_reg, 0);
    lua_setfield(L, -1, "__index");
}

static int view_new(lua_State* L) {
    BinaryArray* ba = check_binaryarray(L, 1);
    uint32_t position = (uint32_t)luaL_checkinteger(L, 2);
    View** udata = (View**)lua_newuserdata(L, sizeof(View*));
    *udata = new View(ba, position);
    luaL_getmetatable(L, "view_mt");
    lua_setmetatable(L, -2);
    return 1;
}

static int view_offset(lua_State* L) {
    View* view = check_view(L, 1);
    uint32_t vtableOffset = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t ret = view->Offset(vtableOffset);
    lua_pushinteger(L, ret);
    return 1;
}

static int view_indirect(lua_State* L) {
    View* view = check_view(L, 1);
    uint32_t offset = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t ret = view->Indirect(offset);
    lua_pushinteger(L, ret);
    return 1;
}

static int view_string(lua_State* L) {
    View* view = check_view(L, 1);
    uint32_t offset = (uint32_t)luaL_checkinteger(L, 2);
    SizedString ret = view->String(offset);
    lua_pushlstring(L, ret.string, ret.size);
    return 1;
}

static int view_vector_len(lua_State* L) {
    View* view = check_view(L, 1);
    uint32_t offset = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t ret = view->VectorLen(offset);
    lua_pushinteger(L, ret);
    return 1;
}

static int view_vector(lua_State* L) {
    View* view = check_view(L, 1);
    uint32_t offset = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t ret = view->Vector(offset);
    lua_pushinteger(L, ret);
    return 1;
}

static int view_union(lua_State* L) {
    View* view = check_view(L, 1);
    View* view2 = check_view(L, 2);
    uint32_t offset = (uint32_t)luaL_checkinteger(L, 3);
    view->Union(view2, offset);
    return 0;
}

static int view_get(lua_State* L) {
    View* view = check_view(L, 1);
    uint32_t offset = (uint32_t)luaL_checkinteger(L, 3);

    NumType* num_type = nullptr;
    if (num_type = test_num_type(L, 2, "Bool")) {
        lua_pushinteger(L, view->Get<uint8_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Uint8")) {
        lua_pushinteger(L, view->Get<uint8_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Uint16")) {
        lua_pushinteger(L, view->Get<uint16_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Uint32")) {
        lua_pushinteger(L, view->Get<uint32_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Uint64")) {
        lua_pushinteger(L, view->Get<uint64_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Int8")) {
        lua_pushinteger(L, view->Get<int8_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Int16")) {
        lua_pushinteger(L, view->Get<int16_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Int32")) {
        lua_pushinteger(L, view->Get<int32_t>(offset));
        return 1;
    } else if (num_type = test_num_type(L, 2, "Int64")) {
        lua_pushinteger(L, view->Get<int64_t>(offset));
        return 1;
    }

    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_index(lua_State* L) {
    View* view = check_view(L, 1);
    const char* key = luaL_checkstring(L, 2);
    if (strcmp(key, "pos") == 0) {
        lua_pushinteger(L, view->position);
        return 1;
    } else if (strcmp(key, "bytes") == 0) {
        BinaryArrayRef* ba = (BinaryArrayRef*)lua_newuserdata(L, sizeof(BinaryArrayRef));
        ba->ptr = view->binaryArray;
        ba->is_owner = false;
        luaL_getmetatable(L, "ba_mt");
        lua_setmetatable(L, -2);
        return 1;
    } else {
        luaL_getmetatable(L, "view_mt_mt");
        lua_getfield(L, -1, key);
        lua_replace(L, -2);
        return 1;
    }
}

static int view_gc(lua_State* L) {
    View* view = check_view(L, 1);
    delete view;
    return 0;
}

static void register_view(lua_State* L) {
    luaL_Reg view_mt_mt_reg[] = {
        { "Offset", view_offset },
        { "Indirect", view_indirect },
        { "String", view_string },
        { "VectorLen", view_vector_len },
        { "Vector", view_vector },
        { "Union", view_union },
        { "Get", view_get },
        { nullptr, nullptr }
    };

    luaL_Reg view_mt_reg[] = {
        { "Offset", view_offset },
        { "Indirect", view_indirect },
        { "String", view_string },
        { "VectorLen", view_vector_len },
        { "Vector", view_vector },
        { "Union", view_union },
        { "Get", view_get },
        { "__index", view_index },
        { "__gc", view_gc },
        { nullptr, nullptr }
    };

    luaL_newmetatable(L, "view_mt_mt");
    luaL_setfuncs(L, view_mt_mt_reg, 0);
    lua_pop(L, 1);

    luaL_newmetatable(L, "view_mt");
    luaL_setfuncs(L, view_mt_reg, 0);
    lua_pop(L, 1);
}

extern "C" {

LUALIB_API int luaopen_flatbuffers(lua_State* L)
{
    register_binaryarray(L);
    register_view(L);

	lua_newtable(L); // [flatbuffers]

	lua_pushliteral(L, "flatbuffers"); // [flatbuffers, name]
	lua_setfield(L, -2, "_NAME"); // [flatbuffers]

	lua_pushliteral(L, "v0.1"); // [flatbuffers, version]
	lua_setfield(L, -2, "_VERSION"); // [flatbuffers]

	lua_pushcfunction(L, ba_new); // [flatbuffers, new_binaryarray]
	lua_setfield(L, -2, "new_binaryarray"); // [flatbuffers]

	new_num_types_table(L); // [flatbuffers, num_types]
	lua_setfield(L, -2, "N"); // [flatbuffers]

	lua_pushcfunction(L, view_new); // [flatbuffers, new_view]
	lua_setfield(L, -2, "new_view"); // [flatbuffers]

	return 1;
}

}
