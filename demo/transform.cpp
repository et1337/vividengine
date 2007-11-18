// Vivid Engine v0.01
// Copyright (C) 2006 Evan Todd

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License (included in README.txt) for more details.

// You can contact the author at evanofsky@sbcglobal.net.

#include "transform.h"

Transform::Transform() {
	position.x = 0.0f; position.y = 0.0f; position.z = 0.0f;
	rotation.x = 0.0f; rotation.y = 0.0f; rotation.z = 0.0f;
	scale.x = 1.0f; scale.y = 1.0f; scale.z = 1.0f;
}
Transform::Transform(float x, float y, float z) {
	position.x = x; position.y = y; position.z = z;
	rotation.x = 0.0f; rotation.y = 0.0f; rotation.z = 0.0f;
	scale.x = 1.0f; scale.y = 1.0f; scale.z = 1.0f;
}
Transform::Transform(float x, float y, float z, float rx, float ry, float rz) {
	position.x = x; position.y = y; position.z = z;
	rotation.x = rx; rotation.y = ry; rotation.z = rz;
	scale.x = 1.0f; scale.y = 1.0f; scale.z = 1.0f;
}
Transform::Transform(const Transform& transform) {
	position = transform.position;
	rotation = transform.rotation;
	scale = transform.scale;
}
Transform::~Transform() {}
// Gets the position of the transform
D3DXVECTOR3 Transform::GetPosition() {
	return position;
}
// Gets the rotation of the transform
D3DXVECTOR3 Transform::GetRotation() {
	return rotation;
}
// Gets the scaling of the transform
D3DXVECTOR3 Transform::GetScale() {
	return scale;
}
// Sets the position of the transform
void Transform::SetPosition(D3DXVECTOR3* nPosition) {
	position = *nPosition;
}
// Sets the rotation of the transform
void Transform::SetRotation(D3DXVECTOR3* nRotation) {
	rotation = *nRotation;
}
// Sets the scaling of the transform
void Transform::SetScale(D3DXVECTOR3* nScale) {
	scale = *nScale;
}
// Sets the position of the transform
void Transform::SetPosition(float x, float y, float z) {
	position.x = x; position.y = y; position.z = z;
}
// Sets the rotation of the transform
void Transform::SetRotation(float rx, float ry, float rz) {
	rotation.x = rx; rotation.y = ry; rotation.z = rz;
}
// Sets the scaling of the transform
void Transform::SetScale(float sx, float sy, float sz) {
	scale.x = sx; scale.y = sy; scale.z = sz;
}
// Adds the specified vector to the position
void Transform::AddPosition(D3DXVECTOR3* nPosition) {
	position += *nPosition;
}
// Adds the specified vector to the position relative to the transform's rotation
void Transform::AddPositionRelative(D3DXVECTOR3* nPosition) {
	position += GetRightVector() * nPosition->x; position += GetUpVector() * nPosition->y; position += GetLookVector() * nPosition->z;
}
// Adds the specified vector to the rotation
void Transform::AddRotation(D3DXVECTOR3* nRotation) {
	rotation += *nRotation;
}
// Adds the specified vector to the scaling
void Transform::AddScale(D3DXVECTOR3* nScale) {
	scale += *nScale;
}
// Adds the specified values to the position
void Transform::AddPosition(float x, float y, float z) {
	position.x += x; position.y += y; position.z += z;
}
// Adds the specified values to the position relative to the transform's rotation
void Transform::AddPositionRelative(float x, float y, float z) {
	position += GetRightVector() * x; position += GetUpVector() * y; position += GetLookVector() * z;
}
// Adds the specified values to the rotation
void Transform::AddRotation(float rx, float ry, float rz) {
	rotation.x += rx; rotation.y += ry; rotation.z += rz;
}
// Adds the specified values to the scaling
void Transform::AddScale(float sx, float sy, float sz) {
	scale.x += sx; scale.y += sy; scale.z += sz;
}
// Gets the look vector of the transform for view matrix generation
D3DXVECTOR3 Transform::GetLookVector() {
	D3DXVECTOR3 look(0.0f, 0.0f, 1.0f);
	D3DXMATRIX rotation = GetRotationMatrix();
	D3DXVec3TransformCoord(&look, &look, &rotation);
	return look;
}
// Gets the up vector of the transform for view matrix generation
D3DXVECTOR3 Transform::GetUpVector() {
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX rotation = GetRotationMatrix();
	D3DXVec3TransformCoord(&up, &up, &rotation);
	return up;
}
// Gets the right vector of the transform for view matrix generation
D3DXVECTOR3 Transform::GetRightVector() {
	D3DXVECTOR3 right(1.0f, 0.0f, 0.0f);
	D3DXMATRIX rotation = GetRotationMatrix();
	D3DXVec3TransformCoord(&right, &right, &rotation);
	return right;
}
// Gets the rotation matrix of the transform
D3DXMATRIX Transform::GetRotationMatrix() {
	D3DXMATRIX rot, mrz, mrx, mry;
	D3DXMatrixRotationZ(&mrz, rotation.z);
	D3DXMatrixRotationX(&mrx, rotation.x);
	D3DXMatrixRotationY(&mry, rotation.y);
	rot = mrz * mrx * mry;
	return rot;
}
// Gets the matrix of the transform
D3DXMATRIX Transform::GetMatrix() {
	D3DXMATRIX mat, sca, rot, trans;

	D3DXMatrixIdentity(&sca);
	D3DXMatrixIdentity(&rot);
	D3DXMatrixIdentity(&trans);

	D3DXMatrixScaling(&sca, scale.x, scale.y, scale.z);

	D3DXMATRIX mrz, mrx, mry;
	D3DXMatrixRotationZ(&mrz, rotation.z);
	D3DXMatrixRotationX(&mrx, rotation.x);
	D3DXMatrixRotationY(&mry, rotation.y);
	rot = mrz * mrx * mry;

	D3DXMatrixTranslation(&trans, position.x, position.y, position.z);

	mat = rot * sca * trans;
	return mat;
}