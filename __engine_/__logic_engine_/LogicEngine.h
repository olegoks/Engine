#pragma once
#ifndef _LOGICENGINE_H_
#define _LOGICENGINE_H_

#include <windows.h>
#include "__camera_/Camera.h"
#include "__database_/DataBase.h"
#include <vector>
#include "cuda_runtime_api.h"

enum TransformationType {

	Rotate = 1,
	Translate = 2,
	Scale = 3

};

class LogicEngine {

private:

	bool engine_is_running_;
	std::vector<const char*> models_names_;

	Camera* camera_;
	DataInfo data_info_;
	GpuDataPointers device_data_;
	std::vector<Keystroke> keystroke_buffer_;
	SpecialFunction special_function_;
	LogicFunction logic_function;
	unsigned int previous_keyboard_process_;
	unsigned int time_per_update_logic_;

public:

	explicit LogicEngine()noexcept;
	std::vector<Keystroke>* GetKeystrokeBuffer() noexcept { return &keystroke_buffer_; };
	void UpdateWorldLogic();
	void SetTimePerUpdateLogic(const unsigned int time_per_update_logic)noexcept { time_per_update_logic_ = time_per_update_logic; };
	inline void SetPreviousKeyboardProcess(unsigned int time_point) noexcept { previous_keyboard_process_ = time_point; };
	inline void InitSpecialFunction(SpecialFunction func) noexcept { special_function_ = func; };
	inline void InitLogicFunction(LogicFunction func) noexcept { logic_function = func; }
	inline void StartEngine()noexcept { this->engine_is_running_ = true; };
	inline void PauseEngine()noexcept { this->engine_is_running_ = false; };
	inline void SetCamera(Camera* camera_) { this->camera_ = camera_; };
	void LoadDataInfo(DataInfo data_info) { this->data_info_ = data_info; }
	void AddModelName(const char* model_name);
	void LoadDeviceData(const GpuDataPointers& device_data) { this->device_data_ = device_data; };
	void RotateModel(unsigned int model_id, const float alpha_degree, const Vector3D& around_vector, const Vertex3D& around_point);
	void ScaleModel(const unsigned int model_id, const float coefficient);
	void TranslateModel(const unsigned int model_id, const Vertex3D& translate_vertex);
}
;

#endif
