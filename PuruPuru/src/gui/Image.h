#pragma once
#include <stdint.h>

namespace gte {

	/**
	* @brief Class for an Image representation
	* @warning Saving an image will always result to a .png format
	*/
	class Image {
	public:

		//Constructor(s) & Destructor
		Image(void) = default;
		Image(uint32_t width, uint32_t height, int32_t bpp) noexcept;
		Image(const char* filepath);
		Image(const Image& other) noexcept;
		Image(Image&& other) noexcept;
		~Image(void) noexcept;

		/**
		* @brief Size of the image in bytes
		* @returns bytes for the image Data
		*/
		[[nodiscard]] size_t Size(void) const noexcept;

		/**
		* @brief Getter for Image's width in pixels
		* @returns Number of pixels
		*/
		[[nodiscard]] uint32_t GetWidth(void) const noexcept;

		/**
		* @brief Getter for Image's height in pixels
		* @returns Number of pixels
		*/
		[[nodiscard]] uint32_t GetHeight(void) const noexcept;

		/**
		* @brief Getter for Image's pixel buffer
		*/
		[[nodiscard]] void* Data(void) noexcept;

		/**
		* @brief Getter for Image's pixel buffer
		*/
		[[nodiscard]] const void* Data(void) const noexcept;

		/**
		* @brief Getter for a specific pixel of the Image
		* @details Depending on Image the size of a pixel might varied.
		* @tparam T Type of the underlying pixel
		* @param x X-axis coordinate of the pixel
		* @param y Y-axis coordinate of the pixel
		* @returns
		*/
		template<typename T>
		[[nodiscard]] T& GetPixel(size_t x, size_t y)
		{
			ASSERT(x < _width&& y < _height, "Pixel is out of bounds!");
			T* tbuffer = (T*)buffer + y * _width + x;
			return *tbuffer;
		}

		/**
		* @brief Loads an Image from Disc
		* @param filepath File from where the image should be loaded
		*/
		void Load(const char* filepath);
		void Load(const uint8_t* buffer);
		void Load(const uint8_t* buffer, int32_t length);

		[[nodiscard]] int32_t GetBytePerPixel(void) const noexcept;

		//Assignement operators
		Image& operator=(const Image& rhs) noexcept;
		Image& operator=(Image&& rhs) noexcept;

		//Casting operators
		void* GetHandle(void) noexcept { return reinterpret_cast<void*>((uint64_t)mHandle); }

	private:

		void GenerateTexture(void);

	private:
		uint32_t mHandle = 0;
		/**
		* @brief Image's width
		*/
		uint32_t mWidth = 0;

		/**
		* @brief Image's height
		*/
		uint32_t mHeight = 0;

		/**
		* @brief Bytes for each pixel
		*/
		int32_t mbpp = 0;

		/**
		* @brief Actual Image buffer
		*/
		uint8_t* mBuffer = nullptr;
	};



}