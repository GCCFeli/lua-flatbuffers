project(lua CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(src ${CMAKE_CURRENT_BINARY_DIR})
include_directories(../fixed/include)
set(SRC_CORE
    src/lapi.c
    src/lcode.c
    src/lctype.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/llex.c
    src/lmem.c
    src/lobject.c
    src/lopcodes.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/ltable.c
    src/ltm.c
    src/lundump.c
    src/lvm.c
    src/lzio.c)
set(SRC_LIB
    src/lauxlib.c
    src/lbaselib.c
    src/lbitlib.c
    src/lcorolib.c
    src/ldblib.c
    src/liolib.c
    src/lmathlib.c
    src/loslib.c
    src/lstrlib.c
    src/ltablib.c
    src/lutf8lib.c
    src/loadlib.c
    src/linit.c
    ../fixed/src/fixed_str.cpp
    ../fixed/src/fixed_vector2.cpp)

set(SRC_LUA src/lua.c)
set(SRC_LUAC src/luac.c)

set_source_files_properties(${SRC_CORE} ${SRC_LIB} ${SRC_LUA} ${SRC_LUAC} PROPERTIES LANGUAGE CXX)

if (enable_lua_rapidjson)
    include_directories(../rapidjson/include)
    set(SRC_LUA_RAPIDJSON ../lua-rapidjson/src/Document.cpp ../lua-rapidjson/src/Schema.cpp ../lua-rapidjson/src/Userdata.hpp ../lua-rapidjson/src/file.hpp
                          ../lua-rapidjson/src/luax.hpp ../lua-rapidjson/src/rapidjson.cpp ../lua-rapidjson/src/values.cpp ../lua-rapidjson/src/values.hpp)
endif ()

if (enable_lua_flatbuffers)
    include_directories(../lua-flatbuffers)
    set(SRC_LUA_FLATBUFFERS ../lua-flatbuffers/lua-flatbuffers.cpp)
endif ()

add_library(liblua ${SRC_CORE} ${SRC_LIB} ${SRC_LUA_RAPIDJSON} ${SRC_LUA_FLATBUFFERS})
set_target_properties(liblua PROPERTIES OUTPUT_NAME lua)

if (UNIX)
    target_compile_options(liblua PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-x c++>)
    target_compile_definitions(liblua PUBLIC LUA_USE_POSIX)
endif ()

if (WIN32)
    target_compile_definitions(liblua PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS)
    add_executable(lua ${SRC_LUA})
    target_link_libraries(lua liblua)

    add_executable(luac ${SRC_LUAC})
    target_link_libraries(luac liblua)
    set_target_properties(luac PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
    add_custom_command(TARGET luac POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:luac> "${PROJECT_SOURCE_DIR}/../../tools/$<TARGET_FILE_NAME:luac>")
endif ()

if (enable_lua_fixed32)
    target_compile_definitions(liblua PUBLIC LUA_FIXED32 LUA_FIXED)
elseif (enable_lua_fixed64)
    target_compile_definitions(liblua PUBLIC LUA_FIXED64 LUA_FIXED)
endif ()

target_compile_definitions(liblua PUBLIC ENABLE_LUA_NATIVE_VECTOR2=1)

if (enable_lua_rapidjson)
    target_compile_definitions(liblua PUBLIC ENABLE_LUA_RAPIDJSON=1)
endif ()

if (enable_lua_flatbuffers)
    target_compile_definitions(liblua PUBLIC ENABLE_LUA_FLATBUFFERS=1)
endif ()

add_custom_command(TARGET lua POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/../../test/ $<TARGET_FILE_DIR:lua>/test/)

