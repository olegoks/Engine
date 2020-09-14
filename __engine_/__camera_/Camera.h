#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "D:\C++\ProjectEngine\__engine_\Structures.h"
#include <cmath>

struct SizeOfProjectionPlane {

	SizeOfProjectionPlane() noexcept:length(1920), width(1080){}
	SizeOfProjectionPlane(unsigned int length_, unsigned int width_) noexcept: length(length_), width(width_) {};
	unsigned int length;
	unsigned int width;

};

class Camera {

private:

	Vector3D vector_x_;
	Vector3D vector_y_;
	Vector3D vector_z_;
	Vertex3D position_Of_observer_;
	float distance_to_proj_plane_;
	SizeOfProjectionPlane size_Of_projection_plane_;


public:

	Camera() noexcept;
	void SetVectorZ(float x, float y, float z);
	void ChangeDistanceToProgPlane(float offset_distance);
	void Translate(const Vertex3D &delta_vertex);
	void RotateCamera(const float alpha_deegree, const Vector3D& around_vector, const Vertex3D& around_point );
	inline const Vertex3D* GetPosition()const noexcept { return &position_Of_observer_; }
	inline float GetDistanceToProjPlane() const noexcept { return this->distance_to_proj_plane_; };
	inline Vector3D GetDirection()const noexcept { return this->vector_z_;}
	inline Vector3D GetVectorX()const noexcept { return this->vector_x_;}
	inline Vector3D GetVectorY()const noexcept { return this->vector_y_;}

	inline void SetPosition(const float x, const float y, const float z)noexcept {
		this->position_Of_observer_.x = x;
		this->position_Of_observer_.y = y;
		this->position_Of_observer_.z = z;
	}

	inline void SetDirection(const float x, const float y, const float z)noexcept {
		this->vector_z_.x = x;
		this->vector_z_.y = y;
		this->vector_z_.z = z;
	}

	inline void SetPlaneSize(const unsigned int length, const unsigned int width)noexcept {
		this->size_Of_projection_plane_.length = length;
		this->size_Of_projection_plane_.width = width;
	}

	inline void SetDistanceToPlane(const float distance)noexcept { this->distance_to_proj_plane_ = distance; }
};

#endif