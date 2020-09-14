#include <windows.h>
#include "GameEngine.h"
#include <iostream>

//enum ObjectType {
//	NOTHING = 0,
//	WALL = 1
//};
class Map {
private:

	const unsigned int size_x_ = 10;
	const unsigned int size_y_ = 10;
	int* array_;

public:
	Map() :array_(nullptr) {

	}
	void SetMap(int* new_map) {
		array_ = new_map;
	}

};

void display() {

	const Vector3D rotate_vector(0.0f, 1.0f, 0.0f);
	const Vertex3D rotate_vertex(0.0f, 0.0f, 0.0f);
	float rotate_angle = 10;
	//engineDisplayModeOfModel(0, SHOW);
	for (size_t i = 0; i < 100; i++)
	{
		engineTranslateModel(i, Vertex3D((i % 10) - 5.0f, 0.0f, (i / 10) - 5.0f  ));
	}

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
	if (key == ArrowLeft)for (size_t i = 0; i < 100; i++)
	engineRotateModel(i, -10.0f, rotate_vector, rotate_vertex);
	if (key == ArrowRight)for (size_t i = 0; i < 100; i++)
		engineRotateModel(i, 10.0f, rotate_vector, rotate_vertex);
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

	Map first_floor;
	int* first = new int[100]{
		0,0,0,1,0,0,0,1,1,0,
		0,1,0,0,0,1,0,0,1,0,
		0,1,1,1,1,1,1,0,0,0,
		0,0,0,0,0,0,1,1,1,0,
		1,1,1,1,1,0,0,0,0,0,
		0,0,0,0,0,0,0,1,1,0,
		0,1,1,1,0,1,0,1,0,0,
		0,0,0,0,0,1,1,1,0,1,
		1,1,1,1,0,0,0,0,0,1,
		1,1,1,1,0,1,1,1,1,1
	};

	first_floor.SetMap(first);

	engineCreateConsole("Log console");
	engineInitCUDA(CHOOSE_DEVICE, "GeForce MX150");
	engineInitDisplayMode(RIBS_MODE, DOUBLE_BUFFERED_ON);
	engineInitWindow(hInstance);
	engineInitWindowSize(FULL_SCREEN, 1920, 1080);
	for (size_t i = 0; i < 100; i++)
	{
		if (first[i] == 0) {
			engineLoad3DModel("__obj_models/floor_new.obj", "body");

		}else
			if(first[i] == 1)engineLoad3DModel("__obj_models/wall_new.obj", "body");
	}
	/*engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/Wall.obj", "body");*/
	/*engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/floor.obj", "body");
	engineLoad3DModel("__obj_models/floor.obj", "body");*/
	engineInitDisplayFunction(display);
	engineInitSpecialFunction(processKeystrokes);
	engineStartMainLoop();
	engineShowWindow("My first game written from scratch.");

	return SUCCESS_APP_EXIT;

}
