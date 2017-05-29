#pragma once

#include <algorithm>
#include <utility>

#define USAGE(text) inline static auto Usage() { return CmdWrapper::has_usage_t{ text }; }
#define NO_USAGE() inline static auto Usage() { return CmdWrapper::has_no_usage_t{}; }
#define GET_USAGE() Usage()

namespace CmdWrapper
{
	struct has_usage_t {
		const char* text;
	};
	struct has_no_usage_t {};

	template<typename T>
	struct Parser;

	template<>
	struct Parser<int>
	{
		inline static int Parse(const char *s)
		{
			return std::atoi(s);
		}
	};

	template<>
	struct Parser<unsigned long>
	{
		inline static unsigned long Parse(const char *s)
		{
			return std::strtoul(s, nullptr, 10);
		}
	};

	template<>
	struct Parser<float>
	{
		inline static float Parse(const char *s)
		{
			return std::atof(s);
		}
	};

	template<>
	struct Parser<const char *>
	{
		inline static const char *Parse(const char *s)
		{
			return s;
		}
	};

	template<typename... Args>
	class Handler
	{
	public:
		template<typename CmdFuncs, typename H, typename Indices = std::make_integer_sequence<int, sizeof...(Args)>>
		inline static bool Call(int argc)
		{
			if (argc == sizeof...(Args) + 1) {
				CallImpl<CmdFuncs, H>(Indices());
				return true;
			} else {
				return false;
			}
		}

	private:
		template<typename CmdFuncs, typename H, int... Is>
		inline static void CallImpl(std::integer_sequence<int, Is...>)
		{
			H::handler(Parser<Args>::Parse(CmdFuncs::Argv(Is + 1))...);
		}
	};

	template<typename CmdFuncs>
	class CmdWrapper
	{
	public:
		template<typename H, typename... Handlers>
		static void Add(const char *name)
		{
			CmdFuncs::AddCommand(name, [] {
				CallHandlers<H, Handlers...>(CmdFuncs::Argc());
			});
		}

		template<typename H, typename... Handlers>
		static void AddCheat(const char *name)
		{
			CmdFuncs::AddCommand(name, [] {
				if (CmdFuncs::IsCheating())
					CallHandlers<H, Handlers...>(CmdFuncs::Argc());
			});
		}

	private:
		inline static void PrintUsage(has_no_usage_t)
		{
			// No usage, do nothing.
		}

		inline static void PrintUsage(has_usage_t usage)
		{
			CmdFuncs::UsagePrint(usage.text);
		}

		template<typename H>
		inline static void CallHandlers(int argc)
		{
			PrintUsage(H::Usage());
		}

		template<typename H, typename Handler, typename... Handlers>
		inline static void CallHandlers(int argc)
		{
			if (!Handler::template Call<CmdFuncs, H>(argc))
				CallHandlers<H, Handlers...>(argc);
		}
	};
}
