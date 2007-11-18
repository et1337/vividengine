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

#include "imagefilter.h"

ImageFilter::ImageFilter(LPCSTR nFilename) {
	filename = nFilename;
	effect = 0;

	ID3DXBuffer* errorBuffer = 0;
	IDirect3DDevice9* device = vvd::GetDevice();

	// Create the effect
	HRESULT hr = D3DXCreateEffectFromFile(
		device,
		filename,
		0,
		0,
		D3DXSHADER_DEBUG | D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE,
		0,
		&effect,
		&errorBuffer);

	if(FAILED(hr)) {
		std::string msg = "Vivid: Failed to load effect file: ";
			msg += filename;
			vvd::Log(msg.c_str());
		// Output any errors to the log file
		if(errorBuffer) {
			vvd::Log((LPCSTR)errorBuffer->GetBufferPointer());
			vvd::Release<ID3DXBuffer*>(errorBuffer);
		}
		exit(1);
	}
}
ImageFilter::ImageFilter(const ImageFilter& imgfilter) {
	effect = imgfilter.effect;
	if(effect)
		effect->AddRef();
	filename = imgfilter.filename;
}
ImageFilter::~ImageFilter() {
	vvd::Release<ID3DXEffect*>(effect);
}
void ImageFilter::Filter(IDirect3DSurface9* surface) {}