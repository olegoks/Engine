#pragma once
#ifndef _GRAPHICENGINE_H_
#define _GRAPHICENGINE_H_

#include <chrono>
#include <list>
#include <cstdlib>
#include <thread>
#include <windows.h>
#include "__camera_\Camera.h"
#include "__logic_engine_\LogicEngine.h"
#include "EngineStructures.h"
#include "__window_\WindowStructures.h"
#include "__graphic_engine_\GraphicEngineCUDA.cuh"
#include "cuda_runtime_api.h"

struct Vertex2D {

	float x;
	float y;
};

//test
struct z_mutex {
	float z;
	bool mutex;
};

struct CameraInfo {
	Vertex3D camera_pos;
	Vector3D vector_x;
	Vector3D vector_y;
	Vector3D vector_z;
	float dis_proj_plane;
};
//
class GraphicEngine {

private:

	bool engine_is_running_;
	Camera* camera_;
	EngineMode mode_;
	DataInfo data_info_;
	GpuDataPointers device_data_;
	HWND window_handle_;
	unsigned int threads_per_triangle_ = 3;

	unsigned int display_height_ = 1080;
	unsigned int display_width_ = 1920;
	//Test
	Vertex2D* device_vertexs_2d_;
	Vertex2D* host_vertexs_2d_;
	Polygon3D* device_polygons_;
	
	RgbPixel* device_display_buffer_;
	RgbPixel* host_display_buffer_;

	unsigned int number_of_pixels_;
	unsigned int size_of_display_buffer_;
	void SetDisplayBufferColor(const RgbColor& rgb_color);
	void AllocateVertex2D();

	//test
	bool* models_visibility;
	bool* polygons_visibility;
	void TestFunction();
	CameraInfo GetCameraInfo();

	z_mutex* z_mutex_;
	//RgbPixel* z_buffer_;

	void AllocateZBuffer() {

		cudaMalloc((void**)&z_mutex_, display_height_ * display_width_ * sizeof(z_mutex));
		//cudaMalloc((void**)&z_buffer_, display_height_ * display_width_ * sizeof(RgbPixel));
	}
public:

	void CreateMeshFrame();
	void CreateFlatFrame();

	inline void SetDeviceDisplayBuffer(RgbPixel* display_buffer) noexcept {
		device_display_buffer_ = display_buffer;
	};

	inline void SetHostDisplayBuffer(RgbPixel* host_buffer) noexcept { host_display_buffer_ = host_buffer; };
	inline RgbPixel* GetDeviceDisplayBuffer()const noexcept { return  device_display_buffer_; };
	void SetDeviceDataPointers(GpuDataPointers device_data_info);
	inline bool StartRendering()noexcept { engine_is_running_ = true; };
	inline void StopRendering()noexcept { engine_is_running_ = false; };
	inline void SetCamera(Camera* camera) { camera_ = camera; };
	inline void SetWindowHandle(HWND handle) noexcept { window_handle_ = handle; };
	inline void SetEngineMode(EngineMode engineMode) noexcept { mode_ = engineMode; };

	inline DataInfo GetDataInfo()const noexcept { 
		return data_info_; 
	};
	
	inline void LoadDataInfo(DataInfo dataInfo) { 

		data_info_ = dataInfo;
		AllocateVertex2D();
		AllocateZBuffer();

	};

};

#endif