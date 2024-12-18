#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#include <roblox/offsets.hpp>

struct jobs_t
{
	std::uintptr_t* functions;
	std::uint8_t pad0[12];
	std::string name;
	std::uint8_t pad1[16];
	double time;
	std::uint8_t pad2[16];
	double time_spend;
	std::uint8_t pad3[8];
	std::uintptr_t state;
};

class task_scheduler
{
private:
	std::uintptr_t scheduler = r_taskscheduler();

public:
	std::uintptr_t get_data_model() const;
	std::uintptr_t get_script_context() const;

	std::uintptr_t get_job_by_name(std::string_view name) const;

	std::int32_t get_fps_cap() const;
	std::int32_t set_fps_cap(const double value) const;

	bool is_game_loaded() const;
	std::uintptr_t hook_job(std::uintptr_t job, void* to_hook) const;
	void hook_scripts_job() const;

	~task_scheduler() {}
};