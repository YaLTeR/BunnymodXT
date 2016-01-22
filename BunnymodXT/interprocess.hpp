#pragma once

namespace Interprocess
{
	void Initialize();
	void Shutdown();
	void Write(const std::vector<char>& data);
}
