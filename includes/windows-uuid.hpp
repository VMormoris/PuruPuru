#ifdef PLATFORM_WINDOWS
#pragma once
#include "uuid.h"

#include <array>


namespace gte::internal::utils {
	static constexpr uuid_t nullGUID = { 0 };
	static constexpr char nullString[] = "00000000-0000-0000-0000-000000000000";
	static constexpr char hexDigits[] = "0123456789ABCDEF";
	
	inline constexpr byte hexToByte(char c) noexcept
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		ASSERT(false, "Character: {} is not a valid character for a hex representation.", c);
		return 0;
	}

	template<typename T, size_t N>
	inline constexpr T hexToUInt(const char(&hex)[N])
	{
		T integer = 0;
		for (size_t i = 0; i < N - 1; i++)
			integer = (integer << 4) | hexToByte(hex[i]);
		return integer;
	}

	template<size_t N>
	inline constexpr std::array<char, N> u32ToHex(u32 value) noexcept
	{
		std::array<char, N> result = {};
		for (size_t i = 0; i < N; i++)
		{
			result[N - 1 - i] = hexDigits[value & 0xF];
			value >>= 4;
		}
		return result;
	}

	inline constexpr std::array<char, 2> byteToHex(byte pByte) noexcept { return { hexDigits[pByte >> 4], hexDigits[pByte & 0xF] }; }

	inline uuid_t hexToGUID(std::string_view str)
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

		return {
			hexToUInt<u32>({str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], '\0'}),
			hexToUInt<u16>({str[9], str[10], str[11], str[12], '\0'}),
			hexToUInt<u16>({str[14], str[15], str[16], str[17], '\0'}),
			{
				hexToUInt<u8>({str[19], str[20], '\0'}),
				hexToUInt<u8>({str[21], str[22], '\0'}),
				hexToUInt<u8>({str[24], str[25], '\0'}),
				hexToUInt<u8>({str[26], str[27], '\0'}),
				hexToUInt<u8>({str[28], str[29], '\0'}),
				hexToUInt<u8>({str[30], str[31], '\0'}),
				hexToUInt<u8>({str[32], str[33], '\0'}),
				hexToUInt<u8>({str[34], str[35], '\0'})
			}
		};
	}

	inline constexpr bool areSame(const uuid_t& lhs, const uuid_t& rhs) noexcept
	{
		return lhs.Data1 == rhs.Data1 &&
			lhs.Data2 == rhs.Data2 &&
			lhs.Data3 == rhs.Data3 &&
			std::equal(
				std::begin(lhs.Data4),
				std::end(lhs.Data4),
				std::begin(rhs.Data4),
				std::end(rhs.Data4)
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

		size_t i = 0;
		for (let c : u32ToHex<8>(mUUID.Data1)) cstr[i++] = c; cstr[i++] = '-';
		for (let c : u32ToHex<4>(mUUID.Data2)) cstr[i++] = c; cstr[i++] = '-';
		for (let c : u32ToHex<4>(mUUID.Data3)) cstr[i++] = c; cstr[i++] = '-';

		let firstSlice = byteToHex(mUUID.Data4[0]);
		cstr[i++] = firstSlice[0];
		cstr[i++] = firstSlice[1];

		let secondSlice = byteToHex(mUUID.Data4[1]);
		cstr[i++] = secondSlice[0];
		cstr[i++] = secondSlice[1];
		cstr[i++] = '-';

		for (size_t j = 2; j < 8; j++)
		{
			let slice = byteToHex(mUUID.Data4[j]);
			cstr[i++] = slice[0];
			cstr[i++] = slice[1];
		}

		return std::string(cstr);
	}

	inline uuid::uuid(const std::string& str) noexcept { mUUID = internal::utils::hexToGUID(str); }
	inline uuid::uuid(std::string_view str) noexcept { mUUID = internal::utils::hexToGUID(str); }

	[[nodiscard]] inline uuid& uuid::operator=(std::string_view str) noexcept
	{
		mUUID = internal::utils::hexToGUID(str);
		return *this;
	}

	[[nodiscard]] inline uuid& uuid::operator=(const std::string& str) noexcept
	{
		mUUID = internal::utils::hexToGUID(str);
		return *this;
	}

	[[nodiscard]] inline uuid& uuid::operator=(const char* str) noexcept
	{
		mUUID = internal::utils::hexToGUID(str);
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
		return !internal::utils::areSame(mUUID, nullGUID);
	}
}

#endif