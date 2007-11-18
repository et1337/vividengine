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

#ifndef rendertarget_h
#define rendertarget_h
#include "vivid.h"
#include <list>

class RenderTarget {
public:
	RenderTarget();
	RenderTarget(int nwidth, int nheight, D3DFORMAT format);
	RenderTarget(const RenderTarget& rt);
	~RenderTarget();
	static void Init(); // Called in vvd::Init(); initializes defaultTarget to the back buffer
	static void Update(); // Updates the back buffer access texture
	static void Update(RenderTarget* rt);
	void Init(int nwidth, int nheight, D3DFORMAT format); // Initializes the render target
	static RenderTarget defaultTarget; // Default render target; points to the back buffer
	static RenderTarget backBufferAccess; // Back buffer target; for access in the pixel shader
	static std::list<RenderTarget*> targets; // Static list of targets for rendering
	IDirect3DSurface9* GetSurface(); // Gets the rendering surface
	IDirect3DTexture9* GetTexture(); // Gets the rendering texture
	void SetDrawToScreen(bool nDrawToScreen); // If drawToScreen is true, this RenderTarget
											  // will show up on the right side of the screen
	bool DrawToScreen(); // Returns true if this RenderTarget should be drawn to the screen
	int GetWidth(); // Gets the width of the texture/surface
	int GetHeight(); // Gets the height of the texture/surface
	void SetWidth(int nwidth); // Sets the width of the texture/surface
	void SetHeight(int nheight); // Sets the height of the texture/surface
	void SetTexture(IDirect3DTexture9* nRenderTex, int nwidth, int nheight);
	void SetSurface(IDirect3DSurface9* nRenderSurface);
protected:
	IDirect3DTexture9* renderTex; // Render texture
	IDirect3DSurface9* renderSurface; // Render surface
	bool drawToScreen; // True if the RenderTarget should be drawn to the screen
	int width; // Width of the texture/surface
	int height; // Height of the texture/surface
};

#endif