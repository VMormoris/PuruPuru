#pragma once

#include <string>
#include <ostream>

#if defined(PLATFORM_WINDOWS)
	#include <combaseapi.h>
	#pragma comment( lib, "rpcrt4.lib" )
#else
	#include <uuid/uuid.h>
#endif

#include <cstdint>
// Green-Tea engine definitions **needed for uuid**
// TODO: Define proper functionality 
#define ASSERT(...) 

//Type definitions
typedef std::int8_t int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;

typedef std::uint8_t uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

typedef float float32;
typedef double float64;

typedef unsigned char byte;

//Rust-like types
using i8 = int8;
using i16 = int16;
using i32 = int32;
using i64 = int64;

using u8 = uint8;
using u16 = uint16;
using u32 = uint32;
using u64 = uint64;

using f32 = float32;
using f64 = float64;

#define let const auto

namespace gte {


	/**
	* @brief Universal unique identifier of 128 bits
	* @details We are using the native API calls of each Platform to produce the number. Using the default constructor will produce an "invalid" identifier, to create a new "valid" identifier use the Create() function:
	*
	* @code{.cpp}
	* uuid newID = uuid::Create();
	* @endcode
	*/
	class uuid {
	public:

		/**
		* @brief Constructs an "invalid" uuid
		* @details An "invalid" uuid is effectively 128 bits of zeros
		*/
		constexpr uuid(void) = default;

		//Defaults
		/// @brief Default constructor
		~uuid(void) = default;
		/// @brief Default copy constructor
		constexpr uuid(const uuid&) = default;
		/// @brief Default move constructor
		constexpr uuid(uuid&&) = default;
		/// @brief Default copy assignment operator
		[[nodiscard]] uuid& operator=(const uuid&) = default;
		/// @brief Default move assignment operator
		[[nodiscard]] uuid& operator=(uuid&&) = default;

		/**
		* @brief Constructs a unique identifier from the given string
		* @param str The string which the uuid will be constructed from
		*/
		uuid(const std::string& str) noexcept;

		/**
		* @brief Constructs a unique identifier from the given string view
		* @param str The string view which the uuid will be constructed from
		*/
		uuid(std::string_view str) noexcept;

		/**
		* @brief Constructs a unique identifier from the given string
		* @param str C-style string which the uuid will be constructed from
		*/
		uuid(const char* str) noexcept
			: uuid(std::string_view(str)) {}

		/**
		* @brief Assigns value to the uuid
		* @param str The string which the uuid will be assign from
		*/
		[[nodiscard]] uuid& operator=(const std::string& str) noexcept;

		/**
		* @brief Assigns value to the uuid
		* @param str The string view which the uuid will be assign from
		*/
		[[nodiscard]] uuid& operator=(std::string_view str) noexcept;

		/**
		* @brief Assigns value to the uuid
		* @param str C-style string which the uuid will be assign from
		*/
		[[nodiscard]] uuid& operator=(const char* str) noexcept;

		/**
		* @brief Gets the hexadecimal representation of the uuid as string
		* @return A string with the representation of the uuid
		*/
		[[nodiscard]] std::string str(void) const noexcept;

		/**
		* @brief Creates a new unique identifier
		* @return A new generated uuid from the native API
		*/
		[[nodiscard]] static uuid Create(void) noexcept;

		/**
		* @brief Checks whether the identifier is "valid" or not
		* @return True if uuid it's "valid" a unique identifier, false otherwise
		*/
		[[nodiscard]] constexpr bool IsValid(void) const noexcept;

		/**
		* @brief Checks whether the identifier is "valid" or not
		* @sa IsValid
		*/
        [[nodiscard]] constexpr operator bool(void) const noexcept { return IsValid(); }

		/// @brief Checks whether the two identifiers are equal
		[[nodiscard]] bool operator==(const uuid& rhs) const noexcept;
		/// @brief Checks whether the identifier is the same as the given string representation
		[[nodiscard]] bool operator==(const std::string& rhs) const noexcept;
		/// @brief Checks whether the identifier is the same as the given string(_view) representation
		[[nodiscard]] bool operator==(const std::string_view rhs) const noexcept;
		/// @brief Checks whether the identifier is the same as the given C-style string representation
		[[nodiscard]] bool operator==(const char* rhs) const noexcept;

		/// @brief Checks whether the two identifiers are not equal
		[[nodiscard]] bool operator!=(const uuid& rhs) const noexcept;
		/// @brief Checks whether the identifier is not the same as the given string representation
		[[nodiscard]] bool operator!=(const std::string& rhs) const noexcept;
		/// @brief Checks whether the identifier is not the same as the given string(_view) representation
		[[nodiscard]] bool operator!=(const std::string_view& rhs) const noexcept;
		/// @brief Checks whether the identifier is not the same as the given C-style string representation
		[[nodiscard]] bool operator!=(const char* rhs) const noexcept;

		/// @brief Acquires a null uuid (aka "invalid")
		[[nodiscard]] inline static constexpr uuid null() { return {}; }

	private:

		uuid_t mUUID = { 0 };
		friend struct std::hash<uuid>;
	};
}


// ************** std bindings **************
/// @brief Writes the identifier into a stream (will be using the string representation)
std::ostream& operator<<(std::ostream& lhs, const gte::uuid& rhs);

namespace std {

	template<>
	struct hash<gte::uuid> {
		[[nodiscard]] size_t operator()(const gte::uuid& id) const;
	};

}
// ******************************************

#ifdef PLATFORM_WINDOWS
	#include "windows-uuid.hpp"
#else
	#include "linux-uuid.hpp"
#endif
