#include "stdafx.hpp"

#include "modules/HwDLL.hpp"
#include "modules/ServerDLL.hpp"
#include "hud_custom.hpp"
#include "runtime_data.hpp"
#include "splits.hpp"

namespace Splits
{
	std::vector<Split> splits;

	// To keep track of the split trigger creation
	Vector place_start;
	bool placing = false;

	Split* last_reached = nullptr;

	bool printed_on_end = false;

	Split::Split()
		: Trigger(Vector(0, 0, 0), Vector(0, 0, 0))
		, map_name()
		, name()
		, track_horizontal_speed(true)
		, track_vertical_speed(false)
		, track_x(false)
		, track_y(false)
		, track_z(false)
		, reached(false)
		, time()
		, speed()
		, targets_entity(false)
	{
	}

	Split::Split(std::string targetname)
		: Trigger(Vector(0, 0, 0), Vector(0, 0, 0))
		, map_name()
		, name(targetname)
		, track_horizontal_speed(true)
		, track_vertical_speed(false)
		, track_x(false)
		, track_y(false)
		, track_z(false)
		, reached(false)
		, time()
		, speed()
		, targets_entity(true)
	{
	}

	Split::Split(std::string targetname, std::string map_name)
		: Trigger(Vector(0, 0, 0), Vector(0, 0, 0))
		, map_name(map_name)
		, name(targetname)
		, track_horizontal_speed(true)
		, track_vertical_speed(false)
		, track_x(false)
		, track_y(false)
		, track_z(false)
		, reached(false)
		, time()
		, speed()
		, targets_entity(true)
	{
	}

	Split::Split(Vector corner1, Vector corner2)
		: Trigger(corner1, corner2)
		, map_name()
		, name()
		, track_horizontal_speed(true)
		, track_vertical_speed(false)
		, track_x(false)
		, track_y(false)
		, track_z(false)
		, reached(false)
		, time()
		, speed()
		, targets_entity(false)
	{
	}

	Split::Split(Vector corner1, Vector corner2, std::string map_name)
		: Trigger(corner1, corner2)
		, map_name(map_name)
		, name()
		, track_horizontal_speed(true)
		, track_vertical_speed(false)
		, track_x(false)
		, track_y(false)
		, track_z(false)
		, reached(false)
		, time()
		, speed()
		, targets_entity(false)
	{
	}

	Split::Split(Vector corner1, Vector corner2, std::string map_name, std::string name)
		: Trigger(corner1, corner2)
		, map_name(map_name)
		, name(name)
		, track_horizontal_speed(true)
		, track_vertical_speed(false)
		, track_x(false)
		, track_y(false)
		, track_z(false)
		, reached(false)
		, time()
		, speed()
		, targets_entity(false)
	{
	}

	void Split::set_command(std::string new_command)
	{
		// Do nothing since we're a split
		// This method override is just to make sure no command is set for the split, which
		// only has Trigger as parent for convenience of using all of its other members,
		// because this is just a more specific kind of trigger
	}

	void Split::touch()
	{
		if (targets_entity)
			return;

		activate();
	}

	void Activate(const char* idOrName)
	{
		const auto split = Splits::GetSplitByNameOrId(idOrName, false);
		if (split && !split->reached && split->targets_entity) {
			split->activate();
		}
	}

	void Split::activate()
	{
		if (placing)
			return;

		if (!map_name.empty() && map_name != HwDLL::GetInstance().lastLoadedMap)
		{
			// This split is scoped to one specific map, and we're not in that map
			return;
		}

		if (reached)
		{
			// We've already passed through this split, so don't do anything if player goes back through it
			// If they want to start over, they have to reset the timer
			// TODO: maybe add a cvar to clear the split times (Reset()) when reaching the last split, in case
			// someone has a circuit-like map with laps where they want to keep splitting on the same splits
			// but without resetting the timer
			return;
		}

		bool found_non_reached = false;
		bool found_reached = false;
		for (size_t i = 0; i < splits.size(); ++i)
		{
			if (&splits[i] == this)
				continue; // ignore the one we've just touched

			if (splits[i].reached)
				found_reached = true;
			else
				found_non_reached = true;
		}

		if (!found_reached)
		{
			if (CVars::bxt_splits_start_timer_on_first_split.GetBool())
			{
				HwDLL::GetInstance().TimerReset();
				HwDLL::GetInstance().TimerStart();
			}

			const auto& record_name = CVars::bxt_splits_autorecord_on_first_split.GetString();
			if (!record_name.empty())
			{
				std::ostringstream ss;
				ss << "bxt_autorecord " << record_name << "\n";
				HwDLL::GetInstance().ORIG_Cbuf_InsertText(ss.str().c_str());
			}
		}

		RuntimeData::Add(RuntimeData::SplitMarker {
			corner_min,
			corner_max,
			name,
			map_name
		});

		last_reached = this;

		reached = true;
		time = CustomHud::GetTime();
		const auto& player = HwDLL::GetInstance().GetPlayerEdict();
		if (player)
		{
			speed = player->v.velocity;
			origin = player->v.origin;
		}

		if (CVars::bxt_splits_print.GetBool())
			PrintSplitCompletion(*this);
		
		if (CVars::bxt_splits_end_on_last_split.GetBool() && !found_non_reached)
			ServerDLL::GetInstance().DoAutoStopTasks();

		// LiveSplit should have a split with the same name, in the "Split on the following maps" section
		Interprocess::WriteMapChange(time, name);
	}

	Split* GetSplitByNameOrId(const char* idOrName)
	{
		return GetSplitByNameOrId(idOrName, true);
	}

	Split* GetSplitByNameOrId(const char* idOrName, bool warnIfMissing)
	{
		auto itr = std::find_if(splits.begin(), splits.end(),
			[&idOrName](const Split& s) { return !strcmp(idOrName, s.name.c_str()); });

		unsigned long idx = 0;
		if (itr == splits.end())
			idx = std::strtoul(idOrName, nullptr, 10);
		else
			idx = itr - Splits::splits.begin() + 1;

		if (idx == 0 || splits.size() < idx) {
			if (warnIfMissing)
				HwDLL::GetInstance().ORIG_Con_Printf("There's no split with this name or id.\n");

			return nullptr;
		}

		return &splits[idx - 1];
	}

	const std::string GetSummary(Split& split)
	{
		const auto dataSeparator = " | ";

		auto name     = split.name;
		auto map_name = split.map_name;

		const auto time  = split.time;
		const auto speed = split.speed;

		if (name.empty())
			name = "(unnamed)";

		if (map_name.empty())
			map_name = "(global)";

		std::ostringstream oss;

		// TODO: maybe take the highest split time and pad the lower ones with some spaces
		// so that all times are visually aligned. At least the times; doing it for the rest
		// of data would be more tedious
		if (time.hours)
		{
			oss << static_cast<int>(time.hours) << ":";
			if (time.minutes < 10)
				oss << "0";
		}

		if (time.minutes)
			oss << static_cast<int>(time.minutes) << ":";

		if ((time.hours || time.minutes) && time.seconds < 10)
			oss << "0";

		oss << static_cast<int>(time.seconds) << ".";

		if (time.milliseconds < 100)
		{
			oss << "0";

			if (time.milliseconds < 10)
				oss << "0";
		}
		oss << static_cast<int>(time.milliseconds);

		if (split.track_horizontal_speed)
		{
			if (split.track_vertical_speed)
				oss << dataSeparator << speed.Length() << " u/s 3D";
			else
				oss << dataSeparator << speed.Length2D() << " u/s horiz.";
		}
		else if (split.track_vertical_speed)
			oss << dataSeparator << speed.z <<" u/s vert.";

		if (split.track_x)
			oss << dataSeparator << "x = " << split.origin.x;

		if (split.track_y)
			oss << dataSeparator << "y = " << split.origin.y;

		if (split.track_z)
			oss << dataSeparator << "z = " << split.origin.z;

		oss << dataSeparator << name << dataSeparator << map_name;

		return oss.str();
	}

	void PrintAll()
	{
		if (splits.empty())
			return;

		std::vector<Splits::Split> completedSplits;
		std::copy_if(Splits::splits.begin(), Splits::splits.end(), std::back_inserter(completedSplits),
			[&](Splits::Split &s){ return s.reached; } );

		if (completedSplits.empty())
		{
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't completed any split yet.\n");
			return;
		}
		std::ostringstream oss;
		oss << "Splits:\n";

		for (size_t i = 0; i < completedSplits.size(); ++i)
			oss << i + 1 << ": " << GetSummary(completedSplits[i]) << "\n";

		HwDLL::GetInstance().ORIG_Con_Printf(oss.str().c_str());
	}

	void PrintList(std::vector<Split> splits)
	{
		for (size_t i = 0; i < splits.size(); ++i) {
			const auto& split = splits[i];
			const auto corners = split.get_corner_positions();

			std::ostringstream oss;
			oss << i + 1 << ": `" << split.name << "` - `" << split.map_name << "` - ("
				<< corners.first.x << ", " << corners.first.y << ", " << corners.first.z << ") | ("
				<< corners.second.x << ", " << corners.second.y << ", " << corners.second.z << ")";

			if (split.track_horizontal_speed)
			{
				if (split.track_vertical_speed)
					oss << " - 3D";
				else
					oss << " - horizontal";
			}
			else if (split.track_vertical_speed)
				oss << " - vertical";

			if (split.track_x)
				oss << " - tracking X";

			if (split.track_y)
				oss << " - tracking Y";

			if (split.track_z)
				oss << " - tracking Z";

			oss << "\n";

			HwDLL::GetInstance().ORIG_Con_Printf(oss.str().c_str());
		}
	}

	void PrintSplitCompletion(Split& split)
	{
		HwDLL::GetInstance().ORIG_Con_Printf("Split: %s\n", GetSummary(split).c_str());
	}

	void Reset()
	{
		for (auto& split : splits)
		{
			split.reached = false;
			split.time = Interprocess::Time{};
			split.speed = Vector(0, 0, 0);
			split.origin = Vector(0, 0, 0);
		}
		last_reached = nullptr;
		printed_on_end = false;
	}

	void Update(const Vector& player_position, bool ducking)
	{
		for (auto& split : splits)
			split.update(player_position, ducking);

		if (placing) {
			if (!splits.empty())
			{
				auto trace = HwDLL::GetInstance().CameraTrace();
				Vector place_end(trace.EndPos);

				splits.back().set_corner_positions(place_start, place_end);
			}
			else
				placing = false;
		}
	}

	void Update(const Vector& player_position_start, const Vector& player_position_end, bool ducking)
	{
		for (auto& split : splits)
			split.update(player_position_start, player_position_end, ducking);
	}
}
