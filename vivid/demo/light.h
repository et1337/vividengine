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

#ifndef light_h
#define light_h
#include "vivid.h"
#include <list>
#include "world.h"

struct Cell;

#define MAX_LIGHTS 6
#define SHADOW_SIZE 512

class Light {
public:
	static std::list<Light*> lights; // Static list of lights
	Light();
	Light(const Light& light);
	~Light();
	void SetRange(float nRange); // Sets the range of the light
	float GetRange(); // Gets the range
	void SetColor(float r, float g, float b); // Sets the color of the light
	D3DXVECTOR3 GetColor(); // Gets the color
	void SetPosition(float x, float y, float z, float nw); // Sets the position of the light;
														  // make W 0.0f if you want the light to be directional
	D3DXVECTOR4 GetPosition(); // Gets the position
	void ClearCells(); // Clears the list of cells this light affects
	void AddCell(Cell* cell); // Adds the specified cell to the list of cells this light affects
	std::vector<Cell*>* GetCells(); // Gets the list of cells this light affects
	bool LoadTexture(LPCSTR texFilename); // Loads the light texture from the specified cubemap file
	IDirect3DTexture9* GetTexture(); // Gets the light texture
	IDirect3DTexture9* GetShadowMap(); // Gets this light's shadow map texture
	RenderTarget* GetShadowMapTarget(int index); // Gets the specified shadow map render target
	void RotateTexture(float rx, float ry, float rz); // Rotates the texture the specified amount
	void SetTextureRotation(float rx, float ry, float rz); // Sets the texture rotation
	D3DXMATRIX GetTextureMatrix(); // Gets the texture rotation matrix
private:
	D3DXVECTOR3 color; // The color of the light
	D3DXVECTOR4 position; // The position of the light
	float range; // The maximum distance from which the light can affect an object
	std::vector<Cell*> cells; // List of cells this light affects
	IDirect3DCubeTexture9* tex; // Light texture
	IDirect3DCubeTexture9* shadowMapTex; // Shadow map cube texture
	Transform transform; // Texture rotation transform
	RenderTarget shadowMap[6]; // Shadow map render targets that point to the faces of the shadow map texture
};

#endif