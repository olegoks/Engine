#pragma once
#ifndef _MEMORYMANAGERCPU_H_
#define _MEMORYMANAGERCPU_H_
#include <windows.h>
#include "Structures.h"

typedef BITMAPINFO DISPLAYBUFFERINFO;

class MemoryManagerCPU {
private:

	const unsigned int display_buffer_height_;
	const unsigned int display_buffer_width_;
	const unsigned int number_of_pixels;
	RgbPixel* display_buffer_;
	unsigned int display_buffer_size_;

	void AllocateMemoryForDisplayBuffer();

public:

	explicit MemoryManagerCPU(const unsigned int display_buffer_height, const unsigned int display_buffer_width)noexcept;
	inline unsigned int GetDisplayBufferSize()noexcept { return this->display_buffer_size_; };
	RgbPixel* GetDisplayBufferPointer()noexcept { return display_buffer_; }
	
	inline RgbPixel* ReturnDisplayBufferPointer()const noexcept { return display_buffer_; };


};
#endif