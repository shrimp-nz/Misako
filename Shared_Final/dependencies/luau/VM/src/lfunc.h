// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lobject.h"

#define sizeCclosure(n) (offsetof(Closure, c.upvals) + sizeof(TValue) * (n))
#define sizeLclosure(n) (offsetof(Closure, l.uprefs) + sizeof(TValue) * (n))

LUAI_FUNC Proto* luaF_newproto(misako_State* L);
LUAI_FUNC Closure* luaF_newLclosure(misako_State* L, int nelems, Table* e, Proto* p);
LUAI_FUNC Closure* luaF_newCclosure(misako_State* L, int nelems, Table* e);
LUAI_FUNC UpVal* luaF_findupval(misako_State* L, StkId level);
LUAI_FUNC void luaF_close(misako_State* L, StkId level);
LUAI_FUNC void luaF_closeupval(misako_State* L, UpVal* uv, bool dead);
LUAI_FUNC void luaF_freeproto(misako_State* L, Proto* f, struct lua_Page* page);
LUAI_FUNC void luaF_freeclosure(misako_State* L, Closure* c, struct lua_Page* page);
LUAI_FUNC void luaF_unlinkupval(UpVal* uv);
LUAI_FUNC void luaF_freeupval(misako_State* L, UpVal* uv, struct lua_Page* page);
LUAI_FUNC const LocVar* luaF_getlocal(const Proto* func, int local_number, int pc);
LUAI_FUNC const LocVar* luaF_findlocal(const Proto* func, int local_reg, int pc);
