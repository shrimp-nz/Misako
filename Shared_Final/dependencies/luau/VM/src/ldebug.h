// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lstate.h"

#define pcRel(pc, p) ((pc) ? cast_to(int, (pc) - (p)->code) - 1 : 0)

#define luaG_typeerror(L, o, opname) luaG_typeerrorL(L, o, opname)
#define luaG_forerror(L, o, what) luaG_forerrorL(L, o, what)
#define luaG_runerror(L, fmt, ...) luaG_runerrorL(L, fmt, ##__VA_ARGS__)

#define LUA_MEMERRMSG "not enough memory"
#define LUA_ERRERRMSG "error in error handling"

LUAI_FUNC l_noret luaG_typeerrorL(misako_State* L, const TValue* o, const char* opname);
LUAI_FUNC l_noret luaG_forerrorL(misako_State* L, const TValue* o, const char* what);
LUAI_FUNC l_noret luaG_concaterror(misako_State* L, StkId p1, StkId p2);
LUAI_FUNC l_noret luaG_aritherror(misako_State* L, const TValue* p1, const TValue* p2, TMS op);
LUAI_FUNC l_noret luaG_ordererror(misako_State* L, const TValue* p1, const TValue* p2, TMS op);
LUAI_FUNC l_noret luaG_indexerror(misako_State* L, const TValue* p1, const TValue* p2);
LUAI_FUNC l_noret luaG_methoderror(misako_State* L, const TValue* p1, const TString* p2);
LUAI_FUNC l_noret luaG_readonlyerror(misako_State* L);

LUAI_FUNC LUA_PRINTF_ATTR(2, 3) l_noret luaG_runerrorL(misako_State* L, const char* fmt, ...);
LUAI_FUNC void luaG_pusherror(misako_State* L, const char* error);

LUAI_FUNC void luaG_breakpoint(misako_State* L, Proto* p, int line, bool enable);
LUAI_FUNC bool luaG_onbreak(misako_State* L);

LUAI_FUNC int luaG_getline(Proto* p, int pc);
