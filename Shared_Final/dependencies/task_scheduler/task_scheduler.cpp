#include "task_scheduler.hpp"

#include <dependencies/xorstring/xorstring.hpp>
#include <dependencies/vft_hook/vft_hooking.hpp>

#include <dllmain.hpp>

// i hereby announce that I will be abusing "const" in this class.
// -Rexi

std::uintptr_t ts_job_cache;
std::uintptr_t ts_new_vftable[6];

std::uintptr_t task_scheduler::get_data_model() const
{
	const auto datamodel_job = this->get_job_by_name("LuaGc");
	const auto datamodel_instance = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(datamodel_job + taskscheduler_datamodel) + 8);

	return datamodel_instance;
}

std::uintptr_t task_scheduler::get_script_context() const
{
	const auto waiting_scripts = this->get_job_by_name("WaitingHybridScriptsJob");
	const auto state = *reinterpret_cast<const std::uintptr_t*>(waiting_scripts + taskscheduler_scriptcontext);

	return state;
}

std::uintptr_t task_scheduler::get_job_by_name(std::string_view name) const
{
	auto results = 0;

	auto iterator = *reinterpret_cast<const std::uintptr_t*>(this->scheduler + taskscheduler_jobstart);
	const auto iterator_end = *reinterpret_cast<std::uintptr_t*>(this->scheduler + taskscheduler_jobend);

	while (iterator != iterator_end) // scan till the end
	{
		const auto instance = *reinterpret_cast<jobs_t**>(iterator);

		if (instance->name == name.data()) // check if the instance matches what we inputted
			results = reinterpret_cast<std::uintptr_t>(instance); // if it is, return the instance!

		iterator += 8;
	}

	return results;
}

std::int32_t task_scheduler::get_fps_cap() const
{
	return *reinterpret_cast<std::uintptr_t*>(this->scheduler + taskscheduler_delayed);
}

std::int32_t task_scheduler::set_fps_cap(const double value) const
{
	const auto taskscheduler = this-scheduler;
	const auto framerate = 1.0 / value;

	*reinterpret_cast<double*>(this->scheduler + taskscheduler_delayed) = framerate;
	return 1;
}

bool task_scheduler::is_game_loaded() const
{
	const auto datamodel_instance = this->get_data_model();
	const auto is_gameloaded = *reinterpret_cast<bool*>(datamodel_instance + 844);

	return is_gameloaded;
}

std::int32_t __fastcall run_on_queue(std::uintptr_t job, std::uintptr_t, std::uintptr_t to_call)
{
	if (!scheduled_script.empty())
	{
		main::execute(scheduled_script.front());
		scheduled_script.pop();
	}

	const auto func_t = reinterpret_cast<std::int32_t(__thiscall*)(std::uintptr_t, std::uintptr_t)>
		(ts_job_cache)(job, to_call);

	return func_t;
}

std::uintptr_t task_scheduler::hook_job(std::uintptr_t job, void* to_hook) const
{
	std::memcpy(ts_new_vftable, *reinterpret_cast<void**>(job), sizeof(std::uintptr_t) * 6);

	const auto ts_old_vftable = *reinterpret_cast<std::uintptr_t**>(job);
	ts_new_vftable[2] = reinterpret_cast<std::uintptr_t>(to_hook);

	*reinterpret_cast<std::uintptr_t**>(job) = ts_new_vftable;
	return ts_old_vftable[2];
}

void task_scheduler::hook_scripts_job() const
{
	const auto waiting_scripts_job = this->get_job_by_name("WaitingHybridScriptsJob");
	ts_job_cache = this->hook_job(waiting_scripts_job, run_on_queue);
}