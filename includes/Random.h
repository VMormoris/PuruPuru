#pragma once

/**
* @brief Helper Class for random number generation
*/
class Random {
public:

	/**
	* @brief Initializing the Random generator
	*/
	static void Init(void);

	/**
	* @brief Generates a random float in range [0.0f, 1.0f]
	* @details Numbers generate by this function follow a uniform distrubution
	*/
	static float Float(void);

};