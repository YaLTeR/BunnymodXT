#include "discord_integration.hpp"

namespace discord_integration
{
	namespace
	{
		// From Discord developer dashboard.
		constexpr const char CLIENT_ID[] = "1005991693406973993";

		// Text names of game states
		const std::string STATE_NAMES[] = {
			"In the menu"s,
			"Playing"s
		};

		// Possible game states.
		enum class game_state
		{
			NOT_PLAYING = 0,
			PLAYING
		};

		// For tracking if we're in-game.
		bool updated_client_data = false;

		// Start timestamp
		int64_t start_timestamp;

		// Shortcuts for call Client, Server or HwDLL functions
		auto &cl = ClientDLL::GetInstance();
		auto &sv = ServerDLL::GetInstance();
		auto &hw = HwDLL::GetInstance();

		// Class that handles tracking state changes.
		class DiscordState {
		public:
			DiscordState()
				: cur_state(game_state::NOT_PLAYING)
				, dirty(true)
			{
				update_presence();
			};

			inline void set_game_state(game_state new_game_state)
			{
				if (cur_state != new_game_state)
				{
					cur_state = new_game_state;

					dirty = true;
				}
			}

			inline game_state get_game_state() const
			{
				return cur_state;
			}

			inline void update_presence_if_dirty()
			{
				if (hw.Called_Timer)
				{
					hw.Called_Timer = false;

					dirty = true;
				}

				if (dirty)
					update_presence();
			}

		protected:
			void update_presence()
			{
				dirty = false;

				DiscordRichPresence presence{};

				std::string state = STATE_NAMES[static_cast<size_t>(cur_state)];

				// Default icon.
				presence.largeImageKey = "default";

				// Declare these outside of the following block, so they are in scope for Discord_UpdatePresence().
				char map_name[64];
				const char* game_dir = hw.GetGameDir(false).c_str();
				char buffer_details[128];

				#define set_discord_image(img) \
				if (BXT_CONCAT(img, _map_name_to_thumbnail).find(map_name) != BXT_CONCAT(img, _map_name_to_thumbnail).cend()) \
				{ \
					presence.largeImageKey = BXT_CONCAT(img, _map_name_to_thumbnail).find(map_name)->second.data(); \
					presence.largeImageText = BXT_CONCAT(img, _thumbnail_to_chapter).find(presence.largeImageKey)->second.data(); \
				}

				#define set_discord_image_as_map_name(map_list) \
				if (BXT_CONCAT(map_list, _maps).find(map_name) != BXT_CONCAT(map_list, _maps).cend()) \
				{ \
					presence.largeImageKey = map_name; \
					presence.largeImageText = map_name; \
				}

				if (game_dir && game_dir[0])
				{
					// Yes GCC we know this can be truncated.
					#if defined(__GNUC__) && !defined(__clang__)
					#pragma GCC diagnostic push
					#pragma GCC diagnostic ignored "-Wformat-truncation"
					#endif
					snprintf(buffer_details, sizeof(buffer_details), "Build: %i | Game: %s", hw.GetBuildNumber(), game_dir);
					#if defined(__GNUC__) && !defined(__clang__)
					#pragma GCC diagnostic pop
					#endif

					presence.details = buffer_details;
				}

				if (cur_state != game_state::NOT_PLAYING)
				{
					// Get the map name and icon.
					cl.GetMapName(map_name, ARRAYSIZE_HL(map_name));
					if (game_dir && game_dir[0] && map_name[0])
					{
						// Adjust map_name to lowercase
						helper_functions::convert_to_lowercase(map_name);

						// Yes GCC we know this can be truncated.
						#if defined(__GNUC__) && !defined(__clang__)
						#pragma GCC diagnostic push
						#pragma GCC diagnostic ignored "-Wformat-truncation"
						#endif
						snprintf(buffer_details, sizeof(buffer_details), "Build: %i | Map: %s | Game: %s", hw.GetBuildNumber(), map_name, game_dir);
						#if defined(__GNUC__) && !defined(__clang__)
						#pragma GCC diagnostic pop
						#endif

						presence.largeImageText = map_name;
						presence.details = buffer_details;

						if (IsGameDirMatch(HL))
						{
							set_discord_image(hl1)
						}
						else if (IsGameDirMatch(OPFOR))
						{
							set_discord_image(op4)
						}
						else if (IsGameDirStartsWith(BSHIFT))
						{
							set_discord_image(bs)
						}
						else if (IsGameDirStartsWith(GUNMAN))
						{
							if (gmc_map_name_to_thumbnail.find(map_name) != gmc_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = gmc_map_name_to_thumbnail.find(map_name)->second.data();

								if (!strncmp(presence.largeImageKey, "gmcchapter4", 11))
									presence.largeImageText = "Rust";
								else
									presence.largeImageText = gmc_thumbnail_to_chapter.find(presence.largeImageKey)->second.data();
							}
						}
						else if (IsGameDirStartsWith(CSCZDS))
						{
							set_discord_image(czds)
						}
						else if (IsGameDirMatch(WANTED))
						{
							set_discord_image(wanted)
						}
						else if (IsGameDirMatch(ECHOES))
						{
							set_discord_image(echoes)
						}
						else if (IsGameDirMatch(CAGED))
						{
							set_discord_image(caged)
						}
						else if (IsGameDirStartsWith(POKE646))
						{
							set_discord_image(poke646)
						}
						else if (IsGameDirStartsWith(PARANOIA))
						{
							if (paranoia_map_name_to_thumbnail.find(map_name) != paranoia_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = paranoia_map_name_to_thumbnail.find(map_name)->second.data();

								if (!strncmp(presence.largeImageKey, "paranoiachapter1", 16))
									presence.largeImageText = "Army";
								else if (!strncmp(presence.largeImageKey, "paranoiachapter2", 16))
									presence.largeImageText = "Industrial";
								else if (!strncmp(presence.largeImageKey, "paranoiachapter3", 16))
									presence.largeImageText = "Bunker";
							}
						}
						else if (IsGameDirMatch(TWHLTOWER2))
						{
							set_discord_image(twhltower2)
						}
						else if (IsGameDirMatch(AOMDC))
						{
							if (aomdc_map_name_to_thumbnail.find(map_name) != aomdc_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = aomdc_map_name_to_thumbnail.find(map_name)->second.data();

								if (aomdc_map_name_to_chapter_misc.find(map_name) != aomdc_map_name_to_chapter_misc.cend())
									presence.largeImageText = aomdc_map_name_to_chapter_misc.find(map_name)->second.data();
								else if (!strncmp(map_name, "1", 1) || !strncmp(map_name, "cityx", 5))
									presence.largeImageText = "Ending 1 or 4";
								else if (!strncmp(map_name, "2", 1))
									presence.largeImageText = "Ending 2 or 4";
								else if (!strncmp(map_name, "3", 1) || !strncmp(map_name, "cityz", 5))
									presence.largeImageText = "Ending 3 or 4";
								else if (!strncmp(map_name, "4", 1))
									presence.largeImageText = "Ending 4";
								else
									presence.largeImageText = "All Endings";
							}
						}
						else if (IsGameDirMatch(HLRATS_PARASOMNIA))
						{
							set_discord_image(hlrats_parasomnia)
						}
						else if (IsGameDirMatch(URBICIDE))
						{
							set_discord_image_as_map_name(urbicide)
						}
						else if (IsGameDirStartsWith(HUNGER))
						{
							if (th_map_name_to_thumbnail.find(map_name) != th_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = th_map_name_to_thumbnail.find(map_name)->second.data();

								if (!strncmp(presence.largeImageKey, "thchapter1", 10))
									presence.largeImageText = "They Hunger Episode 1";
								else if (!strncmp(presence.largeImageKey, "thchapter2", 10))
									presence.largeImageText = "They Hunger Episode 2";
								else if (!strncmp(presence.largeImageKey, "thchapter3", 10))
									presence.largeImageText = "They Hunger Episode 3";
							}
						}
						else if (IsGameDirStartsWith(COF))
						{
							set_discord_image(cof)
						}
					}
				}

				char buffer_state[128];
				char buffer_stop[128];

				double total_time = helper_functions::ret_bxt_time();
				const auto current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				start_timestamp = current_timestamp - static_cast<int>(total_time);

				const char *skillName;

				switch (CVars::skill.GetInt())
				{
					case 1:
						skillName = "Easy";
						break;
					case 2:
						skillName = "Normal";
						break;
					case 3:
						skillName = "Hard";
						break;
					default:
						skillName = "";
				}

				const auto& gt = CustomHud::GetTime();

				if (CVars::host_framerate.GetFloat() > 0.0f)
					snprintf(buffer_state, sizeof(buffer_state), "%s | FPS (HFR): %.1f | %s", state.c_str(), 1.0f / CVars::host_framerate.GetFloat(), skillName);
				else
					snprintf(buffer_state, sizeof(buffer_state), "%s | FPS: %.1f | %s", state.c_str(), CVars::fps_max.GetFloat(), skillName);
				presence.state = buffer_state;

				if (CustomHud::GetCountingTime())
				{
					if (CVars::bxt_autopause.GetBool()) {
						presence.smallImageKey = "discord_blue";
						presence.smallImageText = "Segmenting";
					} else if (CustomHud::GetInvalidRun()) {
						presence.smallImageKey = "discord_pink";
						presence.smallImageText = "Invalid run (game crash prevented by BXT)";
					} else {
						presence.smallImageKey = "discord_green";
						presence.smallImageText = "Run in progress";
					}

					presence.startTimestamp = start_timestamp;
				}
				else if ((total_time == 0.0) && !CustomHud::GetCountingTime())
				{
					if (cl.pEngfuncs && cl.pEngfuncs->pDemoAPI->IsPlayingback()) {
						presence.smallImageKey = "discord_brown";
						presence.smallImageText = "Watching a demo";
					} else {
						presence.smallImageKey = "discord_red";
						presence.smallImageText = "Not running";
					}

					presence.startTimestamp = current_timestamp;
				}
				else if ((total_time > 0.0) && !CustomHud::GetCountingTime())
				{
					snprintf(buffer_stop, sizeof(buffer_stop), "Timer stopped at %d:%02d:%02d.%03d", gt.hours, gt.minutes, gt.seconds, gt.milliseconds);
					presence.state = buffer_stop;

					if (cl.pEngfuncs && cl.pEngfuncs->pDemoAPI->IsPlayingback()) {
						presence.smallImageKey = "discord_brown";
						presence.smallImageText = "Watching a demo";
					} else {
						presence.smallImageKey = "discord_yellow";
						presence.smallImageText = "Timer stopped";
					}

					presence.startTimestamp = 0;
				}

				presence.buttonLabel[0] = "Download";
				presence.buttonLabel[1] = "Join SourceRuns Discord";
				presence.buttonUrl[0] = "https://github.com/YaLTeR/BunnymodXT/releases";
				presence.buttonUrl[1] = "https://discord.gg/sourceruns";

				Discord_UpdatePresence(&presence);
			}

			game_state cur_state;

			// Flag indicating there are some changes not sent to Discord yet.
			bool dirty;
		};

		// Pointer so the constructor doesn't run too early.
		std::unique_ptr<DiscordState> discord_state;

		void handle_ready(const DiscordUser*)
		{
			if (cl.pEngfuncs)
				cl.pEngfuncs->Con_Printf(const_cast<char*>("Connected to Discord.\n"));
		}

		void handle_errored(int error_code, const char* message)
		{
			if (cl.pEngfuncs)
				cl.pEngfuncs->Con_Printf(const_cast<char*>("Discord error (%d): %s\n"), error_code, message);
		}

		void handle_disconnected(int error_code, const char* message)
		{
			if (cl.pEngfuncs)
				cl.pEngfuncs->Con_Printf(const_cast<char*>("Disconnected from Discord (%d): %s\n"), error_code, message);
		}
	}

	void initialize()
	{
		DiscordEventHandlers handlers{};
		handlers.ready = handle_ready;
		handlers.errored = handle_errored;
		handlers.disconnected = handle_disconnected;
		Discord_Initialize(CLIENT_ID, &handlers, 1, NULL);

		discord_state = std::make_unique<DiscordState>();

		Discord_RunCallbacks();
	}

	void on_update_client_data()
	{
		updated_client_data = true;
	}

	void on_frame()
	{
		static bool bxtDisableDiscordRPC = getenv("BXT_DISABLE_DISCORD_RPC");
		if (bxtDisableDiscordRPC)
			return;

		static bool rpc_initialized = false;

		if (!rpc_initialized)
		{
			initialize();
			rpc_initialized = true;
		}

		float FPS_current;
		if (CVars::host_framerate.GetFloat() > 0.0f)
			FPS_current = CVars::host_framerate.GetFloat();
		else
			FPS_current = CVars::fps_max.GetFloat();

		static float FPS_previous = FPS_current;

		if (FPS_current != FPS_previous)
			hw.Called_Timer = true;

		FPS_previous = FPS_current;

		// Check if we're still in-game.
		if (!updated_client_data)
			discord_state->set_game_state(game_state::NOT_PLAYING);
		else if (discord_state->get_game_state() == game_state::NOT_PLAYING)
			// Only set this if we weren't playing, otherwise we might overwrite some other state.
			discord_state->set_game_state(game_state::PLAYING);

		updated_client_data = false;

		discord_state->update_presence_if_dirty();

		Discord_RunCallbacks();
	}
}
