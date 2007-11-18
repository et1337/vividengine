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

#ifndef material_h
#define material_h
#include "vivid.h"
#include <list>
#include <vector>
#include <iostream>
#include <fstream>

#define MAX_LIGHTS 6

class Material {
public:
	Material();
	Material(LPCSTR nFilename); // Loads the specified material file
	Material(const Material& material);
	~Material();
	static std::list<Material*> materials;
	ID3DXEffect* GetEffect(); // Gets the effect loaded from the material file
	bool RequiresLights(LPCSTR technique); // Returns true if the specified technique requires light data
	D3DXHANDLE GetViewProjHandle(); // Gets the effect handle to the view projection matrix
	D3DXHANDLE GetWorldMatHandle(); // Gets the effect handle to the world matrix
	D3DXHANDLE GetCameraPosHandle(); // Gets the effect handle to the camera position
	D3DXHANDLE GetLightPosHandle(); // Gets the effect handle to the light position array
	D3DXHANDLE GetLightRangeHandle(); // Gets the effect handle to the light range array
	D3DXHANDLE GetLightColorHandle(); // Gets the effect handle to the light color array
	D3DXHANDLE GetLightNumHandle(); // Gets the effect handle to the number of lights
	D3DXHANDLE GetLightAmbientHandle(); // Gets the effect handle to the ambient light array
	D3DXHANDLE GetLightTextureHandle(int index); // Gets the effect handle to the specified light texture
	D3DXHANDLE GetLightMatrixHandle(); // Gets the effect handle to the light texture matrix array
	D3DXHANDLE GetFarPlaneHandle(); // Gets the effect handle to the far plane float
	D3DXHANDLE GetNearPlaneHandle(); // Gets the effect handle to the near plane float
	D3DXHANDLE GetProjectionHandle(); // Gets the effect handle to the projection matrix
	D3DXHANDLE GetViewMatHandle(); // Gets the effect handle to the view matrix
	D3DXHANDLE GetShadowMapHandle(int index); // Gets the effect handle to the specified shadow map cube texture
	D3DXHANDLE GetDepthBiasHandle(); // Gets the effect handle to the depth bias variable
	int MaxLights(); // Returns the maximum number of lights the effect can handle
	bool IsAlpha(); // Returns true if this material has alpha information
	bool IsTranslucent(); // Returns true if this material needs access to the back buffer
	bool OverrideTexture(LPCSTR texName, IDirect3DTexture9* tex); // Overrides the specified texture
																  // Returns true if procedure succeeded
	bool OverrideCubeTexture(LPCSTR texName, IDirect3DCubeTexture9* tex); // Overrides the specified cube texture
																		  // Returns true if procedure succeeded
	bool OverrideVector(LPCSTR vecName, D3DXVECTOR4* vector); // Overrides the specified vector
															  // Returns true if procedure succeeded
	bool OverrideFloat(LPCSTR floatName, float num); // Overrides the specified float
													 // Returns true if procedure succeeded
	bool OverrideMatrix(LPCSTR matName, D3DXMATRIX* mat); // Overrides the specified matrix
														  // Returns true if procedure succeeded
	void ParseFile(LPCSTR nFilename); // Loads the specified material file
	void UpdateLightArrays(); // Relays all the light information from the Renderer to the effect
	static float* GetLightRanges(); // Gets the range of each effective light
	static D3DXVECTOR4* GetLightPositions(); // Gets the position of each effective light
	static D3DXVECTOR4* GetLightColors(); // Gets the color of each effective light
	static D3DXVECTOR4* GetAmbients(); // Gets the ambient color of each effective cell
	static IDirect3DTexture9** GetLightTextures(); // Gets the texture of each effective light
	static IDirect3DTexture9** GetShadowMaps(); // Gets the shadow map of each effective light
	static D3DXMATRIX* GetLightTexMatrices(); // Gets the texture rotation matrix of each effective light
protected:
	LPCSTR filename; // Material filename
	ID3DXEffect* effect; // The effect
	std::vector<IDirect3DTexture9*> textures; // List of textures; for memory management
	D3DXHANDLE viewProjHandle; // Effect handle to the view projection matrix
	D3DXHANDLE worldMatHandle; // Effect handle to the world matrix
	D3DXHANDLE cameraPosHandle; // Effect handle to the camera position
	D3DXHANDLE lightPosHandle; // Effect handle to the light position array
	D3DXHANDLE lightRangeHandle; // Effect handle to the light range array
	D3DXHANDLE lightColorHandle; // Effect handle to the light color array
	D3DXHANDLE lightNumHandle; // Effect handle to the number of lights
	D3DXHANDLE lightAmbientHandle; // Effect handle to the ambient light array
	D3DXHANDLE lightMatrixHandle; // Effect handle to the light texture matrix array
	D3DXHANDLE farPlaneHandle; // Effect handle to the far plane float
	D3DXHANDLE nearPlaneHandle; // Effect handle to the near plane float
	D3DXHANDLE projectionHandle; // Effect handle to the projection matrix
	D3DXHANDLE viewMatHandle; // Effect handle to the view matrix
	D3DXHANDLE depthBiasHandle; // Effect handle to the depth bias variable
	static float lightRanges[MAX_LIGHTS]; // The range of each effective light
	static D3DXVECTOR4 lightPositions[MAX_LIGHTS]; // The position of each effective light
	static D3DXVECTOR4 lightColors[MAX_LIGHTS]; // The color of each effective light
	static D3DXVECTOR4 ambients[MAX_LIGHTS]; // The ambient color of each effective cell
	static IDirect3DTexture9* lightTextures[MAX_LIGHTS]; // The texture of each effective light
	static IDirect3DTexture9* shadowMaps[MAX_LIGHTS]; // The shadow map of each effective light
	static D3DXMATRIX lightTexMatrices[MAX_LIGHTS]; // The texture rotation matrix of each effective light
	int maxLights; // Maximum number of lights the effect can handle
	bool alpha; // True if this material has alpha data
	bool translucent; // True if this material needs access to the back buffer
	void LoadEffect(LPCSTR effectFile); // Loads the specified effect file
	void FillOutHandles(); // Attempts to retrieve all the default handles from the effect file
	void SetTo(Material* material);
};

#endif