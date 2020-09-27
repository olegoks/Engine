#include <windows.h>
#include "__engine_/GameEngine.h"
#include <iostream>

void logicFunction() {

}
void display() {

	/*const Vector3D rotate_vector(0.0f, 1.0f, 0.0f);
	const Vertex3D rotate_vertex(0.0f, 0.0f, 0.0f);
	float rotate_angle = 10;*/
	//engineDisplayModeOfModel(0, SHOW);
	                                
}



void processKeystrokes(KeyType key) {

	const Vector3D rotate_vector(0.0f, 1.0f, 0.0f);
	const Vertex3D rotate_vertex(0.0f, 0.0f, 0.0f);

	const Vertex3D translate_vertex_left(-1.0f, 0.0f, 0.0f);
	const Vertex3D translate_vertex_right(1.0f, 0.0f, 0.0f);
	const Vertex3D translate_vertex_down(0.0f, 0.0f, -1.0f);
	const Vertex3D translate_vertex_up(0.0f, 0.0f, 1.0f);

	const Vertex3D* const camera_position = engineGetCameraPosition(0);
	const Vertex3D origin(0.0f, 0.0f, 0.0f);

	const Vector3D dir_vector = engineCalculateVector(camera_position, &origin);

	const Vertex3D delta_vertex_first = { dir_vector.x, dir_vector.y , dir_vector.z };
	const Vertex3D delta_vertex_second = { -dir_vector.x, -dir_vector.y , -dir_vector.z };

	switch (key) {

		case A: {
			engineTranslateModel(0, translate_vertex_left);
			break;
		}
		case D: {
			engineTranslateModel(0, translate_vertex_right);
			break;
		}
		case W: {
			engineTranslateModel(0, translate_vertex_up);
			break;
		}
		case S: {
			engineTranslateModel(0, translate_vertex_down);
			break;
		}
		case ArrowLeft: {
			engineRotateCamera(0, -10.0f, rotate_vector, rotate_vertex);
			break;
		}
		case ArrowRight: {
			engineRotateCamera(0, 10.0f, rotate_vector, rotate_vertex);
			break;
		}
		case WheelDown: {
			engineTranslateCamera(0, delta_vertex_first);
			break;
		}
		case WheelUp: {
			engineTranslateCamera(0, delta_vertex_second);
			break;
		}
	}


}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	engineCreateConsole("Log console");
	engineInitCUDA(CHOOSE_DEVICE, "GeForce MX150");
	engineInitDisplayMode(RIBS_MODE, DOUBLE_BUFFERED_ON);
	engineInitWindow(hInstance);
	engineInitWindowSize(FULL_SCREEN, 1920, 1080);
	engineLoad3DModel("__obj_models/Cube.obj", "body");
	engineInitDisplayFunction(display);
	engineInitSpecialFunction(processKeystrokes);
	engineInitLogicFunction(logicFunction);
	engineStartMainLoop();
	engineShowWindow("My first game written from scratch.");

	return SUCCESS_APP_EXIT;

}
