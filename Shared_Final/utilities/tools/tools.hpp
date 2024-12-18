#pragma once
#include <iostream>
#include <Windows.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <streambuf>
#include <chrono>
#include <emmintrin.h>
#include <mutex>

#include <wininet.h>
#pragma comment(lib, "wininet") 

namespace tools
{
    namespace internet_addons
    {
        std::string read_website(const std::string& url);
    }

    namespace io_addons
    {
        void append_file(const std::wstring& Path, std::size_t size, const char* Content);
        void write_file(const std::wstring& WPath, std::size_t size, const char* Content);

        std::string get_dll_path(const char* Addon = "");
        std::string random_string(std::size_t length);
    }

    namespace curl_addons
    {
        size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);
        size_t read_callback(char* dest, size_t size, size_t nmemb, void* userp);
    }
}