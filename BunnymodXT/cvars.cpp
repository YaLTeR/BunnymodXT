#include "stdafx.hpp"
#include "cvars.hpp"

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
