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

#include "material.h"

std::list<Material*> Material::materials;

float Material::lightRanges[MAX_LIGHTS]; // The range of each effective light
D3DXVECTOR4 Material::lightPositions[MAX_LIGHTS]; // The position of each effective light
D3DXVECTOR4 Material::lightColors[MAX_LIGHTS]; // The color of each effective light
D3DXVECTOR4 Material::ambients[MAX_LIGHTS]; // The ambient color of each effective cell
IDirect3DTexture9* Material::lightTextures[MAX_LIGHTS]; // The texture of each effective light
IDirect3DTexture9* Material::shadowMaps[MAX_LIGHTS]; // The shadow map of each effective light
D3DXMATRIX Material::lightTexMatrices[MAX_LIGHTS]; // The texture rotation matrix of each effective light

Material::Material() {
	effect = 0;
	filename = 0;
	maxLights = 0;
	alpha = false;
	translucent = false;
	materials.push_back(this);
}
// Loads the specified material file
Material::Material(LPCSTR nFilename) {
	effect = 0;
	filename = 0;
	maxLights = 0;
	alpha = false;
	translucent = false;
	ParseFile(nFilename);
	materials.push_back(this);
}
Material::Material(const Material& material) {
	effect = material.effect;
	if(effect)
		effect->AddRef();

	for(int i = 0; i < (int)textures.size(); i++)
		vvd::Release<IDirect3DTexture9*>(textures[i]);

	filename = material.filename;

	viewProjHandle = material.viewProjHandle;
	worldMatHandle = material.worldMatHandle;
	cameraPosHandle = material.cameraPosHandle;
	lightPosHandle = material.lightPosHandle;
	lightRangeHandle = material.lightRangeHandle;
	lightColorHandle = material.lightColorHandle;
	lightNumHandle = material.lightNumHandle;
	lightAmbientHandle = material.lightAmbientHandle;
	lightMatrixHandle = material.lightMatrixHandle;
	farPlaneHandle = material.farPlaneHandle;
	nearPlaneHandle = material.nearPlaneHandle;
	projectionHandle =  material.projectionHandle;
	viewMatHandle = material.viewMatHandle;
	depthBiasHandle = material.depthBiasHandle;
	maxLights = material.maxLights;
	alpha = material.alpha;
	translucent = material.translucent;

	textures = material.textures;
	for(int i = 0; i < (int)textures.size(); i++)
		textures[i]->AddRef();
	materials.push_back(this);
}
Material::~Material() {
	std::list<Material*>::iterator i = materials.begin(); // Iterate through all the materials
	while(i != materials.end()) {
		if(*i == this) {
			materials.erase(i); // Erase this material from the list so we don't have a stray pointer in the mesh list
			break;
		}
		i++;
	}

	vvd::Release<ID3DXEffect*>(effect);
	for(int i = 0; i < (int)textures.size(); i++)
		vvd::Release<IDirect3DTexture9*>(textures[i]);
}
// Gets the effect loaded from the material file
ID3DXEffect* Material::GetEffect() {
	return effect;
}
// Returns true if the specified technique requires light data
bool Material::RequiresLights(LPCSTR technique) {
	if(technique) {
		return strcmp(technique, "Default") == 0;
	} else {
		D3DXHANDLE handle = effect->GetParameterByName(0, "lightPos");
		return handle != NULL;
	}
}
// Gets the effect handle to the view projection matrix
D3DXHANDLE Material::GetViewProjHandle() {
	return viewProjHandle;
}
// Gets the effect handle to the world matrix
D3DXHANDLE Material::GetWorldMatHandle() {
	return worldMatHandle;
}
// Gets the effect handle to the camera position
D3DXHANDLE Material::GetCameraPosHandle() {
	return cameraPosHandle;
}
// Gets the effect handle to the light position array
D3DXHANDLE Material::GetLightPosHandle() {
	return lightPosHandle;
}
// Gets the effect handle to the light range array
D3DXHANDLE Material::GetLightRangeHandle() {
	return lightRangeHandle;
}
// Gets the effect handle to the light color array
D3DXHANDLE Material::GetLightColorHandle() {
	return lightColorHandle;
}
// Gets the effect handle to the number of lights
D3DXHANDLE Material::GetLightNumHandle() {
	return lightNumHandle;
}
// Gets the effect handle to the ambient light array
D3DXHANDLE Material::GetLightAmbientHandle() {
	return lightAmbientHandle;
}
// Gets the effect handle to the specified light texture
D3DXHANDLE Material::GetLightTextureHandle(int index) {
	std::string texName = "lightTexture";
	texName += vvd::stringconv(index);
	D3DXHANDLE output = effect->GetParameterByName(0, texName.c_str());
	return output;
}
// Gets the effect handle to the light texture matrix array
D3DXHANDLE Material::GetLightMatrixHandle() {
	return lightMatrixHandle;
}
// Gets the effect handle to the far plane float
D3DXHANDLE Material::GetFarPlaneHandle() {
	return farPlaneHandle;
}
// Gets the effect handle to the near plane float
D3DXHANDLE Material::GetNearPlaneHandle() {
	return nearPlaneHandle;
}
// Gets the effect handle to the projection matrix
D3DXHANDLE Material::GetProjectionHandle() {
	return projectionHandle;
}
// Gets the effect handle to the view matrix
D3DXHANDLE Material::GetViewMatHandle() {
	return viewMatHandle;
}
// Gets the effect handle to the specified shadow map cube texture
D3DXHANDLE Material::GetShadowMapHandle(int index) {
	std::string texName = "shadowMap";
	texName += vvd::stringconv(index);
	D3DXHANDLE output = effect->GetParameterByName(0, texName.c_str());
	return output;
}
// Gets the effect handle to the depth bias variable
D3DXHANDLE Material::GetDepthBiasHandle() {
	return depthBiasHandle;
}
// Returns the maximum number of lights the effect can handle
int Material::MaxLights() {
	return maxLights;
}
// Returns true if this material has alpha information
bool Material::IsAlpha() {
	return alpha;
}
// Returns true if this material needs access to the back buffer
bool Material::IsTranslucent() {
	return translucent;
}
// Overrides the specified texture; returns true if procedure succeeded
bool Material::OverrideTexture(LPCSTR texName, IDirect3DTexture9* tex) {
	D3DXHANDLE handle = effect->GetParameterByName(0, texName);
	if(handle != NULL) {
		effect->SetTexture(handle, tex);
		return true;
	}
	return false;
}
bool Material::OverrideCubeTexture(LPCSTR texName, IDirect3DCubeTexture9* tex) {
	return OverrideTexture(texName, (IDirect3DTexture9*)tex);
}
// Overrides the specified vector; returns true if procedure succeeded
bool Material::OverrideVector(LPCSTR vecName, D3DXVECTOR4* vector) {
	D3DXHANDLE handle = effect->GetParameterByName(0, vecName);
	if(handle != NULL) {
		effect->SetVector(handle, vector);
		return true;
	}
	return false;
}
// Overrides the specified float; returns true if procedure succeeded
bool Material::OverrideFloat(LPCSTR floatName, float num) {
	D3DXHANDLE handle = effect->GetParameterByName(0, floatName);
	if(handle != NULL) {
		effect->SetFloat(handle, num);
		return true;
	}
	return false;
}
// Overrides the specified matrix; returns true if procedure succeeded
bool Material::OverrideMatrix(LPCSTR matName, D3DXMATRIX* mat) {
	D3DXHANDLE handle = effect->GetParameterByName(0, matName);
	if(handle != NULL) {
		effect->SetMatrix(handle, mat);
		return true;
	}
	return false;
}
// Loads the specified material file
void Material::ParseFile(LPCSTR nFilename) {
	filename = nFilename;

	// Release effect just in case
	vvd::Release<ID3DXEffect*>(effect);

	// Logging
	std::string msg = "Vivid: Loading material: ";
	msg += filename;
	vvd::Log(msg.c_str());

	std::list<Material*>::iterator i = materials.begin(); // Iterate through all the materials
	while(i != materials.end()) {
		if(strcmp((*i)->filename, filename) == 0) {
			SetTo(*i);
			msg = "Vivid: succesfully loaded precached material: ";
			msg += filename;
			vvd::Log(msg.c_str());
			return;
		}
		i++;
	}

	// Load the material file
	std::ifstream fs;
	fs.open(filename);
	if(fs.fail()) {
		msg = "Vivid: Failed to load material: ";
		msg += filename;
		vvd::Log(msg.c_str());
		exit(1);
	} else {
		char cmd[25] = "";

		int line = 0;
		do {
			line++;
			fs >> cmd;

			std::string test = cmd;
			if(strcmp(&(test.at(0)), "//") == 0) { // It's a comment; skip it
				fs.ignore(256, '\n'); // Ignore this entire line
				continue;
			}

			// Add a texture
			if(strcmp(cmd, "texture") == 0) {
				char texHandleName[25] = "";
				char imageFilename[25] = "";
				fs >> texHandleName;
				fs >> imageFilename;
				if(effect) {
					IDirect3DTexture9* nTex;
					// Create the texture
					if(FAILED(D3DXCreateTextureFromFile(vvd::GetDevice(), imageFilename, &nTex))) {
						msg = "Vivid: Failed to add texture ";
						msg += imageFilename;
						msg += " to material ";
						msg += filename;
						vvd::Log(msg.c_str());
						exit(1);
					}

					// Add the texture to the effect
					D3DXHANDLE texHandle = effect->GetParameterByName(0, texHandleName);
					if(SUCCEEDED(effect->SetTexture(texHandle, nTex))) {
						msg = "Vivid: Successfully added texture ";
						msg += imageFilename;
						msg += " to material ";
						msg += filename;
						textures.push_back(nTex);
					} else {
						msg = "Vivid: Failed to add texture ";
						msg += imageFilename;
						msg += " to material ";
						msg += filename;
						vvd::Release<IDirect3DTexture9*>(nTex);
					}
					vvd::Log(msg.c_str());
				}
			} else if(strcmp(cmd, "cubeTexture") == 0) {
				char texHandleName[25] = "";
				char imageFilename[25] = "";
				fs >> texHandleName;
				fs >> imageFilename;
				if(effect) {
					IDirect3DCubeTexture9* nTex;
					// Create the cube texture
					if(FAILED(D3DXCreateCubeTextureFromFile(vvd::GetDevice(), imageFilename, &nTex))) {
						msg = "Vivid: Failed to add cube texture ";
						msg += imageFilename;
						msg += " to material ";
						msg += filename;
						vvd::Log(msg.c_str());
						exit(1);
					}

					// Add the cube texture to the effect
					D3DXHANDLE texHandle = effect->GetParameterByName(0, texHandleName);
					if(SUCCEEDED(effect->SetTexture(texHandle, (IDirect3DTexture9*)nTex))) {
						msg = "Vivid: Successfully added cube texture ";
						msg += imageFilename;
						msg += " to material ";
						msg += filename;
						textures.push_back((IDirect3DTexture9*)nTex);
					} else {
						msg = "Vivid: Failed to add cube texture ";
						msg += imageFilename;
						msg += " to material ";
						msg += filename;
						vvd::Release<IDirect3DCubeTexture9*>(nTex);
					}
					vvd::Log(msg.c_str());
				}
			} else if(strcmp(cmd, "effect") == 0) {
				// Add an effect
				char nEffectFile[25] = "";
				fs >> nEffectFile;
				LoadEffect(nEffectFile);

				msg = "Vivid: Successfully added effect ";
				msg += nEffectFile;
				msg += " to material ";
				msg += filename;
				vvd::Log(msg.c_str());
			} else if(strcmp(cmd, "float") == 0) {
				// Add a float
				char handleName[25] = "";
				float num = 0.0f;
				fs >> handleName;
				fs >> num;
				if(effect) {
					// Add the float to the effect
					D3DXHANDLE handle = effect->GetParameterByName(0, handleName);
					if(SUCCEEDED(effect->SetFloat(handle, num))) {
						msg = "Vivid: successfully added float ";
						msg += handleName;
						msg += " to material ";
						msg += filename;
					} else {
						msg = "Vivid: failed to add float ";
						msg += handleName;
						msg += " to material ";
						msg += filename;
					}
					vvd::Log(msg.c_str());
				}
			} else if(strcmp(cmd, "vector") == 0) {
				// Add a vector
				char handleName[25] = "";
				D3DXVECTOR4 vector;
				fs >> handleName;
				fs >> vector.x; fs >> vector.y; fs >> vector.z; fs >> vector.w;
				if(effect) {
					// Add the vector to the effect
					D3DXHANDLE handle = effect->GetParameterByName(0, handleName);
					if(SUCCEEDED(effect->SetVector(handle, &vector))) {
						msg = "Vivid: successfully added vector ";
						msg += handleName;
						msg += " to material ";
						msg += filename;
					} else {
						msg = "Vivid: failed to add vector ";
						msg += handleName;
						msg += " to material ";
						msg += filename;
					}
					vvd::Log(msg.c_str());
				}
			} else if(strcmp(cmd, "alpha:") == 0) {
				// Record whether this material has alpha information or not
				char alphaValue[25] = "";
				fs >> alphaValue;
				if(strcmp(alphaValue, "true") == 0) {
					alpha = true;
				} else {
					alpha = false;
				}
			} else if(strcmp(cmd, "translucent:") == 0) {
				// Record whether this material needs access to the back buffer or not
				char translucentValue[25] = "";
				fs >> translucentValue;
				if(strcmp(translucentValue, "true") == 0) {
					translucent = true;
				} else {
					translucent = false;
				}
			} else {
				// Error
				msg = "Vivid: Invalid material command ";
				msg += cmd;
				msg += " in ";
				msg += filename;
				msg += " line ";
				msg += vvd::stringconv(line);
				vvd::Log(msg.c_str());
				exit(1);
			}
		} while (fs.peek() != EOF);
	}
	fs.close();

	msg = "Vivid: Successfully loaded material: ";
	msg += filename;
	vvd::Log(msg.c_str());
}
// Loads the specified effect file
void Material::LoadEffect(LPCSTR effectFile) {
	effect = 0;

	ID3DXBuffer* errorBuffer = 0;
	IDirect3DDevice9* device = vvd::GetDevice();

	// Create the effect
	HRESULT hr = D3DXCreateEffectFromFile(
		device,
		effectFile,
		0,
		0,
		D3DXSHADER_DEBUG | D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE,
		0,
		&effect,
		&errorBuffer);

	if(FAILED(hr)) {
		std::string msg = "Vivid: Failed to load effect file: ";
			msg += effectFile;
			vvd::Log(msg.c_str());
		// Output any errors to the log file
		if(errorBuffer) {
			vvd::Log((LPCSTR)errorBuffer->GetBufferPointer());
			vvd::Release<ID3DXBuffer*>(errorBuffer);
		}
		exit(1);
	}

	FillOutHandles();
}
// Attempts to retrieve all the default handles from the effect file
void Material::FillOutHandles() {
	if(effect) {
		// Try to fill out all the default handles
		viewProjHandle = effect->GetParameterByName(0, "view_proj_mat");
		worldMatHandle = effect->GetParameterByName(0, "world_mat");
		cameraPosHandle = effect->GetParameterByName(0, "cameraPos");
		lightPosHandle = effect->GetParameterByName(0, "lightPos");
		lightRangeHandle = effect->GetParameterByName(0, "lightRange");
		lightColorHandle = effect->GetParameterByName(0, "lightColor");
		lightNumHandle = effect->GetParameterByName(0, "numLights");
		lightAmbientHandle = effect->GetParameterByName(0, "ambientLight");
		lightMatrixHandle = effect->GetParameterByName(0, "lightMatrix");
		farPlaneHandle = effect->GetParameterByName(0, "farPlane");
		nearPlaneHandle = effect->GetParameterByName(0, "nearPlane");
		projectionHandle = effect->GetParameterByName(0, "projection_mat");
		viewMatHandle = effect->GetParameterByName(0, "view_mat");
		depthBiasHandle = effect->GetParameterByName(0, "depthBias");
		
		D3DXHANDLE maxLightsHandle = effect->GetParameterByName(0, "maxLights");
		effect->GetInt(maxLightsHandle, &maxLights);

		D3DXHANDLE backBufferHandle = effect->GetParameterByName(0, "backBuffer");
		effect->SetTexture(backBufferHandle, RenderTarget::backBufferAccess.GetTexture());

		D3DXHANDLE backBufferWidthHandle = effect->GetParameterByName(0, "backBufferWidth");
		effect->SetInt(backBufferWidthHandle, RenderTarget::backBufferAccess.GetWidth());

		D3DXHANDLE backBufferHeightHandle = effect->GetParameterByName(0, "backBufferHeight");
		effect->SetInt(backBufferHeightHandle, RenderTarget::backBufferAccess.GetHeight());
		UpdateLightArrays();
	}
}
// Relays all the light information from the Renderer to the effect
void Material::UpdateLightArrays() {
	ID3DXEffect* effect = GetEffect();

	// Retrieve all the handles
	D3DXHANDLE rangeHandle, posHandle, colorHandle, ambientHandle, texMatrixHandle;
	rangeHandle = GetLightRangeHandle();
	posHandle = GetLightPosHandle();
	colorHandle = GetLightColorHandle();
	ambientHandle = GetLightAmbientHandle();
	texMatrixHandle = GetLightMatrixHandle();

	int maxLights = min(MaxLights(), MAX_LIGHTS);
	// Set all the light data arrays
	effect->SetVectorArray(posHandle, lightPositions, maxLights);
	effect->SetFloatArray(rangeHandle, lightRanges, maxLights);
	effect->SetVectorArray(colorHandle, lightColors, maxLights);
	effect->SetVectorArray(ambientHandle, ambients, maxLights);
	effect->SetMatrixArray(texMatrixHandle, lightTexMatrices, maxLights);

	for(int j = 0; j < MAX_LIGHTS; j++) {
		if(lightTextures[j]) {
			effect->SetTexture(GetLightTextureHandle(j), lightTextures[j]);
		}
	}
	for(int k = 0; k < MAX_LIGHTS; k++) {
		if(shadowMaps[k]) {
			effect->SetTexture(GetShadowMapHandle(k), shadowMaps[k]);
		}
	}
}
void Material::SetTo(Material* material) {
	vvd::Release<ID3DXEffect*>(effect);
	for(int i = 0; i < (int)textures.size(); i++) {
		vvd::Release<IDirect3DTexture9*>(textures[i]);
	}
	textures.clear();

	effect = material->effect;
	if(effect)
		effect->AddRef();

	filename = material->filename;

	viewProjHandle = material->viewProjHandle;
	worldMatHandle = material->worldMatHandle;
	cameraPosHandle = material->cameraPosHandle;
	lightPosHandle = material->lightPosHandle;
	lightRangeHandle = material->lightRangeHandle;
	lightColorHandle = material->lightColorHandle;
	lightNumHandle = material->lightNumHandle;
	lightAmbientHandle = material->lightAmbientHandle;
	lightMatrixHandle = material->lightMatrixHandle;
	farPlaneHandle = material->farPlaneHandle;
	nearPlaneHandle = material->nearPlaneHandle;
	projectionHandle =  material->projectionHandle;
	viewMatHandle = material->viewMatHandle;
	depthBiasHandle = material->depthBiasHandle;
	maxLights = material->maxLights;
	alpha = material->alpha;
	translucent = material->translucent;

	textures = material->textures;
	for(int i = 0; i < (int)textures.size(); i++) {
		textures[i]->AddRef();
	}
}
// Gets the range of each effective light
float* Material::GetLightRanges() {
	return &lightRanges[0];
}
// Gets the position of each effective light
D3DXVECTOR4* Material::GetLightPositions() {
	return &lightPositions[0];
}
// Gets the color of each effective light
D3DXVECTOR4* Material::GetLightColors() {
	return &lightColors[0];
}
// Gets the ambient color of each effective cell
D3DXVECTOR4* Material::GetAmbients() {
	return &ambients[0];
}
// Gets the texture of each effective light
IDirect3DTexture9** Material::GetLightTextures() {
	return &lightTextures[0];
}
// Gets the shadow map of each effective light
IDirect3DTexture9** Material::GetShadowMaps() {
	return &shadowMaps[0];
}
// Gets the texture rotation matrix of each effective light
D3DXMATRIX* Material::GetLightTexMatrices() {
	return &lightTexMatrices[0];
}