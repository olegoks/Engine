#pragma once
#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

using longSize = unsigned long long;

typedef unsigned int ModelId;

enum KeyType {

	ArrowLeft = 1,
	ArrowRight = 2,
	ArrowUp = 3,
	ArrowDown = 4,
	WheelDown = 5,
	WheelUp = 6,
	MouseLeft = 7,
	MouseRight = 8,
	A = 9,
	S = 10,
	D = 11,
	W = 12

};

struct Keystroke {

	KeyType key_type;
	unsigned int time_point;

};

typedef void(*DisplayFunction)(void);
typedef void(*SpecialFunction)(KeyType);
typedef void(*LogicFunction)(void);
struct Coordinats3D {

	float x;
	float y;
	float z;

};
struct Vector2D {
	float x;
	float y;
};
struct Vector3D {

	inline Vector3D()noexcept : x(0.0f), y(0.0f), z(0.0f) {};
	inline Vector3D(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {};
	float x;
	float y;
	float z;

};

struct RgbColor {

	unsigned char	rgb_blue;
	unsigned char   rgb_green;
	unsigned char   rgb_red;
	unsigned char   rgb_reserved;

};


struct Normal3D {

	inline Normal3D() noexcept : x(0.0f), y(0.0f), z(0.0f) {};
	inline Normal3D(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {};
	float x;
	float y;
	float z;

};

struct Vertex3D {

	Vertex3D(): x(0.0f), y(0.0f), z(0.0f) {};
	Vertex3D(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {};
	float x;
	float y;
	float z;

};

struct Ratio {

	unsigned int vertexNumber;
	unsigned int normalNumber;

};
struct Polygon3D {
	Ratio ratios[3];
};

enum DisplayMode {
	SHOW,
	NOT_SHOW
};

struct ModelInfo3D {

	const char* file_name;
	const char* model_name;

	longSize numberOfVertexs;
	longSize numberOfNormals;
	longSize numberOfPolygons;
	longSize numberOfColors;

	Vertex3D* vertexs_pointer;
	Normal3D* normals_pointer;
	Polygon3D* polygons_pointer;
	RgbColor* colors_pointer;

	Vertex3D* device_vertexs_pointer;
	Normal3D* device_normals_pointer;
	Polygon3D* device_polygons_pointer;
	RgbColor* device_colors_pointer;

	inline ModelInfo3D()noexcept :
		numberOfVertexs(0), 
		numberOfNormals(0),
		numberOfPolygons(0), 
		file_name(nullptr), 
		model_name(nullptr),
		normals_pointer(nullptr),
		vertexs_pointer(nullptr),
		polygons_pointer(nullptr),
		device_normals_pointer(nullptr),
		device_vertexs_pointer(nullptr),
		device_polygons_pointer(nullptr)
	{};

};
//Properties of device that return CUDA
struct DeviceProperties {

	char         name[256];					 /**< ASCII string identifying device */
	size_t       totalGlobalMem;             /**< Global memory available on device in bytes */
	size_t       sharedMemPerBlock;			 /**< Shared memory available per block in bytes */
	int          regsPerBlock;			     /**< 32-bit registers available per block */
	int          maxThreadsPerBlock;         /**< Maximum number of threads per block */
	int          maxThreadsDim[3];           /**< Maximum size of each dimension of a block */
	int          maxGridSize[3];             /**< Maximum size of each dimension of a grid */
	int          clockRate;					 /**< Clock frequency in kilohertz */
	size_t       totalConstMem;              /**< Constant memory available on device in bytes */
	int          major;                      /**< Major compute capability */
	int          minor;						 /**< Minor compute capability */
	int          multiProcessorCount;        /**< Number of multiprocessors on device */
	int          kernelExecTimeoutEnabled;	 /**< Specified whether there is a run time limit on kernels */
	int          memoryClockRate;            /**< Peak memory clock frequency in kilohertz */
	int          memoryBusWidth;             /**< Global memory bus width in bits */
	size_t       sharedMemPerMultiprocessor; /**< Shared memory available per multiprocessor in bytes */
	int          regsPerMultiprocessor;      /**< 32-bit registers available per multiprocessor */
	int          maxBlocksPerMultiProcessor; /**< Maximum number of resident blocks per multiprocessor */

};

// All info of 3D models
struct DataInfo {

	ModelInfo3D* allModels;
	Vertex3D* allVertexs;
	Normal3D* allNormals;
	Polygon3D* allPolygons;
	RgbColor* allRgbColors;

	longSize numberOfModels;
	longSize numberOfVertexs;
	longSize numberOfNormals;
	longSize numberOfPolygons;
	longSize numberOfRgbColors;

};

struct objFileErrorInfo {

	objFileErrorInfo(const char* fileNameWithError, const char* errorIdentity) :fileName(fileNameWithError), errorName(errorIdentity) {};
	const char* fileName;
	const char* errorName;

};

struct GpuDataPointers {

	Vertex3D* deviceVertexs;
	Normal3D* deviceNormals;
	Polygon3D* devicePolygons;
	RgbColor* device_colors;

};

struct RgbPixel {
	unsigned char	rgb_blue;
	unsigned char   rgb_green;
	unsigned char   rgb_red;
	unsigned char   rgb_reserved;
};
#endif