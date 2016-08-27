#include "stdafx.hpp"

#include "modules/HwDLL.hpp"
#include "custom_triggers.hpp"

namespace CustomTriggers
{
	std::vector<Trigger> triggers;

	void Trigger::normalize()
	{
		Vector c1 = corner_min, c2 = corner_max;

		corner_min.x = std::min(c1.x, c2.x);
		corner_min.y = std::min(c1.y, c2.y);
		corner_min.z = std::min(c1.z, c2.z);

		corner_max.x = std::max(c1.x, c2.x);
		corner_max.y = std::max(c1.y, c2.y);
		corner_max.z = std::max(c1.z, c2.z);
	}

	void Trigger::touch()
	{
		HwDLL::GetInstance().ORIG_Cbuf_InsertText(command.c_str());
	}

	Trigger::Trigger(Vector corner1, Vector corner2)
		: corner_min(corner1)
		, corner_max(corner2)
		, command()
		, player_touching(false)
	{
		normalize();
	}

	Trigger::Trigger(Vector corner1, Vector corner2, std::string command)
		: corner_min(corner1)
		, corner_max(corner2)
		, command(command)
		, player_touching(false)
	{
		normalize();
	}

	std::pair<Vector, Vector> Trigger::get_corner_positions() const
	{
		return std::make_pair(corner_min, corner_max);
	}

	const std::string& Trigger::get_command() const
	{
		return command;
	}

	void Trigger::set_command(std::string new_command)
	{
		command = std::move(new_command);

		// Make sure the command is correctly terminated.
		if (!command.empty()) {
			const auto last_char = command.back();

			if (last_char != '\n' && last_char != ';')
				command += '\n';
		}
	}

	void Trigger::update(const Vector& player_position, bool ducking)
	{
		const Vector VEC_HULL_MIN(-16, -16, -36);
		const Vector VEC_HULL_MAX(16,  16,  36);
		const Vector VEC_DUCK_HULL_MIN(-16, -16, -18);
		const Vector VEC_DUCK_HULL_MAX(16,  16,  18);

		const Vector& hull_max = ducking ? VEC_DUCK_HULL_MAX : VEC_HULL_MAX;
		const Vector& hull_min = ducking ? VEC_DUCK_HULL_MIN : VEC_HULL_MIN;

		if (player_position[0] + hull_min[0] < corner_max[0]
			&& player_position[1] + hull_min[1] < corner_max[1]
			&& player_position[2] + hull_min[2] < corner_max[2]
			&& player_position[0] + hull_max[0] > corner_min[0]
			&& player_position[1] + hull_max[1] > corner_min[1]
			&& player_position[2] + hull_max[2] > corner_min[2]) {
			// The player is touching the trigger.
			if (!player_touching)
				touch();

			player_touching = true;
		} else {
			player_touching = false;
		}
	}

	void Update(const Vector& player_position, bool ducking)
	{
		for (auto& trigger : triggers)
			trigger.update(player_position, ducking);
	}
}
