#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image_write.h>

namespace gte {

	Image::Image(uint32_t width, uint32_t height, int32_t bpp) noexcept
		: mWidth(width), mHeight(height), mbpp(bpp)
	{
		mBuffer = (uint8_t*) ::operator new(Size());
	}

	Image::~Image(void) noexcept
	{
		if (mBuffer) ::operator delete(mBuffer, Size());
		mBuffer = nullptr;
	}

	Image::Image(const Image& other) noexcept
		: mWidth(other.mWidth), mHeight(other.mHeight), mbpp(other.mbpp)
	{
		mBuffer = (uint8_t*) ::operator new(Size());
		memcpy(mBuffer, other.mBuffer, Size());
	}

	Image::Image(Image&& other) noexcept
		: mWidth(other.mWidth), mHeight(other.mHeight), mbpp(other.mbpp)
	{
		mBuffer = other.mBuffer;

		other.mBuffer = nullptr;
		other.mWidth = 0;
		other.mHeight = 0;
		other.mbpp = 0;
	}

	Image& Image::operator=(const Image& rhs) noexcept
	{
		if (this != &rhs)
		{
			if (mBuffer) ::operator delete(mBuffer, Size());
			mWidth = rhs.mWidth;
			mHeight = rhs.mHeight;
			mbpp = rhs.mbpp;

			mBuffer = (uint8_t*) ::operator new(Size());
			memcpy(mBuffer, rhs.mBuffer, Size());
		}
		return *this;
	}

	Image& Image::operator=(Image&& rhs) noexcept
	{
		if (this != &rhs)
		{
			if (mBuffer) ::operator delete(mBuffer, Size());
			mWidth = rhs.mWidth;
			mHeight = rhs.mHeight;
			mbpp = rhs.mbpp;

			mBuffer = rhs.mBuffer;
			rhs.mBuffer = nullptr;
			rhs.mWidth = 0;
			rhs.mHeight = 0;
			rhs.mbpp = 0;
		}
		return *this;
	}

	void Image::Load(const char* filepath)
	{
		int32_t width, height, channels;
		if (mBuffer) ::operator delete(mBuffer, Size());

		//stbi_set_flip_vertically_on_load_thread(1);
		uint8_t* tbuffer = stbi_load(filepath, &width, &height, &channels, 0);

		if (!tbuffer)
		{
			//GTE_ERROR_LOG("Failed to Load Image: ", filepath);
			mBuffer = nullptr;
			return;
		}

		mWidth = width;
		mHeight = height;
		mbpp = channels;

		mBuffer = (uint8_t*) ::operator new(Size());
		memcpy(mBuffer, tbuffer, Size());
		int result = memcmp(mBuffer, tbuffer, Size());
		stbi_image_free(tbuffer);
		GenerateTexture();
	}

	static uint32_t sIndex = 0;
	void Image::Load(const uint8_t* buffer)
	{
		if (mBuffer) ::operator delete(mBuffer, Size());
		mWidth = *(uint32_t*)buffer;
		mHeight = *(uint32_t*)(buffer + 4);
		mbpp = *(int32_t*)(buffer + 8);

		mBuffer = (uint8_t*) ::operator new(Size());
		memcpy(mBuffer, buffer + 12, Size());
		GenerateTexture();
	}

	void Image::Load(const uint8_t* buffer, int32_t length)
	{
		if (mBuffer) ::operator delete(mBuffer, Size());

		int32_t width, height, channels;
		uint64_t size = 0;

		uint8_t* data = (uint8_t*) stbi_load_from_memory((const stbi_uc*)buffer, length, &width, &height, &channels, 4);
		mWidth = width;
		mHeight = height;
		mbpp = channels;
		
		mBuffer = (uint8_t*) ::operator new(Size());
		memcpy(mBuffer, data, Size());
		stbi_image_free(data);
		GenerateTexture();
	}

	void Image::GenerateTexture(void)
	{
		if (mHandle)
		{
			glDeleteTextures(1, &mHandle);
			mHandle = 0;
		}

		glGenTextures(1, &mHandle);
		glBindTexture(GL_TEXTURE_2D, mHandle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mBuffer);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	}


	Image::Image(const char* filepath) { Load(filepath); }

	[[nodiscard]] size_t Image::Size(void) const noexcept { return (size_t)mWidth * mHeight * mbpp; }
	[[nodiscard]] uint32_t Image::GetWidth(void) const noexcept { return mWidth; }
	[[nodiscard]] uint32_t Image::GetHeight(void) const noexcept { return mHeight; }
	[[nodiscard]] int32_t Image::GetBytePerPixel(void) const noexcept { return mbpp; }

	[[nodiscard]] void* Image::Data(void) noexcept { return mBuffer; }
	[[nodiscard]] const void* Image::Data(void) const noexcept { return mBuffer; }

}