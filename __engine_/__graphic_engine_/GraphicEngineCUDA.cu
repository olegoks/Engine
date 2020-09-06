
#include "GraphicEngineCUDA.cuh"
#include "__graphic_engine_/GraphicEngine.h"


void GraphicEngine::AllocateVertex2D() {

	unsigned int vertexs2d_size = data_info_.numberOfVertexs * sizeof(Vertex2D);

	cudaMalloc((void**)& device_vertexs_2d_, vertexs2d_size);
	cudaMallocHost((void**)& host_vertexs_2d_, vertexs2d_size);

};


__global__ void ProjectVertexs(const Vertex3D* const vertexs_3d, Vertex2D* const vertexs_2d, const unsigned int number_of_vertexs, const Vertex3D cameraPosition, const Vector3D vectorX, const Vector3D vectorY, const Vector3D vectorZ, const float k) {

		int threadIndex = threadIdx.x + blockIdx.x * blockDim.x;
	
	if (threadIndex < number_of_vertexs) {

		//Offset

		float offset_x = vertexs_3d[threadIndex].x - cameraPosition.x;
		float offset_y = vertexs_3d[threadIndex].y - cameraPosition.y;
		float offset_z = vertexs_3d[threadIndex].z - cameraPosition.z;

		float new_x = offset_x * vectorX.x + offset_y * vectorX.y + offset_z * vectorX.z;
		float new_y = offset_x * vectorY.x + offset_y * vectorY.y + offset_z * vectorY.z;
		float new_z = offset_x * vectorZ.x + offset_y * vectorZ.y + offset_z * vectorZ.z;
		
		vertexs_2d[threadIndex].x = ( k * new_x / (new_z + k) ) ;
		vertexs_2d[threadIndex].y = ( k * new_y / (new_z + k) ) ;

	}

}

__global__ void ConvertInDisplayCoordinats(Vertex2D* const vertexs_2d, const unsigned int number_of_vertexs, const unsigned int display_width, const unsigned int display_height) {
	
	int threadIndex = threadIdx.x + blockIdx.x * blockDim.x;

	if (threadIndex < number_of_vertexs) {

		vertexs_2d[threadIndex].x = vertexs_2d[threadIndex].x * 1000.0f + display_width / 2;
		vertexs_2d[threadIndex].y = display_height / 2 - vertexs_2d[threadIndex].y * 1000.0f;
		
	}

}
__global__ void DrawLines(const Vertex2D* const vertexs_2d, const Polygon3D* const device_polygons, const unsigned int number_of_polygons, RgbPixel* const display_buffer, const unsigned int display_width, const unsigned int display_height) {

	int threadIndex = threadIdx.x + blockIdx.x * blockDim.x;

	if (threadIndex < number_of_polygons * 3) {

		const unsigned int polygon_number = threadIndex / 3;
		const unsigned int vertex_number = threadIndex % 3;
	
		const unsigned int first_vertex_index = device_polygons[polygon_number].ratios[vertex_number].vertexNumber;
		const unsigned int second_vertex_number = ((vertex_number + 1) < 3) ? (vertex_number + 1) : 0;
		const unsigned int second_vertex_index = device_polygons[polygon_number].ratios[second_vertex_number].vertexNumber ;

		int x1 = vertexs_2d[first_vertex_index ].x;
		int y1 = vertexs_2d[first_vertex_index].y;
		const int x2 = vertexs_2d[second_vertex_index].x;
		const int y2 = vertexs_2d[second_vertex_index].y;

		const bool coordinats_are_correct = (x1 > 0 && x1 < display_width) && (x2 > 0 && x2 < display_width) && (y1 > 0 && y1 < display_height) && (y2 > 0 && y2 < display_height);

		if (coordinats_are_correct) {

			const int deltaX = abs(x2 - x1);
			const int deltaY = abs(y2 - y1);
			const int signX = x1 < x2 ? 1 : -1;
			const int signY = y1 < y2 ? 1 : -1;
			//
			int error = deltaX - deltaY;

			display_buffer[display_width * y2 + x2].rgb_reserved = 0;
			display_buffer[display_width * y2 + x2].rgb_red = 255;
			display_buffer[display_width * y2 + x2].rgb_green = 0;
			display_buffer[display_width * y2 + x2].rgb_blue = 0;

			while (x1 != x2 || y1 != y2)
			{

				display_buffer[display_width * y1 + x1].rgb_reserved = 0;
				display_buffer[display_width * y1 + x1].rgb_red = 255;
				display_buffer[display_width * y1 + x1].rgb_green = 0;
				display_buffer[display_width * y1 + x1].rgb_blue = 0;

				const int error2 = error * 2;
				//
				if (error2 > -deltaY)
				{
					error -= deltaY;
					x1 += signX;
				}

				if (error2 < deltaX)
				{
					error += deltaX;
					y1 += signY;
				}

			}
		}
	}

}
__global__ void  SetScreenColor(RgbPixel* const device_display_buffer, const RgbPixel rgb_pixel, const unsigned int number_of_pixels) {

	int thread_index = threadIdx.x + blockIdx.x * blockDim.x;

	if (thread_index < number_of_pixels) device_display_buffer[thread_index] = rgb_pixel;

}

void GraphicEngine::SetDisplayBufferColor(const RgbColor& rgb_color)
{
	const RgbPixel rgb_pixel = { rgb_color.rgb_blue, rgb_color.rgb_green, rgb_color.rgb_red, rgb_color.rgb_reserved };

	const unsigned int number_of_threads = 1024;

	const unsigned int number_of_blocks = (1920 * 1080 * sizeof(RgbPixel) + number_of_threads - 1) / number_of_threads;

	SetScreenColor <<< number_of_blocks, number_of_threads >>> (this->device_display_buffer_, rgb_pixel, 1920 * 1080);


}
void CPUCountingProjectVertexs(const Vertex3D* const vertexs_3d, Vertex2D* const vertexs_2d, const unsigned int number_of_vertexs, const Vertex3D cameraPosition, const Vector3D vectorX, const Vector3D vectorY, const Vector3D vectorZ, const float k) {

	

	for (size_t i = 0; i < number_of_vertexs; i++)
	{



		//Offset
		const float offset_x = vertexs_3d[i].x - cameraPosition.x;
		const float offset_y = vertexs_3d[i].y - cameraPosition.y;
		const float offset_z = vertexs_3d[i].z - cameraPosition.z;

		const float new_x = offset_x * vectorX.x + offset_y * vectorX.y + offset_z * vectorX.z;
		const float new_y = offset_x * vectorY.x + offset_y * vectorY.y + offset_z * vectorY.z;
		const float new_z = offset_x * vectorZ.x + offset_y * vectorZ.y + offset_z * vectorZ.z;

		vertexs_2d[i].x = (k * new_x / (new_z + k));
		vertexs_2d[i].y = (k * new_y / (new_z + k));
	}
	
}
void CPUCountingConvertInDisplayCoordinats(Vertex2D* const vertexs_2d, const unsigned int number_of_vertexs, const unsigned int display_width, const unsigned int display_height) {

	

	for (size_t i = 0; i < number_of_vertexs; i++)
	{

		vertexs_2d[i].x = vertexs_2d[i].x * (float) 1000 + display_width / 2;
		vertexs_2d[i].y = display_height / 2 - vertexs_2d[i].y * 1000;

	}
}
void CPUCountingDrawLines(const Vertex2D* const vertexs_2d, const Polygon3D* const device_polygons, const unsigned int number_of_polygons, RgbPixel* const display_buffer, const unsigned int display_width, const unsigned int display_height) {

	{
		const unsigned int number_of_lines = number_of_polygons * 3;

		for (size_t i = 0; i < number_of_lines; i++)
		{

			const unsigned int polygon_number = i / 3;
			const unsigned int vertex_number = i % 3;

			const unsigned int first_vertex_index = device_polygons[polygon_number].ratios[vertex_number].vertexNumber;
			const unsigned int second_vertex_number = ((vertex_number + 1) < 3) ? (vertex_number + 1) : 0;
			const unsigned int second_vertex_index = device_polygons[polygon_number].ratios[second_vertex_number].vertexNumber;


			int x1 = vertexs_2d[first_vertex_index].x;
			int y1 = vertexs_2d[first_vertex_index].y;
			int x2 = vertexs_2d[second_vertex_index].x;
			int y2 = vertexs_2d[second_vertex_index].y;

			const bool coordinats_are_correct = (x1 > 0 && x1 < display_width) && (x2 > 0 && x2 < display_width) && (y1 > 0 && y1 < display_height) && (y2 > 0 && y2 < display_height);

			if (coordinats_are_correct) {

				const int deltaX = abs(x2 - x1);
				const int deltaY = abs(y2 - y1);
				const int signX = x1 < x2 ? 1 : -1;
				const int signY = y1 < y2 ? 1 : -1;
				//
				int error = deltaX - deltaY;

				display_buffer[display_width * y2 + x2].rgb_reserved = 0;
				display_buffer[display_width * y2 + x2].rgb_red = 255;
				display_buffer[display_width * y2 + x2].rgb_green = 0;
				display_buffer[display_width * y2 + x2].rgb_blue = 0;

				/*display_buffer[display_width * y1 + x1].rgb_reserved = 0;
				display_buffer[display_width * y1 + x1].rgb_red = 255;
				display_buffer[display_width * y1 + x1].rgb_green = 0;
				display_buffer[display_width * y1 + x1].rgb_blue = 0;*/


				while (x1 != x2 || y1 != y2)
				{
					display_buffer[display_width * y1 + x1].rgb_reserved = 0;
					display_buffer[display_width * y1 + x1].rgb_red = 255;
					display_buffer[display_width * y1 + x1].rgb_green = 0;
					display_buffer[display_width * y1 + x1].rgb_blue = 0;

					const int error2 = error * 2;
					//
					if (error2 > -deltaY)
					{
						error -= deltaY;
						x1 += signX;
					}
					if (error2 < deltaX)
					{
						error += deltaX;
						y1 += signY;
					}
				}

			}
		}
	}
}
__global__ void draw(RgbPixel* display_buffer) {
	int thread_id = threadIdx.x + blockIdx.x * blockDim.x;

	display_buffer[thread_id].rgb_green = 255;

}

CameraInfo GraphicEngine::GetCameraInfo() {
	CameraInfo info;
	info.camera_pos = (*camera_->GetPosition());
	info.dis_proj_plane = camera_->GetDistanceToProjPlane();
	info.vector_x = camera_->GetVectorX();
	info.vector_y = camera_->GetVectorY();
	info.vector_z = camera_->GetDirection();
	return info;
}
void GraphicEngine::TestFunction()
{


}

void GraphicEngine::CreateMeshFrame() {

	const Vertex3D const  camera_position  = *(camera_->GetPosition());
	
	const Vector3D vector_x = camera_->GetVectorX();
	const Vector3D vector_y = camera_->GetVectorY();
	const Vector3D vector_z = camera_->GetDirection();

	const float distance_to_projection_plane = camera_->GetDistanceToProjPlane();

	const Vertex3D* const device_vertexs_3d = device_data_.deviceVertexs;
	Vertex2D* const device_vertexs_2d = device_vertexs_2d_;
	const Polygon3D* const device_polygons = device_data_.devicePolygons;

	
	RgbColor color;
	color.rgb_blue = 20;
	color.rgb_green = 255;
	color.rgb_red = 0;

	const unsigned int number_of_threads = 1024;
	unsigned int number_of_blocks = (data_info_.numberOfVertexs + number_of_threads - 1) / number_of_threads;

	SetDisplayBufferColor(color);

	ProjectVertexs <<<number_of_blocks, number_of_threads>>> (device_vertexs_3d, device_vertexs_2d, data_info_.numberOfVertexs, camera_position, vector_x, vector_y, vector_z, distance_to_projection_plane);

	ConvertInDisplayCoordinats <<<number_of_blocks, number_of_threads >>> (device_vertexs_2d, data_info_.numberOfVertexs, display_width_, display_height_);

	number_of_blocks = (data_info_.numberOfPolygons * 3 + number_of_threads - 1) / number_of_threads;

	DrawLines <<<number_of_blocks, number_of_threads >>> (device_vertexs_2d, device_polygons, data_info_.numberOfPolygons, device_display_buffer_, display_width_, display_height_);	

	//cudaMemcpy((void**)host_display_buffer_, device_display_buffer_, display_buffer_size_, cudaMemcpyDeviceToHost);
	//const Vertex3D* const host_vertexs_3d = data_info_.allVertexs;
	//cudaMemcpy(host_display_buffer_, device_display_buffer_, size_of_display_buffer_, cudaMemcpyDeviceToHost);
	//CPUCountingProjectVertexs(host_vertexs_3d, this->host_vertexs_2d_, this->data_info_.numberOfVertexs, camera_position, vector_x, vector_y, vector_z, distance_to_projection_plane);
	//CPUCountingConvertInDisplayCoordinats(host_vertexs_2d_, this->data_info_.numberOfVertexs, display_width_, display_height_);
	//Polygon3D* host_polygons = this->data_info_.allPolygons;
	//CPUCountingDrawLines(host_vertexs_2d_, host_polygons, this->data_info_.numberOfPolygons, this->host_display_buffer_, display_width_, display_height_);
	//cudaMemcpy(host_display_buffer_, device_display_buffer_, size_of_display_buffer_, cudaMemcpyDeviceToHost);

}

__global__ void DrawPolygons(z_element* z_buffer, Vertex2D* vertexs_2d, Polygon3D* polygons, Vertex3D* vertexs_3d) {

	int thread_index = threadIdx.x + blockDim.x * blockIdx.x;

	Polygon3D polygon = polygons[thread_index];

	Vertex2D f_vertex_2d = vertexs_2d[polygon.ratios[0].vertexNumber];
	float f_z = vertexs_3d[polygon.ratios[0].vertexNumber].z;

	Vertex2D s_vertex_2d = vertexs_2d[polygon.ratios[1].vertexNumber];
	float s_z = vertexs_3d[polygon.ratios[1].vertexNumber].z;

	Vertex2D t_vertex_2d = vertexs_2d[polygon.ratios[2].vertexNumber];
	float t_z = vertexs_3d[polygon.ratios[2].vertexNumber].z;



}

void GraphicEngine::CreateFlatFrame() {

	const Vertex3D const  camera_position = *(camera_->GetPosition());

	const Vector3D vector_x = camera_->GetVectorX();
	const Vector3D vector_y = camera_->GetVectorY();
	const Vector3D vector_z = camera_->GetDirection();

	const float distance_to_projection_plane = camera_->GetDistanceToProjPlane();

	const Vertex3D* const device_vertexs_3d = device_data_.deviceVertexs;
	Vertex2D* const device_vertexs_2d = device_vertexs_2d_;
	const Polygon3D* const device_polygons = device_data_.devicePolygons;

	RgbColor color;
	color.rgb_blue = 20;
	color.rgb_green = 255;
	color.rgb_red = 0;

	const unsigned int number_of_threads = 1024;
	unsigned int number_of_blocks = (data_info_.numberOfVertexs + number_of_threads - 1) / number_of_threads;

	SetDisplayBufferColor(color);

	ProjectVertexs <<<number_of_blocks, number_of_threads >>> (device_vertexs_3d, device_vertexs_2d, data_info_.numberOfVertexs, camera_position, vector_x, vector_y, vector_z, distance_to_projection_plane);

	ConvertInDisplayCoordinats <<<number_of_blocks, number_of_threads >>> (device_vertexs_2d, data_info_.numberOfVertexs, display_width_, display_height_);

	number_of_blocks = (data_info_.numberOfPolygons * 3 + number_of_threads - 1) / number_of_threads;

	DrawPolygons <<<number_of_blocks, number_of_threads >>> ();

}



