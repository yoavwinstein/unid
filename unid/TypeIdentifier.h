#pragma once

#include <cstdint>

namespace unid::details {

template <typename T>
constexpr uint64_t type_id() {
	constexpr const char sig[] = __FUNCSIG__;

	uint64_t sum = 0;
	for (size_t i = 0; i < sizeof(sig); ++i) {
		sum += sig[i];
	}
	return sum;
}

}
