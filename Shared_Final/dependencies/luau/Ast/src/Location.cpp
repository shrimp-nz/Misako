// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
#include "Luau/Location.h"

namespace MisakoRVM
{

std::string toString(const Position& position)
{
    return "{ line = " + std::to_string(position.line) + ", col = " + std::to_string(position.column) + " }";
}

std::string toString(const Location& location)
{
    return "Location { " + toString(location.begin) + ", " + toString(location.end) + " }";
}

} // namespace MisakoRVM
