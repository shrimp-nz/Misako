// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lobject.h"

typedef int (*luau_FastFunction)(misako_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams);

extern luau_FastFunction luauF_table[256];
