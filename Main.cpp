#include <windows.h>
#include "GameEngine.h"
#include <iostream>


void display() {

	const float rotate_angle = 1.0f;
	const float vector_x = 0;
	const float vector_y = 1;
	const float vector_z = 0;
	
	//engineDisplayModeOfModel(0, SHOW);
	//engineRotateModel("Cube", rotate_angle, vector_x, vector_y, vector_z);

}

void processKeystrokes(KeyType key) {

	const Vector3D rotate_vector(0.0f, 1.0f, 0.0f);
	const Vertex3D rotate_vertex(0.0f, 0.0f, 0.0f);

	const Vertex3D translate_vertex_left(-1.0f, 0.0f, 0.0f);
	const Vertex3D translate_vertex_right(1.0f, 0.0f, 0.0f);
	const Vertex3D translate_vertex_down(0.0f, 0.0f, -1.0f);
	const Vertex3D translate_vertex_up(0.0f, 0.0f, 1.0f);

	if (key == A)engineTranslateModel(0, translate_vertex_left);
	if (key == D)engineTranslateModel(0, translate_vertex_right);
	if (key == W)engineTranslateModel(0, translate_vertex_up);
	if (key == S)engineTranslateModel(0, translate_vertex_down);
	//engineRotateModel(0, 0.1, rotate_vector, rotate_vertex);
	if (key == ArrowLeft)engineRotateModel(0, -10.0f, rotate_vector, rotate_vertex);
	if (key == ArrowRight)engineRotateModel(0, 10.0f, rotate_vector, rotate_vertex);
	//if (key == WheelDown) engineScaleModel(0, 0.5);
	//if (key == WheelUp) engineScaleModel(0, 1.5);

	const Vertex3D* const camera_position = engineGetCameraPosition(0);
	const Vertex3D origin(0.0f, 0.0f, 0.0f);

	const Vector3D dir_vector = engineCalculateVector(camera_position, &origin);

	const Vertex3D delta_vertex_first = { dir_vector.x, dir_vector.y , dir_vector.z };
	const Vertex3D delta_vertex_second = { -dir_vector.x, -dir_vector.y , -dir_vector.z };
	//Translate camera position for vector direction 

	if (key == WheelDown)engineTranslateCamera(0, delta_vertex_first );
	if (key == WheelUp)engineTranslateCamera(0, delta_vertex_second);

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	engineCreateConsole("Log console");
	engineInitCUDA(CHOOSE_DEVICE, "GeForce MX150");
	engineInitDisplayMode(RIBS_MODE, DOUBLE_BUFFERED_ON);
	engineInitWindow(hInstance);
	engineInitWindowSize(FULL_SCREEN, 1920, 1080);
	engineLoad3DModel("__obj_models/manbody.obj", "body");
	engineInitDisplayFunction(display);
	engineInitSpecialFunction(processKeystrokes);
	engineStartMainLoop();
	engineShowWindow("My first game written from scratch.");

	return SUCCESS_APP_EXIT;

}
