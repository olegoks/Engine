#include "Camera.h"

static const float pi = 3.14159265;

Camera::Camera() noexcept :

	size_Of_projection_plane_(1920, 1080),
	position_Of_observer_(0.0f, 10.0f, -11.0f),
	vector_x_(1.0f, 0.0f, 0.0f),
	vector_y_(0.0f, 0.8f, 0.4f),
	vector_z_(0.0f, -0.4f, 0.8f),
	distance_to_proj_plane_(0.5f)
{
	float length_y = sqrt(vector_y_.x * vector_y_.x + vector_y_.y* vector_y_.y + vector_y_.z * vector_y_.z);
	vector_y_.x /= length_y;
	vector_y_.y /= length_y;
	vector_y_.z /= length_y;

	float length_z = sqrt(vector_z_.x * vector_z_.x + vector_z_.y * vector_z_.y + vector_z_.z * vector_z_.z);
	vector_z_.x /= length_z;
	vector_z_.y /= length_z;
	vector_z_.z /= length_z;

};

void Camera::RotateCamera(float alpha_deegree, const Vector3D &around_vector, const Vertex3D& around_point) {

	float alpha_rad = alpha_deegree * pi / 180.0f;

	const float sin_alpha = sin(alpha_rad);
	const float cos_alpha = cos(alpha_rad);

	float VX = around_vector.x;
	float VY = around_vector.y;
	float VZ = around_vector.z;

	float XP = this->position_Of_observer_.x - around_point.x;
	float YP = this->position_Of_observer_.y - around_point.y;
	float ZP = this->position_Of_observer_.z - around_point.z;

	this->position_Of_observer_.x = (cos_alpha + (1 - cos_alpha) * VX * VX) * XP + ((1 - cos_alpha) * VX * VY - sin_alpha * VZ) * YP + ((1 - cos_alpha) * VX * VZ + sin_alpha * VY) * ZP + around_point.x;
	this->position_Of_observer_.y = ((1 - cos_alpha) * VY * VX + sin_alpha * VZ) * XP + (cos_alpha + (1 - cos_alpha) * VY * VY) * YP + ((1 - cos_alpha) * VY * VZ - sin_alpha * VX) * ZP + around_point.y;
	this->position_Of_observer_.z = ((1 - cos_alpha) * VZ * VX - sin_alpha * VY) * XP + ((1 - cos_alpha) * VZ * VY + sin_alpha * VX) * YP + (cos_alpha + (1 - cos_alpha) * VZ * VZ) * ZP + around_point.z;
	
	XP = this->vector_x_.x - around_point.x;
	YP = this->vector_x_.y - around_point.y;
	ZP = this->vector_x_.z - around_point.z;

	this->vector_x_.x = (cos_alpha + (1 - cos_alpha) * VX * VX) * XP + ((1 - cos_alpha) * VX * VY - sin_alpha * VZ) * YP + ((1 - cos_alpha) * VX * VZ + sin_alpha * VY) * ZP + around_point.x;
	this->vector_x_.y = ((1 - cos_alpha) * VY * VX + sin_alpha * VZ) * XP + (cos_alpha + (1 - cos_alpha) * VY * VY) * YP + ((1 - cos_alpha) * VY * VZ - sin_alpha * VX) * ZP + around_point.y;
	this->vector_x_.z = ((1 - cos_alpha) * VZ * VX - sin_alpha * VY) * XP + ((1 - cos_alpha) * VZ * VY + sin_alpha * VX) * YP + (cos_alpha + (1 - cos_alpha) * VZ * VZ) * ZP + around_point.z;

	XP = this->vector_y_.x - around_point.x;
	YP = this->vector_y_.y - around_point.y;
	ZP = this->vector_y_.z - around_point.z;

	this->vector_y_.x = (cos_alpha + (1 - cos_alpha) * VX * VX) * XP + ((1 - cos_alpha) * VX * VY - sin_alpha * VZ) * YP + ((1 - cos_alpha) * VX * VZ + sin_alpha * VY) * ZP + around_point.x;
	this->vector_y_.y = ((1 - cos_alpha) * VY * VX + sin_alpha * VZ) * XP + (cos_alpha + (1 - cos_alpha) * VY * VY) * YP + ((1 - cos_alpha) * VY * VZ - sin_alpha * VX) * ZP + around_point.y;
	this->vector_y_.z = ((1 - cos_alpha) * VZ * VX - sin_alpha * VY) * XP + ((1 - cos_alpha) * VZ * VY + sin_alpha * VX) * YP + (cos_alpha + (1 - cos_alpha) * VZ * VZ) * ZP + around_point.z;

	XP = this->vector_z_.x - around_point.x;
	YP = this->vector_z_.y - around_point.y;
	ZP = this->vector_z_.z - around_point.z;

	this->vector_z_.x = (cos_alpha + (1 - cos_alpha) * VX * VX) * XP + ((1 - cos_alpha) * VX * VY - sin_alpha * VZ) * YP + ((1 - cos_alpha) * VX * VZ + sin_alpha * VY) * ZP + around_point.x;
	this->vector_z_.y = ((1 - cos_alpha) * VY * VX + sin_alpha * VZ) * XP + (cos_alpha + (1 - cos_alpha) * VY * VY) * YP + ((1 - cos_alpha) * VY * VZ - sin_alpha * VX) * ZP + around_point.y;
	this->vector_z_.z = ((1 - cos_alpha) * VZ * VX - sin_alpha * VY) * XP + ((1 - cos_alpha) * VZ * VY + sin_alpha * VX) * YP + (cos_alpha + (1 - cos_alpha) * VZ * VZ) * ZP + around_point.z;
}

void Camera::Translate(const Vertex3D &delta_vertex) {

	position_Of_observer_.x += delta_vertex.x;
	position_Of_observer_.y += delta_vertex.y;
	position_Of_observer_.z += delta_vertex.z;

}

void Camera::ChangeDistanceToProgPlane(float offset_distance){

	this->distance_to_proj_plane_ += offset_distance;

}

void Camera::SetVectorZ(float x, float y, float z) {

	const float vector_length = sqrt( x * x + y * y + z * z );

	position_Of_observer_.x = x / vector_length;
	position_Of_observer_.y = y / vector_length;
	position_Of_observer_.z = z / vector_length;

}


