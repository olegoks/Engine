#include "MemoryManagerGPU.h"
#include "cuda_runtime_api.h"

void MemoryManagerGPU::AllocateMemoryForPrimitives() {

	Vertex3D* device_vertexs;
	Normal3D* device_normals;
	Polygon3D* device_polygons;
	RgbColor* device_colors;

	longSize vertexs_size = host_data_info_.numberOfVertexs * sizeof(Vertex3D);
	longSize normals_size = host_data_info_.numberOfNormals * sizeof(Normal3D);
	longSize polygons_size = host_data_info_.numberOfPolygons * sizeof(Polygon3D);
	longSize colors_size = host_data_info_.numberOfRgbColors * sizeof(RgbColor);

	cudaMalloc((void**)&device_vertexs, vertexs_size);
	cudaMalloc((void**)&device_normals, normals_size);
	cudaMalloc((void**)&device_polygons, polygons_size);
	cudaMalloc((void**)&device_colors, colors_size);

	cudaMemcpy(device_vertexs, host_data_info_.allVertexs, vertexs_size, cudaMemcpyHostToDevice);
	cudaMemcpy(device_normals, host_data_info_.allNormals, normals_size, cudaMemcpyHostToDevice);
	cudaMemcpy(device_polygons, host_data_info_.allPolygons, polygons_size, cudaMemcpyHostToDevice);
	cudaMemcpy(device_colors, host_data_info_.allRgbColors, colors_size, cudaMemcpyHostToDevice);

	this->device_data_info_.deviceVertexs = device_vertexs;
	this->device_data_info_.deviceNormals = device_normals;
	this->device_data_info_.devicePolygons = device_polygons;
	this->device_data_info_.device_colors = device_colors;

	Vertex3D* dev_vertexs_ptr = device_vertexs;
	Normal3D* dev_normals_ptr = device_normals;
	Polygon3D* dev_polygons_ptr = device_polygons;
	RgbColor* dev_colors_ptr = device_colors;

	for (size_t index = 0; index < host_data_info_.numberOfModels; index++) {

		host_data_info_.allModels[index].device_vertexs_pointer = dev_vertexs_ptr;
		dev_vertexs_ptr += host_data_info_.allModels[index].numberOfVertexs;

		host_data_info_.allModels[index].device_normals_pointer = dev_normals_ptr;
		dev_normals_ptr += host_data_info_.allModels[index].numberOfNormals;

		host_data_info_.allModels[index].device_polygons_pointer = dev_polygons_ptr;
		dev_polygons_ptr += host_data_info_.allModels[index].numberOfPolygons;

		host_data_info_.allModels[index].device_colors_pointer = dev_colors_ptr;
		dev_polygons_ptr += host_data_info_.allModels[index].numberOfColors;

	}

	

}

void MemoryManagerGPU::AllocateMemoryForBuffer()
{


	cudaMalloc((void**)&this->device_display_buffer_, this->display_buffer_size_);
	//cudaMalloc((void**)&second_buffer, this->display_buffer_size_);


}

