#pragma once

#include "StateMachine.h"

template<>
inline void StateMachine::SetValue<bool>(const std::string& key, bool val) { mBooleans[key] = val; }

template<>
inline void StateMachine::SetValue<int>(const std::string& key, int32_t val) { mIntegers[key] = val; }

//template<typename T>
//inline T StateMachine::GetValue(const std::string key)
//{
//	if constexpr (std::is_same_v<T, bool>)
//		return mBooleans.find(key) != mBooleans.end() ? mBooleans.at(key) : 0;
//	else
//		return mIntegers.find(key) != mIntegers.end() ? mIntegers.at(key) : 0;
//}