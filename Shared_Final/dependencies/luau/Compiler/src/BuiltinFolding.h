// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "ConstantFolding.h"

namespace MisakoRVM
{
namespace Compile
{

Constant foldBuiltin(int bfid, const Constant* args, size_t count);

} // namespace Compile
} // namespace MisakoRVM
