#pragma once

namespace CustomTriggers
{
	// To be extended as new ways of defining the triggers are added.
	class Trigger
	{
		// Opposite corners of an axis-aligned parallelepiped.
		Vector corner_min, corner_max;

		// The command to be executed.
		std::string command;

		// Whether the player is currently touching this trigger.
		bool player_touching;

		// Normalize the corners so corner_min has the min coordinates and corner_max the max ones.
		void normalize();

		void touch();

	public:
		Trigger(Vector corner1, Vector corner2);
		Trigger(Vector corner1, Vector corner2, std::string command);

		std::pair<Vector, Vector> get_corner_positions() const;
		const std::string& get_command() const;
		void set_command(std::string new_command);

		void update(const Vector& player_position, bool ducking);

		/*
		 * The player has moved from player_position_start to player_position_end.
		 * Check if they touched the trigger anywhere during that (linear) motion.
		 */
		void update(const Vector& player_position_start, const Vector& player_position_end, bool ducking);
	};

	extern std::vector<Trigger> triggers;

	void Update(const Vector& player_position, bool ducking);
	void Update(const Vector& player_position_start, const Vector& player_position_end, bool ducking);
}
