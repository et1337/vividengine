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

#ifndef imagefilter_h
#define imagefilter_h

#include "vivid.h"
#include "rendertarget.h"

class ImageFilter {
public:
	ImageFilter(LPCSTR nFilename);
	ImageFilter(const ImageFilter& imgfilter);
	~ImageFilter();
	void Filter(IDirect3DSurface9* surface);
	static void Init();
protected:
	LPCSTR filename;
	ID3DXEffect* effect;
};

#endif