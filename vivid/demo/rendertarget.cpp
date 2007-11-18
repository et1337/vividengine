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

#include "rendertarget.h"
std::list<RenderTarget*> RenderTarget::targets; // Static list of targets for rendering
RenderTarget RenderTarget::defaultTarget; // Default render target; points to the back buffer for rendering
RenderTarget RenderTarget::backBufferAccess; // Back buffer target for access in the pixel shader

RenderTarget::RenderTarget() {
	renderTex = 0;
	renderSurface = 0;
	drawToScreen = false;
	targets.push_back(this);
}
RenderTarget::RenderTarget(int nwidth, int nheight, D3DFORMAT format) {
	IDirect3DDevice9* device = vvd::GetDevice();
	width = nwidth; height = nheight;

	// Create the texture
	if(FAILED(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &renderTex, NULL))) {
		// Log the error and exit
		vvd::Log("Failed to create render target");
		std::string msg = "Width: ";
		msg += vvd::stringconv(width);
		msg += " Height: ";
		msg += vvd::stringconv(height);
		vvd::Log(msg.c_str());
		exit(1);
	}

	// Get the surface from the texture
	renderTex->GetSurfaceLevel(0, &renderSurface);

	// By default, don't draw this RenderTarget to the screen
	drawToScreen = false;

	targets.push_back(this);
}
RenderTarget::RenderTarget(const RenderTarget& rt) {
	renderTex = rt.renderTex;
	if(renderTex)
		renderTex->AddRef();
	renderSurface = rt.renderSurface;
	if(renderSurface)
		renderSurface->AddRef();
	drawToScreen = rt.drawToScreen;
	width = rt.width;
	height = rt.height;
}
RenderTarget::~RenderTarget() {
	std::list<RenderTarget*>::iterator i = targets.begin();
	while(i != targets.end()) {
		if(*i == this) {
			targets.erase(i);
			break;
		}
		i++;
	}
	vvd::Release<IDirect3DTexture9*>(renderTex);
	vvd::Release<IDirect3DSurface9*>(renderSurface);
}
// Initializes the render target
void RenderTarget::Init(int nwidth, int nheight, D3DFORMAT format) {
	IDirect3DDevice9* device = vvd::GetDevice();

	width = nwidth; height = nheight;

	// Release the texture and surface just in case
	vvd::Release<IDirect3DTexture9*>(renderTex);
	vvd::Release<IDirect3DSurface9*>(renderSurface);

	// Create the texture
	if(FAILED(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &renderTex, NULL))) {
		// Log the error and exit
		vvd::Log("Failed to create render target");
		std::string msg = "Width: ";
		msg += vvd::stringconv(width);
		msg += " Height: ";
		msg += vvd::stringconv(height);
		vvd::Log(msg.c_str());
		exit(1);
	}

	// Get the surface from the texture
	renderTex->GetSurfaceLevel(0, &renderSurface);

	// By default, don't draw this RenderTarget to the screen
	drawToScreen = false;
}
// Called in vvd::Init(); initializes defaultTarget to the back buffer
void RenderTarget::Init() {
	IDirect3DDevice9* device = vvd::GetDevice();
	device->GetRenderTarget(0, &(defaultTarget.renderSurface));
	defaultTarget.SetWidth(vvd::GetWidth());
	defaultTarget.SetHeight(vvd::GetHeight());
	backBufferAccess.Init(vvd::GetWidth(), vvd::GetHeight(), vvd::GetPresentParameters()->BackBufferFormat);
}
// Updates the back buffer access texture
void RenderTarget::Update() {
	Update(&defaultTarget);
}
void RenderTarget::Update(RenderTarget* rt) {
	IDirect3DDevice9* device = vvd::GetDevice();
	device->StretchRect(rt->GetSurface(), NULL, backBufferAccess.GetSurface(), NULL, D3DTEXF_NONE);
}
// Gets the rendering surface
IDirect3DSurface9* RenderTarget::GetSurface() {
	return renderSurface;
}
// Gets the rendering texture
IDirect3DTexture9* RenderTarget::GetTexture() {
	return renderTex;
}
// If drawToScreen is true, this RenderTarget will show up on the right side of the screen
void RenderTarget::SetDrawToScreen(bool nDrawToScreen) {
	drawToScreen = nDrawToScreen;
}
// Returns true if this RenderTarget should be drawn to the screen
bool RenderTarget::DrawToScreen() {
	return drawToScreen;
}
// Gets the width of the texture/surface
int RenderTarget::GetWidth() {
	return width;
}
// Gets the height of the texture/surface
int RenderTarget::GetHeight() {
	return height;
}
// Sets the width of the texture/surface
void RenderTarget::SetWidth(int nwidth) {
	width = nwidth;
}
// Sets the height of the texture/surface
void RenderTarget::SetHeight(int nheight) {
	height = nheight;
}
void RenderTarget::SetTexture(IDirect3DTexture9* nRenderTex, int nwidth, int nheight) {
	renderTex = nRenderTex;
	width = nwidth;
	height = nheight;
}
void RenderTarget::SetSurface(IDirect3DSurface9* nRenderSurface) {
	renderSurface = nRenderSurface;
}