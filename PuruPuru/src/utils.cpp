#include "utils.h"

uint64_t distance(const std::string& lhs, const std::string& rhs)
{
	uint64_t distance = 0;// std::abs((const long long)(lhs.size() - rhs.size()));
	
	const size_t length = std::min(lhs.size(), rhs.size());
	for (size_t i = 0; i < length; i++)
		if (lhs[i] != rhs[i])
			distance++;

	return distance != length ? distance : std::numeric_limits<uint64_t>::max();
}