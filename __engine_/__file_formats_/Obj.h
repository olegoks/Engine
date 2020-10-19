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

	using PVertexs = std::shared_ptr<Vertex3D>;
	using PNormals = std::shared_ptr<Vertex3D>;
	using PPolygons = std::shared_ptr<Vertex3D>;
	using PRgbColors = std::shared_ptr<Vertex3D>;
	using PRatios = std::shared_ptr<Vertex3D>;
	using FileObject = std::ifstream;
	static struct FilePointers {

	};

private:

	PVertexs vertexs_ = nullptr;
	PNormals normals_ = nullptr;
	PPolygons polygons_ = nullptr;
	PRgbColors rgb_colors_ = nullptr;
	PRatios ratios_ = nullptr;
	
	std::string file_name_ = "no_file_name.txt";
	File—ondition condition_ = File—ondition::FILE_IS_NOT_OPEN;
	FileObject file_object_;

	void CountNumberOfPrimitives();
	void AllocateMemory();
	void CopyData();
	void ProcessLineWithVertex(string* newLine);
	void ProcessLineWithNormal(string* newLine);
	void ProcessLineWithPolygon(string* newLine);
	void ProcessLineWithRgbColor(string* newLine);
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
	inline PVertexs ReturnVertexs()const noexcept { return vertexs_; };
	inline PNormals ReturnNormals()const noexcept { return normals_; }
	inline PPolygons ReturnPolygons()const noexcept { return polygons_; }
	inline PRgbColors ReturnRgbColors()const noexcept { return rgb_colors_; }

};

#endif