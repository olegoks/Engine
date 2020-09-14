
#include "GraphicEngineCUDA.cuh"
#include "__graphic_engine_/GraphicEngine.h"

#include<iostream>
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
__global__ void DrawLines(const Vertex2D* const vertexs_2d, const Polygon3D* const device_polygons, const Normal3D* normals, const unsigned int number_of_polygons, RgbPixel* const display_buffer, const unsigned int display_width, const unsigned int display_height, Vector3D camera_dir) {

	int threadIndex = threadIdx.x + blockIdx.x * blockDim.x;

	if (threadIndex < number_of_polygons * 3) {

		const unsigned int polygon_number = threadIndex / 3;
		const unsigned int vertex_number = threadIndex % 3;
		
		Normal3D normal = normals[ device_polygons[polygon_number].ratios[vertex_number].normalNumber];
		float scalar = camera_dir.x * normal.x + camera_dir.y * normal.y + camera_dir.z * normal.z;
		//if (scalar <= 0.0f) 
		{

			const unsigned int first_vertex_index = device_polygons[polygon_number].ratios[vertex_number].vertexNumber;
			const unsigned int second_vertex_number = ((vertex_number + 1) < 3) ? (vertex_number + 1) : 0;
			const unsigned int second_vertex_index = device_polygons[polygon_number].ratios[second_vertex_number].vertexNumber;

			int x1 = vertexs_2d[first_vertex_index].x;
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
	Normal3D* device_normals = device_data_.deviceNormals;
	
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

	DrawLines <<<number_of_blocks, number_of_threads >>> (device_vertexs_2d, device_polygons, device_normals, data_info_.numberOfPolygons, device_display_buffer_, display_width_, display_height_,vector_z);	

	//cudaMemcpy((void**)host_display_buffer_, device_display_buffer_, display_buffer_size_, cudaMemcpyDeviceToHost);
	//const Vertex3D* const host_vertexs_3d = data_info_.allVertexs;
	//cudaMemcpy(host_display_buffer_, device_display_buffer_, size_of_display_buffer_, cudaMemcpyDeviceToHost);
	//CPUCountingProjectVertexs(host_vertexs_3d, this->host_vertexs_2d_, this->data_info_.numberOfVertexs, camera_position, vector_x, vector_y, vector_z, distance_to_projection_plane);
	//CPUCountingConvertInDisplayCoordinats(host_vertexs_2d_, this->data_info_.numberOfVertexs, display_width_, display_height_);
	//Polygon3D* host_polygons = this->data_info_.allPolygons;
	//CPUCountingDrawLines(host_vertexs_2d_, host_polygons, this->data_info_.numberOfPolygons, this->host_display_buffer_, display_width_, display_height_);
	//cudaMemcpy(host_display_buffer_, device_display_buffer_, size_of_display_buffer_, cudaMemcpyDeviceToHost);

}


struct Proj_vertex {

	float x;
	float y;
	float _z;

};
inline __device__ void swap(Proj_vertex& a, Proj_vertex& b) {

	Proj_vertex temporary = b;
	b = a;
	a = temporary;

}
inline __device__ bool InPositiveHalfPlane(const Vertex2D& pixel, const Vertex2D& triangle_vertex, Vector2D& _normal) {

	Vector2D pixel_vector;
	pixel_vector.x = pixel.x - triangle_vertex.x;
	pixel_vector.y = pixel.y - triangle_vertex.y;


	Vector2D normal = _normal;
	float length_n = sqrt(normal.x * normal.x + normal.y * normal.y);
	normal.x /= length_n;
	normal.y /= length_n;

	float length_p = sqrt(pixel_vector.x * pixel_vector.x +  pixel_vector.y * pixel_vector.y);
	pixel_vector.x /= length_p;
	pixel_vector.y /=length_p;
	float scalar = pixel_vector.x * normal.x + pixel_vector.y * normal.y;

	if (scalar >= 0.0f) return true;
	else 
		return false;

}

struct InfoForPainting {
	Vertex3D* d_vertexs;
	Polygon3D* d_polygons;
	unsigned int number_of_polygons;
	RgbPixel* d_rgb;
	unsigned int number_of_colors;

};

__global__ void DrawPolygons(z_mutex* z_buffer,RgbPixel* display_buffer, Vertex2D* vertexs_2d, InfoForPainting info) {

	int thread_index = threadIdx.x + blockDim.x * blockIdx.x;

	if (thread_index < info.number_of_polygons) {
		//printf("%d", thread_index);
		Polygon3D polygon = info.d_polygons[thread_index];

		Proj_vertex proj_vertexs[3];
		for (int i = 0; i < 3; i++)
		{

			proj_vertexs[i].x = vertexs_2d[polygon.ratios[i].vertexNumber].x;
			//printf("x: %2f", proj_vertexs[i].x);
			proj_vertexs[i].y = vertexs_2d[polygon.ratios[i].vertexNumber].y;
			//printf("y: %2f \n", proj_vertexs[i].y);
			proj_vertexs[i]._z = 1.0f / info.d_vertexs[polygon.ratios[i].vertexNumber].z;
			
		}

		int min_x = 10000, min_y = 10000, max_x = -1, max_y = -1;

		for (int i = 0; i < 3; i++)
		{
			if (proj_vertexs[i].x < min_x) min_x = floor(proj_vertexs[i].x);
			if (proj_vertexs[i].y < min_y) min_y = floor(proj_vertexs[i].y);
			if (proj_vertexs[i].x > max_x) max_x = ceil(proj_vertexs[i].x);
			if (proj_vertexs[i].y > max_y) max_y = ceil(proj_vertexs[i].y);
		}
		
		RgbPixel polygon_color = info.d_rgb[thread_index /2];
		/*polygon_color.rgb_blue = 0;
		polygon_color.rgb_green = 0;
		polygon_color.rgb_red = 255;*/
		
		//Sorting vertexs by y 2d coordinat

		//Clockwise direction
		Vertex2D AToB;
		AToB.x = proj_vertexs[1].x - proj_vertexs[0].x;
		AToB.y = proj_vertexs[1].y - proj_vertexs[0].y;
		Vertex2D BToC;
		BToC.x = proj_vertexs[2].x - proj_vertexs[1].x;
		BToC.y = proj_vertexs[2].y - proj_vertexs[1].y;

		float crossz = AToB.x * BToC.y - AToB.y * BToC.x;
		if (crossz > 0.0f)
		{
			Proj_vertex temporary = proj_vertexs[2];
			proj_vertexs[2] = proj_vertexs[1];
			proj_vertexs[1] = temporary;
		}
	
		//if (proj_vertexs[0].x > proj_vertexs[1].x) swap(proj_vertexs[0], proj_vertexs[1]);
		//if (proj_vertexs[0].x > proj_vertexs[2].x) swap(proj_vertexs[0], proj_vertexs[2]);
		//if (proj_vertexs[1].x > proj_vertexs[2].x) swap(proj_vertexs[1], proj_vertexs[2]);
		
		float length;
		Vector2D bot_mid = { proj_vertexs[1].y - proj_vertexs[0].y, -proj_vertexs[1].x + proj_vertexs[0].x };
		length = sqrt(bot_mid.x * bot_mid.x + bot_mid.y * bot_mid.y);
		bot_mid.x /= length;
		bot_mid.y /= length;

		Vector2D mid_top = {proj_vertexs[2].y - proj_vertexs[1].y,  -proj_vertexs[2].x + proj_vertexs[1].x };
		length = sqrt(mid_top.x * mid_top.x + mid_top.y * mid_top.y);
		mid_top.x /= length;
		mid_top.y /= length;

		Vector2D top_bot = { proj_vertexs[0].y - proj_vertexs[2].y, -proj_vertexs[0].x + proj_vertexs[2].x, };
		length = sqrt(top_bot.x * top_bot.x + top_bot.y * top_bot.y);
		top_bot.x /= length;
		top_bot.y /= length;

		const Vertex2D bot = { proj_vertexs[0].x, proj_vertexs[0].y };
		const Vertex2D mid = { proj_vertexs[1].x, proj_vertexs[1].y };
		const Vertex2D top = { proj_vertexs[2].x, proj_vertexs[2].y };

		//printf("bot: %2f, %2f, mid: %2f %2f, top: %2f %2f \n", bot.x, bot.y,mid.x, mid.y,top.x, top.y);
		//printf("bot_mid: %2f %2f, mid_top: %2f %2f, top_bot: %2f %2f \n", bot_mid.x, bot_mid.y, mid_top.x, mid_top.y, top_bot.x, top_bot.y);
		for (int y = min_y; y < max_y; y++)
			for (int x = min_x; x < max_x; x++)
			{
				Vertex2D pixel;
				pixel.x = ((float)x + 0.5f);
				pixel.y = ((float)y + 0.5f);
				
				bool PixelInTriangle = InPositiveHalfPlane(pixel, bot, bot_mid) && InPositiveHalfPlane(pixel, mid, mid_top) && InPositiveHalfPlane(pixel, top, top_bot);

				if (PixelInTriangle) {
						
								
					//Vertex3D bot, mid, top;

					while ( (z_buffer + 1920 * y + x)->mutex == true ) continue;
					(z_buffer + 1920 * y + x)->mutex = true;

					//if (1.0f / (z_buffer + 1920 * y + x)->z > _z) {
					* (display_buffer + 1920 * y + x) = polygon_color;
					//}

					(z_buffer + 1920 * y + x)->mutex = false;
				}
	
				
			}
	}
	
}

void GraphicEngine::CreateFlatFrame() {

	const Vertex3D const  camera_position = *(camera_->GetPosition());

	const Vector3D vector_x = camera_->GetVectorX();
	const Vector3D vector_y = camera_->GetVectorY();
	const Vector3D vector_z = camera_->GetDirection();

	const float distance_to_projection_plane = camera_->GetDistanceToProjPlane();

	Vertex3D* const device_vertexs_3d = device_data_.deviceVertexs;
	Vertex2D* const device_vertexs_2d = device_vertexs_2d_;
	Polygon3D* const device_polygons = device_data_.devicePolygons;

	RgbColor color;
	color.rgb_blue = 20;
	color.rgb_green = 255;
	color.rgb_red = 0;

	cudaMemset(z_mutex_, 0, display_width_ * display_height_ * sizeof(z_mutex));

	const unsigned int number_of_threads = 1024;
	unsigned int number_of_blocks = (data_info_.numberOfVertexs + number_of_threads - 1) / number_of_threads;

	SetDisplayBufferColor(color);

	ProjectVertexs <<<number_of_blocks, number_of_threads >>> (device_vertexs_3d, device_vertexs_2d, data_info_.numberOfVertexs, camera_position, vector_x, vector_y, vector_z, distance_to_projection_plane);

	ConvertInDisplayCoordinats <<<number_of_blocks, number_of_threads >>> (device_vertexs_2d, data_info_.numberOfVertexs, display_width_, display_height_);

	number_of_blocks = (data_info_.numberOfPolygons * threads_per_triangle_ + number_of_threads - 1) / number_of_threads;


	InfoForPainting info = { device_vertexs_3d, device_polygons, data_info_.numberOfPolygons, (RgbPixel*)device_data_.device_colors, data_info_.numberOfRgbColors };
	printf("%d", data_info_.numberOfRgbColors);
	DrawPolygons <<< number_of_blocks, number_of_threads >> > (z_mutex_, device_display_buffer_, device_vertexs_2d, info);

	//system("pause");

}



