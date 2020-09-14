#include <windows.h>
#include "__engine_/GameEngine.h"
#include <iostream>

//enum ObjectType {
//	NOTHING = 0,
//	WALL = 1
//};
struct Position {
	int x;
	int y;
};
class Man {
private:
	Position current = { 0, 0 };

public:

	void SetPosition(int x, int y) {
		current.x = x;
		current.y = y;
	}
	Position CurrentPosition() {
		return current;
	}
} man;


class Map {
private:

	const unsigned int size_x_ = 10;
	const unsigned int size_y_ = 10;
	int* array_;

public:

	Map() :array_(nullptr) {
		array_ = new int[100]{
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
	}
	inline int GetObjectType(int x, int y) { return *(array_ + y * 10 + x); }

	void SetMap(int* new_map) {
		array_ = new_map;
	}

	int* GetArray() {
		return array_;
	}

}map;

void logicFunction() {

}
void display() {

	const Vector3D rotate_vector(0.0f, 1.0f, 0.0f);
	const Vertex3D rotate_vertex(0.0f, 0.0f, 0.0f);
	float rotate_angle = 10;
	//engineDisplayModeOfModel(0, SHOW);
	for (size_t i = 0; i < 101; i++)
	{
		engineTranslateModel(i, Vertex3D((i % 10) - 5.0f, 0.0f, (i / 10) - 5.0f  ));
	}

	engineTranslateModel(100, Vertex3D(0.1f, 0.0f, -9.5f));

}

bool PositionIsCorrect(Position pos, Map* map) {
	
	if (pos.x == 4 && pos.y == 8)
		MessageBox(NULL, L"You Win", L"Caption", MB_TOPMOST);
	
	if (map->GetObjectType(pos.x, pos.y) == 0 && pos.x >=0 && pos.x < 10 && pos.y >=0 && pos.y <10) return true;
	else
		return false;

	
}

void processKeystrokes(KeyType key) {

	const Vector3D rotate_vector(0.0f, 1.0f, 0.0f);
	const Vertex3D rotate_vertex(0.0f, 0.0f, 0.0f);

	const Vertex3D translate_vertex_left(-1.0f, 0.0f, 0.0f);
	const Vertex3D translate_vertex_right(1.0f, 0.0f, 0.0f);
	const Vertex3D translate_vertex_down(0.0f, 0.0f, -1.0f);
	const Vertex3D translate_vertex_up(0.0f, 0.0f, 1.0f);

	Position current = man.CurrentPosition();
	Position new_pos = current;
	printf("%d, %d", new_pos.x, new_pos.y);
	if (key == A){
		new_pos.x -= 1;
		if (PositionIsCorrect(new_pos, &map)) {
			man.SetPosition(new_pos.x, new_pos.y);
			engineTranslateModel(100, translate_vertex_left);
		}
	}

	if (key == D) {
		new_pos.x += 1;
		if (PositionIsCorrect(new_pos, &map)) {
			man.SetPosition(new_pos.x, new_pos.y);
			engineTranslateModel(100, translate_vertex_right);
		}
	}

	if (key == W) {
		new_pos.y += 1;
		if (PositionIsCorrect(new_pos, &map)) {
			man.SetPosition(new_pos.x, new_pos.y);
			engineTranslateModel(100, translate_vertex_up);
		}
	}
	if (key == S) { 
		new_pos.y -= 1;
		if (PositionIsCorrect(new_pos, &map)) {
			man.SetPosition(new_pos.x, new_pos.y);
			engineTranslateModel(100, translate_vertex_down);
		}
	}
	//engineRotateModel(0, 0.1, rotate_vector, rotate_vertex);
	if (key == ArrowLeft)engineRotateCamera(0, -10.0f, rotate_vector, rotate_vertex);//for (size_t i = 0; i < 101; i++)
	//engineRotateModel(i, -10.0f, rotate_vector, rotate_vertex);
	if (key == ArrowRight)engineRotateCamera(0, 10.0f, rotate_vector, rotate_vertex);//for (size_t i = 0; i < 101; i++)
		//engineRotateModel(i, 10.0f, rotate_vector, rotate_vertex);
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
	int* first = map.GetArray();
	for (size_t i = 0; i < 100; i++)
	{
		if (first[i] == 0) {
			engineLoad3DModel("__obj_models/floor_new.obj", "body");

		}else
			if(first[i] == 1)engineLoad3DModel("__obj_models/wall_new.obj", "body");
	}
	engineLoad3DModel("__obj_models/lego.obj", "body");
	/*engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/Wall.obj", "body");*/
	/*engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/Wall.obj", "body");
	engineLoad3DModel("__obj_models/floor.obj", "body");
	engineLoad3DModel("__obj_models/floor.obj", "body");*/
	engineInitDisplayFunction(display);
	engineInitSpecialFunction(processKeystrokes);
	engineInitLogicFunction(logicFunction);
	engineStartMainLoop();
	engineShowWindow("My first game written from scratch.");

	return SUCCESS_APP_EXIT;

}
