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

#include "light.h"

// Static list of lights; used in World
std::list<Light*> Light::lights;
Light::Light(){
	range = 0.0f;
	position.x = 0.0f; position.y = 0.0f; position.z = 0.0f; position.w = 1.0f;
	color.x = 1.0f; color.y = 1.0f; color.z = 1.0f;
	tex = 0;
	shadowMapTex = 0;
	// Create shadow map cube texture
	if(FAILED(vvd::GetDevice()->CreateCubeTexture(SHADOW_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R16F, D3DPOOL_DEFAULT, &shadowMapTex, 0))) {
		vvd::Log("Failed to create shadow map texture");
		exit(1);
	}
	// Set up the shadow map render targets
	for(int i = 0; i < 6; i++) {
		shadowMap[i].SetWidth(SHADOW_SIZE);
		shadowMap[i].SetHeight(SHADOW_SIZE);
		IDirect3DSurface9* surface = 0;
		shadowMapTex->GetCubeMapSurface(vvd::GetCubeFace(i), 0, &surface);
		shadowMap[i].SetSurface(surface);
	}

	lights.push_back(this);
}
Light::Light(const Light& light) {
	color = light.color;
	position = light.position;
	range = light.range;
	cells = light.cells;
	tex = light.tex;
	transform = light.transform;
	for(int i = 0; i < 6; i++) {
		shadowMap[i] = light.shadowMap[i];
	}
	vvd::Release<IDirect3DCubeTexture9*>(tex);
	vvd::Release<IDirect3DCubeTexture9*>(shadowMapTex);
	shadowMapTex = light.shadowMapTex;
	if(shadowMapTex)
		shadowMapTex->AddRef();
	if(tex)
		tex->AddRef();
}
Light::~Light() {
	// Erase this light from the static light list
	std::list<Light*>::iterator i = lights.begin();
	while(i != lights.end()) {
		if(*i == this) {
			lights.erase(i);
			break;
		}
		i++;
	}
	vvd::Release<IDirect3DCubeTexture9*>(tex);
	vvd::Release<IDirect3DCubeTexture9*>(shadowMapTex);
}
// Sets the range of the light
void Light::SetRange(float nRange) {
	range = nRange;
}
// Gets the range
float Light::GetRange() {
	return range;
}
// Sets the color of the light
void Light::SetColor(float r, float g, float b) {
	color.x = r; color.y = g; color.z = b;
}
// Gets the color
D3DXVECTOR3 Light::GetColor() {
	return color;
}
// Sets the position of the light; make W 0.0f if you want the light to be directional
void Light::SetPosition(float x, float y, float z, float w) {
	position.x = x; position.y = y; position.z = z; position.w = w;
}
// Gets the position
D3DXVECTOR4 Light::GetPosition() {
	return position;
}
// Clears the list of cells this light affects
void Light::ClearCells() {
	cells.clear();
}
// Adds the specified cell to the list of cells this light affects
void Light::AddCell(Cell* cell) {
	cells.push_back(cell);
}
// Gets the list of cells this light affects
std::vector<Cell*>* Light::GetCells() {
	return &cells;
}
// Loads the light texture from the specified cubemap file
bool Light::LoadTexture(LPCSTR texFilename) {
	if(!SUCCEEDED(D3DXCreateCubeTextureFromFile(vvd::GetDevice(), texFilename, &tex))) {
		std::string msg = "Vivid: failed to load light cube map file: ";
		msg += texFilename;
		vvd::Log(msg.c_str());
		exit(1);
	}
	return true;
}
// Gets the light texture
IDirect3DTexture9* Light::GetTexture() {
	return (IDirect3DTexture9*)tex;
}
// Gets this light's shadow map texture
IDirect3DTexture9* Light::GetShadowMap() {
	return (IDirect3DTexture9*)shadowMapTex;
}
// Gets the specified shadow map render target
RenderTarget* Light::GetShadowMapTarget(int index) {
	return &(shadowMap[index]);
}
// Rotates the texture the specified amount
void Light::RotateTexture(float rx, float ry, float rz) {
	transform.AddRotation(rx, ry, rz);
}
// Sets the texture rotation
void Light::SetTextureRotation(float rx, float ry, float rz) {
	transform.SetRotation(rx, ry, rz);
}
// Gets the texture rotation matrix
D3DXMATRIX Light::GetTextureMatrix() {
	return transform.GetRotationMatrix();
}