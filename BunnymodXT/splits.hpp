#pragma once

#include "custom_triggers.hpp"
#include "interprocess.hpp"

namespace Splits
{
	class Split : public CustomTriggers::Trigger
	{
		// If non-empty, it means this split is scoped to a single map, and
		// it won't work outside of it. If empty, it's applied on every map
		std::string map_name;

		// Split name that can be matched with the list of "maps" provided in BunnySplit
		std::string name;

		// Whether this split is gonna account for the horizontal and/or vertical components
		// of velocity when printing the speed at this split. Sometimes you just want to know
		// the vertical speed, because you go through the split as you're falling down
		bool track_horizontal_speed;
		bool track_vertical_speed;

		// Whether this split will print the different components of the position where you touch
		// the split trigger/entity. e.g.: if there's a split right after a door that opens upwards
		// and you usually try to go under the door with some speed as soon as the you can fit through
		// the gap, then the height (Z) at which you touch the trigger can be useful data, thus you can
		// tell if that specific part was optimal or not
		bool track_x;
		bool track_y;
		bool track_z;

		// Whether the player already went through this split
		bool reached;
		
		// Time, speed and position at which the player got to this split
		Interprocess::Time time;
		Vector speed;
		Vector origin;

		bool targets_entity;
		
	public:
		Split(); // For bxt_split and changelevel triggers
		Split(std::string name);
		Split(std::string name, std::string map_name);
		Split(Vector corner1, Vector corner2);
		Split(Vector corner1, Vector corner2, std::string map_name);
		Split(Vector corner1, Vector corner2, std::string map_name, std::string name);

		void set_command(std::string new_command); // override

		const std::string& get_name() const;
		void set_name(std::string new_name);
		
		const std::string& get_map() const;
		void set_map(std::string new_map);
		
		bool get_track_horizontal() const;
		void set_track_horizontal_speed(bool value);
		
		bool get_track_vertical() const;
		void set_track_vertical_speed(bool value);
		
		bool get_track_x() const;
		void set_track_x(bool value);
		
		bool get_track_y() const;
		void set_track_y(bool value);
		
		bool get_track_z() const;
		void set_track_z(bool value);
		
		bool get_reached() const;
		void set_reached(bool value);
		
		const Interprocess::Time& get_time() const;
		void set_time(Interprocess::Time value);

		const Vector get_speed() const;
		void set_speed(Vector value);

		const Vector get_origin() const;
		void set_origin(Vector value);
		
		bool get_targets_entity() const;
		void set_targets_entity(bool value);

		void activate();

	protected:
		void touch(); // override
	};

	extern std::vector<Split> splits;

	// We need to keep track of the placement start because normalize()
	// can cause the start and end points to be swapped.
	extern Vector place_start;
	extern bool placing;
	extern Split* last_reached;
	extern bool printed_on_end;

	void Activate(const char* id_or_name);

	Split* GetSplitByNameOrId(const char* id_or_name);
	Split* GetSplitByNameOrId(const char* id_or_name, bool warn);
	const std::string GetSummary(Split& split);

	void PrintAll();
	void PrintList(std::vector<Split> splits);
	void PrintSplitCompletion(Split& split);

	void Reset();

	void Update(const Vector& player_position, bool ducking);
	void Update(const Vector& player_position_start, const Vector& player_position_end, bool ducking);
}
