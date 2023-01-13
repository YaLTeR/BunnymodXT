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

	const std::string& Split::get_name() const
	{
		return name;
	}

	void Split::set_name(std::string new_map)
	{
		name = std::move(new_map);
	}

	const std::string& Split::get_map() const
	{
		return map_name;
	}

	void Split::set_map(std::string new_map)
	{
		map_name = std::move(new_map);
	}

	bool Split::get_track_horizontal() const
	{
		return track_horizontal_speed;
	}

	void Split::set_track_horizontal_speed(bool value)
	{
		track_horizontal_speed = value;
	}

	bool Split::get_track_vertical() const
	{
		return track_vertical_speed;
	}

	void Split::set_track_vertical_speed(bool value)
	{
		track_vertical_speed = value;
	}

	bool Split::get_track_x() const
	{
		return track_x;
	}

	void Split::set_track_x(bool value)
	{
		track_x = value;
	}

	bool Split::get_track_y() const
	{
		return track_y;
	}

	void Split::set_track_y(bool value)
	{
		track_y = value;
	}

	bool Split::get_track_z() const
	{
		return track_z;
	}

	void Split::set_track_z(bool value)
	{
		track_z = value;
	}

	bool Split::get_reached() const
	{
		return reached;
	}

	void Split::set_reached(bool value)
	{
		reached = value;
	}

	const Interprocess::Time& Split::get_time() const
	{
		return time;
	}

	void Split::set_time(Interprocess::Time value)
	{
		time = value;
	}

	const Vector Split::get_speed() const
	{
		return speed;
	}

	void Split::set_speed(Vector value)
	{
		speed = value;
	}

	const Vector Split::get_origin() const
	{
		return origin;
	}

	void Split::set_origin(Vector value)
	{
		origin = value;
	}

	bool Split::get_targets_entity() const
	{
		return targets_entity;
	}

	void Split::set_targets_entity(bool value)
	{
		targets_entity = value;
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
		if (split && !split->get_reached() && split->get_targets_entity()) {
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

			if (splits[i].get_reached())
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

		set_reached(true);
		set_time(CustomHud::GetTime());
		const auto& player = HwDLL::GetInstance().GetPlayerEdict();
		if (player)
		{
			set_speed(player->v.velocity);
			set_origin(player->v.origin);
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
			[&idOrName](const Split& s) { return !strcmp(idOrName, s.get_name().c_str()); });

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

		auto name     = split.get_name();
		auto map_name = split.get_map();

		const auto time  = split.get_time();
		const auto speed = split.get_speed();

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

		if (split.get_track_horizontal())
		{
			if (split.get_track_vertical())
				oss << dataSeparator << speed.Length() << " u/s 3D";
			else
				oss << dataSeparator << speed.Length2D() << " u/s horiz.";
		}
		else if (split.get_track_vertical())
			oss << dataSeparator << speed.z <<" u/s vert.";

		if (split.get_track_x())
			oss << dataSeparator << "x = " << split.get_origin().x;

		if (split.get_track_y())
			oss << dataSeparator << "y = " << split.get_origin().y;

		if (split.get_track_z())
			oss << dataSeparator << "z = " << split.get_origin().z;

		oss << dataSeparator << name << dataSeparator << map_name;

		return oss.str();
	}

	void PrintAll()
	{
		if (splits.empty())
			return;

		std::vector<Splits::Split> completedSplits;
		std::copy_if(Splits::splits.begin(), Splits::splits.end(), std::back_inserter(completedSplits),
			[&](Splits::Split &s){ return s.get_reached(); } );

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
			oss << i + 1 << ": `" << split.get_name() << "` - `" << split.get_map() << "` - ("
				<< corners.first.x << ", " << corners.first.y << ", " << corners.first.z << ") | ("
				<< corners.second.x << ", " << corners.second.y << ", " << corners.second.z << ")";

			if (split.get_track_horizontal())
			{
				if (split.get_track_vertical())
					oss << " - 3D";
				else
					oss << " - horizontal";
			}
			else if (split.get_track_vertical())
				oss << " - vertical";

			if (split.get_track_x())
				oss << " - tracking X";

			if (split.get_track_y())
				oss << " - tracking Y";

			if (split.get_track_z())
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
			split.set_reached(false);
			split.set_time(Interprocess::Time{});
			split.set_speed(Vector(0, 0, 0));
			split.set_origin(Vector(0, 0, 0));
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
