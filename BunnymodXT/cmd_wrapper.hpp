#pragma once

#include <utility>
#include <algorithm>

#define USAGE(text) inline static const char *usage() { return text; }
#define NO_USAGE() inline static const char *usage() { return ""; }

namespace CmdWrapper
{
	template<typename T>
	struct Parser;

	template<>
	struct Parser<int>
	{
		inline static int parse(const char *s)
		{
			return std::atoi(s);
		}
	};

	template<>
	struct Parser<float>
	{
		inline static float parse(const char *s)
		{
			return std::atof(s);
		}
	};

	template<>
	struct Parser<const char *>
	{
		inline static const char *parse(const char *s)
		{
			return s;
		}
	};

	template<typename... Args>
	class Handler
	{
	public:
		template<typename CmdFuncs, typename H, typename Indices = std::make_integer_sequence<int, sizeof...(Args)>>
		inline static bool call(int argc)
		{
			if (argc == sizeof...(Args) + 1) {
				callImpl<CmdFuncs, H>(Indices());
				return true;
			} else {
				return false;
			}
		}

	private:
		template<typename CmdFuncs, typename H, int... Is>
		inline static void callImpl(std::integer_sequence<int, Is...>)
		{
			H::handler(Parser<Args>::parse(CmdFuncs::Argv(Is + 1))...);
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
				CallHandlers<H, Handlers...>();
			});
		}

		template<typename H, typename... Handlers>
		static void AddCheat(const char *name)
		{
			CmdFuncs::AddCommand(name, [] {
				if (CmdFuncs::IsCheating())
					CallHandlers<H, Handlers...>();
			});
		}

	private:
		template<typename H, typename... Handlers>
		static void CallHandlers()
		{
			const int argc = CmdFuncs::Argc();
			std::array<bool, sizeof...(Handlers)> results{ Handlers::template call<CmdFuncs, H>(argc)... };
			if (std::none_of(results.cbegin(), results.cend(), [](const bool &b) { return b; })) {
				CmdFuncs::UsagePrint(H::usage());
			}
		}
	};
}
