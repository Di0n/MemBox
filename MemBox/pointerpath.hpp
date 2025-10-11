#pragma once
#include <vector>

namespace MemBox
{
	struct PointerPath
	{
		PointerPath() = default;
		PointerPath(uintptr_t baseAddress, std::vector<uintptr_t> offsets)
			: baseAddress(baseAddress), offsets(offsets) {
		}
		uintptr_t baseAddress;
		std::vector<uintptr_t> offsets;
	};
}