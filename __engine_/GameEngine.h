#pragma once
#ifndef _GAMEENGINE_H_
#define _GAMEENGINE_H_

#include "EngineV1dot0.h"

	bool engineCreateConsole(const char* console_caption);
	bool engineInitCUDA(ChoiceOfCUDADevice cuda_device, const char* device_name);
	void engineInitDisplayMode(EngineMode engine_mode, DoubleBuffered double_buffered);
	void engineInitWindow(HINSTANCE hInstance);
	void engineInitWindowSize(SizeMode size_mode, const unsigned int width,  const unsigned int height);
	void engineLoad3DModel(const char* file_name, const char* model_name);
	void engineInitDisplayFunction(DisplayFunction display_function_pointer);
	void engineStartMainLoop();
	void engineShowWindow(const char* window_caption);
	void engineTranslateModel(const unsigned int model_id, const Vertex3D& translate_vertex);
	void engineRotateModel(unsigned int model_id, const float angle_degree, const Vector3D& around_vector, const  Vertex3D& around_vertex);
	void engineScaleModel(const unsigned int model_id, const float coefficient);
	void engineTranslateCamera(const unsigned int camera_id, const Vertex3D &delta_vertex);
	void engineRotateCamera(const unsigned int camera_id, const float alpha_degree, const Vector3D& rotate_vector, const Vertex3D &rotate_vertex);
	void engineInitLogicFunction(LogicFunction func);
	void engineInitSpecialFunction(SpecialFunction func);
	const Vertex3D* engineGetCameraPosition(const unsigned int camera_id);
	Vector3D engineCalculateVector(const Vertex3D* const begin, const Vertex3D* const end);
#endif