#pragma once

namespace Interprocess
{
	void Initialize();
	void Shutdown();
	void Write(const std::vector<unsigned char>& data);
}
