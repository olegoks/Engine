#include "__memory_managers_/MemoryManagerGPU.h"

MemoryManagerGPU::MemoryManagerGPU(const unsigned int display_width, const unsigned int display_height):
	display_buffer_width_(display_width),
	display_buffer_height_(display_height),
	number_of_pixels_(display_height * display_width),
	device_display_buffer_(nullptr),
	display_buffer_size_(display_height * display_width * sizeof(RgbPixel))
{

	AllocateMemoryForBuffer();

}
