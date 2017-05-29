#include "stdafx.hpp"

#include "cvars.hpp"
#include "modules.hpp"
#include "runtime_data.hpp"
#include "TEA.hpp"

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/variant.hpp>

#define CEREAL_SIZE_TYPE uint32_t
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>

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
					auto to_move = std::min(n, free_space);
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

		private:
			Archive& archive;
		};
	}

	// Save implementation for our Data.
	template<typename Archive>
	static void save(Archive& archive, const Data& data) {
		boost::apply_visitor(save_visitor<Archive>(archive), data);
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
}
