#include "EngineV1dot0.h"
#include "GameEngine.h"
static Engine* const engine = (Engine*) new Engine;

bool engineCreateConsole(const char* console_caption) {

	try {

		engine->CreateConsole("LogConsole");
		return true;

	}
	catch (Engine::ConsoleCreationException) {

		return false;

	}

}

bool engineInitCUDA(ChoiceOfCUDADevice cuda_device, const char* device_name) 
{

	return true;

}
void engineInitDisplayMode(EngineMode engine_mode, DoubleBuffered double_buffered) 
{

	engine->InitDisplayMode(engine_mode, double_buffered);

}
void engineInitWindow(HINSTANCE hInstance) 
{

	engine->InitWindow(hInstance);

}
void engineInitWindowSize(SizeMode size_mode, const unsigned int width, const unsigned int height) 
{

	engine->InitWindowSize(size_mode, width, height);

}

void engineLoad3DModel(const char* file_name, const char* model_name) 
{

	engine->Load3DModel(file_name, model_name);

}

void engineInitDisplayFunction(DisplayFunction display_function_pointer) 
{
	
	engine->InitDisplayFunction(display_function_pointer);

}

void engineStartMainLoop() 
{

	engine->StartMainLoop();

}

void engineShowWindow(const char* window_caption) 
{

	engine->ShowWindow(window_caption);

}

void engineTranslateModel(const unsigned int model_id, const Vertex3D& translate_vertex)
{
	engine->TranslateModel(model_id, translate_vertex);

}

void engineRotateModel(unsigned int model_id, const float angle_degree, const Vector3D &around_vector, const  Vertex3D &around_vertex) 
{

	engine->RotateModel(model_id, angle_degree, around_vector, around_vertex );

}

void engineScaleModel(const unsigned int model_id, const float coefficient)
{

	engine->ScaleModel(model_id, coefficient);

}

void engineInitSpecialFunction(SpecialFunction func)
{

	engine->InitSpecialFunction(func);

}
void engineInitLogicFunction(LogicFunction func) {

	engine->InitLogicFunction(func);
}
const Vertex3D* engineGetCameraPosition(const unsigned int camera_id)
{
	return engine->GetCameraPosition(camera_id);
}

//constexpr Vector3D engineCalculateVector(Vertex3D begin, Vertex3D end)
//{
//
//	//Vector3D vector(end->x - begin->x, end->y - begin->y, end->z - begin->z );
//	Vector3D vectorX = { end.x - begin.x, end.y - begin.y, end.z - begin.z };
//	return vectorX;
//}

Vector3D engineCalculateVector(const Vertex3D* const begin, const Vertex3D* const end) {
	
	Vector3D vector(end->x - begin->x, end->y - begin->y, end->z - begin->z);
	const float length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	
	vector.x /= length;
	vector.y /= length;
	vector.z /= length;

	return vector;
}
void engineTranslateCamera(const unsigned int camera_id, const Vertex3D &delta_vertex) 
{

	engine->TranslateCamera(camera_id, delta_vertex);

}
void engineRotateCamera(const unsigned int camera_id, const float alpha_degree, const Vector3D& rotate_vector, const Vertex3D& rotate_vertex)
{

	engine->RotateCamera(camera_id, alpha_degree, rotate_vector, rotate_vertex);

}
