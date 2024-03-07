#include "stdafx.hpp"

#include "cvars.hpp"
#include "modules.hpp"
#include "runtime_data.hpp"
#include "TEA.hpp"

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/variant.hpp>

#define CEREAL_SIZE_TYPE uint32_t
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>

// Attention! New runtime data types must be added AT THE END, to keep compatibility with parsing old demos.

namespace RuntimeData
{
	namespace
	{
		std::vector<Data> stored_data;

		const uint32_t KEY[4] = { 0x1337FACE, 0x12345678, 0xDEADBEEF, 0xFEEDABCD };
		constexpr uint8_t ESCAPE_BYTE = 0xFF;

		// Macro so that we can initialize arrays with it.
		#define HEADER "//BXTD0"

		enum class RuntimeDataType : uint8_t {
			VERSION_INFO = 1,
			CVAR_VALUES ,
			TIME,
			BOUND_COMMAND,
			ALIAS_EXPANSION,
			SCRIPT_EXECUTION,
			COMMAND_EXECUTION,
			GAME_END_MARKER,
			LOADED_MODULES,
			CUSTOM_TRIGGER_COMMAND,
			EDICTS,
			PLAYERHEALTH,
			SPLIT_MARKER,
			FLAGS,
		};

		// Encrypting filter.
		class encrypt_filter : public boost::iostreams::output_filter {
		public:
			constexpr static uint8_t FILL_BYTE = 0xFE;

			explicit encrypt_filter()
				: buffer()
				, pos(0) {
			}

			template<typename Sink>
			bool put(Sink& dest, char c) {
				buffer[pos++] = c;

				if (pos == 8) {
					encrypt();

					if (!write_buffer(dest))
						return false;

					pos = 0;
				}

				return true;
			}

			template<typename Sink>
			void close(Sink& dest) {
				if (pos == 0)
					return;

				for (; pos < 8; ++pos)
					buffer[pos] = FILL_BYTE;

				encrypt();
				write_buffer(dest);
			}

		private:
			void encrypt() {
				TEA::encrypt(reinterpret_cast<uint32_t*>(buffer.data()), KEY);
			}

			template<typename Sink>
			bool write_buffer(Sink& dest) {
				for (uint8_t i = 0; i < pos; ++i)
					if (!boost::iostreams::put(dest, buffer[i]))
						return false;

				return true;
			}

			std::array<uint8_t, 8> buffer;
			uint8_t pos;
		};

		// Decrypting filter.
		class decrypt_filter : public boost::iostreams::input_filter {
		public:
			template<typename Source>
			int get(Source& src) {
				if(pos == 0) {
					if(!fill_buffer(src))
						return EOF;

					decrypt();
				}

				int buf_char = (int)buffer[pos];
				pos = (pos + 1) % 8;

				return buf_char;
			}

		private:
			void decrypt() {
				TEA::decrypt(reinterpret_cast<uint32_t*>(buffer.data()), KEY);
			}

			template<typename Source>
			bool fill_buffer(Source& src) {
				for(int i = pos; i < 8; ++i)
				{
					int c = boost::iostreams::get(src);

					if (c == EOF || c == boost::iostreams::WOULD_BLOCK)
						return false;

					buffer[i] = (uint8_t)c;
				}

				return true;
			}

			std::array<uint8_t, 8> buffer;
			uint8_t pos;
		};

		// Filter that escapes special characters.
		class escape_filter : public boost::iostreams::dual_use_filter {
		public:
			const static std::unordered_map<uint8_t, uint8_t> ESCAPE_CHARACTERS;

			explicit escape_filter()
				: escaping(false) {
			}

			template<typename Sink>
			bool put(Sink& dest, char c) {
				const auto escape = ESCAPE_CHARACTERS.find(c);

				if (escape != ESCAPE_CHARACTERS.cend()) {
					if (!boost::iostreams::put(dest, ESCAPE_BYTE))
						return false;
					if (!boost::iostreams::put(dest, escape->second))
						return false;
				} else {
					if (!boost::iostreams::put(dest, c))
						return false;
				}

				return true;
			}

			template<typename Source>
			int get(Source& src) {
				int c = boost::iostreams::get(src);

				if (c == EOF || c == boost::iostreams::WOULD_BLOCK)
					return c;

				if (escaping) {
					escaping = false;
					return get_original(c);
				} else {
					if (c != ESCAPE_BYTE)
						return c;

					escaping = true;

					c = boost::iostreams::get(src);
					if (c == EOF || c == boost::iostreams::WOULD_BLOCK)
						return c;

					escaping = false;
					return get_original(c);
				}
			}

		private:
			int get_original(uint8_t escaped) {
				for (auto it = ESCAPE_CHARACTERS.cbegin(); it != ESCAPE_CHARACTERS.cend(); ++it)
					if (it->second == escaped)
						return it->first;

				return EOF;
			}

			bool escaping;
		};

		const std::unordered_map<uint8_t, uint8_t> escape_filter::ESCAPE_CHARACTERS = {
			{ 0x00, 0x01 },
			{ '"', 0x02 },
			{ '\n', 0x03 },
			{ ';', 0x04 },
			{ 0xFF, 0xFF },
		};

		// Sink that outputs BXT console command frames.
		class concmd_frame_sink : public boost::iostreams::sink {
		public:
			explicit concmd_frame_sink()
				: buffer(HEADER)
				, pos(sizeof(HEADER) - 1) {
			}

			std::streamsize write(const char* s, std::streamsize n) {
				const auto rv = n;

				do {
					std::streamsize free_space = 63 - pos;
					auto to_move = static_cast<uint8_t>(std::min(n, free_space));
					std::memmove(&buffer[pos], s, to_move);

					pos += to_move;
					n -= to_move;
					s += to_move;

					if (pos == 63) {
						HwDLL::GetInstance().StoreCommand(buffer);
						pos = sizeof(HEADER) - 1;
					}
				} while (n > 0);

				return rv;
			}

			void close() {
				if (pos == sizeof(HEADER) - 1)
					return;

				std::memset(&buffer[pos], 0, 63 - pos);
				HwDLL::GetInstance().StoreCommand(buffer);
			}

		private:
			char buffer[64];
			uint8_t pos;
		};

		template<typename Archive>
		class save_visitor : public boost::static_visitor<> {
		public:
			explicit save_visitor(Archive& archive)
				: archive(archive) {
			}

			void operator()(const VersionInfo& v) const {
				archive(RuntimeDataType::VERSION_INFO);

				archive(v.build_number);
				archive(v.bxt_version);
			}

			void operator()(const CVarValues& v) const {
				archive(RuntimeDataType::CVAR_VALUES);

				archive(v);
			}

			void operator()(const Time& t) const {
				archive(RuntimeDataType::TIME);

				archive(t.hours);
				archive(t.minutes);
				archive(t.seconds);
				archive(t.remainder);
			}

			void operator()(const BoundCommand& c) const {
				archive(RuntimeDataType::BOUND_COMMAND);

				archive(c.command);
			}

			void operator()(const AliasExpansion& e) const {
				archive(RuntimeDataType::ALIAS_EXPANSION);

				archive(e.name);
				archive(e.command);
			}

			void operator()(const ScriptExecution& e) const {
				archive(RuntimeDataType::SCRIPT_EXECUTION);

				archive(e.filename);
				archive(e.contents);
			}

			void operator()(const CommandExecution& e) const {
				archive(RuntimeDataType::COMMAND_EXECUTION);

				archive(e.command);
			}

			void operator()(const GameEndMarker& m) const {
				archive(RuntimeDataType::GAME_END_MARKER);
			}

			void operator()(const LoadedModules& m) const {
				archive(RuntimeDataType::LOADED_MODULES);

				archive(m.filenames);
			}

			void operator()(const CustomTriggerCommand& c) const {
				archive(RuntimeDataType::CUSTOM_TRIGGER_COMMAND);

				archive(c.corner_min.x);
				archive(c.corner_min.y);
				archive(c.corner_min.z);
				archive(c.corner_max.x);
				archive(c.corner_max.y);
				archive(c.corner_max.z);
				archive(c.command);
			}

			void operator()(const Edicts& e) const {
				archive(RuntimeDataType::EDICTS);

				archive(e.edicts);
			}

			void operator()(const PlayerHealth& p) const {
				archive(RuntimeDataType::PLAYERHEALTH);

				archive(p.health);
			}

			void operator()(const SplitMarker& m) const {
				archive(RuntimeDataType::SPLIT_MARKER);

				archive(m.corner_min.x);
				archive(m.corner_min.y);
				archive(m.corner_min.z);
				archive(m.corner_max.x);
				archive(m.corner_max.y);
				archive(m.corner_max.z);
				archive(m.name);
				archive(m.map_name);
			}

			void operator()(const Flags& f) const {
				archive(RuntimeDataType::FLAGS);

				archive(f.flags);
			}

		private:
			Archive& archive;
		};
	}

	// Save implementation for our Data.
	template<typename Archive>
	static void save(Archive& archive, const Data& data) {
		boost::apply_visitor(save_visitor<Archive>(archive), data);
	}

	template<class Archive>
	void load(Archive& archive, Data& data)
	{
		RuntimeDataType data_type;
		archive(data_type);

		switch (data_type)
		{
			case RuntimeDataType::VERSION_INFO: {
				VersionInfo v;
				archive(v.build_number);
				archive(v.bxt_version);

				data = v;
				break;
			}
			case RuntimeDataType::CVAR_VALUES: {
				CVarValues v;
				archive(v);
				data = v;
				break;
			}
			case RuntimeDataType::TIME: {
				Time t;
				archive(t.hours);
				archive(t.minutes);
				archive(t.seconds);
				archive(t.remainder);
				data = t;
				break;
			}
			case RuntimeDataType::BOUND_COMMAND: {
				BoundCommand c;
				archive(c.command);
				data = c;
				break;
			}
			case RuntimeDataType::ALIAS_EXPANSION: {
				AliasExpansion e;
				archive(e.name);
				archive(e.command);
				data = e;
				break;
			}
			case RuntimeDataType::SCRIPT_EXECUTION: {
				ScriptExecution e;
				archive(e.filename);
				archive(e.contents);
				data = e;
				break;
			}
			case RuntimeDataType::COMMAND_EXECUTION: {
				CommandExecution e;
				archive(e.command);
				data = e;
				break;
			}
			case RuntimeDataType::GAME_END_MARKER: {
				GameEndMarker m;
				data = m;
				break;
			}
			case RuntimeDataType::LOADED_MODULES: {
				LoadedModules m;
				archive(m.filenames);
				data = m;
				break;
			}
			case RuntimeDataType::CUSTOM_TRIGGER_COMMAND: {
				CustomTriggerCommand c;
				archive(c.corner_min.x);
				archive(c.corner_min.y);
				archive(c.corner_min.z);
				archive(c.corner_max.x);
				archive(c.corner_max.y);
				archive(c.corner_max.z);
				archive(c.command);
				data = c;
				break;
			}
			case RuntimeDataType::EDICTS: {
				Edicts e;
				archive(e.edicts);
				data = e;
				break;
			}
			case RuntimeDataType::PLAYERHEALTH: {
				PlayerHealth p;
				archive(p.health);
				data = p;
				break;
			}
			case RuntimeDataType::SPLIT_MARKER: {
				SplitMarker m;
				archive(m.corner_min.x);
				archive(m.corner_min.y);
				archive(m.corner_min.z);
				archive(m.corner_max.x);
				archive(m.corner_max.y);
				archive(m.corner_max.z);
				archive(m.name);
				archive(m.map_name);
				data = m;
				break;
			}
			case RuntimeDataType::FLAGS: {
				Flags f;
				archive(f.flags);
				data = f;
				break;
			}
			default: {
				EngineDevWarning("Read unknown RuntimeDataType %d\n", data_type);
				break;
			}
		}
	}

	void Add(Data data) {
		stored_data.push_back(std::move(data));
	}

	void Clear() {
		stored_data.clear();
	}

	void SaveStored() {
		if (!HwDLL::GetInstance().IsRecordingDemo()
			|| !CVars::_bxt_save_runtime_data_in_demos.GetBool())
			Clear();

		if (stored_data.empty())
			return;

		boost::iostreams::filtering_ostream out;
		out.push(encrypt_filter());
		out.push(escape_filter());
		out.push(concmd_frame_sink());

		cereal::BinaryOutputArchive oa(out);
		oa(stored_data);

		Clear();
	}

	// Visitor applied to each Data object found in demo.
	class demo_data_visitor : public boost::static_visitor<> {
	public:
		void operator()(const RuntimeData::Time& t) const {
			CustomHud::SetTime((int)t.hours, (int)t.minutes, (int)t.seconds, t.remainder);
		}

		void operator()(const RuntimeData::CommandExecution& e) const {
			for (const auto& cmd : cmd_whitelist) {
				if (e.command.rfind(cmd, 0) == 0) {
					HwDLL::GetInstance().ORIG_Cbuf_InsertText(e.command.c_str());
					HwDLL::GetInstance().ORIG_Cbuf_InsertText("\n");
				}
			}
		}
		
		void operator()(const RuntimeData::GameEndMarker& m) const {
			CustomHud::SetCountingTime(false);
		}

		void operator()(const RuntimeData::PlayerHealth& p) const {}
		void operator()(const RuntimeData::VersionInfo& v) const {}
		void operator()(const RuntimeData::CVarValues& v) const {}
		void operator()(const RuntimeData::BoundCommand& c) const {}
		void operator()(const RuntimeData::AliasExpansion& e) const {}
		void operator()(const RuntimeData::ScriptExecution& e) const {}
		void operator()(const RuntimeData::LoadedModules& m) const {}
		void operator()(const RuntimeData::CustomTriggerCommand& c) const {}
		void operator()(const RuntimeData::Edicts& e) const {}
		void operator()(const RuntimeData::SplitMarker& m) const {}
		void operator()(const RuntimeData::Flags& f) const {}

	private:
		// bxt commands that should be executed when found during demo playback
		const std::vector<std::string> cmd_whitelist =  {
			"bxt_timer_stop",
			"bxt_timer_start",
			"bxt_timer_reset"
		};
	};

	void ProcessRuntimeData(std::vector<char>& data) {
		if(data.empty())
			return;

		boost::iostreams::filtering_istream in;
		in.push(decrypt_filter());
		in.push(escape_filter());
		in.push(boost::iostreams::array_source(
			data.data(),
			data.size()
		));

		cereal::BinaryInputArchive ia(in);
		std::vector<Data> demo_data;
		
		try {
			ia(demo_data);
		} catch (...) {
			EngineDevWarning("Could not parse runtime data.\n");
		}

		for(const auto& d : demo_data)
			boost::apply_visitor(demo_data_visitor(), d);
	}
}
