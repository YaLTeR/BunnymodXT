#pragma once

#include <utility>
#include <algorithm>

#define CMDWRAPPER_USAGE(name, usage) struct name { inline static const char *text() { return usage; } };

namespace CmdWrapper
{
	CMDWRAPPER_USAGE(NoUsage, "");

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

	template<typename H, typename... Args>
	class Handler
	{
	public:
		template<typename CmdFuncs, typename Indices = std::make_integer_sequence<int, sizeof...(Args)>>
		inline static bool call(int argc)
		{
			if (argc == sizeof...(Args) + 1) {
				callImpl<CmdFuncs>(Indices());
				return true;
			} else {
				return false;
			}
		}

	private:
		template<typename CmdFuncs, int... Is>
		inline static void callImpl(std::integer_sequence<int, Is...>)
		{
			H::handler(Parser<Args>::parse(CmdFuncs::Argv(Is + 1))...);
		}
	};

	template<typename CmdFuncs>
	class CmdWrapper
	{
	public:
		template<typename Usage, typename... Handlers>
		static void Add(const char *name)
		{
			CmdFuncs::AddCommand(name, [] {
				CallHandlers<Usage, Handlers...>();
			});
		}

		template<typename Usage, typename... Handlers>
		static void AddCheat(const char *name)
		{
			CmdFuncs::AddCommand(name, [] {
				if (CmdFuncs::IsCheating())
					CallHandlers<Usage, Handlers...>();
			});
		}

	private:
		template<typename Usage, typename... Handlers>
		static void CallHandlers()
		{
			const int argc = CmdFuncs::Argc();
			std::array<bool, sizeof...(Handlers)> results{ Handlers::template call<CmdFuncs>(argc)... };
			if (std::none_of(results.cbegin(), results.cend(), [](const bool &b) { return b; })) {
				CmdFuncs::UsagePrint(Usage::text());
			}
		}
	};
}
