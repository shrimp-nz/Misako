#include <Windows.h>
#include <fstream>

class vmthook
{
	std::unique_ptr<uintptr_t[]> vt_new = nullptr;
	std::uintptr_t* vt_backup = nullptr;
	std::uintptr_t vt_functions = 0;

public:
	vmthook(void* instance);
	void hook_func(void* new_pointer, const std::int32_t index);
	void* replace_func(std::uintptr_t* vtable, const std::int32_t index, const std::uintptr_t new_pointer);
};