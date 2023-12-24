#pragma once

#include <unordered_map>
#include <string>

class StateMachine {
public:

	template<typename T>
	void SetValue(const std::string& key, T val) { static_assert(false, "Not implemented yet!"); }

	void SetFork(const gte::uuid& key, bool val) { mForks.insert({ key, val }); }


	bool Exists(const std::string& key) const noexcept { return mBooleans.find(key) != mBooleans.end(); }
	int32_t GetIntValue(const std::string& key) { return mIntegers.find(key) != mIntegers.end() ? mIntegers.at(key) : 0; }
	bool GetBoolValue(const std::string& key) { return Exists(key) ? mBooleans.at(key) : 0; }

	std::unordered_map<std::string, bool>& Booleans(void) noexcept { return mBooleans; }
	std::unordered_map<gte::uuid, bool>& Forks(void) noexcept { return mForks; }
	const std::unordered_map<std::string, bool>& Booleans(void) const noexcept { return mBooleans; }
	const std::unordered_map<gte::uuid, bool>& Forks(void) const noexcept { return mForks; }


	std::unordered_map<std::string, int32_t>& Integers(void) noexcept { return mIntegers; }
	const std::unordered_map<std::string, int32_t>& Integers(void) const noexcept { return mIntegers; }

	void Clear(void) { mBooleans.clear(); mIntegers.clear(); }

private:

	std::unordered_map<std::string, bool> mBooleans;
	std::unordered_map<gte::uuid, bool> mForks;
	std::unordered_map<std::string, int32_t> mIntegers;

};

#include "StateMachine.hpp"