#pragma once
#ifndef _OBJ_H_
#define _OBJ_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "Structures.h"


using longSize = unsigned long long;
using std::string;

enum ObjectFileStatus {

	FILE_IS_NOT_OPEN,
	FILE_IS_OPEN_WITHOUT_ERRORS,
	FILE_OPEN_EXCEPTION,
	ALLOCATION_MEMORY_EXCEPTION,
	DOUBLE_READING_FILE_EXCEPTION

};

class ObjFile {

private:

	Vertex3D* vertexs;
	longSize currentNumberOfVertexs;
	longSize numberOfVertexsRead;
	Normal3D* normals;
	longSize currentNumberOfNormals;
	longSize numberOfNormalsRead;
	Polygon3D* polygons;
	longSize currentNumberOfPolygons;
	longSize numberOfPolygonsRead;
	RgbColor* rgbColors;
	longSize currentNumberOfRgbColors;
	longSize numberOfRgbColorsRead;

	ObjectFileStatus currentStatus;

	const char* fileName;
	std::ifstream* fileObject;
	unsigned int number_of_lines_;
	void CountNumberOfPrimitives();
	void AllocateMemory();
	void CopyData();
	void ProcessLineWithVertex(string* newLine);
	void ProcessLineWithNormal(string* newLine);
	void ProcessLineWithPolygon(string* newLine);
	void ProcessLineWithRgbColor(string* newLine);
	void ReadFile();

public:

	class FileOpenException {
	public:

		int codeError;
		const char* fileName;

		FileOpenException(int openCodeError, const char* unopenedFileName):
			codeError(openCodeError),
			fileName(unopenedFileName) {
		
		}

	};

	class AllocationMemoryException {};
	class DoubleReadingFileException {
	public:
		const char* firstFileName;
		const char* secondFileName;

		DoubleReadingFileException(const char* firstFileName, const char* secondFileName) {

			this->firstFileName = firstFileName;
			this->secondFileName = secondFileName;

		}
	};
	ObjFile();
	ObjFile(const char* fileName);
	~ObjFile();
	void OpenFile(const char* fileName);
	inline Vertex3D* ReturnVertexs()const noexcept { return this->vertexs; };
	inline Normal3D* ReturnNormals()const noexcept { return this->normals; }
	inline Polygon3D* ReturnPolygons()const noexcept { return this->polygons; }
	inline RgbColor* ReturnRgbColors()const noexcept { return this->rgbColors; }
	inline longSize GetNumberOfVertexs()const noexcept { return this->currentNumberOfVertexs; };
	inline longSize GetNumberOfNormals()const noexcept { return this->currentNumberOfNormals; };
	inline longSize GetNumberOfRgbColors()const noexcept { return this->currentNumberOfRgbColors; };
	inline longSize GetNumberOfPolygons()const noexcept { return this->currentNumberOfPolygons; };

};

#endif