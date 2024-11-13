#ifndef PLATFORM_WINDOWS
#include <uuid.h>

namespace gte {

	[[nodiscard]] uuid uuid::Create(void) noexcept
	{
		uuid newone;
		uuid_generate(newone.mUUID);
		return newone;
	}

}

[[nodiscard]] std::ostream& operator<<(std::ostream& lhs, const gte::uuid& rhs)
{
	lhs << rhs.str();
	return lhs;
}

namespace std {

	[[nodiscard]] size_t hash<gte::uuid>::operator()(const gte::uuid& id) const { return std::hash<std::string>()(id.str()); }

}
#endif