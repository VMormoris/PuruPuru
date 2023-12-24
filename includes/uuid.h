//Modified from https://github.com/VMormoris/GreenTea/blob/master/GreenTea/src/Engine/Core/uuid.h

#pragma once

#include <string>
#include <ostream>

#include <combaseapi.h>
#pragma comment( lib, "rpcrt4.lib" )

namespace gte {


	/**
	* @brief Universal unique identifier of 128 bits
	* @details We are using the native API calls of each Platform to produce the number. By default a new uuid is "invalid",
	*	use the Create() function in order to create a new unique identifier:
	*
	* @code{.cpp}
	* uuid ID = uuid::Create();
	* @endcode
	*/
	class uuid {
		//TODO(Vasilis): Add assingement operator for std::string as rhs
	public:

		/**
		* @brief Constructs an "invalid" uuid
		* @details An "invalid" uuid is efectively 128 bits of zeros
		*/
		uuid(void) noexcept;

		//Defaults
		~uuid(void) = default;
		uuid(const uuid&) = default;
		uuid(uuid&&) = default;
		[[nodiscard]] uuid& operator=(const uuid&) = default;
		[[nodiscard]] uuid& operator=(uuid&&) = default;

		/**
		* @brief Constructs a unique identifier from the given string
		* @param str The string from which the uuid will be extracted from
		*/
		uuid(const std::string& str) noexcept;

		/**
		* @brief Gets the hexadecimal representation of the uuid as string
		* @return A string with the representation of the uuid
		*/
		[[nodiscard]] std::string str(void) const noexcept;

		/**
		* @brief Creates a new unique identifier
		* @return A uuid
		*/
		[[nodiscard]] static uuid Create(void) noexcept;

		/**
		* @brief Checks whether the identifier is "valid" or not
		* @return True if uuid it's trully a unique identifier, false otherwise
		*/
		[[nodiscard]] operator bool() const noexcept;

		//Comparison operators
		[[nodiscard]] bool operator==(const uuid& rhs) const noexcept;
		[[nodiscard]] bool operator!=(const uuid& lhs) const noexcept;

	private:
		_GUID mUUID;
		friend struct std::hash<uuid>;
	};

}

//We need to be able to print it
std::ostream& operator<<(std::ostream& lhs, const gte::uuid& rhs);

namespace std {
	template<>
	struct hash<gte::uuid> {
		[[nodiscard]] size_t operator()(const gte::uuid& id) const;
	};

}