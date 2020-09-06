#pragma once
#ifndef _MEMORYMANAGERGPU_H_
#define _MEMORYMANAGERGPU_H_
#include "Structures.h"

class MemoryManagerGPU {
private:
	
	const unsigned int display_buffer_height_;
	const unsigned int display_buffer_width_;
	const unsigned int number_of_pixels_;
	const unsigned int display_buffer_size_;
	RgbPixel*  device_display_buffer_;
	GpuDataPointers device_data_info_;
	DataInfo host_data_info_;

public:

	MemoryManagerGPU(const unsigned int display_width, const unsigned int display_height);
	void SetHostDataInfo(const DataInfo host_data_info) { this->host_data_info_ = host_data_info; };
	inline GpuDataPointers GetDeviceData()const noexcept { return this->device_data_info_; };
	void AllocateMemoryForPrimitives();

	RgbPixel* GetDisplayBufferPointer()const noexcept { return this->device_display_buffer_; };
	
	void AllocateMemoryForBuffer();
	RgbPixel* GetDeviceBuffer()const noexcept { return device_display_buffer_; }

};
#endif