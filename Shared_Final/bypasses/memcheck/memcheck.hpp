#pragma once
#include <Windows.h>
#include "utilities.hpp"

class memcheck_t
{
	std::uintptr_t task_scheduler;
	std::uintptr_t task_scheduler_start;
	std::uintptr_t task_scheduler_end;

public:
	section_t text;
	section_t rdata;
	section_t vmpx;
	section_t vmp0; // not cloned, not scanned
	section_t vmp1;

public:
	memcheck_t();
	// can use std::string_view since c++17 and is a better alternative to const std::string&
	// because c strings / string literals *will* have temporary copies made.
	std::uintptr_t get_job_by_name(std::string_view job_name) const;
	void initialize_bypass() const;
};

extern memcheck_t meme;