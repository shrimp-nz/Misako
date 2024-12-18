// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lua.h"

#define luaL_error(L, fmt, ...) luaL_errorL(L, fmt, ##__VA_ARGS__)
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)

struct luaL_Reg
{
    const char* name;
    lua_CFunction func;
};
typedef struct luaL_Reg luaL_Reg;

LUALIB_API void luaL_register(misako_State* L, const char* libname, const luaL_Reg* l);
LUALIB_API int luaL_getmetafield(misako_State* L, int obj, const char* e);
LUALIB_API int luaL_callmeta(misako_State* L, int obj, const char* e);
LUALIB_API l_noret luaL_typeerrorL(misako_State* L, int narg, const char* tname);
LUALIB_API l_noret luaL_argerrorL(misako_State* L, int narg, const char* extramsg);
LUALIB_API const char* luaL_checklstring(misako_State* L, int numArg, size_t* l);
LUALIB_API const char* luaL_optlstring(misako_State* L, int numArg, const char* def, size_t* l);
LUALIB_API double luaL_checknumber(misako_State* L, int numArg);
LUALIB_API double luaL_optnumber(misako_State* L, int nArg, double def);

LUALIB_API int luaL_checkboolean(misako_State* L, int narg);
LUALIB_API int luaL_optboolean(misako_State* L, int narg, int def);

LUALIB_API int luaL_checkinteger(misako_State* L, int numArg);
LUALIB_API int luaL_optinteger(misako_State* L, int nArg, int def);
LUALIB_API unsigned luaL_checkunsigned(misako_State* L, int numArg);
LUALIB_API unsigned luaL_optunsigned(misako_State* L, int numArg, unsigned def);

LUALIB_API const float* luaL_checkvector(misako_State* L, int narg);
LUALIB_API const float* luaL_optvector(misako_State* L, int narg, const float* def);

LUALIB_API void luaL_checkstack(misako_State* L, int sz, const char* msg);
LUALIB_API void luaL_checktype(misako_State* L, int narg, int t);
LUALIB_API void luaL_checkany(misako_State* L, int narg);

LUALIB_API int luaL_newmetatable(misako_State* L, const char* tname);
LUALIB_API void* luaL_checkudata(misako_State* L, int ud, const char* tname);

LUALIB_API void luaL_where(misako_State* L, int lvl);
LUALIB_API LUA_PRINTF_ATTR(2, 3) l_noret luaL_errorL(misako_State* L, const char* fmt, ...);

LUALIB_API int luaL_checkoption(misako_State* L, int narg, const char* def, const char* const lst[]);

LUALIB_API const char* luaL_tolstring(misako_State* L, int idx, size_t* len);

LUALIB_API misako_State* luaL_newstate(void);

LUALIB_API const char* luaL_findtable(misako_State* L, int idx, const char* fname, int szhint);

LUALIB_API const char* luaL_typename(misako_State* L, int idx);

/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define luaL_argcheck(L, cond, arg, extramsg) ((void)((cond) ? (void)0 : luaL_argerror(L, arg, extramsg)))
#define luaL_argexpected(L, cond, arg, tname) ((void)((cond) ? (void)0 : luaL_typeerror(L, arg, tname)))

#define luaL_checkstring(L, n) (luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L, n, d) (luaL_optlstring(L, (n), (d), NULL))

#define luaL_getmetatable(L, n) (lua_getfield(L, LUA_REGISTRYINDEX, (n)))

#define luaL_opt(L, f, n, d) (lua_isnoneornil(L, (n)) ? (d) : f(L, (n)))

// generic buffer manipulation

struct luaL_Buffer
{
    char* p;   // current position in buffer
    char* end; // end of the current buffer
    misako_State* L;
    struct TString* storage;
    char buffer[LUA_BUFFERSIZE];
};
typedef struct luaL_Buffer luaL_Buffer;

// when internal buffer storage is exhausted, a mutable string value 'storage' will be placed on the stack
// in general, functions expect the mutable string buffer to be placed on top of the stack (top-1)
// with the exception of luaL_addvalue that expects the value at the top and string buffer further away (top-2)
// functions that accept a 'boxloc' support string buffer placement at any location in the stack
// all the buffer users we have in MisakoRVM match this pattern, but it's something to keep in mind for new uses of buffers

#define luaL_addchar(B, c) ((void)((B)->p < (B)->end || luaL_extendbuffer(B, 1, -1)), (*(B)->p++ = (char)(c)))
#define luaL_addstring(B, s) luaL_addlstring(B, s, strlen(s))

LUALIB_API void luaL_buffinit(misako_State* L, luaL_Buffer* B);
LUALIB_API char* luaL_buffinitsize(misako_State* L, luaL_Buffer* B, size_t size);
LUALIB_API char* luaL_extendbuffer(luaL_Buffer* B, size_t additionalsize, int boxloc);
LUALIB_API void luaL_reservebuffer(luaL_Buffer* B, size_t size, int boxloc);
LUALIB_API void luaL_addlstring(luaL_Buffer* B, const char* s, size_t l);
LUALIB_API void luaL_addvalue(luaL_Buffer* B);
LUALIB_API void luaL_pushresult(luaL_Buffer* B);
LUALIB_API void luaL_pushresultsize(luaL_Buffer* B, size_t size);

// builtin libraries
LUALIB_API int luaopen_base(misako_State* L);

#define LUA_COLIBNAME "coroutine"
LUALIB_API int luaopen_coroutine(misako_State* L);

#define LUA_TABLIBNAME "table"
LUALIB_API int luaopen_table(misako_State* L);

#define LUA_OSLIBNAME "os"
LUALIB_API int luaopen_os(misako_State* L);

#define LUA_STRLIBNAME "string"
LUALIB_API int luaopen_string(misako_State* L);

#define LUA_BITLIBNAME "bit32"
LUALIB_API int luaopen_bit32(misako_State* L);

#define LUA_UTF8LIBNAME "utf8"
LUALIB_API int luaopen_utf8(misako_State* L);

#define LUA_MATHLIBNAME "math"
LUALIB_API int luaopen_math(misako_State* L);

#define LUA_DBLIBNAME "debug"
LUALIB_API int luaopen_debug(misako_State* L);

// open all builtin libraries
LUALIB_API void luaL_openlibs(misako_State* L);

// sandbox libraries and globals
LUALIB_API void luaL_sandbox(misako_State* L);
LUALIB_API void luaL_sandboxthread(misako_State* L);
