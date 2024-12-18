#include "Tools.hpp"
#include <TlHelp32.h>
#include <random>
#include <roblox/config.hpp>
#include <dependencies/xorstring/xorstring.hpp>

namespace tools
{
	namespace internet_addons
	{
		std::string replace_all(std::string subject, const std::string& search, const std::string& replace)
		{
			size_t pos = 0;
			while ((pos = subject.find(search, pos)) != std::string::npos)
			{
				subject.replace(pos, search.length(), replace);
				pos += replace.length();
			}
			return subject;
		}

		std::string read_website(const std::string& url)
		{
			HINTERNET interwebs = InternetOpenA(xorstr("Mozilla/5.0"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
			std::string rtn;

			if (interwebs)
			{
				HINTERNET url_file = InternetOpenUrlA(interwebs, url.c_str(), NULL, NULL, NULL, NULL);

				if (url_file)
				{
					char buffer[2000];
					DWORD bytes_read;

					do
					{
						InternetReadFile(url_file, buffer, 2000, &bytes_read);
						rtn.append(buffer, bytes_read);
						memset(buffer, 0, 2000);
					} while (bytes_read);

					InternetCloseHandle(interwebs);
					InternetCloseHandle(url_file);
					std::string p = replace_all(rtn, "|n", "\r\n");
					return p;
				}
			}

			InternetCloseHandle(interwebs);
			std::string p = replace_all(rtn, "|n", "\r\n");
			return p;
		}
	}

	namespace curl_addons
	{
		size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
		{
			((std::string*)userp)->append((char*)contents, size * nmemb);
			return size * nmemb;
		}

		struct WriteThis
		{
			const char* readptr;
			size_t sizeleft;
		};

		size_t read_callback(char* dest, size_t size, size_t nmemb, void* userp)
		{
			struct WriteThis* wt = (struct WriteThis*)userp;
			size_t buffer_size = size * nmemb;

			if (wt->sizeleft)
			{
				/* copy as much as possible from the source to the destination */
				size_t copy_this_much = wt->sizeleft;
				if (copy_this_much > buffer_size)
					copy_this_much = buffer_size;
				memcpy(dest, wt->readptr, copy_this_much);

				wt->readptr += copy_this_much;
				wt->sizeleft -= copy_this_much;
				return copy_this_much; /* we copied this many bytes */
			}

			return 0; /* no more data left to deliver */
		}
	}

	namespace io_addons
	{
		void append_file(const std::wstring& Path, std::size_t size, const char* Content)
		{
			std::ofstream Out;
			Out.open(Path, std::ios_base::app | std::ios::binary);
			Out.write(Content, std::strlen(Content));
			Out.close();
		}

		void write_file(const std::wstring& WPath, std::size_t size, const char* Content)
		{
			std::ofstream Out;
			Out.open(WPath, std::ios::out | std::ios::binary);
			Out.write(Content, size);
			Out.close();
		}

		std::string get_dll_path(const char* Addon)
		{
			HMODULE hModule = GetModuleHandleW(std::wstring(xorstr(L"Shared_Final.dll")).c_str());

			if (hModule == NULL)
				throw std::exception(xorstr("Failed to grab module handle! (GETDLLPATH)"));

			char buffer[MAX_PATH + 1];
			GetModuleFileNameA(hModule, buffer, MAX_PATH);

			std::string::size_type pos = std::string(buffer).find_last_of("\\/");
			return std::string(buffer).substr(0, pos).append(Addon);
		}

		std::string random_string(std::size_t length)
		{
			std::string str(xorstr("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));

			std::random_device rd;
			std::mt19937 generator(rd());

			std::shuffle(str.begin(), str.end(), generator);
			return str.substr(0, length);
		}
	}
}