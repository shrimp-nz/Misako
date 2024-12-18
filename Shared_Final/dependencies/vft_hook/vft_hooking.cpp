#include "vft_hooking.hpp"

// ty kind guidedhacking

vmthook::vmthook(void* instance)
{
	vt_backup = *reinterpret_cast<std::uintptr_t**>(instance);

	while (reinterpret_cast<const std::uintptr_t*>(*reinterpret_cast<const std::uintptr_t*>(instance))[vt_functions])
		vt_functions++;

	const auto vt_size = ((vt_functions * 4) + 4);
	vt_new = std::make_unique<std::uintptr_t[]>(vt_functions + 1);

	memcpy(vt_new.get(), &vt_backup[-1], vt_size);
	*reinterpret_cast<const std::uintptr_t**>(instance) = &vt_new.get()[1];
}

void vmthook::hook_func(void* new_pointer, const std::int32_t index)
{
	vt_new[index + 1] = reinterpret_cast<const std::uintptr_t>(new_pointer);
}

void* vmthook::replace_func(std::uintptr_t* vtable, const std::int32_t index, const std::uintptr_t new_pointer)
{
	DWORD old_protection, new_protection;

	VirtualProtect(&vtable[index], 4ul, 0x40, &old_protection);
	const auto old_func = reinterpret_cast<void*>(vtable[index]);
	vtable[index] = new_pointer;
	VirtualProtect(&vtable[index], 4ul, old_protection, &new_protection);

	return old_func;
}