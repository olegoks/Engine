#pragma once
#ifndef _OBJ_H_
#define _OBJ_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "Structures.h"
#include <memory>
#include <vector>

using longSize = unsigned long long;
using std::string;

enum class File—ondition: uint8_t {

	FILE_IS_NOT_OPEN,
	FILE_IS_OPEN_WITHOUT_ERRORS,
	FILE_OPEN_EXCEPTION,
	ALLOCATION_MEMORY_EXCEPTION,
	DOUBLE_READING_FILE_EXCEPTION

};

class ObjFile final {

	using PVertexs = Vertex3D*;
	using PNormals = Normal3D*;
	using PPolygons = Polygon3D*;
	using PRgbColors = RgbColor*;
	using PRatios = Ratio*;

private:

	PVertexs vertexs_ = nullptr;
	PNormals normals_ = nullptr;
	PPolygons polygons_ = nullptr;
	PRgbColors rgb_colors_ = nullptr;
	PRatios ratios_ = nullptr;
	
	size_t number_of_vertexs_ = 0;
	size_t number_of_normals_ = 0;
	size_t number_of_polygons_ = 0;
	size_t number_of_rgb_colors_ = 0;
	size_t number_of_ratios_ = 0;

	size_t number_of_lines_ = 0;

	string file_name_ = "no_file_name.txt";
	File—ondition condition_ = File—ondition::FILE_IS_NOT_OPEN;
	std::ifstream file_object_;
	RgbPixel current_polygon_color_;
	void CountNumberOfPrimitives()noexcept;

	void AllocateMemory();
	void CopyData();
	void ProcessLineWithVertex(string& newLine, const size_t index);
	void ProcessLineWithNormal(string& newLine, const size_t index);
	void ProcessLineWithPolygon(string& newLine, const size_t index);
	void ProcessLineWithRgbColor(string& newLine, const size_t index);
	void ReadFile();

public:

	class ObjFileException final{
	private:

		const File—ondition condition_;
		const std::string error_;
		
	public:
		
		ObjFileException() = default;
		ObjFileException(const File—ondition condition, const std::string& error)noexcept :
			condition_(condition), error_(error) {};

		inline const std::string& GetErrorName()const noexcept { return error_; };
		inline const File—ondition GetCondition()const noexcept { return condition_; };
			
	};

	explicit ObjFile()noexcept;
	explicit ObjFile(const std::string& file_name)noexcept;
	~ObjFile();
	void open(const std::string& file_name);

};

#endif