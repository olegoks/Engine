#include "MemoryManagerCPU.h"
#include "cuda_runtime_api.h"
void MemoryManagerCPU::AllocateMemoryForDisplayBuffer()
{

	//this->display_buffer_ = (RgbPixel*) new RgbPixel[number_of_pixels];
	cudaMallocHost((void**)&display_buffer_, display_buffer_size_);
	ZeroMemory(display_buffer_, display_buffer_size_);

}

MemoryManagerCPU::MemoryManagerCPU(const unsigned int display_buffer_width, const unsigned int display_buffer_height) noexcept :
	display_buffer_height_(display_buffer_height),
	display_buffer_width_(display_buffer_width),
	display_buffer_(nullptr),
	number_of_pixels( display_buffer_width * display_buffer_height)
{

	this->display_buffer_size_ = number_of_pixels * sizeof(RgbPixel);

	AllocateMemoryForDisplayBuffer();

}
