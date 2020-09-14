#include"DataBase.h"
#include <string.h>

DataBase::DataBase():
	data_info_{ 
	0,
	nullptr,
	nullptr,
	nullptr,
	nullptr, 
	0,	
	0,
	0,
	0}

{	
	
}

DataBase::~DataBase()
{

	if (data_info_.numberOfRgbColors > 0) delete[] (RgbColor*) this->data_info_.allRgbColors;
	if (data_info_.numberOfModels > 0) delete[] (Normal3D*) this->data_info_.allNormals;
	if (data_info_.numberOfPolygons > 0)delete[] (Polygon3D*) this->data_info_.allPolygons;
	if (data_info_.numberOfVertexs > 0)delete[] (Vertex3D*) this->data_info_.allVertexs;

}

bool DataBase::LoadObjFiles()
{
	size_t numberOfFiles = this->objFilesList.size();

	try { for (size_t i = 0; i < numberOfFiles; i++) LoadObjFile(objFilesList[i]); }
	
	catch (ObjFile::FileOpenException obj_file_open_error) {

		throw LoadFilesException(obj_file_open_error.fileName, obj_file_open_error.codeError);

	}


	return EXIT_SUCCESS;

}

bool DataBase::LoadObjFile(const char* file_name) {

	ObjFile obj_file(file_name); 
	
	longSize data_base_number_of_vertexs = this->data_info_.numberOfVertexs;
	longSize data_base_number_of_normals = this->data_info_.numberOfNormals;

	//Add new vertexs
	longSize new_size_array_Of_vertexs = obj_file.GetNumberOfVertexs() + this->data_info_.numberOfVertexs;
	Vertex3D* new_array_of_vertexs = (Vertex3D*) new Vertex3D[ (unsigned int)new_size_array_Of_vertexs];
	memcpy(new_array_of_vertexs, this->data_info_.allVertexs, sizeof(Vertex3D) * this->data_info_.numberOfVertexs);
	Vertex3D* objFileVertexs = obj_file.ReturnVertexs();
	memcpy(new_array_of_vertexs + this->data_info_.numberOfVertexs, objFileVertexs, sizeof(Vertex3D) * obj_file.GetNumberOfVertexs());
	this->data_info_.numberOfVertexs += obj_file.GetNumberOfVertexs();
	delete[] data_info_.allVertexs;
	data_info_.allVertexs = new_array_of_vertexs;

	//Add new normals
	longSize newSizeArrayOfNormals = obj_file.GetNumberOfNormals() + this->data_info_.numberOfNormals;
	Normal3D* newArrayOfNormals = (Normal3D*) new Normal3D[ (unsigned int) newSizeArrayOfNormals];
	memcpy(newArrayOfNormals, this->data_info_.allNormals, sizeof(Normal3D) * this->data_info_.numberOfNormals);
	Normal3D* objFileNormals = obj_file.ReturnNormals();
	memcpy(newArrayOfNormals + this->data_info_.numberOfNormals, objFileNormals, sizeof(Normal3D) * obj_file.GetNumberOfNormals());
	this->data_info_.numberOfNormals += obj_file.GetNumberOfNormals();
	delete[] data_info_.allNormals;
	data_info_.allNormals = newArrayOfNormals;

	Polygon3D* obj_file_polygons = obj_file.ReturnPolygons();

	for (size_t index = 0; index < obj_file.GetNumberOfPolygons(); index++)
	{

		obj_file_polygons[index].ratios[0].vertexNumber += data_base_number_of_vertexs - 1;
		obj_file_polygons[index].ratios[1].vertexNumber += data_base_number_of_vertexs - 1;
		obj_file_polygons[index].ratios[2].vertexNumber += data_base_number_of_vertexs - 1;

		obj_file_polygons[index].ratios[0].normalNumber += data_base_number_of_normals - 1;
		obj_file_polygons[index].ratios[1].normalNumber += data_base_number_of_normals - 1;
		obj_file_polygons[index].ratios[2].normalNumber += data_base_number_of_normals - 1;

	}

	//Add new polygons
	longSize newSizeArrayOfPolygons = obj_file.GetNumberOfPolygons() + this->data_info_.numberOfPolygons;
	Polygon3D* newArrayOfPolygons = (Polygon3D*) new Polygon3D[(unsigned int)newSizeArrayOfPolygons];
	memcpy(newArrayOfPolygons, this->data_info_.allPolygons, sizeof(Polygon3D) * this->data_info_.numberOfPolygons);
	
	memcpy(newArrayOfPolygons + this->data_info_.numberOfPolygons, obj_file_polygons, sizeof(Polygon3D) * obj_file.GetNumberOfPolygons());
	this->data_info_.numberOfPolygons += obj_file.GetNumberOfPolygons();
	delete[] data_info_.allPolygons;
	data_info_.allPolygons = newArrayOfPolygons;

	//Add new color 
	longSize newSizeArrayOfRgbColors = this->data_info_.numberOfRgbColors + obj_file.GetNumberOfRgbColors(); //Every triangle will have own color, that is why number of Colors = number of ratio
	RgbColor* newArrayOfRgbColors = (RgbColor*) new RgbColor[newSizeArrayOfRgbColors];
	memcpy(newArrayOfRgbColors, this->data_info_.allRgbColors, sizeof(RgbColor) * this->data_info_.numberOfRgbColors);
	RgbColor* objFileRgbColors = obj_file.ReturnRgbColors();
	memcpy(newArrayOfRgbColors + this->data_info_.numberOfRgbColors, objFileRgbColors, sizeof(RgbColor) * obj_file.GetNumberOfRgbColors());
	this->data_info_.numberOfRgbColors += obj_file.GetNumberOfRgbColors();
	delete[] data_info_.allRgbColors;
	data_info_.allRgbColors = newArrayOfRgbColors;
	
	//Add model info 
	longSize newSizeOfArrayOfModels = this->data_info_.numberOfModels + 1;
	ModelInfo3D* newArrayOfModels = (ModelInfo3D*) new ModelInfo3D[newSizeOfArrayOfModels];
	memcpy(newArrayOfModels, this->data_info_.allModels, this->data_info_.numberOfModels * sizeof(ModelInfo3D) );
	newArrayOfModels[newSizeOfArrayOfModels - 1].file_name = file_name;
	newArrayOfModels[newSizeOfArrayOfModels - 1].numberOfNormals = obj_file.GetNumberOfNormals();
	newArrayOfModels[newSizeOfArrayOfModels - 1].numberOfPolygons = obj_file.GetNumberOfPolygons();
	newArrayOfModels[newSizeOfArrayOfModels - 1].numberOfVertexs = obj_file.GetNumberOfVertexs();
	newArrayOfModels[newSizeOfArrayOfModels - 1].numberOfColors = obj_file.GetNumberOfRgbColors();

	newArrayOfModels[newSizeOfArrayOfModels - 1].model_name = nullptr; //<<<<<<<<<<<<<<<<<<<<<<<<<<<
	newArrayOfModels[newSizeOfArrayOfModels - 1].vertexs_pointer = data_info_.allVertexs + data_info_.numberOfVertexs - obj_file.GetNumberOfVertexs();
	newArrayOfModels[newSizeOfArrayOfModels - 1].normals_pointer = data_info_.allNormals + data_info_.numberOfNormals - obj_file.GetNumberOfNormals();
	newArrayOfModels[newSizeOfArrayOfModels - 1].polygons_pointer = data_info_.allPolygons + data_info_.numberOfPolygons - obj_file.GetNumberOfPolygons();
	newArrayOfModels[newSizeOfArrayOfModels - 1].colors_pointer = data_info_.allRgbColors + data_info_.numberOfRgbColors - obj_file.GetNumberOfRgbColors();
	
	delete[] this->data_info_.allModels;
	this->data_info_.allModels = newArrayOfModels;
	this->data_info_.numberOfModels++;

	return EXIT_SUCCESS;

}
void DataBase::AddFileName(const char* file_name) { 

	this->objFilesList.push_back(file_name); 
	
};
unsigned int DataBase::last_model_id_ = 0;
