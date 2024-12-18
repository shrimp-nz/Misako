// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lobject.h"
#include "ltm.h"

#define tostring(L, o) ((ttype(o) == LUA_TSTRING) || (luaV_tostring(L, o)))

#define tonumber(o, n) (ttype(o) == LUA_TNUMBER || (((o) = luaV_tonumber(o, n)) != NULL))

#define equalobj(L, o1, o2) (ttype(o1) == ttype(o2) && luaV_equalval(L, o1, o2))

LUAI_FUNC int luaV_strcmp(const TString* ls, const TString* rs);
LUAI_FUNC int luaV_lessthan(misako_State* L, const TValue* l, const TValue* r);
LUAI_FUNC int luaV_lessequal(misako_State* L, const TValue* l, const TValue* r);
LUAI_FUNC int luaV_equalval(misako_State* L, const TValue* t1, const TValue* t2);
LUAI_FUNC void luaV_doarith(misako_State* L, StkId ra, const TValue* rb, const TValue* rc, TMS op);
LUAI_FUNC void luaV_dolen(misako_State* L, StkId ra, const TValue* rb);
LUAI_FUNC const TValue* luaV_tonumber(const TValue* obj, TValue* n);
LUAI_FUNC const float* luaV_tovector(const TValue* obj);
LUAI_FUNC int luaV_tostring(misako_State* L, StkId obj);
LUAI_FUNC void luaV_gettable(misako_State* L, const TValue* t, TValue* key, StkId val);
LUAI_FUNC void luaV_settable(misako_State* L, const TValue* t, TValue* key, StkId val);
LUAI_FUNC void luaV_concat(misako_State* L, int total, int last);
LUAI_FUNC void luaV_getimport(misako_State* L, Table* env, TValue* k, uint32_t id, bool propagatenil);

LUAI_FUNC void luau_execute(misako_State* L);
LUAI_FUNC int luau_precall(misako_State* L, struct lua_TValue* func, int nresults);
LUAI_FUNC void luau_poscall(misako_State* L, StkId first);
LUAI_FUNC void luau_callhook(misako_State* L, lua_Hook hook, void* userdata);
