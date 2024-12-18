#ifndef BASE_64_HPP
#define BASE_64_HPP

#include <algorithm>
#include <string>

#include <dependencies/xorstring/xorstring.hpp>

namespace base64 {

    std::string get_base64_chars();
    std::string to_base64(std::string const& data);
    std::string from_base64(std::string const& data);
}

#endif // BASE_64_HPP