#ifndef PLATFORM_WINDOWS
#pragma once
#include "uuid.h"
#include <array>

namespace gte::internal::utils {
	
	inline constexpr uuid_t nullGUID = { 0 };
	inline constexpr char nullString[] = "00000000-0000-0000-0000-000000000000";
	inline constexpr char hexDigits[] = "0123456789ABCDEF";
	
	inline constexpr byte hexToByte(char c) noexcept
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		ASSERT(false, "Character: {} is not a valid character for a hex representation.", c);
		return 0;
	}

	template<typename T, u64 N>
	inline constexpr T hexToUInt(const char(&hex)[N])
	{
		T integer = 0;
		for (u64 i = 0; i < N - 1; i++)
			integer = (integer << 4) | hexToByte(hex[i]);
		return integer;
	}

	template<u64 N>
	inline constexpr std::array<char, N> u32ToHex(u32 value) noexcept
	{
		std::array<char, N> result = {};
		for (u64 i = 0; i < N; i++)
		{
			result[N - 1 - i] = hexDigits[value & 0xF];
			value >>= 4;
		}
		return result;
	}

	inline constexpr std::array<char, 2> byteToHex(byte pByte) noexcept { return { hexDigits[pByte >> 4], hexDigits[pByte & 0xF] }; }
	template<typename T>
	inline constexpr u8 byteFromUint(T value, u64 index) noexcept { return static_cast<u8>((value >> (index * 8)) & 0xFF); }
	
	struct test {
		uuid_t Data;
	};

	inline test hexToGUID(std::string_view str)
	{
		ASSERT(
			str.size() == 36 &&
			str[8] == '-' &&
			str[13] == '-' &&
			str[18] == '-' &&
			str[23] == '-',
			"String: {} is not following the format of a uuid.",
			str
		);

		u32 firstPart =	hexToUInt<u32>({str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], '\0'});
		u16 secondPart = hexToUInt<u16>({str[9], str[10], str[11], str[12], '\0'});
		u16 thirdPart = hexToUInt<u16>({str[14], str[15], str[16], str[17], '\0'});
		
		return {
			byteFromUint(firstPart, 0), byteFromUint(firstPart, 1), byteFromUint(firstPart, 2), byteFromUint(firstPart, 3), 
			byteFromUint(secondPart, 0), byteFromUint(secondPart, 1),
			byteFromUint(thirdPart, 0), byteFromUint(thirdPart, 1),
			hexToUInt<u8>({str[19], str[20], '\0'}),
			hexToUInt<u8>({str[21], str[22], '\0'}),
			hexToUInt<u8>({str[24], str[25], '\0'}),
			hexToUInt<u8>({str[26], str[27], '\0'}),
			hexToUInt<u8>({str[28], str[29], '\0'}),
			hexToUInt<u8>({str[30], str[31], '\0'}),
			hexToUInt<u8>({str[32], str[33], '\0'}),
			hexToUInt<u8>({str[34], str[35], '\0'})
		};
	}

	inline constexpr bool areSame(const uuid_t& lhs, const uuid_t& rhs) noexcept
	{
		return std::equal(
				std::begin(lhs),
				std::end(lhs),
				std::begin(rhs),
				std::end(rhs)
			);
	}
}

namespace gte {

	[[nodiscard]] inline std::string uuid::str(void) const noexcept
	{
		using namespace internal::utils;

		if (areSame(mUUID, nullGUID))
			return std::string(nullString);

		char cstr[37] = { 0 };
		u64 i = 0;

		for (u64 j = 0; j < 4; j++)
			for (let c : byteToHex(mUUID[j]))
				cstr[i++] = c;
		cstr[i++] = '-';

		for (u64 j = 4; j < 6; j++)
			for (let c : byteToHex(mUUID[j]))
				cstr[i++] = c;
		cstr[i++] = '-';

		for (u64 j = 6; j < 8; j++)
			for (let c : byteToHex(mUUID[j]))
				cstr[i++] = c;
		cstr[i++] = '-';

		for (u64 j = 8; j < 10; j++)
			for (let c : byteToHex(mUUID[j]))
				cstr[i++] = c;
		cstr[i++] = '-';

		for (u64 j = 10; j < 16; j++)
			for (let c : byteToHex(mUUID[j]))
				cstr[i++] = c;

		return std::string(cstr);
	}

	inline uuid::uuid(const std::string& str) noexcept
	{
		let aux = internal::utils::hexToGUID(str);
		let id = aux.Data;
		for (int i = 0; i < 16; i++)
			mUUID[i] = id[i];
	}

	inline uuid::uuid(std::string_view str) noexcept
	{
		let aux = internal::utils::hexToGUID(str);
		let id = aux.Data;
		for (int i = 0; i < 16; i++)
			mUUID[i] = id[i];
	}

	[[nodiscard]] inline uuid& uuid::operator=(std::string_view str) noexcept
	{
		let aux = internal::utils::hexToGUID(str);
		let id = aux.Data;
		for (int i = 0; i < 16; i++)
			mUUID[i] = id[i];
		return *this;
	}

	[[nodiscard]] inline uuid& uuid::operator=(const std::string& str) noexcept
	{
		let aux = internal::utils::hexToGUID(str);
		let id = aux.Data;
		for (int i = 0; i < 16; i++)
			mUUID[i] = id[i];
		return *this;
	}

	[[nodiscard]] inline uuid& uuid::operator=(const char* str) noexcept
	{
		let aux = internal::utils::hexToGUID(str);
		let id = aux.Data;
		for (int i = 0; i < 16; i++)
			mUUID[i] = id[i];
		return *this;
	}

	[[nodiscard]] inline bool uuid::operator==(const uuid& rhs) const noexcept { return internal::utils::areSame(mUUID, rhs.mUUID); }
	[[nodiscard]] inline bool uuid::operator==(const std::string& rhs) const noexcept { return *this == uuid{ rhs }; }
	[[nodiscard]] inline bool uuid::operator==(const std::string_view rhs) const noexcept { return *this == uuid{ rhs }; }
	[[nodiscard]] inline bool uuid::operator==(const char* rhs) const noexcept { return *this == uuid{ rhs }; }

	[[nodiscard]] inline bool uuid::operator!=(const uuid& rhs) const noexcept { return !internal::utils::areSame(mUUID, rhs.mUUID); }
	[[nodiscard]] inline bool uuid::operator!=(const std::string& rhs) const noexcept { return *this != uuid{ rhs }; }
	[[nodiscard]] inline bool uuid::operator!=(const std::string_view& rhs) const noexcept { return *this != uuid{ rhs }; }
	[[nodiscard]] inline bool uuid::operator!=(const char* rhs) const noexcept { return *this != uuid{ rhs }; }

	[[nodiscard]] inline constexpr bool uuid::IsValid(void) const noexcept
	{
		using namespace internal::utils;
		return !areSame(mUUID, nullGUID);
	}
}
#endif