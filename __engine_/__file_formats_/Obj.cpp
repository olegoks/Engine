#include "Obj.h"
#pragma warning(disable : 6386)

#include <iostream>

explicit ObjFile::ObjFile()noexcept
{

}

ObjFile::ObjFile(const std::string& file_name)noexcept:
	file_name_(file_name)
{
	
	open(file_name_);
	
}

ObjFile::~ObjFile()
{
	

}
void ObjFile::open(const std::string& file_name) {
	
	file_object_.open(file_name_, std::ios::in, std::ios::_Nocreate);

	if (!file_object_.is_open()) throw ObjFileException(FileÑondition::FILE_OPEN_EXCEPTION, "File not opened.");

	ReadFile();

};
void ObjFile::ReadFile() {

	file_object_
	AllocateMemory();
	CopyData();
	file_object_.close();
	condition_ = FileÑondition::FILE_IS_OPEN_WITHOUT_ERRORS;

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
void ObjFile::CountNumberOfPrimitives()noexcept{

	string new_line;
	string two_symbols;


	number_of_vertexs_ = 0;
	number_of_normals_ = 0;
	number_of_polygons_ = 0;
	number_of_rgb_colors_ = 0;
	number_of_lines_ = 0;

	while (getline(file_object_, new_line)) {

		number_of_lines_++; 

		if (new_line.length() > 2) {

			two_symbols.clear();
			two_symbols += new_line[0];
			two_symbols += new_line[1];

			if (two_symbols == "v ") number_of_vertexs_++;
			else
			if (two_symbols == "vn") number_of_normals_++;
			else
			if (two_symbols == "f ") number_of_polygons_++;
			else
			if (two_symbols == "us") number_of_rgb_colors_++;
				
		}

	}

}
void ObjFile::CopyData() {

	using namespace std;

	string new_line;
	string two_symbols;
	size_t vertex_number = 0 , polygon_number = 0, normal_number = 0, rgb_color_number = 0;
	file_object_.seekg(0);
	
	cout << "Loading file : " << file_name_ << endl;
	
	size_t current_number_of_lines = 0;

	cout.setf(ios::right);

	while (getline(file_object_, new_line)) {

		current_number_of_lines++;

		const float persent = round((static_cast<float>(current_number_of_lines) / static_cast<float>(number_of_lines_)) * 100);

		cout <<'\r'<< std::setw(5) << persent  << '%';

		if (new_line.length() > 2) {
	
			two_symbols.clear();
			two_symbols += new_line[0];
			two_symbols += new_line[1];

			if (two_symbols == "v ") { 
				vertex_number++;
				ProcessLineWithVertex(new_line, vertex_number); 
			}
			else
			if (two_symbols == "vn") ProcessLineWithNormal(new_line, normal_number);
			else
			if (two_symbols == "f ") ProcessLineWithPolygon(new_line, polygon_number);
			else
			if (two_symbols == "us") ProcessLineWithRgbColor(new_line, rgb_color_number);
			
		}

	}

	cout.setf(ios::left);

	cout << endl;

}
void ObjFile::ProcessLineWithVertex(string& newLine, const size_t index) {

	newLine.erase(0, 2);
	newLine.push_back(' ');

	std::stringstream stream;
	unsigned int pos;
	std::string strFloat;
	float xyz[3];

	for (size_t j = 0; j < 3; j++)
	{
		pos = newLine.find(' ');

		strFloat.clear();
		for (size_t i = 0; i < pos; i++) {

			strFloat += newLine.at(i);

		}

		newLine.erase(0, pos + 1);

		stream << strFloat;
		stream >> xyz[j];
		stream.clear();

	}

	vertexs_[index].x = xyz[0];
	vertexs_[index].y = xyz[1];
	vertexs_[index].z = xyz[2];
	   
}
void ObjFile::ProcessLineWithNormal(string& newLine, const size_t index) {

	newLine.erase(0, 3);
	newLine.push_back(' ');

	std::stringstream stream;

	unsigned int pos;
	std::string strFloat;
	float xyz[3];

	for (size_t j = 0; j < 3; j++)
	{
		pos = newLine.find(' ');

		strFloat.clear();
		for (size_t i = 0; i < pos; i++) {

			strFloat += newLine.at(i);

		}

		newLine.erase(0, pos + 1);

		stream << strFloat;
		stream >> xyz[j];
		stream.clear();

	}

	normals_[index].x = xyz[0];
	normals_[index].y = xyz[1];
	normals_[index].z = xyz[2];

}
void ObjFile::ProcessLineWithPolygon(string& newLine, const size_t index) {
	
	newLine.erase(0, 2);
	newLine.push_back(' ');

	unsigned int pos;
	std::string strInt;
	std::stringstream stream;

	for (size_t i = 0; i < 3; i++)
	{
		pos = newLine.find('/');

		strInt.clear();
		for (size_t j = 0; j < pos; j++)
		{
			strInt += newLine.at(j);
		}

		stream.clear();
		stream << strInt;
		stream >> polygons_[index].ratios[i].vertexNumber;

		newLine.erase(0, pos + 1);
		pos = newLine.find('/');
		newLine.erase(0, pos + 1);

		pos = newLine.find(' ');

		strInt.clear();
		for (size_t j = 0; j < pos; j++)
		{
			strInt += newLine.at(j);
		}


		stream.clear();
		stream << strInt;
		stream >> polygons_[index].ratios[i].normalNumber;

		newLine.erase(0, pos + 1);
	}

	polygons_[index].color = (RgbPixel)current_polygon_color_;

}
void ObjFile::ProcessLineWithRgbColor(string& newLine, const size_t index) {

	newLine.erase(0, 7);
	unsigned int length_of_the_line = newLine.size();

	if (length_of_the_line <= 6) {

		if (length_of_the_line < 6) {
			for (size_t i = 0; i <  6 - length_of_the_line; i++) newLine.insert(0, "0");	
		}

		this->rgb_colors_[index];
		std::string strRGB;
		unsigned int intTemp;
		std::stringstream convertStream;

		for (size_t i = 0; i < 6 - length_of_the_line; i++)
		{
			(newLine) = "0" + (newLine);
		}

		//R
		strRGB = newLine.substr(0, 2);
		convertStream << strRGB;
		convertStream >> std::hex >> intTemp;
		convertStream.clear();
		//rgbColors[currentNumberOfRgbColors].rgb_red = intTemp;
		current_polygon_color_.rgb_red = intTemp;
		//G
		strRGB = newLine.substr(2, 2);
		convertStream << strRGB;
		convertStream >> std::hex >> intTemp;
		convertStream.clear();
		//rgbColors[currentNumberOfRgbColors].rgb_green = intTemp;
		current_polygon_color_.rgb_green = intTemp;
		//B
		strRGB = newLine.substr(4, 2);
		convertStream << strRGB;
		convertStream >> std::hex >> intTemp;
		convertStream.clear();

		//rgbColors[currentNumberOfRgbColors].rgb_blue = intTemp;
		current_polygon_color_.rgb_blue= intTemp;
	}
	else
	{
		
		rgb_colors_[index].rgb_red = 0;
		rgb_colors_[index].rgb_green = 0;
		rgb_colors_[index].rgb_blue = 0;

	}
}

