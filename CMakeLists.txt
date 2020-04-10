cmake_minimum_required(VERSION 3.12)

project(lua-flatbuffers)

set(CMAKE_VERBOSE_MAKEFILE off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE on)
set(THREADS_PREFER_PTHREAD_FLAG on)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

option(enable_lua "Enable Lua" on)
option(enable_lua_fixed64 "Enable Fixed64 number in lua or luajit" on)
option(enable_lua_rapidjson "Enable lua-rapidjson" on)
option(enable_lua_flatbuffers "Enable lua-flatbuffers" on)

macro (print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach ()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro ()

# treat warnings as errors
if (__COMPILER_CLANG)
    # Clang
    set(LDFLAGS="${LDFLAGS} -llog")
    add_compile_options(-Werror -Wunused-variable -Wno-unused-function -Wreorder -Wunused-private-field -Woverloaded-virtual -Wunused-lambda-capture -Wunused-local-typedef)
    if (UNIX
        AND NOT ANDROID
        AND NOT IOS)
        # Linux or Mac
        add_compile_options(-mbmi2)
    endif ()
    if (ANDROID)
        add_compile_options(-fforce-enable-int128)
    endif ()
else ()
    if (MSVC)
        add_compile_options(/WX /EHsc /MP /sdl /utf-8 /wd4334 /Zc:__cplusplus)
    endif ()
endif ()

if (MSVC)
    add_compile_definitions(UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external/lua-5.3.5)
