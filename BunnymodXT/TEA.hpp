#pragma once

namespace TEA
{
	void encrypt(uint32_t data[2], const uint32_t key[4]);
	void decrypt(uint32_t data[2], const uint32_t key[4]);
}
