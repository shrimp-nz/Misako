// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lobject.h"

struct CallS
{ /* data to `f_call' */
    StkId func;
    int nresults;
};

LUAI_FUNC TValue* index2addr(misako_State* L, int idx);
LUAI_FUNC const TValue* luaA_toobject(misako_State* L, int idx);
LUAI_FUNC void luaA_pushobject(misako_State* L, const TValue* o);
