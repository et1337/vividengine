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

#ifndef transform_h
#define transform_h
#include "vivid.h"

class Transform {
public:
	Transform();
	Transform(float x, float y, float z);
	Transform(float x, float y, float z, float rx, float ry, float rz);
	Transform(const Transform& transform);
	~Transform();
	D3DXVECTOR3 GetPosition(); // Gets the position of the transform
	D3DXVECTOR3 GetRotation(); // Gets the rotation of the transform
	D3DXVECTOR3 GetScale(); // Gets the scaling of the transform
	void SetPosition(D3DXVECTOR3* nPosition); // Sets the position
	void SetRotation(D3DXVECTOR3* nRotation); // Sets the rotation
	void SetScale(D3DXVECTOR3* nScale); // Sets the scaling
	void SetPosition(float x, float y, float z); // Sets the position
	void SetRotation(float rx, float ry, float rz); // Sets the rotation
	void SetScale(float sx, float sy, float sz); // Sets the scaling
	void AddPosition(D3DXVECTOR3* nPosition); // Adds the specified vector to the position
	void AddPositionRelative(D3DXVECTOR3* nPosition); // Adds the specified vector to the position
													  // relative to the transform's rotation
	void AddRotation(D3DXVECTOR3* nRotation); // Adds the specified vector to the rotation
	void AddScale(D3DXVECTOR3* nScale);  // Adds the specified vector to the scaling
	void AddPosition(float x, float y, float z); // Adds the specified values to the position
	void AddPositionRelative(float x, float y, float z); // Adds the specified values to the position
														 // relative to the transform's rotation
	void AddRotation(float rx, float ry, float rz); // Adds the specified values to the rotation
	void AddScale(float sx, float sy, float sz); // Adds the specified valeus to the scaling
	D3DXVECTOR3 GetLookVector(); // Gets the look vector of the transform for view matrix generation
	D3DXVECTOR3 GetUpVector(); // Gets the up vector of the transform for view matrix generation
	D3DXVECTOR3 GetRightVector(); // Gets the right vector of the transform for view matrix generation
	D3DXMATRIX GetRotationMatrix(); // Gets the rotation matrix of the transform
	D3DXMATRIX GetMatrix(); // Gets the matrix of the transform
protected:
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 scale;
};

#endif