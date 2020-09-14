#include "EngineV1dot0.h"

Engine::Engine()noexcept
{

}

bool Engine::CreateConsole(const char* console_name) {

	try {

		if (!this->console_is_created_) {

			this->console_ = (Console*) new Console(console_name);
			this->console_is_created_ = true;

		}

		std::cout << "Success console creation." << std::endl;

	}

	catch (Console::ConsoleCreationException) {

		throw ConsoleCreationException();
		return false;

	}


}
void Engine::ProcessKeyboardAndMouse() {

	using namespace std;
	using namespace std::chrono;

	static high_resolution_clock::time_point previous_call_function= high_resolution_clock::now();

	const unsigned int previos_call_milliseconds = duration_cast<milliseconds>(previous_call_function.time_since_epoch()).count();

	std::vector<Keystroke>* keystroke_buffer_window = window_->GetKeystrokesBuffer();
	std::vector<Keystroke>* keystroke_buffer_logic_engine = logic_engine_->GetKeystrokeBuffer();
	keystroke_buffer_logic_engine->insert(keystroke_buffer_logic_engine->end(), keystroke_buffer_window->begin(), keystroke_buffer_window->end());
	keystroke_buffer_window->clear();

	logic_engine_->SetPreviousKeyboardProcess(previos_call_milliseconds);

	previous_call_function = high_resolution_clock::now();

}

void Engine::UpdateWorldLogic() {

	logic_engine_->UpdateWorldLogic();
	
}



void Engine::DisplayFrame() {

	//PostMessage(main_window_handle, WM_SWAP_BUFFERS, NULL, NULL);
	window_->DisplayFrame();

}
void Engine::PauseLoop()noexcept {

	main_loop_is_running_ = false;

}


void Engine::StartLoop() {

	using namespace std::chrono;

	typedef  high_resolution_clock::duration duration;
	typedef  high_resolution_clock::time_point time_point;

	main_loop_is_running_ = true;

	time_point newCycleTimePoint, end_cycle, end_render_and_update;
	duration elapsed, cycle_duration, render_and_update_duration;
	 
	duration DURATION_PER_UPDATE_LOGIC = 10ms;
	logic_engine_->SetTimePerUpdateLogic(duration_cast<milliseconds>(DURATION_PER_UPDATE_LOGIC).count());
	const unsigned int milliseconds_to_frame = 1000 / this->fps_;
	const duration duration_fps_limit = milliseconds(milliseconds_to_frame);

	duration lag = high_resolution_clock::duration::zero();
	time_point previous = high_resolution_clock::now();

	while (main_loop_is_running_) {
		
		newCycleTimePoint = high_resolution_clock::now();

		elapsed = newCycleTimePoint - previous;
		previous = newCycleTimePoint;
		lag += elapsed;

		ProcessKeyboardAndMouse();
		
		while (lag >= DURATION_PER_UPDATE_LOGIC) {

			UpdateWorldLogic();
			lag -= DURATION_PER_UPDATE_LOGIC;

		}	

		RenderFrame();
		SwapBuffers();
		ShowFrame();
		end_render_and_update = high_resolution_clock::now();
		render_and_update_duration = end_render_and_update - newCycleTimePoint;
		
		if(render_and_update_duration < duration_fps_limit)
			std::this_thread::sleep_for(duration_fps_limit - render_and_update_duration);
		
	}

}
void Engine::StartMainLoop() {

	try {

		data_base_->LoadObjFiles();
		const DataInfo host_data_info = data_base_->GetDataInfo();
		logic_engine_->LoadDataInfo(host_data_info);
		memory_manager_gpu_->SetHostDataInfo(host_data_info);
		memory_manager_gpu_->AllocateMemoryForPrimitives();
		graphic_engine_->LoadDataInfo(host_data_info);
		//graphic_engine_->AllocateVertex2D();
		graphic_engine_->SetDeviceDisplayBuffer(memory_manager_gpu_->GetDisplayBufferPointer());
		graphic_engine_->SetHostDisplayBuffer(memory_manager_cpu_->GetDisplayBufferPointer());
		graphic_engine_->SetDeviceDataPointers(memory_manager_gpu_->GetDeviceData());
		logic_engine_->LoadDeviceData(memory_manager_gpu_->GetDeviceData());
		
		main_loop_thread_ = std::thread{&Engine::StartLoop, this };
		this->display_function_();
	}

	catch (DataBase::LoadFilesException load_file_error) {

		if (console_is_created_) {

			std::cout << "File load error: " << std::endl;
			std::cout << "File name: " << load_file_error.file_name << std::endl;
			std::cout << "Erorr code: " << load_file_error.error_code << std::endl;

			/*for (size_t i = 0; i < loadFileError.->size(); i++)
			{

				std::cout << "File name: " << loadFileError.errors->at(i).fileName << std::endl;
				std::cout << "Error identity: " << loadFileError.errors->at(i).errorName << std::endl;
			
			}*/

		}

	}

}

void Engine::Load3DModel(const char* file_name, const char* model_name) {


	this->logic_engine_->AddModelName(model_name);
	this->data_base_->AddFileName(file_name);
	
}

bool Engine::InitCuda(ChoiceOfCUDADevice cudaDevice) {
	
	//Search CUDA devices

	return true;
}

bool Engine::InitCuda(ChoiceOfCUDADevice cuda_device, const char* const device_name) {

	bool device_is_init = initCUDADevice(device_name);

	if (console_is_created_) {

		if (device_is_init) {

			std::cout << "Success device initialization. Device name: " << device_name << std::endl;
			return true;
		}
		else {

			std::cout << "Error device initialization." << std::endl;
		
			return false;
		}

	}

}

void Engine::InitDisplayMode(EngineMode engine_mode, DoubleBuffered double_buffered) {

	data_base_ = (DataBase*) new DataBase;
	logic_engine_ = (LogicEngine*) new LogicEngine;
	graphic_engine_ = (GraphicEngine*) new GraphicEngine;
	camera_ = (Camera*) new Camera;

	logic_engine_->SetCamera(camera_);
	graphic_engine_->SetCamera(camera_);
	graphic_engine_->SetEngineMode(engine_mode);

}

bool Engine::InitWindowSize(SizeMode size_mode, const unsigned int width, const unsigned int height) {

	if (window_ == nullptr) return false;

	window_width_ = width;
	window_height_ = height;


	window_->SetSize(width, height);
	window_->SetSizeMode(size_mode);

	memory_manager_cpu_ = (MemoryManagerCPU*) new MemoryManagerCPU(width, height);
	memory_manager_gpu_ = (MemoryManagerGPU*) new MemoryManagerGPU(width, height);

	RgbPixel* const host_display_buffer = memory_manager_cpu_->GetDisplayBufferPointer();

	host_display_buffer_ = host_display_buffer;

	RgbPixel* const device_display_buffer = memory_manager_gpu_->GetDisplayBufferPointer();

	device_display_buffer_ = device_display_buffer;

	display_buffer_size_ = memory_manager_cpu_->GetDisplayBufferSize();

	window_->SetDisplayBuffer(host_display_buffer, display_buffer_size_);
	
	graphic_engine_->SetDeviceDisplayBuffer(device_display_buffer);
	
	return true;
}

void Engine::ClearMemory() {

	
		if (console_ != nullptr) delete console_;
		if (logic_engine_ != nullptr) delete logic_engine_;
		if (graphic_engine_ != nullptr) delete graphic_engine_;
		if (window_ != nullptr) delete window_;
		if (data_base_ != nullptr) delete data_base_;
		if (camera_ != nullptr) delete camera_;
	
}

void Engine::ShowWindow(const char* windowHandle) {

	window_->SetWindowCaption(ConvertString(windowHandle));
	window_->Create();
	main_window_handle_ = window_->GetHandle();
	window_->Show();
	window_->StartMessageLoop();
	main_loop_is_running_ = false;
	main_loop_thread_.join();
	ClearMemory();

}

void Engine::InitDisplayFunction(DisplayFunction display) {

	display_function_ = display;

}
void Engine::InitSpecialFunction(SpecialFunction func) {

	logic_engine_->InitSpecialFunction(func);

}
void Engine::InitLogicFunction(LogicFunction func) {
	logic_engine_->InitLogicFunction(func);
}
void Engine::InitWindow(HINSTANCE windowHandle) {

	window_ = (wnd::Window*) new wnd::Window(windowHandle);
	window_->RegisterWindowClass();

}

void Engine::RotateModel(unsigned int model_id, const float alpha_degree, const Vector3D &around_vector,const Vertex3D &around_point) {

	logic_engine_->RotateModel(model_id, alpha_degree, around_vector, around_point);

}
void Engine::TranslateModel(const unsigned int model_id, const Vertex3D& translate_vertex)const {

	logic_engine_->TranslateModel(model_id, translate_vertex);


}
void Engine::TranslateCamera(const unsigned int camera_id, const Vertex3D &delta_vertex) {

	// 
	camera_->Translate( delta_vertex );

}
void Engine::RotateCamera(const unsigned int camera_id, const float alpha_degree, const Vector3D& rotate_vector, const Vertex3D& rotate_vertex) {

	camera_->RotateCamera(alpha_degree, rotate_vector, rotate_vertex);

}

void Engine::ScaleModel(const unsigned int model_id, const float coefficient) {

	logic_engine_->ScaleModel(model_id, coefficient);

		

}

const Vertex3D* Engine::GetCameraPosition(const unsigned int camera_id) const noexcept {

	return camera_->GetPosition();
	
}
