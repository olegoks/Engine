#include "Obj.h"
#pragma warning(disable : 6386)

#include <iostream>

ObjFile::ObjFile() :
	currentNumberOfVertexs(0),
	currentNumberOfNormals(0),
	currentNumberOfRgbColors(0),
	currentNumberOfPolygons(0),
	numberOfVertexsRead(0),
	numberOfNormalsRead(0),
	numberOfRgbColorsRead(0),
	numberOfPolygonsRead(0),
	number_of_lines_(0),
	normals(nullptr),
	vertexs(nullptr),
	rgbColors(nullptr),
	currentStatus(FILE_IS_NOT_OPEN)
{

}

ObjFile::ObjFile(const char* fileName) :
	currentNumberOfVertexs(0),
	currentNumberOfNormals(0),
	currentNumberOfRgbColors(0),
	currentNumberOfPolygons(0),
	numberOfVertexsRead(0),
	numberOfNormalsRead(0),
	numberOfRgbColorsRead(0),
	numberOfPolygonsRead(0),
	number_of_lines_(0),
	normals(nullptr),
	vertexs(nullptr),
	rgbColors(nullptr),
	currentStatus(FILE_IS_NOT_OPEN)
{
	this->fileName = fileName;

	ReadFile();
}

ObjFile::~ObjFile()
{
	delete[](RgbColor*) this->rgbColors;
	delete[](Vertex3D*) this->vertexs;
	delete[](Ratio*) this->polygons;
	delete[](Normal3D*) this->normals;

}
void ObjFile::OpenFile(const char* fileName) {
	
	if (this->currentStatus == FILE_IS_OPEN_WITHOUT_ERRORS || this->currentStatus == DOUBLE_READING_FILE_EXCEPTION) {
		
		this->currentStatus = DOUBLE_READING_FILE_EXCEPTION;
		throw DoubleReadingFileException(fileName, this->fileName);
	
	};

	this->fileName = fileName;
	this->ReadFile();

};
void ObjFile::ReadFile() {

	std::ifstream objFile;
	objFile.open(this->fileName, std::ios::in, std::ios::_Nocreate);
	if (!objFile.is_open()) throw FileOpenException(objFile.rdstate(), this->fileName);
	this->fileObject = &objFile;
	AllocateMemory();
	CopyData();
	objFile.close();
	this->currentStatus = FILE_IS_OPEN_WITHOUT_ERRORS;

}
void ObjFile::AllocateMemory() {

	CountNumberOfPrimitives();
	try {
		this->vertexs = (Vertex3D*) new Vertex3D[this->numberOfVertexsRead];
		this->normals = (Normal3D*) new Normal3D[this->numberOfNormalsRead];
		this->polygons = (Polygon3D*) new Polygon3D[this->numberOfPolygonsRead];
		this->rgbColors = (RgbColor*) new RgbColor[this->numberOfRgbColorsRead];
	}
	catch(std::bad_alloc){
		throw AllocationMemoryException();
	}

}
void ObjFile::CountNumberOfPrimitives(){

	string newLine;
	string twoSymbols;

	this->number_of_lines_ = 0;

	while (getline(*this->fileObject, newLine)) {

		this->number_of_lines_++; 

		if (newLine.length() > 2) {

			twoSymbols.resize(0);
			twoSymbols += newLine.at(0);
			twoSymbols += newLine.at(1);

			if (twoSymbols == "v ") {
				this->numberOfVertexsRead++;
				continue;
			}

			if (twoSymbols == "vn") {
				this->numberOfNormalsRead++;
				continue;
			}

			if (twoSymbols == "f ") {
				this->numberOfPolygonsRead++;
				continue;
			}

			if (twoSymbols == "us") {
				this->numberOfRgbColorsRead++;
				continue;
			}

		}

	}

}
void ObjFile::CopyData() {

	string newLine;
	string twoSymbols;
	this->fileObject->clear();
	this->fileObject->seekg(0);

	unsigned int number_of_current_line = 0;
	
	std::cout << "Loading file : " << this->fileName << std::endl;

	while (getline(*this->fileObject, newLine)) {

		number_of_current_line++;
		std::cout.setf(std::ios::right);
		std::cout <<'\r'<< std::setw(5)  << round((float)number_of_current_line / (float)this->number_of_lines_ * 100)  << '%';

		if (newLine.length() > 2) {
			//std::cout << newLine << std::endl;
			twoSymbols.clear();
			twoSymbols += newLine.at(0);
			twoSymbols += newLine.at(1);

			if (twoSymbols == "v ") {
				ProcessLineWithVertex(&newLine);
				
				continue;
			}

			if (twoSymbols == "vn") {
				ProcessLineWithNormal(&newLine);
				continue;
			}

			if (twoSymbols == "f ") {
				ProcessLineWithPolygon(&newLine);
				continue;
			}

			if (twoSymbols == "us") {
				ProcessLineWithRgbColor(&newLine);
				continue;
			}

		}

	}

	std::cout << std::endl;

}
void ObjFile::ProcessLineWithVertex(string* newLine) {

	newLine->erase(0, 2);
	newLine->push_back(' ');

	std::stringstream stream;
	unsigned int pos;
	std::string strFloat;
	float xyz[3];

	for (size_t j = 0; j < 3; j++)
	{
		pos = newLine->find(' ');

		strFloat.clear();
		for (size_t i = 0; i < pos; i++) {

			strFloat += newLine->at(i);

		}

		newLine->erase(0, pos + 1);

		stream << strFloat;
		stream >> xyz[j];
		stream.clear();

	}

	this->vertexs[this->currentNumberOfVertexs].x = xyz[0];
	this->vertexs[this->currentNumberOfVertexs].y = xyz[1];
	this->vertexs[this->currentNumberOfVertexs].z = xyz[2];

	this->currentNumberOfVertexs++;
	   
}
void ObjFile::ProcessLineWithNormal(string* newLine) {

	newLine->erase(0, 3);
	newLine->push_back(' ');

	std::stringstream stream;

	unsigned int pos;
	std::string strFloat;
	float xyz[3];

	for (size_t j = 0; j < 3; j++)
	{
		pos = newLine->find(' ');

		strFloat.clear();
		for (size_t i = 0; i < pos; i++) {

			strFloat += newLine->at(i);

		}

		newLine->erase(0, pos + 1);

		stream << strFloat;
		stream >> xyz[j];
		stream.clear();

	}

	this->normals[this->currentNumberOfNormals].x = xyz[0];
	this->normals[this->currentNumberOfNormals].y = xyz[1];
	this->normals[this->currentNumberOfNormals].z = xyz[2];

	this->currentNumberOfNormals++;

}
void ObjFile::ProcessLineWithPolygon(string* newLine) {
	
	newLine->erase(0, 2);
	newLine->push_back(' ');

	unsigned int pos;
	std::string strInt;
	std::stringstream stream;

	for (size_t i = 0; i < 3; i++)
	{
		pos = newLine->find('/');

		strInt.clear();
		for (size_t j = 0; j < pos; j++)
		{
			strInt += newLine->at(j);
		}

		stream.clear();
		stream << strInt;
		stream >> this->polygons[currentNumberOfPolygons].ratios[i].vertexNumber;


		newLine->erase(0, pos + 1);
		pos = newLine->find('/');
		newLine->erase(0, pos + 1);

		pos = newLine->find(' ');

		strInt.clear();
		for (size_t j = 0; j < pos; j++)
		{
			strInt += newLine->at(j);
		}


		stream.clear();
		stream << strInt;
		stream >> this->polygons[currentNumberOfPolygons].ratios[i].normalNumber;

		newLine->erase(0, pos + 1);
	}

	this->currentNumberOfPolygons++;

}
void ObjFile::ProcessLineWithRgbColor(string* newLine) {
	newLine->erase(0, 7);
	unsigned int length_of_the_line = newLine->size();

	if (length_of_the_line <= 6) {

		if (length_of_the_line < 6) {
			for (size_t i = 0; i <  6 - length_of_the_line; i++) newLine->insert(0, "0");	
		}

		this->rgbColors[this->currentNumberOfRgbColors];
		std::string strRGB;
		unsigned int intTemp;
		std::stringstream convertStream;

		for (size_t i = 0; i < 6 - length_of_the_line; i++)
		{
			(*newLine) = "0" + (*newLine);
		}

		//R
		strRGB = newLine->substr(0, 2);
		convertStream << strRGB;
		convertStream >> std::hex >> intTemp;
		convertStream.clear();
		this->rgbColors[this->currentNumberOfRgbColors].rgb_red = intTemp;

		//G
		strRGB = newLine->substr(2, 2);
		convertStream << strRGB;
		convertStream >> std::hex >> intTemp;
		convertStream.clear();
		this->rgbColors[this->currentNumberOfRgbColors].rgb_green = intTemp;

		//B
		strRGB = newLine->substr(4, 2);
		convertStream << strRGB;
		convertStream >> std::hex >> intTemp;
		convertStream.clear();

		this->rgbColors[this->currentNumberOfRgbColors].rgb_blue = intTemp;

		this->currentNumberOfRgbColors++;
	}
	else
	{
		
		this->rgbColors[this->currentNumberOfRgbColors].rgb_red = 0;
		this->rgbColors[this->currentNumberOfRgbColors].rgb_green = 0;
		this->rgbColors[this->currentNumberOfRgbColors].rgb_blue = 0;

	}
}

