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

#include "renderer.h"

Renderer::Renderer() {
	SetCullMode(D3DCULL_CCW); // Default cull mode: counter clockwise culling
	SetFillMode(D3DFILL_SOLID); // Default fill mode: solid fill
	SetBackgroundColor(0x000000); // Default background color: black
	SetBackgroundTexture(0); // Default background texture: none
	SetCameraPosition(0.0f, 0.0f, 0.0f); // Default position: origin

	D3DXVECTOR3 nLook(0.0f, 0.0f, 1.0f); // Default look direction: Z axis
	D3DXVECTOR3 nUp(0.0f, 1.0f, 0.0f); // Default up direction: Y axis
	SetCameraRotation(&nLook, &nUp); // Sets the camera rotation

	// Set up to render to the back buffer by default
	SetViewport(0, 0, vvd::GetWidth(), vvd::GetHeight(), 0.0f, 1.0f);

	SetDepthBias(0.99f);

	renderTargets.resize(vvd::GetDeviceCaps()->NumSimultaneousRTs);
	for(int i = 0; i < (int)renderTargets.size(); i++) {
		renderTargets[i] = 0;
	}

	// Set the default render target to the back buffer
	SetRenderTarget(0, &(RenderTarget::defaultTarget));

	// No default technique
	technique = 0;

	// Set up the projection matrix
	SetProjection(3.14159265358f * 0.5f, (float)vvd::GetWidth() / (float)vvd::GetHeight(), 1.0f, 10000.0f);
}
Renderer::~Renderer() {}
Renderer::Renderer(const Renderer& renderer) {
	cameraPos = renderer.cameraPos;
	cameraMat = renderer.cameraMat;
	cullMode = renderer.cullMode;
	fillMode = renderer.fillMode;
	backgroundColor = renderer.backgroundColor;
	backgroundTex = renderer.backgroundTex;
	projectionMat = renderer.projectionMat;
	view = renderer.view;
	look = renderer.look;
	up = renderer.up;
	renderTargets = renderer.renderTargets;
	technique = renderer.technique;
	nearPlane = renderer.nearPlane;
	farPlane = renderer.farPlane;
	fovY = renderer.fovY;
	filters = renderer.filters;
}
// Sets the position of the camera
void Renderer::SetCameraPosition(D3DXVECTOR3* nCameraPos) {
	cameraPos = *nCameraPos;
}
// Sets the position of the camera
void Renderer::SetCameraPosition(float x, float y, float z) {
	cameraPos.x = x; cameraPos.y = y; cameraPos.z = z;
}
// Sets the rotation of the camera;
// Look defines where the camera points
// Up defines the rotation on the Z axis
void Renderer::SetCameraRotation(D3DXVECTOR3* nLook, D3DXVECTOR3* nUp) {
	look = cameraPos + (*nLook);
	up = *nUp;
}
// Three options: D3DCULL_NONE, D3DCULL_CW, and D3DCULL_CCW
void Renderer::SetCullMode(DWORD nCullMode) {
	cullMode = nCullMode;
}
// Three options: D3DFILL_POINT, D3DFILL_WIREFRAME, and D3DFILL_SOLID
void Renderer::SetFillMode(DWORD nFillMode) {
	fillMode = nFillMode;
}
// Sets the color of the background (in hex code)
void Renderer::SetBackgroundColor(D3DCOLOR nBackgroundColor) {
	backgroundColor = nBackgroundColor;
}
// Sets the background texture
void Renderer::SetBackgroundTexture(IDirect3DTexture9* nBackgroundTex) {
	backgroundTex = nBackgroundTex;
}
// Sets the projection matrix
void Renderer::SetProjectionMatrix(D3DXMATRIX* nProjectionMat) {
	projectionMat = *nProjectionMat;
}
// Generates the projection matrix based on the values given
void Renderer::SetProjection(float nFovY, float aspect, float zn, float zf) {
	D3DXMatrixPerspectiveFovLH(&projectionMat, nFovY, aspect, zn, zf);
	fovY = nFovY;
	nearPlane = zn;
	farPlane = zf;
}
// Sets the viewport
void Renderer::SetViewport(D3DVIEWPORT9* nView) {
	view = *nView;
}
// Generates the viewport based on the values given
// X and Y define the position of the upper left hand corner
// width and height define the width and height of the rendering area
// minZ and maxZ define the minimum and maximum values for the Z buffer
void Renderer::SetViewport(DWORD x, DWORD y, DWORD width, DWORD height, float minZ, float maxZ) {
	view.Height = height;
	view.Width = width;
	view.X = x;
	view.Y = y;
	view.MinZ = minZ;
	view.MaxZ = maxZ;
}
// Points the camera at the target specified
void Renderer::CameraLookAt(D3DXVECTOR3* target) {
	look = *target - cameraPos;
}
// Points the camera at the target specified
void Renderer::CameraLookAt(float x, float y, float z) {
	look.x = x - cameraPos.x;
	look.y = y - cameraPos.y;
	look.z = z - cameraPos.z;
}
// Sets the render target
void Renderer::SetRenderTarget(int index, RenderTarget* nTarget) {
	if(index >= (int)renderTargets.size()) {
		std::string msg = "Failed to set render target ";
		msg += vvd::stringconv(index);
		msg += "; device only supports ";
		msg += vvd::stringconv((int)renderTargets.size());
		msg += " render targets";
		vvd::Log(msg.c_str());
		exit(1);
	}
	renderTargets[index] = nTarget;
	if(index == 0) {
		view.Height = renderTargets[index]->GetHeight();
		view.Width = renderTargets[index]->GetWidth();
		SetProjection(fovY, (float)renderTargets[index]->GetWidth() / (float)renderTargets[index]->GetHeight(), nearPlane, farPlane);
	}
}
// Sets the desired effect technique
void Renderer::SetTechnique(LPCSTR nTechnique) {
	technique = nTechnique;
}
// Sets the depth bias
void Renderer::SetDepthBias(float nBias) {
	depthBias = nBias;
}
// Draws the entire scene
void Renderer::Draw() {
	IDirect3DDevice9* device = vvd::GetDevice();
	if(vvd::CheckDeviceState()) { // Check if we've lost the device
		BeginScene();

		std::vector<Mesh*> alphaMeshes;
		std::vector<Mesh*> translucentMeshes;

		// Render all the meshes
		std::list<Mesh*>::iterator i = Mesh::meshes.begin();
		while(i != Mesh::meshes.end()) {
			Mesh* mesh = *i;
			if(mesh->IsAlpha() && !mesh->IsTranslucent()) {
				alphaMeshes.push_back(mesh);
			} else if(mesh->IsTranslucent()) {
				translucentMeshes.push_back(mesh);
			} else {
				DrawMesh(mesh);
			}
			i++;
		}

		for(int j = 0; j < (int)alphaMeshes.size(); j++) {
			DrawMesh(alphaMeshes[j]);
		}

		if(renderTargets[0] == &(RenderTarget::defaultTarget))
			RenderTarget::Update();

		for(int j = 0; j < (int)translucentMeshes.size(); j++) {
			DrawMesh(translucentMeshes[j]);
		}

		// Draw the render targets if we're rendering to the back buffer
		if(renderTargets[0] == &(RenderTarget::defaultTarget) && !RenderTarget::targets.empty()) {
			DrawRenderTargets();
		}
		EndScene();
	}
}
// Draws the entire scene's shadows
void Renderer::DrawShadows() {
	IDirect3DDevice9* device = vvd::GetDevice();
	if(vvd::CheckDeviceState()) { // Check if we've lost the device
		D3DXVECTOR3 look;
		D3DXVECTOR3 up;

		SetTechnique("Shadow");
		SetCullMode(D3DCULL_CW);

		std::list<Light*>::iterator i = Light::lights.begin();
		while(i != Light::lights.end()) {
			Light* light = *i;

			SetProjection(D3DX_PI/2, 1.0f, 1.0f, light->GetRange());
			SetCameraPosition(light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);

			for(int k = 0; k < 6; k++) {
				look = GetCubeMapLook(k);
				up = GetCubeMapUp(k);
				SetCameraRotation(&look, &up);
				SetCameraPosition(light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
				SetRenderTarget(0, light->GetShadowMapTarget(k));

				BeginScene();
				// Render all the meshes
				std::list<Mesh*>::iterator j = Mesh::meshes.begin();
				while(j != Mesh::meshes.end()) {
					DrawMesh(*j);
					j++;
				}
				EndScene();
			}
			i++;
		}

	}
}
// Clears the render target to the background color
void Renderer::ClearScreen() {
	BeginScene();
	EndScene();
}
void Renderer::EnableFilter(ImageFilter* imgfilter) {
	if(!Contains(imgfilter)) {
		filters.push_back(imgfilter);
	}
}
void Renderer::DisableFilter(ImageFilter* imgfilter) {
	if(Contains(imgfilter)) {
		filters.erase(GetFilter(imgfilter));
	}
}
// Draws the specified cells
void Renderer::Draw(std::vector<Cell*>* cells) {
	IDirect3DDevice9* device = vvd::GetDevice();
	if(vvd::CheckDeviceState()) { // Check if we've lost the device
		BeginScene(); // Begin rendering

		std::vector<Mesh*> alphaMeshes;
		std::vector<Mesh*> translucentMeshes;

		// Render the cells
		for(int i = 0; i < (int)cells->size(); i++) {
			Cell* cell = (*cells)[i];

			// Render all the meshes
			std::vector<Mesh*>* meshes = cell->GetMeshes();
			for(int j = 0; j < (int)meshes->size(); j++) {
				Mesh* mesh = (*meshes)[j];
				if(mesh->IsAlpha() && !mesh->IsTranslucent()) {
					alphaMeshes.push_back(mesh);
				} else if(mesh->IsTranslucent()) {
					translucentMeshes.push_back(mesh);
				} else {
					DrawMesh(mesh);
				}
			}
		}

		for(int j = 0; j < (int)alphaMeshes.size(); j++) {
			DrawMesh(alphaMeshes[j]);
		}

		if(renderTargets[0] == &(RenderTarget::defaultTarget))
			RenderTarget::Update();

		for(int j = 0; j < (int)translucentMeshes.size(); j++) {
			DrawMesh(translucentMeshes[j]);
		}

		// Draw the render targets if we're rendering to the back buffer
		if(renderTargets[0] == &(RenderTarget::defaultTarget) && !RenderTarget::targets.empty()) {
			DrawRenderTargets();
		}
		EndScene(); // End rendering
	}
}
// Initializes rendering; called before rendering the scene
void Renderer::BeginScene() {
	IDirect3DDevice9* device = vvd::GetDevice();

	// Init view matrix
	D3DXMatrixLookAtLH(&cameraMat, &cameraPos, &look, &up);

	device->SetViewport(&view); // Set the viewport

	device->SetRenderState(D3DRS_CULLMODE, cullMode); // Set cull mode
	device->SetRenderState(D3DRS_FILLMODE, fillMode); // Set fill mode

	device->BeginScene();

	// Set the render targets
	for(int i = 0; i < (int)renderTargets.size(); i++) {
		IDirect3DSurface9* renderTarget = 0;
		device->GetRenderTarget(i, &renderTarget);
		if(renderTarget != (IDirect3DSurface9*)renderTargets[i]) {
			if(renderTargets[i]) {
				IDirect3DSurface9* surface = renderTargets[i]->GetSurface();
				if(FAILED(device->SetRenderTarget(i, surface))) {
					std::string msg = "Failed to set render target ";
					msg += vvd::stringconv(i);
					vvd::Log(msg.c_str());
					exit(1);
				}
				vvd::Release<IDirect3DSurface9*>(surface);
			} else {
				if(FAILED(device->SetRenderTarget(i, 0))) {
					std::string msg = "Failed to set render target ";
					msg += vvd::stringconv(i);
					vvd::Log(msg.c_str());
					exit(1);
				}
			}
		}
	}

	// Clear the render target to black
	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backgroundColor, 1.0f, 0);

	if(backgroundTex && renderTargets[0] == &(RenderTarget::defaultTarget)) {
		// StretchRect the background texture onto the screen
		IDirect3DSurface9* surface = 0;
		backgroundTex->GetSurfaceLevel(0, &surface);
		IDirect3DSurface9* surface2 = renderTargets[0]->GetSurface();
		if(FAILED(device->StretchRect(surface, NULL, surface2, NULL, D3DTEXF_LINEAR))) {
			vvd::Log("Vivid: StretchRect failed to copy background texture to back buffer");
			exit(1);
		}
	}
}
// Ends rendering; called after rendering the scene
void Renderer::EndScene() {
	IDirect3DDevice9* device = vvd::GetDevice();

	std::list<ImageFilter*>::iterator i = filters.begin();
	while(i != filters.end()) {
		RenderTarget::Update(renderTargets[0]);
		(*i)->Filter(renderTargets[0]->GetSurface());
		i++;
	}

	device->EndScene();

	// Copy the back buffer to the screen only if the target is the back buffer
	if(renderTargets[0] == &(RenderTarget::defaultTarget))
		device->Present(0, 0, 0, 0);
}
// Gets the specified look vector for cube map rendering
D3DXVECTOR3 Renderer::GetCubeMapLook(int i) {
	switch(i) {
		case 0:
			return D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			break;
		case 1:
			return D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			break;
		case 2:
			return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case 3:
			return D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			break;
		case 4:
			return D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			break;
		case 5:
			return D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
	}
	return D3DXVECTOR3(1.0f, 0.0f, 0.0f);
}
// Gets the specified up vector for cube map rendering
D3DXVECTOR3 Renderer::GetCubeMapUp(int i) {
	switch(i) {
		case 0:
			return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case 1:
			return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case 2:
			return D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
		case 3:
			return D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			break;
		case 4:
			return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case 5:
			return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
	}
	return D3DXVECTOR3(1.0f, 0.0f, 0.0f);
}
// Draws a single mesh
void Renderer::DrawMesh(Mesh* mesh) {
	IDirect3DDevice9* device = vvd::GetDevice();

	DWORD index = 0;

	std::vector<Material>* materials = mesh->GetMaterials();
	for(int i = 0; i < (int)materials->size(); i++) {
		Material* material = &((*materials)[i]); // Get the material from the mesh

		// Get the number of passes from SetMaterial()
		UINT numPasses = SetMaterial(material, mesh);

		if(numPasses > 0) {
			ID3DXEffect* effect = material->GetEffect();

			// Draw the subset once for each pass
			for(int j = 0; j < (int)numPasses; j++) {
				// Start the pass
				if(effect)
					effect->BeginPass(j);

				// Draw the subset
				mesh->DrawSubset(index);

				// End the pass
				if(effect)
					effect->EndPass();
			}

			// Disable the effect
			if(effect)
				effect->End();
		}

		index++;
	}
}
// Draws the render targets on the right side of the screen
void Renderer::DrawRenderTargets() {
	IDirect3DDevice9* device = vvd::GetDevice();

	std::list<RenderTarget*>::iterator i = RenderTarget::targets.begin();

	int currentHeight = 0;

	// Destination surface is the back buffer
	IDirect3DSurface9* destSurface = RenderTarget::defaultTarget.GetSurface();

	while(i != RenderTarget::targets.end() && currentHeight < (int)view.Height) {
		RenderTarget* target = *i;

		if(target->DrawToScreen()) {
			// Scale to 1/4 the screen size
			float scale = ((float)view.Width / 4.0f) / (float)target->GetWidth();

			float width = (float)target->GetWidth() * scale; // Scale the width
			float height = (float)target->GetHeight() * scale; // Scale the height

			// Set up the rectangle to copy
			RECT rect = {view.Width - (int)width, currentHeight, view.Width, currentHeight + (int)height};

			// Copy the data to the back buffer
			device->StretchRect(target->GetSurface(), NULL, destSurface, &rect, D3DTEXF_LINEAR);

			// Add to the start height so the next render target renders below this one
			currentHeight += (int)height;
		}

		i++;
	}
}
// Sets the material handles; returns the number of passes required by the effect
UINT Renderer::SetMaterial(Material* material, Mesh* mesh) {
	IDirect3DDevice9* device = vvd::GetDevice();
	// Default number of passes: 1
	UINT numPasses = 1;

	D3DXMATRIX worldMat = mesh->transform.GetMatrix();

	ID3DXEffect* effect = material->GetEffect();

	if(effect) {
		if(technique) {
			if(FAILED(effect->SetTechnique(technique)))
				return 0;
		} else {
			D3DXHANDLE handle;
			effect->FindNextValidTechnique(0, &handle);
			effect->SetTechnique(handle);
		}

		// Set the view projection matrix
		D3DXMATRIX viewProj = cameraMat * projectionMat;
		effect->SetMatrix(material->GetViewProjHandle(), &viewProj);

		// Set the projection matrix
		effect->SetMatrix(material->GetProjectionHandle(), &projectionMat);

		// Set the view matrix
		effect->SetMatrix(material->GetViewMatHandle(), &cameraMat);

		// Set the world matrix
		effect->SetMatrix(material->GetWorldMatHandle(), &worldMat);

		// Set the camera position
		D3DXVECTOR4 nCameraPos;
		nCameraPos.x = cameraPos.x; nCameraPos.y = cameraPos.y; nCameraPos.z = cameraPos.z; nCameraPos.w = 1.0f;
		effect->SetVector(material->GetCameraPosHandle(), &nCameraPos);

		// Set the near and far planes
		effect->SetFloat(material->GetFarPlaneHandle(), nearPlane);
		effect->SetFloat(material->GetNearPlaneHandle(), farPlane);

		// Set the depth bias
		effect->SetFloat(material->GetDepthBiasHandle(), depthBias);

		// Setup all the lights affecting the mesh
		if(material->RequiresLights(technique)) {
			CompileLightArray(mesh->GetCells(), material);
		}

		// Now we can start rendering
		effect->Begin(&numPasses, 0);
	}

	return numPasses;
}
// Compiles light data from the cells provided
void Renderer::CompileLightArray(std::vector<Cell*>* cells, Material* material) {
	float* lightRanges = Material::GetLightRanges();
	D3DXVECTOR4* lightPositions = Material::GetLightPositions();
	D3DXVECTOR4* lightColors = Material::GetLightColors();
	D3DXVECTOR4* ambients = Material::GetAmbients();
	IDirect3DTexture9** lightTextures = Material::GetLightTextures();
	IDirect3DTexture9** shadowMaps = Material::GetShadowMaps();
	D3DXMATRIX* lightTexMatrices = Material::GetLightTexMatrices();

	// Clear the arrays
	for(int i = 0; i < MAX_LIGHTS; i++) {
		lightTextures[i] = 0;
		shadowMaps[i] = 0;
	}

	int currentLight = 0;
	int maxLights = min(material->MaxLights(), MAX_LIGHTS);
	for(int i = 0; i < (int)cells->size(); i++) {
		Cell* cell = (*cells)[i];
		// For each cell, set all the lights and the ambient color
		std::vector<Light*>* lights = cell->GetLights();
		for(int j = 0; j < (int)lights->size() && currentLight < maxLights; j++) {
			Light* light = (*lights)[j];
			// For each light, copy the range, position, and color of the light to the output arrays
			lightRanges[currentLight] = light->GetRange();
			lightPositions[currentLight] = light->GetPosition();
			D3DXVECTOR3 color = light->GetColor();
			lightColors[currentLight].x = color.x; lightColors[currentLight].y = color.y; lightColors[currentLight].z = color.z; lightColors[currentLight].w = 1.0f;
			lightTextures[currentLight] = light->GetTexture();
			shadowMaps[currentLight] = light->GetShadowMap();
			lightTexMatrices[currentLight] = light->GetTextureMatrix();
			currentLight = min(currentLight + 1, maxLights);
		}
		// Set the ambient color of the cell
		ambients[min(i, maxLights)] = cell->GetAmbientColor();
	}
	D3DXHANDLE lightNumHandle = material->GetLightNumHandle();
	ID3DXEffect* effect = material->GetEffect();
	effect->SetInt(lightNumHandle, currentLight);
	material->UpdateLightArrays();
}
bool Renderer::Contains(ImageFilter* imgfilter) {
	std::list<ImageFilter*>::iterator i = filters.begin();
	while(i != filters.end()) {
		ImageFilter* nImgFilter = *i;
		if(nImgFilter == imgfilter)
			return true;
		i++;
	}
	return false;
}
std::list<ImageFilter*>::iterator Renderer::GetFilter(ImageFilter* imgfilter) {
	std::list<ImageFilter*>::iterator i = filters.begin();
	while(i != filters.end()) {
		ImageFilter* nImgFilter = *i;
		if(nImgFilter == imgfilter)
			return i;
		i++;
	}
	return i;
}