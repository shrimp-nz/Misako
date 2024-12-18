// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "Luau/Ast.h"
#include "Luau/DenseHash.h"

namespace MisakoRVM
{
namespace Compile
{

struct TableShape
{
    unsigned int arraySize = 0;
    unsigned int hashSize = 0;
};

void predictTableShapes(DenseHashMap<AstExprTable*, TableShape>& shapes, AstNode* root);

} // namespace Compile
} // namespace MisakoRVM
