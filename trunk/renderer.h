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

#ifndef renderer_h
#define renderer_h
#include "vivid.h"
#include "mesh.h"
#include "rendertarget.h"
#include "imagefilter.h"

class Renderer {
public:
	Renderer();
	~Renderer();
	Renderer(const Renderer& renderer);
	void SetCameraPosition(D3DXVECTOR3* nCameraPos); // Sets the position of the camera
	void SetCameraPosition(float x, float y, float z); // Sets the position of the camera
	void SetCameraRotation(D3DXVECTOR3* look, D3DXVECTOR3* up); // Sets the rotation of the camera;
																// Look defines where the camera points
																// Up defines the rotation on the Z axis
	void SetCullMode(DWORD nCullMode); // Three options: D3DCULL_NONE, D3DCULL_CW, and D3DCULL_CCW
	void SetFillMode(DWORD nFillMode); // Three options: D3DFILL_POINT, D3DFILL_WIREFRAME, and D3DFILL_SOLID
	void SetBackgroundColor(D3DCOLOR nBackgroundColor); // Sets the color of the background (in hex code)
	void SetBackgroundTexture(IDirect3DTexture9* nBackgroundTex); // Sets the background texture
	void SetProjectionMatrix(D3DXMATRIX* nProjectionMat); // Sets the projection matrix
	void SetProjection(float nFovY, float aspect, float zn, float zf); // Generates the projection matrix
																	  // based on the values given
	void SetViewport(D3DVIEWPORT9* nView); // Sets the viewport
	void SetViewport(DWORD x, DWORD y, DWORD width, DWORD height, float minZ, float maxZ); // Generates
																// the viewport based on the values given
	void CameraLookAt(D3DXVECTOR3* target); // Points the camera at the position specified
	void CameraLookAt(float x, float y, float z); // Points the camera at the position specified
	void SetRenderTarget(int index, RenderTarget* nTarget); // Sets the rendertarget
	void SetTechnique(LPCSTR nTechnique); // Sets the desired effect technique
	void SetDepthBias(float nBias); // Sets the depth bias
	void Draw(); // Draws the entire scene
	void Draw(std::vector<Cell*>* cells); // Draws the specified cells
	void DrawShadows(); // Draws the entire scene's shadows
	void ClearScreen(); // Clears the render target to the background color
	void EnableFilter(ImageFilter* imgfilter);
	void DisableFilter(ImageFilter* imgfilter);
protected:
	void DrawMesh(Mesh* mesh); // Draws a single mesh
	UINT SetMaterial(Material* material, Mesh* mesh); // Sets the material handles;
													// returns the number of passes required by the effect
	void DrawRenderTargets(); // Draws the render targets on the right side of the screen
	void CompileLightArray(std::vector<Cell*>* cells, Material* material); // Compiles light data from the cells provided
	void BeginScene(); // Initializes rendering; called before rendering the scene
	void EndScene(); // Ends rendering; called after rendering the scene
	D3DXVECTOR3 GetCubeMapLook(int i); // Gets the specified look vector for cube map rendering
	D3DXVECTOR3 GetCubeMapUp(int i); // Gets the specified up vector for cube map rendering
	D3DXVECTOR3 cameraPos; // Position of the camera
	D3DXVECTOR3 look; // The camera look vector; defines camera rotation
	D3DXVECTOR3 up; // The camera up vector; defines camera rotation
	D3DXMATRIX cameraMat; // The camera (view) matrix
	D3DXMATRIX projectionMat; // The projection matrix
	DWORD cullMode; // Cull mode: D3DCULL_NONE, D3DCULL_CW, or D3DCULL_CCW; defaults to D3DCULL_CCW
	DWORD fillMode; // Fill mode: D3DFILL_WIREFRAME, D3DFILL_POINT, or D3DFILL_SOLID; defaults to D3DFILL_SOLID
	D3DCOLOR backgroundColor; // Color of the background
	IDirect3DTexture9* backgroundTex; // Background texture
	D3DVIEWPORT9 view; // The viewport
	std::vector<RenderTarget*> renderTargets; // RenderTarget to render on; defaults to the back buffer
	std::list<ImageFilter*> filters;
	bool Contains(ImageFilter* imgfilter);
	std::list<ImageFilter*>::iterator GetFilter(ImageFilter* imgfilter);
	LPCSTR technique; // Technique; the renderer will activate this technique if an effect supports it
	float nearPlane; // The near clip plane
	float farPlane; // The far clip plane
	float fovY; // Field of vision; for projection matrix generation
	float depthBias; // Depth bias
};

#endif