#pragma once
#ifndef _ENGINEV1DOT0_H_
#define _ENGINE1DOT0_H_

#include "EngineStructures.h"
#include "__window_/WindowStructures.h"
#include <Windows.h>
#include <chrono>
#include <iostream>
#include "__window_/Window.h"
#include "__graphic_engine_/GraphicEngine.h"
#include "__database_/DataBase.h"
#include "__logic_engine_/LogicEngine.h"
#include "__file_formats_/Obj.h"
#include "wchar_tFunctions.h"
#include <thread>
#include "initCUDADevice.cuh"
#include "__console_/Console.h"
#include "__memory_managers_/MemoryManagerCPU.h"
#include "__memory_managers_/MemoryManagerGPU.h"
#include <algorithm>

#define SUCCESS_APP_EXIT 0;

class Engine {
private:

	Console* console_;
	std::thread main_loop_thread_;
	bool main_loop_is_running_;
	DisplayFunction display_function_;
	//SpecialFunction special_function_;
	bool console_is_created_;
	MemoryManagerGPU* memory_manager_gpu_;
	MemoryManagerCPU* memory_manager_cpu_;
	unsigned int fps_ = 60;
	DataBase* data_base_;
	wnd::Window* window_;
	LogicEngine* logic_engine_;
	GraphicEngine* graphic_engine_;
	Camera* camera_;
	HWND main_window_handle_;
	unsigned int window_width_;
	unsigned int window_height_;
	unsigned int display_buffer_size_;
	RgbPixel* device_display_buffer_;
	RgbPixel* host_display_buffer_;
	void  ProcessKeyboardAndMouse();
	void UpdateWorldLogic();
	inline void RenderFrame() { graphic_engine_->CreateMeshFrame(); }
	void DisplayFrame();
	void PauseLoop()noexcept;
	void StartLoop();
	inline void SwapBuffers() { cudaMemcpy((void**)host_display_buffer_, device_display_buffer_, display_buffer_size_, cudaMemcpyDeviceToHost); }
	inline void ShowFrame() { window_->DisplayFrame(); }

public:

	class ConsoleCreationException{};
	class DataBaseFileReadingException{};
	explicit Engine() noexcept;
	void TranslateModel(const unsigned int model_id, const Vertex3D& translate_vertex)const;
	const Vertex3D* GetCameraPosition(const unsigned int model_id) const noexcept;
	inline void SetFps(const unsigned int fps) noexcept { fps_ = fps; };
	bool CreateConsole(const char* console_name);
	bool InitCuda(ChoiceOfCUDADevice cuda_device);
	bool InitCuda(ChoiceOfCUDADevice cuda_device, const char* const device_name);
	void Load3DModel(const char* file_name, const char* model_name);
	void InitDisplayMode(EngineMode engine_mode, DoubleBuffered double_buffered);
	bool InitWindowSize(SizeMode size_mode, const unsigned int length, const unsigned int width);
	void InitWindow(HINSTANCE windowHandle);
	void InitSpecialFunction(SpecialFunction func);
	void InitLogicFunction(LogicFunction func);
	void ShowWindow(const char* windowHandle);
	void StartMainLoop();
	void InitDisplayFunction(DisplayFunction display);
	void RotateModel(unsigned int model_id, const float alpha_degree, const Vector3D &around_vector,const Vertex3D &around_point);
	void ScaleModel(const unsigned int model_id, const float coefficient);
	void TranslateCamera(const unsigned int camera_id, const Vertex3D &delta_vertex);
	void RotateCamera(const unsigned int camera_id, const float alpha_degree, const Vector3D& rotate_vector, const Vertex3D& rotate_vertex);
	void ClearMemory();

};

#endif