#include "stdafx.hpp"

#include "cvars.hpp"

#define FIND_WRAPPER(name) CVarWrapper name;
#define DEFINE_WRAPPER(name, string) CVarWrapper name(#name, string);
#define ADD_TO_FIND_LIST(name) &name,
#define ADD_TO_DEFINE_LIST(name, string) &name,

namespace CVars
{
	FIND_CVARS(FIND_WRAPPER)
	DEFINE_CVARS(DEFINE_WRAPPER)
	const std::vector<CVarWrapper*> allCVars =
	{
		FIND_CVARS(ADD_TO_FIND_LIST)
		DEFINE_CVARS(ADD_TO_DEFINE_LIST)
	};
}
