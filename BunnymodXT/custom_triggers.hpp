#pragma once

namespace CustomTriggers
{
	// To be extended as new ways of defining the triggers are added.
	class Trigger
	{
		// The command to be executed.
		std::string command;

		// Whether the player is currently touching this trigger.
		bool player_touching;

		// Normalize the corners so corner_min has the min coordinates and corner_max the max ones.
		void normalize();

	protected:
		// Opposite corners of an axis-aligned parallelepiped.
		Vector corner_min, corner_max;

		virtual void touch();

	public:
		Trigger(Vector corner1, Vector corner2);
		Trigger(Vector corner1, Vector corner2, std::string command);

		std::pair<Vector, Vector> get_corner_positions() const;
		const std::string& get_command() const;
		void set_command(std::string new_command);
		void set_corner_positions(Vector a, Vector b);

		void update(const Vector& player_position, bool ducking);

		/*
		 * The player has moved from player_position_start to player_position_end.
		 * Check if they touched the trigger anywhere during that (linear) motion.
		 */
		void update(const Vector& player_position_start, const Vector& player_position_end, bool ducking);
	};

	extern std::vector<Trigger> triggers;

	// We need to keep track of the placement start because the Trigger.normalize()
	// can cause the start and end points to be swapped.
	extern Vector place_start;
	extern bool placing;

	void Update(const Vector& player_position, bool ducking);
	void Update(const Vector& player_position_start, const Vector& player_position_end, bool ducking);
}
