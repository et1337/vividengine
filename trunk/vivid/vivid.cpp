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

#include "vivid.h"

// Initializes Vivid
bool vvd::Init(
	HINSTANCE nHInstance,
	int nwidth, int nheight,
	LPCSTR title,
	bool fullscreen,
	D3DFORMAT format,
	UINT presentationInterval)
{
	Log("Vivid: Initializing...");
	//
	// Create the main application window.
	//
	hInstance = nHInstance;

	width = nwidth;
	height = nheight;

	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc; // the message processing function
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance; // application instance
	wc.hIcon         = LoadIcon(hInstance, IDI_APPLICATION); // icon
	wc.hCursor       = LoadCursor(0, IDC_CROSS); // cursor
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = title; // Window title

	if( !RegisterClass(&wc) ) 
	{
		Log("Vivid: RegisterClass() failed!");
		return false;
	}
	Log("Vivid: Creating window...");
	hwnd = ::CreateWindow(title, title, 
		WS_OVERLAPPEDWINDOW,
		0, 0, width, height,
		0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 

	if( !hwnd )
	{
		Log("Vivid: CreateWindow() failed!");
		return false;
	}

	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);

	Log("Vivid: Initializing D3D...");
	//
	// Init D3D
	//

	HRESULT hr = 0;

	//
	// Create the IDirect3D9 object
	//

	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !d3d9 )
	{
		Log("Vivid: Direct3DCreate9() failed!");
		return false;
	}

	//
	// Check for hardware vertex processing
	//

	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//
	// Fill out the D3DPRESENT_PARAMETERS structure
	//
 
	d3dpp.BackBufferWidth            = width;
	d3dpp.BackBufferHeight           = height;
	if(fullscreen)
	{
		d3dpp.BackBufferFormat           = format;
	}
	else
	{
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	}
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = !fullscreen;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = presentationInterval;

	// Output D3D Present Parameters to the log file
	Log("");
	Log("D3D Present Parameters:");
	std::string string = "Width: ";
	string += stringconv(width);
	Log(string.c_str());
	string = "Height: ";
	string += stringconv(height);
	Log(string.c_str());
	if(fullscreen)
		string = "Fullscreen: yes";
	else
		string = "Fullscreen: no";
	Log(string.c_str());
	if(d3dpp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE)
		string = "VSync: no";
	else
		string = "VSync: yes";
	Log(string.c_str());
	Log("");

	Log("Vivid: Creating D3D Device...");
	//
	// Create the device
	//

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		D3DDEVTYPE_HAL,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
	    &d3dpp,             // present parameters
	    &device);            // return created device

	if( FAILED(hr) )
	{
		Log("Vivid: Failed to create device, trying 16-bit depth buffer...");
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hwnd,
			vp,
			&d3dpp,
			&device);

		if( FAILED(hr) )
		{
			Release<IDirect3D9*>(d3d9);
			Log("Vivid: CreateDevice() failed!");
			return false;
		}
	}

	// Release d3d9, we don't need it anymore
	Release<IDirect3D9*>(d3d9);
	
	// Init Vivid Input
	if(fullscreen) {
		InitInput(DISCL_EXCLUSIVE | DISCL_FOREGROUND, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	} else {
		InitInput(DISCL_EXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	// Init the RenderTarget class
	RenderTarget::Init();

	Log("Vivid: Successfully initialized");
	Log("");
	
	// And we're done!
	return true;
}
// Handles messages from Windows
LRESULT CALLBACK vvd::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
// Gets the window width
int vvd::GetWidth() {
	return width;
}
// Gets the window height
int vvd::GetHeight() {
	return height;
}
// MessageBox wrapper; a convenience function
void vvd::Alert(LPCSTR msg) {
	::MessageBox(0, msg, "Vivid", MB_OK);
}
// Updates input and time delta; call this function once every frame
void vvd::Update() {
	// Calculate the time (in seconds) since the last frame
	double currTime  = (double)timeGetTime();
	timeDelta = (float)((currTime - lastTime) * 0.001f);
	lastTime = currTime;

	// Update the inputs
	pollInputs();
}
// Gets the time (in seconds) since the last frame
float vvd::GetDelta() {
	return timeDelta;
}
// Converts a float to an std::string
std::string vvd::stringconv(float num) {
	std::string line;
	char buffer[50] = "";
	gcvt((double)num, 33, buffer);
	line += buffer;
	return line;
}
// Converts an int to an std::string
std::string vvd::stringconv(int num) {
	std::string line;
	char buffer[25] = "";
	itoa(num, buffer, 10);
	line += buffer;
	return line;
}
// Converts a double to an std::string
std::string vvd::stringconv(double num) {
	std::string line;
	char buffer[50] = "";
	gcvt(num, 33, buffer);
	line += buffer;
	return line;
}
// Returns the cube map face that corresponds to the specified index
D3DCUBEMAP_FACES vvd::GetCubeFace(int index) {
	switch(index) {
		case 0:
			return D3DCUBEMAP_FACE_POSITIVE_X;
			break;
		case 1:
			return D3DCUBEMAP_FACE_NEGATIVE_X;
			break;
		case 2:
			return D3DCUBEMAP_FACE_POSITIVE_Y;
			break;
		case 3:
			return D3DCUBEMAP_FACE_NEGATIVE_Y;
			break;
		case 4:
			return D3DCUBEMAP_FACE_POSITIVE_Z;
			break;
		case 5:
			return D3DCUBEMAP_FACE_NEGATIVE_Z;
			break;
	}
	return D3DCUBEMAP_FACE_POSITIVE_X;
}
// Gets the graphics card capabilities
D3DCAPS9* vvd::GetDeviceCaps() {
	return &caps;
}
// Gets the present parameters
D3DPRESENT_PARAMETERS* vvd::GetPresentParameters() {
	return &d3dpp;
}
// Checks if the graphics card is lost. If so, tries to reacquire it
bool vvd::CheckDeviceState() {
	HRESULT hr;
	hr = device->TestCooperativeLevel();
	if(hr == D3DERR_DEVICELOST) { // device is lost and cannot be reset yet
		Log("Vivid: Lost device...");
		Sleep(250); // wait a bit so we don't burn through cycles for no reason
		return false;
	} else if(hr == D3DERR_DEVICENOTRESET){ // lost but we can reset it now
		hr = device->Reset(&d3dpp);
		if(FAILED(hr)) {
			return false;
		} else {
			Log("Vivid: Reacquired device");
		}
	}

	if(lostKeyBoard || lostMouse)
		return false;

	return true;
}
// Gets a pointer to the graphics card
IDirect3DDevice9* vvd::GetDevice() {
	return device;
}
// Releases Vivid resources back to Windows
void vvd::DeInit() {
	Log("");
	Log("Vivid: De-initializing...");
	Log("Vivid: Releasing graphics card...");
	std::string msg = "Vivid: Graphics card reference count: ";
	msg += stringconv(Release<IDirect3DDevice9*>(device));
	Log(msg.c_str());
	DeInitInput();
	Log("Vivid: Closing window...");
	::DestroyWindow(hwnd);
	Log("Vivid: Successfully de-initialized");
	CloseLog();
}
// Initializes DirectInput, the keyboard, and the mouse
void vvd::InitInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags) {
	Log("Vivid: Initializing input...");
	// Init DirectInput
	if(FAILED(DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8, 
		(void**)&DInput, 0)))
	{
		Log("Vivid: DirectInput8Create() failed!");
		exit(1);
	}

	// Clear keyboard states
	for(int i = 0; i < 256; i++) {
		keyPressTime[i] = 0.0f;
		minKeyPressTime[i] = 0.0f;
	}

	// Init keyboard

	if(FAILED(DInput->CreateDevice(GUID_SysKeyboard, &keyboard, 0))) {
		Log("Vivid: DInput->CreateDevice(keyboard) failed!");
		exit(1);
	}
	if(FAILED(keyboard->SetDataFormat(&c_dfDIKeyboard))) {
		Log("Vivid: keyboard->SetDataFormat() failed!");
		exit(1);
	}
	if(FAILED(keyboard->SetCooperativeLevel(hwnd, keyboardCoopFlags))) {
		Log("Vivid: keyboard->SetCooperativeLevel failed!");
		exit(1);
	}
	if(FAILED(keyboard->Acquire())) {
		Log("Vivid: keyboard->Acquire() failed!");
		exit(1);
	}

	// Init mouse

	if(FAILED(DInput->CreateDevice(GUID_SysMouse, &mouse, 0))) {
		Log("Vivid: DInput->CreateDevice(mouse) failed!");
		exit(1);
	}
	if(FAILED(mouse->SetDataFormat(&c_dfDIMouse2))) {
		Log("Vivid: mouse->SetDataFormat() failed!");
		exit(1);
	}
	if(FAILED(mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE))) {
		Log("Vivid: mouse->SetCooperativeLevel() failed!");
		exit(1);
	}
	if(FAILED(mouse->Acquire())) {
		Log("Vivid: mouse->Acquire() failed!");
		exit(1);
	}
	Log("Vivid: Successfully initialized input");
}
// Releases DirectInput, the keyboard, and mouse
void vvd::DeInitInput() {
	Log("Vivid: De-initializing input...");
	Release<IDirectInput8*>(DInput);
	Release<IDirectInputDevice8*>(keyboard);
	Release<IDirectInputDevice8*>(mouse);
	Log("Vivid: Successfully de-initialized input");
}
// Updates the keyboardState and mouseState
void vvd::pollInputs()
{
	// Poll keyboard
	if(FAILED(keyboard->GetDeviceState(sizeof(keyboardState), (void**)&keyboardState))) {
		Log("Vivid: Lost keyboard, attempting to reacquire");
		// Keyboard lost, zero out keyboard data structure.
		ZeroMemory(keyboardState, sizeof(keyboardState));
		lostKeyBoard = true;
		Sleep(250);
		 // Try to acquire for next time we poll.
		keyboard->Acquire();
	} else {
		lostKeyBoard = false;
	}

	// Update keyPressTime
	for(int i = 0; i < 256; i++) {
		if(keyPressTime[i] < minKeyPressTime[i])
			keyPressTime[i] += vvd::GetDelta();
	}

	// Poll mouse
	if(FAILED(mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mouseState))) {
		Log("Vivid: Lost mouse, attempting to reacquire");
		// Mouse lost, zero out mouse data structure.
		ZeroMemory(&mouseState, sizeof(mouseState));
		lostMouse = true;
		Sleep(250);
		// Try to acquire for next time we poll.
		mouse->Acquire(); 
	} else {
		lostMouse = false;
	}
}
// Checks if a certain key (indicated by char key) is pressed
bool vvd::keyDown(char key)
{
	bool pressed = (keyboardState[key] & 0x80) != 0;

	bool withinMinKeyPressTime = true;
	if(pressed) {
		if(keyPressTime[key] >= minKeyPressTime[key]) {
			keyPressTime[key] = 0.0f;
		} else {
			withinMinKeyPressTime = false;
		}
	}

	return pressed && withinMinKeyPressTime;
}
// Sets the minimum amount of time between key presses for a given key
void vvd::SetMinKeyPressTime(char key, float time) {
	minKeyPressTime[key] = time;
	keyPressTime[key] = time;
}
// Checks if a mouse button is pressed. 0 = left mouse button, 1 = scroll wheel button, 2 = right mouse button
bool vvd::mouseButtonDown(int button)
{
	return (mouseState.rgbButtons[button] & 0x80) != 0;
}
// Returns the distance along the X axis that the mouse has moved since the last poll
float vvd::mouseDX()
{
	return (float)mouseState.lX;
}
// Returns the distance along the Y axis that the mouse has moved since the last poll
float vvd::mouseDY()
{
	return (float)mouseState.lY;
}
// Returns the distance the scroll wheel has scrolled since the last poll
float vvd::mouseDZ()
{
	return (float)mouseState.lZ;
}
// Opens the log file specified; overwrites it if it already exists, or creates a new one if it doesn't exist
// Only opens the log file if logging is #defined
void vvd::OpenLog(LPCSTR file) {
#ifdef logging
	logFile = new std::ofstream(file);
	if(logFile->fail())
		Alert("Failed to open log file!");
	Log("---------------------------");
	std::string versionLine = "Vivid Engine v";
	versionLine += vivid_version_str;
	Log(versionLine.c_str());
	Log("---------------------------");
	Log("Copyright (C) 2006 Evan Todd");
	Log("");
#endif
}
// Closes the log file and deletes the ofstream if logging is #defined
void vvd::CloseLog() {
#ifdef logging
	logFile->close();
	Delete<std::ofstream*>(logFile);
#endif
}
// Writes a line in the log file if logging is #defined
// WARNING: There isn't any error checking! Make sure the log file is allocated and open before calling this function
void vvd::Log(LPCSTR msg) {
#ifdef logging
	*logFile << msg << "\n";
#endif
}
// Writes a number to the log file if logging is #defined
// WARNING: There isn't any error checking! Make sure the log file is allocated and open before calling this function
void vvd::Log(float num) {
#ifdef logging
	*logFile << num << "\n";
#endif
}
void vvd::Log(int num) {
#ifdef logging
	*logFile << num << "\n";
#endif
}