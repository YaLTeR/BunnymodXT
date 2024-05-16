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
				if (hw.discord_rpc_update_called)
				{
					hw.discord_rpc_update_called = false;

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
				const char* map_name = cl.GetLevelName(false).c_str();
				const char* game_dir = cl.GetGameDirectory(false).c_str();
				char buffer_details[128];

				#define set_discord_image(imgkey, imgtext) \
				if (imgkey.find(map_name) != imgkey.cend()) \
				{ \
					presence.largeImageKey = imgkey.find(map_name)->second.data(); \
					presence.largeImageText = imgtext.find(presence.largeImageKey)->second.data(); \
				}

				#define set_discord_image_as_map_name(map_list) \
				if (map_list.find(map_name) != map_list.cend()) \
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
					if (hw.ORIG_build_number)
						snprintf(buffer_details, sizeof(buffer_details), "Build: %i | Game: %s", hw.ORIG_build_number(), game_dir);
					else
						snprintf(buffer_details, sizeof(buffer_details), "Game: %s", game_dir);
					#if defined(__GNUC__) && !defined(__clang__)
					#pragma GCC diagnostic pop
					#endif

					presence.details = buffer_details;
				}

				if (cur_state != game_state::NOT_PLAYING)
				{
					if (game_dir && game_dir[0] && map_name && map_name[0])
					{
						// Yes GCC we know this can be truncated.
						#if defined(__GNUC__) && !defined(__clang__)
						#pragma GCC diagnostic push
						#pragma GCC diagnostic ignored "-Wformat-truncation"
						#endif
						if (hw.ORIG_build_number)
							snprintf(buffer_details, sizeof(buffer_details), "Build: %i | Map: %s | Game: %s", hw.ORIG_build_number(), map_name, game_dir);
						else
							snprintf(buffer_details, sizeof(buffer_details), "Map: %s | Game: %s", map_name, game_dir);
						#if defined(__GNUC__) && !defined(__clang__)
						#pragma GCC diagnostic pop
						#endif

						presence.largeImageText = map_name;
						presence.details = buffer_details;

						map_name = cl.GetLevelName(true).c_str();
						game_dir = cl.GetGameDirectory(true).c_str();

						if (CVars::bxt_discord_rpc_ignore_gamedir_for_hl_maps.GetBool()
						|| HF_DoesGameDirStartsWith("valve")
						|| HF_DoesGameDirMatch("abh")
						|| HF_DoesGameDirMatch("glitchless"))
							set_discord_image(hl1_map_name_to_thumbnail, hl1_thumbnail_to_chapter)

						if (CVars::bxt_discord_rpc_ignore_gamedir_for_hl_maps.GetBool() || HF_DoesGameDirStartsWith("gearbox"))
							set_discord_image(op4_map_name_to_thumbnail, op4_thumbnail_to_chapter)

						if (HF_DoesGameDirStartsWith("bshift"))
						{
							if (bs_map_name_to_thumbnail.find(map_name) != bs_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = bs_map_name_to_thumbnail.find(map_name)->second.data();

								int state;
								if (!strcmp(map_name, "ba_teleport2") && sv.GetGlobalState("powercomplete"s, state) && state == 1)
									presence.largeImageText = "A Leap of Faith";
								else
									presence.largeImageText = bs_thumbnail_to_chapter.find(presence.largeImageKey)->second.data();
							}
						}
						else if (HF_DoesGameDirStartsWith("rewolf"))
						{
							if (gmc_map_name_to_thumbnail.find(map_name) != gmc_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = gmc_map_name_to_thumbnail.find(map_name)->second.data();

								if (!strncmp(presence.largeImageKey, HF_StrAndLen("gmcchapter4")))
									presence.largeImageText = "Rust";
								else
									presence.largeImageText = gmc_thumbnail_to_chapter.find(presence.largeImageKey)->second.data();
							}
						}
						else if (hw.is_csczds_dir)
						{
							set_discord_image(czds_map_name_to_thumbnail, czds_thumbnail_to_chapter)
						}
						else if (HF_DoesGameDirStartsWith("wantedsp"))
						{
							set_discord_image(wanted_map_name_to_thumbnail, wanted_thumbnail_to_chapter)
						}
						else if (HF_DoesGameDirStartsWith("echoes"))
						{
							set_discord_image(echoes_map_name_to_thumbnail, echoes_thumbnail_to_chapter)
						}
						else if (HF_DoesGameDirStartsWith("caged_fgs"))
						{
							set_discord_image(caged_map_name_to_thumbnail, caged_thumbnail_to_chapter)
						}
						else if (HF_DoesGameDirStartsWith("poke646"))
						{
							set_discord_image(poke646_map_name_to_thumbnail, poke646_thumbnail_to_chapter)
						}
						else if (HF_DoesGameDirStartsWith("paranoia"))
						{
							if (paranoia_map_name_to_thumbnail.find(map_name) != paranoia_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = paranoia_map_name_to_thumbnail.find(map_name)->second.data();

								if (!strncmp(presence.largeImageKey, HF_StrAndLen("paranoiachapter1")))
									presence.largeImageText = "Army";
								else if (!strncmp(presence.largeImageKey, HF_StrAndLen("paranoiachapter2")))
									presence.largeImageText = "Industrial";
								else if (!strncmp(presence.largeImageKey, HF_StrAndLen("paranoiachapter3")))
									presence.largeImageText = "Bunker";
							}
						}
						else if (HF_DoesGameDirStartsWith("twhltower2"))
						{
							set_discord_image(twhltower2_map_name_to_thumbnail, twhltower2_thumbnail_to_chapter)
						}
						else if (HF_DoesGameDirStartsWith("aomdc"))
						{
							if (aomdc_map_name_to_thumbnail.find(map_name) != aomdc_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = aomdc_map_name_to_thumbnail.find(map_name)->second.data();

								if (aomdc_map_name_to_chapter_misc.find(map_name) != aomdc_map_name_to_chapter_misc.cend())
									presence.largeImageText = aomdc_map_name_to_chapter_misc.find(map_name)->second.data();
								else if (!strncmp(map_name, HF_StrAndLen("1")) || !strncmp(map_name, HF_StrAndLen("cityx")))
									presence.largeImageText = "Ending 1 or 4";
								else if (!strncmp(map_name, HF_StrAndLen("2")))
									presence.largeImageText = "Ending 2 or 4";
								else if (!strncmp(map_name, HF_StrAndLen("3")) || !strncmp(map_name, HF_StrAndLen("cityz")))
									presence.largeImageText = "Ending 3 or 4";
								else if (!strncmp(map_name, HF_StrAndLen("4")))
									presence.largeImageText = "Ending 4";
								else
									presence.largeImageText = "All Endings";
							}
						}
						else if (HF_DoesGameDirStartsWith("hrp"))
						{
							set_discord_image(hlrats_parasomnia_map_name_to_thumbnail, hlrats_parasomnia_thumbnail_to_chapter)
						}
						else if (HF_DoesGameDirStartsWith("hl_urbicide"))
						{
							set_discord_image_as_map_name(urbicide_maps)
						}
						else if (HF_DoesGameDirStartsWith("hunger"))
						{
							if (th_map_name_to_thumbnail.find(map_name) != th_map_name_to_thumbnail.cend())
							{
								presence.largeImageKey = th_map_name_to_thumbnail.find(map_name)->second.data();

								if (!strncmp(presence.largeImageKey, HF_StrAndLen("thchapter1")))
									presence.largeImageText = "They Hunger Episode 1";
								else if (!strncmp(presence.largeImageKey, HF_StrAndLen("thchapter2")))
									presence.largeImageText = "They Hunger Episode 2";
								else if (!strncmp(presence.largeImageKey, HF_StrAndLen("thchapter3")))
									presence.largeImageText = "They Hunger Episode 3";
							}
						}
						else if (hw.is_cof_dir)
						{
							set_discord_image(cof_map_name_to_thumbnail, cof_thumbnail_to_chapter)
						}
					}
				}

				double total_time = helper_functions::ret_bxt_time();
				const auto current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				start_timestamp = current_timestamp - static_cast<int>(total_time);

				const char *skillName = helper_functions::get_difficulty(CVars::skill.GetInt()).c_str();

				char buffer_state[128];
				char buffer_stop[128];

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

		if (!hw.discord_rpc_initialized)
		{
			initialize();
			hw.discord_rpc_initialized = true;
		}

		float FPS_current;
		if (CVars::host_framerate.GetFloat() > 0.0f)
			FPS_current = CVars::host_framerate.GetFloat();
		else
			FPS_current = CVars::fps_max.GetFloat();

		static float FPS_previous = FPS_current;

		if (FPS_current != FPS_previous)
			hw.discord_rpc_update_called = true;

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
