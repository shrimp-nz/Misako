//// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
//// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
//#pragma once
//
//#include <stdarg.h>
//#include <stddef.h>
//#include <stdint.h>
//
//#include "luaconf.h"
//
//
//
//
//// option for multiple returns in `lua_pcall' and `lua_call'
//#define LUA_MULTRET (-1)
//
///*
//** pseudo-indices
//*/
//#define LUA_REGISTRYINDEX (-10000)
//#define LUA_ENVIRONINDEX (-10001)
//#define LUA_GLOBALSINDEX (-10002)
//#define lua_upvalueindex(i) (LUA_GLOBALSINDEX - (i))
//#define lua_ispseudo(i) ((i) <= LUA_REGISTRYINDEX)
//
//// thread status; 0 is OK
//enum lua_Status
//{
//    LUA_OK = 0,
//    LUA_YIELD,
//    LUA_ERRRUN,
//    LUA_ERRSYNTAX,
//    LUA_ERRMEM,
//    LUA_ERRERR,
//    LUA_BREAK, // yielded for a debug breakpoint
//};
//
//typedef struct misako_State misako_State;
//
//typedef int (*lua_CFunction)(misako_State* L);
//typedef int (*lua_Continuation)(misako_State* L, int status);
//
///*
//** prototype for memory-allocation functions
//*/
//
//typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);
//
//// non-return type
//#define l_noret void LUA_NORETURN
//
///*
//** basic types
//*/
//#define LUA_TNONE (-1)
//
///*
// * WARNING: if you change the order of this enumeration,
// * grep "ORDER TYPE"
// */
//// clang-format off
//enum lua_Type
//{
//    LUA_TNIL = 0,     // must be 0 due to lua_isnoneornil
//    LUA_TBOOLEAN = 1, // must be 1 due to l_isfalse
//
//
//    LUA_TLIGHTUSERDATA,
//    LUA_TNUMBER,
//    LUA_TVECTOR,
//
//    LUA_TSTRING, // all types above this must be value types, all types below this must be GC types - see iscollectable
//
//
//    LUA_TTABLE,
//    LUA_TFUNCTION,
//    LUA_TUSERDATA,
//    LUA_TTHREAD,
//
//    // values below this line are used in GCObject tags but may never show up in TValue type tags
//    LUA_TPROTO,
//    LUA_TUPVAL,
//    LUA_TDEADKEY,
//
//    // the count of TValue type tags
//    LUA_T_COUNT = LUA_TPROTO
//};
//// clang-format on
//
//// type of numbers in MisakoRVM
//typedef double lua_Number;
//
//// type for integer functions
//typedef int lua_Integer;
//
//// unsigned integer type
//typedef unsigned lua_Unsigned;
//
///*
//** state manipulation
//*/
//LUA_API misako_State* lua_newstate(lua_Alloc f, void* ud);
//LUA_API void lua_close(misako_State* L);
//LUA_API misako_State* lua_newthread(misako_State* L);
//LUA_API misako_State* lua_mainthread(misako_State* L);
//LUA_API void lua_resetthread(misako_State* L);
//LUA_API int lua_isthreadreset(misako_State* L);
//
///*
//** basic stack manipulation
//*/
//LUA_API int lua_absindex(misako_State* L, int idx);
//LUA_API int lua_gettop(misako_State* L);
//LUA_API void lua_settop(misako_State* L, int idx);
//LUA_API void lua_pushvalue(misako_State* L, int idx);
//LUA_API void lua_remove(misako_State* L, int idx);
//LUA_API void lua_insert(misako_State* L, int idx);
//LUA_API void lua_replace(misako_State* L, int idx);
//LUA_API int lua_checkstack(misako_State* L, int sz);
//LUA_API void lua_rawcheckstack(misako_State* L, int sz); // allows for unlimited stack frames
//
//LUA_API void lua_xmove(misako_State* from, misako_State* to, int n);
//LUA_API void lua_xpush(misako_State* from, misako_State* to, int idx);
//
///*
//** access functions (stack -> C)
//*/
//
//LUA_API int lua_isnumber(misako_State* L, int idx);
//LUA_API int lua_isstring(misako_State* L, int idx);
//LUA_API int lua_iscfunction(misako_State* L, int idx);
//LUA_API int lua_isLfunction(misako_State* L, int idx);
//LUA_API int lua_isuserdata(misako_State* L, int idx);
//LUA_API int lua_type(misako_State* L, int idx);
//LUA_API const char* lua_typename(misako_State* L, int tp);
//
//LUA_API int lua_equal(misako_State* L, int idx1, int idx2);
//LUA_API int lua_rawequal(misako_State* L, int idx1, int idx2);
//LUA_API int lua_lessthan(misako_State* L, int idx1, int idx2);
//
//LUA_API double lua_tonumberx(misako_State* L, int idx, int* isnum);
//LUA_API int lua_tointegerx(misako_State* L, int idx, int* isnum);
//LUA_API unsigned lua_tounsignedx(misako_State* L, int idx, int* isnum);
//LUA_API const float* lua_tovector(misako_State* L, int idx);
//LUA_API int lua_toboolean(misako_State* L, int idx);
//LUA_API const char* lua_tolstring(misako_State* L, int idx, size_t* len);
//LUA_API const char* lua_tostringatom(misako_State* L, int idx, int* atom);
//LUA_API const char* lua_namecallatom(misako_State* L, int* atom);
//LUA_API int lua_objlen(misako_State* L, int idx);
//LUA_API lua_CFunction lua_tocfunction(misako_State* L, int idx);
//LUA_API void* lua_tolightuserdata(misako_State* L, int idx);
//LUA_API void* lua_touserdata(misako_State* L, int idx);
//LUA_API void* lua_touserdatatagged(misako_State* L, int idx, int tag);
//LUA_API int lua_userdatatag(misako_State* L, int idx);
//LUA_API misako_State* lua_tothread(misako_State* L, int idx);
//LUA_API const void* lua_topointer(misako_State* L, int idx);
//
///*
//** push functions (C -> stack)
//*/
//LUA_API void lua_pushnil(misako_State* L);
//LUA_API void lua_pushnumber(misako_State* L, double n);
//LUA_API void lua_pushinteger(misako_State* L, int n);
//LUA_API void lua_pushunsigned(misako_State* L, unsigned n);
//#if LUA_VECTOR_SIZE == 4
//LUA_API void lua_pushvector(misako_State* L, float x, float y, float z, float w);
//#else
//LUA_API void lua_pushvector(misako_State* L, float x, float y, float z);
//#endif
//LUA_API void lua_pushlstring(misako_State* L, const char* s, size_t l);
//LUA_API void lua_pushstring(misako_State* L, const char* s);
//LUA_API const char* lua_pushvfstring(misako_State* L, const char* fmt, va_list argp);
//LUA_API LUA_PRINTF_ATTR(2, 3) const char* lua_pushfstringL(misako_State* L, const char* fmt, ...);
//LUA_API void lua_pushcclosurek(misako_State* L, lua_CFunction fn, const char* debugname, int nup, lua_Continuation cont);
//LUA_API void lua_pushboolean(misako_State* L, int b);
//LUA_API int lua_pushthread(misako_State* L);
//
//LUA_API void lua_pushlightuserdata(misako_State* L, void* p);
//LUA_API void* lua_newuserdatatagged(misako_State* L, size_t sz, int tag);
//LUA_API void* lua_newuserdatadtor(misako_State* L, size_t sz, void (*dtor)(void*));
//
///*
//** get functions (Lua -> stack)
//*/
//LUA_API int lua_gettable(misako_State* L, int idx);
//LUA_API int lua_getfield(misako_State* L, int idx, const char* k);
//LUA_API int lua_rawgetfield(misako_State* L, int idx, const char* k);
//LUA_API int lua_rawget(misako_State* L, int idx);
//LUA_API int lua_rawgeti(misako_State* L, int idx, int n);
//LUA_API void lua_createtable(misako_State* L, int narr, int nrec);
//
//LUA_API void lua_setreadonly(misako_State* L, int idx, int enabled);
//LUA_API int lua_getreadonly(misako_State* L, int idx);
//LUA_API void lua_setsafeenv(misako_State* L, int idx, int enabled);
//
//LUA_API int lua_getmetatable(misako_State* L, int objindex);
//LUA_API void lua_getfenv(misako_State* L, int idx);
//
///*
//** set functions (stack -> Lua)
//*/
//LUA_API void lua_settable(misako_State* L, int idx);
//LUA_API void lua_setfield(misako_State* L, int idx, const char* k);
//LUA_API void lua_rawset(misako_State* L, int idx);
//LUA_API void lua_rawseti(misako_State* L, int idx, int n);
//LUA_API int lua_setmetatable(misako_State* L, int objindex);
//LUA_API int lua_setfenv(misako_State* L, int idx);
//
///*
//** `load' and `call' functions (load and run MisakoRVM bytecode)
//*/
//LUA_API int luau_load(misako_State* L, const char* chunkname, const char* data, size_t size, int env);
//LUA_API void lua_call(misako_State* L, int nargs, int nresults);
//LUA_API int lua_pcall(misako_State* L, int nargs, int nresults, int errfunc);
//
///*
//** coroutine functions
//*/
//LUA_API int lua_yield(misako_State* L, int nresults);
//LUA_API int lua_break(misako_State* L);
//LUA_API int lua_resume(misako_State* L, misako_State* from, int narg);
//LUA_API int lua_resumeerror(misako_State* L, misako_State* from);
//LUA_API int lua_status(misako_State* L);
//LUA_API int lua_isyieldable(misako_State* L);
//LUA_API void* lua_getthreaddata(misako_State* L);
//LUA_API void lua_setthreaddata(misako_State* L, void* data);
//
///*
//** garbage-collection function and options
//*/
//
//enum lua_GCOp
//{
//    // stop and resume incremental garbage collection
//    LUA_GCSTOP,
//    LUA_GCRESTART,
//
//    // run a full GC cycle; not recommended for latency sensitive applications
//    LUA_GCCOLLECT,
//
//    // return the heap size in KB and the remainder in bytes
//    LUA_GCCOUNT,
//    LUA_GCCOUNTB,
//
//    // return 1 if GC is active (not stopped); note that GC may not be actively collecting even if it's running
//    LUA_GCISRUNNING,
//
//    /*
//    ** perform an explicit GC step, with the step size specified in KB
//    **
//    ** garbage collection is handled by 'assists' that perform some amount of GC work matching pace of allocation
//    ** explicit GC steps allow to perform some amount of work at custom points to offset the need for GC assists
//    ** note that GC might also be paused for some duration (until bytes allocated meet the threshold)
//    ** if an explicit step is performed during this pause, it will trigger the start of the next collection cycle
//    */
//    LUA_GCSTEP,
//
//    /*
//    ** tune GC parameters G (goal), S (step multiplier) and step size (usually best left ignored)
//    **
//    ** garbage collection is incremental and tries to maintain the heap size to balance memory and performance overhead
//    ** this overhead is determined by G (goal) which is the ratio between total heap size and the amount of live data in it
//    ** G is specified in percentages; by default G=200% which means that the heap is allowed to grow to ~2x the size of live data.
//    **
//    ** collector tries to collect S% of allocated bytes by interrupting the application after step size bytes were allocated.
//    ** when S is too small, collector may not be able to catch up and the effective goal that can be reached will be larger.
//    ** S is specified in percentages; by default S=200% which means that collector will run at ~2x the pace of allocations.
//    **
//    ** it is recommended to set S in the interval [100 / (G - 100), 100 + 100 / (G - 100))] with a minimum value of 150%; for example:
//    ** - for G=200%, S should be in the interval [150%, 200%]
//    ** - for G=150%, S should be in the interval [200%, 300%]
//    ** - for G=125%, S should be in the interval [400%, 500%]
//    */
//    LUA_GCSETGOAL,
//    LUA_GCSETSTEPMUL,
//    LUA_GCSETSTEPSIZE,
//};
//
//LUA_API int lua_gc(misako_State* L, int what, int data);
//
///*
//** memory statistics
//** all allocated bytes are attributed to the memory category of the running thread (0..LUA_MEMORY_CATEGORIES-1)
//*/
//
//LUA_API void lua_setmemcat(misako_State* L, int category);
//LUA_API size_t lua_totalbytes(misako_State* L, int category);
//
///*
//** miscellaneous functions
//*/
//
//LUA_API l_noret lua_error(misako_State* L);
//
//LUA_API int lua_next(misako_State* L, int idx);
//
//LUA_API void lua_concat(misako_State* L, int n);
//
//LUA_API uintptr_t lua_encodepointer(misako_State* L, uintptr_t p);
//
//LUA_API double lua_clock();
//
//LUA_API void lua_setuserdatatag(misako_State* L, int idx, int tag);
//LUA_API void lua_setuserdatadtor(misako_State* L, int tag, void (*dtor)(misako_State*, void*));
//
//LUA_API void lua_clonefunction(misako_State* L, int idx);
//
///*
//** reference system, can be used to pin objects
//*/
//#define LUA_NOREF -1
//#define LUA_REFNIL 0
//
//LUA_API int lua_ref(misako_State* L, int idx);
//LUA_API void lua_unref(misako_State* L, int ref);
//
//#define lua_getref(L, ref) lua_rawgeti(L, LUA_REGISTRYINDEX, (ref))
//
///*
//** ===============================================================
//** some useful macros
//** ===============================================================
//*/
//#define lua_tonumber(L, i) lua_tonumberx(L, i, NULL)
//#define lua_tointeger(L, i) lua_tointegerx(L, i, NULL)
//#define lua_tounsigned(L, i) lua_tounsignedx(L, i, NULL)
//
//#define lua_pop(L, n) lua_settop(L, -(n)-1)
//
//#define lua_newtable(L) lua_createtable(L, 0, 0)
//#define lua_newuserdata(L, s) lua_newuserdatatagged(L, s, 0)
//
//#define lua_strlen(L, i) lua_objlen(L, (i))
//
//#define lua_isfunction(L, n) (lua_type(L, (n)) == LUA_TFUNCTION)
//#define lua_istable(L, n) (lua_type(L, (n)) == LUA_TTABLE)
//#define lua_islightuserdata(L, n) (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
//#define lua_isnil(L, n) (lua_type(L, (n)) == LUA_TNIL)
//#define lua_isboolean(L, n) (lua_type(L, (n)) == LUA_TBOOLEAN)
//#define lua_isvector(L, n) (lua_type(L, (n)) == LUA_TVECTOR)
//#define lua_isthread(L, n) (lua_type(L, (n)) == LUA_TTHREAD)
//#define lua_isnone(L, n) (lua_type(L, (n)) == LUA_TNONE)
//#define lua_isnoneornil(L, n) (lua_type(L, (n)) <= LUA_TNIL)
//
//#define lua_pushliteral(L, s) lua_pushlstring(L, "" s, (sizeof(s) / sizeof(char)) - 1)
//#define lua_pushcfunction(L, fn, debugname) lua_pushcclosurek(L, fn, debugname, 0, NULL)
//#define lua_pushcclosure(L, fn, debugname, nup) lua_pushcclosurek(L, fn, debugname, nup, NULL)
//
//#define lua_setglobal(L, s) lua_setfield(L, LUA_GLOBALSINDEX, (s))
//#define lua_getglobal(L, s) lua_getfield(L, LUA_GLOBALSINDEX, (s))
//
//#define lua_tostring(L, i) lua_tolstring(L, (i), NULL)
//
//#define lua_pushfstring(L, fmt, ...) lua_pushfstringL(L, fmt, ##__VA_ARGS__)
//
///*
//** {======================================================================
//** Debug API
//** =======================================================================
//*/
//
//typedef struct lua_Debug lua_Debug; // activation record
//
//// Functions to be called by the debugger in specific events
//typedef void (*lua_Hook)(misako_State* L, lua_Debug* ar);
//
//LUA_API int lua_stackdepth(misako_State* L);
//LUA_API int lua_getinfo(misako_State* L, int level, const char* what, lua_Debug* ar);
//LUA_API int lua_getargument(misako_State* L, int level, int n);
//LUA_API const char* lua_getlocal(misako_State* L, int level, int n);
//LUA_API const char* lua_setlocal(misako_State* L, int level, int n);
//LUA_API const char* lua_getupvalue(misako_State* L, int funcindex, int n);
//LUA_API const char* lua_setupvalue(misako_State* L, int funcindex, int n);
//
//LUA_API void lua_singlestep(misako_State* L, int enabled);
//LUA_API int lua_breakpoint(misako_State* L, int funcindex, int line, int enabled);
//
//typedef void (*lua_Coverage)(void* context, const char* function, int linedefined, int depth, const int* hits, size_t size);
//
//LUA_API void lua_getcoverage(misako_State* L, int funcindex, void* context, lua_Coverage callback);
//
//// Warning: this function is not thread-safe since it stores the result in a shared global array! Only use for debugging.
//LUA_API const char* lua_debugtrace(misako_State* L);
//
//struct lua_Debug
//{
//    const char* name;           // (n)
//    const char* what;           // (s) `Lua', `C', `main', `tail'
//    const char* source;         // (s)
//    int linedefined;            // (s)
//    int currentline;            // (l)
//    unsigned char nupvals;      // (u) number of upvalues
//    unsigned char nparams;      // (a) number of parameters
//    char isvararg;              // (a)
//    char short_src[LUA_IDSIZE]; // (s)
//    void* userdata;             // only valid in luau_callhook
//};
//
//// }======================================================================
//
///* Callbacks that can be used to reconfigure behavior of the VM dynamically.
// * These are shared between all coroutines.
// *
// * Note: interrupt is safe to set from an arbitrary thread but all other callbacks
// * can only be changed when the VM is not running any code */
//struct lua_Callbacks
//{
//    void* userdata; // arbitrary userdata pointer that is never overwritten by MisakoRVM
//
//    void (*interrupt)(misako_State* L, int gc);  // gets called at safepoints (loop back edges, call/ret, gc) if set
//    void (*panic)(misako_State* L, int errcode); // gets called when an unprotected error is raised (if longjmp is used)
//
//    void (*userthread)(misako_State* LP, misako_State* L); // gets called when L is created (LP == parent) or destroyed (LP == NULL)
//    int16_t (*useratom)(const char* s, size_t l);    // gets called when a string is created; returned atom can be retrieved via tostringatom
//
//    void (*debugbreak)(misako_State* L, lua_Debug* ar);     // gets called when BREAK instruction is encountered
//    void (*debugstep)(misako_State* L, lua_Debug* ar);      // gets called after each instruction in single step mode
//    void (*debuginterrupt)(misako_State* L, lua_Debug* ar); // gets called when thread execution is interrupted by break in another thread
//    void (*debugprotectederror)(misako_State* L);           // gets called when protected call results in an error
//};
//typedef struct lua_Callbacks lua_Callbacks;
//
//LUA_API lua_Callbacks* lua_callbacks(misako_State* L);
//
///******************************************************************************
// * Copyright (c) 2019-2022 Roblox Corporation
// * Copyright (C) 1994-2008 Lua.org, PUC-Rio.  All rights reserved.
// *
// * Permission is hereby granted, free of charge, to any person obtaining
// * a copy of this software and associated documentation files (the
// * "Software"), to deal in the Software without restriction, including
// * without limitation the rights to use, copy, modify, merge, publish,
// * distribute, sublicense, and/or sell copies of the Software, and to
// * permit persons to whom the Software is furnished to do so, subject to
// * the following conditions:
// *
// * The above copyright notice and this permission notice shall be
// * included in all copies or substantial portions of the Software.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ******************************************************************************/
