#include <vector>
#include "lua-flatbuffers.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

struct SizedString {
    uint32_t size;
    const char* string;
};

template<typename T>
T* get_typed_userdata(lua_State* L, int idx, const char* name) {
    if (lua_isuserdata(L, idx)) {
        if (lua_getmetatable(L, idx)) {
            if (lua_istable(L, -1)) {
                lua_pushliteral(L, "__metatable");
                int type = lua_gettable(L, -2);
                if (lua_isstring(L, -1)) {
                    size_t len;
                    const char* str = lua_tolstring(L, -1, &len);
                    if (strncmp(str, name, len) == 0) {
                        lua_pop(L, 2);
                        return (T*)lua_touserdata(L, idx);
                    }
                }
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
        }
    }
    return nullptr;
}

class BinaryArray {
public:
    BinaryArray(uint32_t size) { data.resize(size); memset(data.data(), 0, size); }
    BinaryArray(SizedString str) { data.resize(str.size); memcpy(data.data(), str.string, str.size); }

    SizedString Slice(uint32_t startPos, uint32_t endPos) {
        if (endPos > data.size()) endPos = (uint32_t)data.size();
        return SizedString{ endPos - startPos, data.data() + startPos }; 
    }

    void Grow(uint32_t newSize) {
        uint32_t oldSize = (uint32_t)data.size();
        uint32_t deltaSize = newSize - oldSize;
        data.resize(newSize);
        memmove(data.data() + deltaSize, data.data(), oldSize);
    }

    void Pad(uint32_t n, uint32_t startPos) {
        memset(data.data() + startPos, '\0', n);
    }

    void Set(SizedString value, uint32_t position) {
        memcpy(data.data() + position, value.string, value.size);
    }

    uint32_t Size() {
        return (uint32_t)data.size();
    }

    char* Data(uint32_t position) {
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

static int num_type_unpack(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 3) {
        if (lua_isuserdata(L, 1) && lua_isuserdata(L, 2) && lua_isinteger(L, 3)) {
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 2, "ba_mt");
            if (ba) {
                uint32_t offset = (uint32_t)lua_tointeger(L, 3);
                void* ptr = ba->Data(offset);

                if (get_typed_userdata<NumType>(L, 1, "Bool")) {
                    lua_pushboolean(L, *((bool*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Uint8")) {
                    lua_pushinteger(L, *((uint8_t*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Uint16")) {
                    lua_pushinteger(L, *((uint16_t*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Uint32")) {
                    lua_pushinteger(L, *((uint32_t*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Uint64")) {
                    lua_pushinteger(L, *((uint64_t*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Int8")) {
                    lua_pushinteger(L, *((int8_t*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Int16")) {
                    lua_pushinteger(L, *((int16_t*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Int32")) {
                    lua_pushinteger(L, *((int32_t*)ptr));
                    return 1;
                } else if (get_typed_userdata<NumType>(L, 1, "Int64")) {
                    lua_pushinteger(L, *((int64_t*)ptr));
                    return 1;
                }
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int num_type_gc(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 1) {
        if (lua_isuserdata(L, 1)) {
            NumType* num_type = get_typed_userdata<NumType>(L, 1, "num_type_mt");
            if (num_type) {
                num_type->~NumType();
                return 0;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static void set_num_type_mt(lua_State* L, const char* name, NumType* num_type) {
    // assume stack top is a num_type
    if (luaL_newmetatable(L, name)) {
        lua_newtable(L); // [num_type, num_type_mt, mt]

        lua_pushinteger(L, num_type->bytewidth); // [num_type, num_type_mt, mt, byte_width]
        lua_setfield(L, -2, "bytewidth"); // [num_type, num_type_mt, mt]

        lua_pushstring(L, num_type->packFmt); // [num_type, num_type_mt, mt, pack_fmt]
        lua_setfield(L, -2, "packFmt"); // [num_type, num_type_mt, mt]

        lua_pushcfunction(L, num_type_unpack); // [num_type, num_type_mt, mt, num_type_unpack]
        lua_setfield(L, -2, "Unpack"); // [num_type, num_type_mt, mt]

        lua_setfield(L, -2, "__index"); // [num_type, num_type_mt]

        lua_pushcfunction(L, num_type_gc); // [num_type, num_type_mt, num_type_gc]
        lua_setfield(L, -2, "__gc"); // [num_type, num_type_mt]

        lua_pushstring(L, name); // [num_type, num_type_mt, name]
        lua_setfield(L, -2, "__metatable"); // [num_type, num_type_mt]
    }

    lua_setmetatable(L, -2); // [num_type]
}

static void new_num_types_table(lua_State* L) {
    lua_newtable(L);

    NumType* Bool = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Bool = new(Bool)NumType("Bool", 1, "<b");
    set_num_type_mt(L, "Bool", Bool);
    lua_setfield(L, -2, "Bool");

    NumType* Uint8 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Uint8 = new(Uint8)NumType("Uint8",1, "<I1");
    set_num_type_mt(L, "Uint8", Uint8);
    lua_setfield(L, -2, "Uint8");

    NumType* Uint16 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Uint16 = new(Uint16)NumType("Uint16",2, "<I2");
    set_num_type_mt(L, "Uint16", Uint16);
    lua_setfield(L, -2, "Uint16");

    NumType* Uint32 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Uint32 = new(Uint32)NumType("Uint32",4, "<I4");
    set_num_type_mt(L, "Uint32", Uint32);
    lua_setfield(L, -2, "Uint32");

    NumType* Uint64 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Uint64 = new(Uint64)NumType("Uint64",8, "<I8");
    set_num_type_mt(L, "Uint64", Uint64);
    lua_setfield(L, -2, "Uint64");

    NumType* Int8 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Int8 = new(Int8)NumType("Int8",1, "<i1");
    set_num_type_mt(L, "Int8", Int8);
    lua_setfield(L, -2, "Int8");

    NumType* Int16 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Int16 = new(Int16)NumType("Int16",2, "<i2");
    set_num_type_mt(L, "Int16", Int16);
    lua_setfield(L, -2, "Int16");

    NumType* Int32 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Int32 = new(Int32)NumType("Int32",4, "<i4");
    set_num_type_mt(L, "Int32", Int32);
    lua_setfield(L, -2, "Int32");

    NumType* Int64 = (NumType*)lua_newuserdata(L, sizeof(NumType));
    Int64 = new(Int64)NumType("Int64",8, "<i8");
    set_num_type_mt(L, "Int64", Int64);
    lua_setfield(L, -2, "Int64");

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

static int ba_slice(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 3) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2) && lua_isinteger(L, 3)) {
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 1, "ba_mt");
            if (ba) {
                uint32_t startPos = (uint32_t)lua_tointeger(L, 2);
                uint32_t endPos = (uint32_t)lua_tointeger(L, 3);
                SizedString ret = ba->Slice(startPos, endPos);
                lua_pushlstring(L, ret.string, ret.size);
                return 1;
            }
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
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 1, "ba_mt");
            if (ba) {
                uint32_t newSize = (uint32_t)lua_tointeger(L, 2);
                ba->Grow(newSize);
                return 0;
            }
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
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 1, "ba_mt");
            if (ba) {
                uint32_t n = (uint32_t)lua_tointeger(L, 2);
                uint32_t startPos = (uint32_t)lua_tointeger(L, 3);
                ba->Pad(n, startPos);
                return 0;
            }
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
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 1, "ba_mt");
            if (ba) {
                SizedString value;
                size_t size;
                value.string = lua_tolstring(L, 2, &size);
                value.size = (uint32_t)size;
                uint32_t position = (uint32_t)lua_tointeger(L, 3);
                ba->Set(value, position);
                return 0;
            }
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
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 1, "ba_mt");
            if (ba) {
                uint32_t size = ba->Size();
                lua_pushinteger(L, size);
                return 1;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int ba_gc(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 1) {
        if (lua_isuserdata(L, 1)) {
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 1, "ba_mt");
            if (ba) {
                ba->~BinaryArray();
                return 0;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static void set_binaryarray_mt(lua_State* L) {
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

        lua_pushcfunction(L, ba_gc); // [ba, ba_mt, ba_gc]
        lua_setfield(L, -2, "__gc"); // [ba, ba_mt]

        lua_pushliteral(L, "ba_mt"); // [ba, ba_mt, "ba_mt"]
        lua_setfield(L, -2, "__metatable"); // [ba, ba_mt]
    }

    lua_setmetatable(L, -2); // [ba]
}

static int new_binaryarray(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 1) {
        if (lua_isnumber(L, 1)) {
            uint32_t size = (uint32_t)lua_tointeger(L, 1);
            void* ptr = lua_newuserdata(L, sizeof(BinaryArray));
            ptr = new(ptr) BinaryArray(size);
            set_binaryarray_mt(L);
            return 1;
        } else if (lua_isstring(L, 1)) {
            SizedString str;
            size_t len;
            str.string = lua_tolstring(L, 1, &len);
            str.size = (uint32_t)len;
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

static int view_offset(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                uint32_t vtableOffset = (uint32_t)lua_tointeger(L, 2);
                uint32_t ret = view->Offset(vtableOffset);
                lua_pushinteger(L, ret);
                return 1;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_indirect(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                uint32_t offset = (uint32_t)lua_tointeger(L, 2);
                uint32_t ret = view->Indirect(offset);
                lua_pushinteger(L, ret);
                return 1;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_string(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                uint32_t offset = (uint32_t)lua_tointeger(L, 2);
                SizedString ret = view->String(offset);
                lua_pushlstring(L, ret.string, ret.size);
                return 1;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_vector_len(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                uint32_t offset = (uint32_t)lua_tointeger(L, 2);
                uint32_t ret = view->VectorLen(offset);
                lua_pushinteger(L, ret);
                return 1;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_vector(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isinteger(L, 2)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                uint32_t offset = (uint32_t)lua_tointeger(L, 2);
                uint32_t ret = view->Vector(offset);
                lua_pushinteger(L, ret);
                return 1;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_union(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 3) {
        if (lua_isuserdata(L, 1) && lua_isuserdata(L, 2) && lua_isinteger(L, 3)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            View* view2 = get_typed_userdata<View>(L, 2, "view_mt");
            if (view && view2) {
                uint32_t offset = (uint32_t)lua_tointeger(L, 3);
                view->Union(view2, offset);
                return 0;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_get(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 3) {
        if (lua_isuserdata(L, 1) && lua_isuserdata(L, 2) && lua_isinteger(L, 3)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                NumType* type = (NumType*)lua_touserdata(L, 2);
                uint32_t offset = (uint32_t)lua_tointeger(L, 3);
                if (strcmp(type->name, "Bool") == 0) {
                    lua_pushboolean(L, view->Get<bool>(offset));
                    return 1;
                } else if (strcmp(type->name, "Uint8") == 0) {
                    lua_pushinteger(L, view->Get<uint8_t>(offset));
                    return 1;
                } else if (strcmp(type->name, "Uint16") == 0) {
                    lua_pushinteger(L, view->Get<uint16_t>(offset));
                    return 1;
                } else if (strcmp(type->name, "Uint32") == 0) {
                    lua_pushinteger(L, view->Get<uint32_t>(offset));
                    return 1;
                } else if (strcmp(type->name, "Uint64") == 0) {
                    lua_pushinteger(L, view->Get<uint64_t>(offset));
                    return 1;
                } else if (strcmp(type->name, "Int8") == 0) {
                    lua_pushinteger(L, view->Get<int8_t>(offset));
                    return 1;
                } else if (strcmp(type->name, "Int16") == 0) {
                    lua_pushinteger(L, view->Get<int16_t>(offset));
                    return 1;
                } else if (strcmp(type->name, "Int32") == 0) {
                    lua_pushinteger(L, view->Get<int32_t>(offset));
                    return 1;
                } else if (strcmp(type->name, "Int64") == 0) {
                    lua_pushinteger(L, view->Get<int64_t>(offset));
                    return 1;
                }
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_index(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isstring(L, 2)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                const char* key = lua_tostring(L, 2);
                if (strcmp(key, "pos") == 0) {
                    lua_pushinteger(L, view->position);
                    return 1;
                } else if (strcmp(key, "bytes") == 0) {
                    //TODO
                    lua_pushlightuserdata(L, view->binaryArray);
                    return 1;
                }
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static int view_gc(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 1) {
        if (lua_isuserdata(L, 1)) {
            View* view = get_typed_userdata<View>(L, 1, "view_mt");
            if (view) {
                view->~View();
                return 0;
            }
        }
    }
    lua_pushliteral(L, "incorrect argument");
    lua_error(L);
    return 0;
}

static void set_view_mt(lua_State* L) {
    // assume stack top is a view
    if (luaL_newmetatable(L, "view_mt")) {
        lua_newtable(L); // [view, view_mt, mt]

        lua_pushcfunction(L, view_offset); // [view, view_mt, mt, view_offset]
        lua_setfield(L, -2, "Offset"); // [view, view_mt, mt]

        lua_pushcfunction(L, view_indirect); // [view, view_mt, mt, view_indirect]
        lua_setfield(L, -2, "Indirect"); // [view, view_mt, mt]

        lua_pushcfunction(L, view_string); // [view, view_mt, mt, view_string]
        lua_setfield(L, -2, "String"); // [view, view_mt, mt]

        lua_pushcfunction(L, view_vector_len); // [view, view_mt, mt, view_vector_len]
        lua_setfield(L, -2, "VectorLen"); // [view, view_mt, mt]

        lua_pushcfunction(L, view_vector); // [view, view_mt, mt, view_vector]
        lua_setfield(L, -2, "Vector"); // [view, view_mt, mt]

        lua_pushcfunction(L, view_union); // [view, view_mt, mt, view_union]
        lua_setfield(L, -2, "Union"); // [view, view_mt, mt]

        lua_pushcfunction(L, view_get); // [view, view_mt, mt, view_get]
        lua_setfield(L, -2, "Get"); // [view, view_mt, mt]

        lua_pushcfunction(L, view_index); // [view, view_mt, mt, view_index]
        lua_setfield(L, -2, "__index"); // [view, view_mt]

        lua_pushcfunction(L, view_gc); // [view, view_mt, view_gc]
        lua_setfield(L, -2, "__gc"); // [view, view_mt]

        lua_pushliteral(L, "view_mt"); // [view, view_mt, "view_mt"]
        lua_setfield(L, -2, "__metatable"); // [view, view_mt]
    }

    lua_setmetatable(L, -2); // [view]
}

static int new_view(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 2) {
        if (lua_isuserdata(L, 1) && lua_isnumber(L, 2)) {
            BinaryArray* ba = get_typed_userdata<BinaryArray>(L, 1, "ba_mt");
            if (ba) {
                uint32_t position = (uint32_t)lua_tointeger(L, 2);
                
                void* ptr = lua_newuserdata(L, sizeof(View));
                ptr = new(ptr) View(ba, position);
                set_view_mt(L);
                return 1;
            }
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

	new_num_types_table(L); // [flatbuffers, num_types]
	lua_setfield(L, -2, "N"); // [flatbuffers]

	lua_pushcfunction(L, new_view); // [flatbuffers, new_view]
	lua_setfield(L, -2, "new_view"); // [flatbuffers]

	return 1;
}

}