//#include "MemoryManagerGPU.h"
//#include "cuda_runtime_api.h"
//
//void MemoryManagerGPU::AllocateMemoryForPrimitives() {
//
//	Vertex3D* device_vertexs;
//	Normal3D* device_normals;
//	Polygon3D* device_polygons;
//
//	longSize vertexs_size = host_data_info_.numberOfVertexs * sizeof(Vertex3D);
//	longSize normals_size = host_data_info_.numberOfNormals * sizeof(Normal3D);
//	longSize polygons_size = host_data_info_.numberOfPolygons * sizeof(Polygon3D);
//
//	cudaMalloc((void **)&device_vertexs, vertexs_size);
//	cudaMalloc((void**)&device_normals, normals_size);
//	cudaMalloc((void**)&device_polygons, polygons_size);
//
//	cudaMemcpy(device_vertexs, host_data_info_.allVertexs, vertexs_size, cudaMemcpyHostToDevice);
//	cudaMemcpy(device_normals, host_data_info_.allNormals, normals_size, cudaMemcpyHostToDevice);
//	cudaMemcpy(device_polygons, host_data_info_.allPolygons, polygons_size, cudaMemcpyHostToDevice);
//
//	this->device_data_info_.deviceVertexs = device_vertexs;
//	this->device_data_info_.deviceNormals = device_normals;
//	this->device_data_info_.devicePolygons = device_polygons;
//
//}
//
//void MemoryManagerGPU::AllocateMemoryForBuffer()
//{
//
//	cudaMalloc((void**)& this->device_display_buffer_, this->display_buffer_size_);
//
//}


