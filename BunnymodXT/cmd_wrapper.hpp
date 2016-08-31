#pragma once

#include <algorithm>
#include <type_traits>
#include <utility>

#define USAGE(text) \
	inline static const char *usage() { return text; } \
	using usage_t = CmdWrapper::has_usage_t

#define NO_USAGE() using usage_t = CmdWrapper::has_no_usage_t

namespace CmdWrapper
{
	struct has_usage_t {};
	struct has_no_usage_t {};

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
		template<typename H>
		inline static typename std::enable_if<std::is_same<typename H::usage_t, has_no_usage_t>::value, void>::type PrintUsage()
		{
			// No usage, do nothing.
		}

		template<
			typename H,
			typename = std::enable_if_t<std::is_same<typename H::usage_t, has_usage_t>::value>
		>
		inline static void PrintUsage()
		{
			CmdFuncs::UsagePrint(H::usage());
		}

		template<typename H>
		inline static void CallHandlers(int argc)
		{
			PrintUsage<H>();
		}

		template<typename H, typename Handler, typename... Handlers>
		inline static void CallHandlers(int argc)
		{
			if (!Handler::template call<CmdFuncs, H>(argc))
				CallHandlers<H, Handlers...>(argc);
		}
	};
}
