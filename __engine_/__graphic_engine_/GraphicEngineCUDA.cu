
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

		//Central projection
		float offset_x = vertexs_3d[threadIndex].x - cameraPosition.x;
		float offset_y = vertexs_3d[threadIndex].y - cameraPosition.y;
		float offset_z = vertexs_3d[threadIndex].z - cameraPosition.z;

		float new_x = offset_x * vectorX.x + offset_y * vectorX.y + offset_z * vectorX.z;
		float new_y = offset_x * vectorY.x + offset_y * vectorY.y + offset_z * vectorY.z;
		float new_z = offset_x * vectorZ.x + offset_y * vectorZ.y + offset_z * vectorZ.z;
		
		vertexs_2d[threadIndex].x = ( k * new_x / (new_z + k) ) ;
		vertexs_2d[threadIndex].y = ( k * new_y / (new_z + k) ) ;
		//Ortogonal
		//vertexs_2d[threadIndex].x = vertexs_3d[threadIndex].x;
		//vertexs_2d[threadIndex].y = vertexs_3d[threadIndex].y;

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

	if (scalar > 0.0f) return true;
	else 
		return false;

}

struct InfoForPainting {
	Vertex3D* d_vertexs;
	Polygon3D* d_polygons;
	unsigned int number_of_polygons;
	//RgbPixel* d_rgb;
	//unsigned int number_of_colors;
	unsigned int threads_per_triangle;
	unsigned int screen_width;
	unsigned int screen_height;
	Vertex3D camera_position;
	Vector3D vector_x;
	Vector3D vector_y;

};


struct Device_vertex3D{
	float x;
	float y;
	float z;
};

struct InEl {

	int x;
	int y;
	float w;

};

__device__ float Dist_plane_vertex(const  Device_vertex3D& plane_v_1, const  Device_vertex3D& plane_v_2, const  Device_vertex3D& plane_v_3, const  Device_vertex3D& vertex) {

	float A, B, C, D;
	A = plane_v_1.y*(plane_v_2.z - plane_v_3.z) + plane_v_2.y*(plane_v_3.z - plane_v_1.z) + plane_v_3.y*(plane_v_1.z - plane_v_2.z);
	B = plane_v_1.z*(plane_v_2.x - plane_v_3.x) + plane_v_2.z*(plane_v_3.x - plane_v_1.x) + plane_v_3.z*(plane_v_1.x - plane_v_2.x);
	C = plane_v_1.x*(plane_v_2.y - plane_v_3.y) + plane_v_2.x*(plane_v_3.y - plane_v_1.y) + plane_v_3.x*(plane_v_1.y - plane_v_2.y);
	D = plane_v_1.x*(plane_v_2.y* plane_v_3.z - plane_v_3.y* plane_v_2.z) + plane_v_2.x*(plane_v_3.y* plane_v_1.z - plane_v_1.y* plane_v_3.z) + plane_v_3.x*(plane_v_1.y* plane_v_2.z - plane_v_2.y* plane_v_1.z);
	D = -D;
	float numerator = (A * vertex.x + B * vertex.y + C * vertex.y + D);
	if (numerator < 0)numerator = -numerator;
	return numerator / sqrt(A*A + B*B + C*C);

}
__device__ float Interpolate(float y1, float I1, float y2, float I2, float ya) {

	
	float Ip = I1 * ((ya - y2) / (y1 - y2)) + I2 * ((y1 - ya) / (y1 - y2));
	
	return Ip;

}

__device__ float BiInterpolate(InEl el_1, InEl el_2, InEl el_3, int x, int y) {

	//float fxy1 = 

}

__global__ void DrawPolygons(int* mutex_buffer, float* w_buffer, RgbPixel* display_buffer, Vertex2D* vertexs_2d, InfoForPainting info) {

	int thread_index = threadIdx.x + blockDim.x * blockIdx.x;
	
	if (thread_index < info.number_of_polygons * info.threads_per_triangle) {
		
		const unsigned int thread_index_in_triangle = (thread_index % info.threads_per_triangle);

		Polygon3D polygon = info.d_polygons[thread_index / info.threads_per_triangle];

		Proj_vertex proj_vertexs[3];

		for (int i = 0; i < 3; i++)
		{

			proj_vertexs[i].x = vertexs_2d[polygon.ratios[i].vertexNumber].x;
			//printf("x: %2f", proj_vertexs[i].x);
			proj_vertexs[i].y = vertexs_2d[polygon.ratios[i].vertexNumber].y;
			//printf("y: %2f \n", proj_vertexs[i].y);

			//printf("%f %f %f \n %f %f %f \n", info.d_vertexs[polygon.ratios[i].vertexNumber].x, info.d_vertexs[polygon.ratios[i].vertexNumber].y, info.d_vertexs[polygon.ratios[i].vertexNumber].z, info.camera_position.x, info.camera_position.y, info.camera_position.z);

			/*float x_2 = info.d_vertexs[polygon.ratios[i].vertexNumber].x - info.camera_position.x;
			float y_2 = info.d_vertexs[polygon.ratios[i].vertexNumber].y - info.camera_position.y;
			float z_2 = info.d_vertexs[polygon.ratios[i].vertexNumber].z - info.camera_position.z;
			float distance_to_camera = sqrt(x_2 * x_2 + y_2 * y_2 +z_2 * z_2 );*/
			//printf("%f \n", distance_to_camera);

			Device_vertex3D plane_v_1, plane_v_2, plane_v_3, vertex;

				plane_v_1.x = info.camera_position.x;
				plane_v_1.y = info.camera_position.y;
				plane_v_1.z = info.camera_position.z;

				plane_v_2.x = info.vector_x.x;
				plane_v_2.y = info.vector_x.y;
				plane_v_2.z = info.vector_x.z;

				plane_v_3.x = info.vector_y.x;
				plane_v_3.y = info.vector_y.y;
				plane_v_3.z = info.vector_y.z;

				vertex.x = info.d_vertexs[polygon.ratios[i].vertexNumber].x;
				vertex.y = info.d_vertexs[polygon.ratios[i].vertexNumber].y;
				vertex.z = info.d_vertexs[polygon.ratios[i].vertexNumber].z;
				
				float distance_to_camera = Dist_plane_vertex(plane_v_1, plane_v_2, plane_v_3, vertex);
				proj_vertexs[i]._z = 1.0f / distance_to_camera;

			}


			RgbPixel polygon_color = polygon.color;

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

			//Normalize 2d vectors
			float length;

			Vector2D bot_mid = { proj_vertexs[1].y - proj_vertexs[0].y, -proj_vertexs[1].x + proj_vertexs[0].x };
			length = sqrt(bot_mid.x * bot_mid.x + bot_mid.y * bot_mid.y);
			bot_mid.x /= length;
			bot_mid.y /= length;

			Vector2D mid_top = { proj_vertexs[2].y - proj_vertexs[1].y,  -proj_vertexs[2].x + proj_vertexs[1].x };
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

		int min_x = 10000, min_y = 10000, max_x = -1, max_y = -1;

		for (int i = 0; i < 3; i++)
		{
			if (proj_vertexs[i].x < min_x) min_x = floor(proj_vertexs[i].x);
			if (proj_vertexs[i].y < min_y) min_y = floor(proj_vertexs[i].y);
			if (proj_vertexs[i].x > max_x) max_x = ceil(proj_vertexs[i].x);
			if (proj_vertexs[i].y > max_y) max_y = ceil(proj_vertexs[i].y);
		}

		unsigned int delta_y = max_y - min_y;
		unsigned int delta_x = max_x - min_x;
		unsigned int index = thread_index_in_triangle;

		for (int i = 0; i < (delta_y * delta_x) / info.threads_per_triangle; i++)
		{
			unsigned int x = (index % delta_x) + min_x;
			unsigned int y = (index / delta_x) + min_y;

			Vertex2D pixel;
			pixel.x = ((float)x + 0.5f);
			pixel.y = ((float)y + 0.5f);

			bool PixelInTriangle = InPositiveHalfPlane(pixel, bot, bot_mid) && InPositiveHalfPlane(pixel, mid, mid_top) && InPositiveHalfPlane(pixel, top, top_bot);

			if (PixelInTriangle) {
				
				Proj_vertex v[3];

				for (int i = 0; i < 3; i++) v[i] = proj_vertexs[i];

				if (v[0].y < v[1].y) swap(v[0], v[1]);
				if (v[1].y < v[2].y) swap(v[1], v[2]); 
				if (v[0].y < v[1].y) swap(v[0], v[1]);
			

				float I1 = v[0]._z, I2 = v[1]._z, I3 = v[2]._z;

				float X1 = v[0].x, X2 = v[1].x, X3 = v[2].x;


				float Xa, Xb;
				float Ia, Ib, Ip;
				
				if (pixel.y > v[1].y) {

					Ia = Interpolate(v[0].y, I1, v[1].y, I2, pixel.y);
					Xa = Interpolate(v[0].y, v[0].x, v[1].y, v[1].x, pixel.y);

				}
				else {

					Ia = Interpolate(v[2].y, I3, v[1].y, I2, pixel.y);
					Xa = Interpolate(v[1].y, v[1].x, v[2].y, v[2].x, pixel.y);

				}
				
				Ib = Interpolate(v[0].y, I1, v[2].y, I3, pixel.y);
				Xb = Interpolate(v[0].y, v[0].x, v[2].y, v[2].x, pixel.y);
				
				Ip = Interpolate(Xa, Ia, Xb, Ib, pixel.x);


				bool is_set = 0;


				do
				{	
					
					is_set = atomicCAS((mutex_buffer + 1920 * y + x), 0, 1);
					
					if (is_set)
					{

						if (Ip > *(w_buffer + y * 1920 + x)) {

							RgbPixel grey;
							grey.rgb_blue = (int)100.0f * Ip;
							grey.rgb_green = (int)100.0f * Ip;
							grey.rgb_red = (int)100.0f * Ip;
							*(display_buffer + y * 1920 + x) = grey;


						}
						
					}

					if (is_set)
					{

						//atomicExch(mutex_buffer + 1920 * y + x, 0);

						*(mutex_buffer + 1920 * y + x) = 0;
					}

				} while (!is_set);
				
			}

		index += info.threads_per_triangle;
		}
		
	}

	//__syncthreads();
}
__global__ void MemSet(float* memory, UINT el_size, UINT el_num, float value ) {

	int thread_index = threadIdx.x + blockDim.x * blockIdx.x;

	if (thread_index < el_num) {
		*((float*)memory + el_size * thread_index) = value;
		
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
	color.rgb_blue = 0;
	color.rgb_green = 0;
	color.rgb_red = 0;

	cudaMemset(z_mutex_, 0.0f, display_width_ * display_height_ * sizeof(int));
	cudaMemset(w_buffer_, 0.0, display_width_ * display_height_ * sizeof(float));

	const unsigned int number_of_threads = 1024;
	//unsigned int number_of_blocks = (display_width_ * display_height_ + number_of_threads - 1) / number_of_threads;;
	
	//MemSet <<<number_of_blocks, number_of_threads >>> (w_buffer_, sizeof(float), display_width_ * display_height_, 0.0f);
	unsigned int number_of_blocks = (data_info_.numberOfVertexs + number_of_threads - 1) / number_of_threads;

	SetDisplayBufferColor(color);

	ProjectVertexs <<< number_of_blocks, number_of_threads >>> (device_vertexs_3d, device_vertexs_2d, data_info_.numberOfVertexs, camera_position, vector_x, vector_y, vector_z, distance_to_projection_plane);

	ConvertInDisplayCoordinats <<< number_of_blocks, number_of_threads >>> (device_vertexs_2d, data_info_.numberOfVertexs, display_width_, display_height_);

	number_of_blocks = (data_info_.numberOfPolygons * threads_per_triangle_ + number_of_threads - 1) / number_of_threads;

	InfoForPainting info = { device_vertexs_3d, device_polygons, data_info_.numberOfPolygons, threads_per_triangle_, display_width_, display_height_, camera_position, vector_x, vector_y };
	//test <<<1, 1 >>> (z_mutex_);
	DrawPolygons <<< number_of_blocks, number_of_threads >>> (z_mutex_, w_buffer_, device_display_buffer_, device_vertexs_2d, info);

	//system("pause");

}



