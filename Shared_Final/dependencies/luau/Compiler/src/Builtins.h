// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "ValueTracking.h"

namespace MisakoRVM
{
struct CompileOptions;
}

namespace MisakoRVM
{
namespace Compile
{

struct Builtin
{
    AstName object;
    AstName method;

    bool empty() const
    {
        return object == AstName() && method == AstName();
    }

    bool isGlobal(const char* name) const
    {
        return object == AstName() && method == name;
    }

    bool isMethod(const char* table, const char* name) const
    {
        return object == table && method == name;
    }
};

Builtin getBuiltin(AstExpr* node, const DenseHashMap<AstName, Global>& globals, const DenseHashMap<AstLocal*, Variable>& variables);

void analyzeBuiltins(DenseHashMap<AstExprCall*, int>& result, const DenseHashMap<AstName, Global>& globals,
    const DenseHashMap<AstLocal*, Variable>& variables, const CompileOptions& options, AstNode* root);

} // namespace Compile
} // namespace MisakoRVM
