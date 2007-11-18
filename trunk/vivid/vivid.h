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

#ifndef vivid_h
#define vivid_h
#define vivid_version 0.01f
#define vivid_version_str "0.01"
#define WIN32_LEAN_AND_MEAN
#define logging
#include <d3dx9.h>
#include <dinput.h>
#include <iostream>
#include <fstream>
#include <mmsystem.h>
#include "rendertarget.h"

namespace vvd {
	//
	// Windows functionality
	//
	static HWND hwnd; // Window
	static HINSTANCE hInstance; // Application instance
	bool Init( // Initializes Vivid
		HINSTANCE hInstance, // application instance.
		int width, int height, // windows size
		LPCSTR title, // window title
		bool fullscreen, // windowed (false)or full screen (true).
		D3DFORMAT format, // back buffer format; only for fullscreen
		UINT presentationInterval); // vsync; D3DPRESENT_INTERVAL_ONE if vsync is on, D3DPRESENT_INTERVAL_IMMEDIATE if not
	void DeInit(); // DeInit the whole shebang
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // Handles messages from Windows
	static int width; // Window width
	static int height; // Window height
	int GetWidth(); // Gets the window width
	int GetHeight(); // Gets the window height

	//
	// Convenience functions
	//
	static float timeDelta; // The amount of time (in seconds) since the last frame
	static double lastTime; // The time stamp of the last frame
	std::string stringconv(float num); // Converts a float to an std::string
	std::string stringconv(int num); // Converts an int to an std::string
	std::string stringconv(double num); // Converts a double to an std::string
	D3DCUBEMAP_FACES GetCubeFace(int index); // Returns the cube map face that corresponds to the specified index
	// Releases a direct3d object with error checking; returns the number of references to the object that still exist
	template<class T> int Release(T& t)
	{
		int refcount = 0;
		if( t )
		{
			refcount = t->Release();
			t = 0;
		}
		return refcount;
	}
	// Deletes an object with error checking
	template<class T> void Delete(T& t)
	{
		if( t )
		{
			delete t;
			t = 0;
		}
	}
	void Update(); // Updates input and time delta; Call this function once every frame
	float GetDelta(); // Gets the time (in seconds) since the last frame
	void Alert(LPCSTR msg); // MessageBox wrapper function

	//
	// Graphics functionality
	//
	static IDirect3DDevice9* device; // Graphics card
	static D3DPRESENT_PARAMETERS d3dpp; // D3D Present Parameters
	static D3DCAPS9 caps; // Graphics card capabilities
	D3DCAPS9* GetDeviceCaps(); // Gets the graphics card capabilities
	D3DPRESENT_PARAMETERS* GetPresentParameters(); // Gets the present parameters
	bool CheckDeviceState(); // Checks if the graphics card is lost; if so, tries to recover it
	IDirect3DDevice9* GetDevice(); // Gets a pointer to the graphics card

	//
	// Input functionality
	//
	static IDirectInput8* DInput; // DirectInput object
	static IDirectInputDevice8* keyboard; // Keyboard object
	static char keyboardState[256]; // Keyboard state
	static float keyPressTime[256]; // The amount of time each key has been pressed
	static float minKeyPressTime[256]; // The minimum amount of time between key presses
	static IDirectInputDevice8* mouse; // Mouse object
	static DIMOUSESTATE2 mouseState; // Mouse state
	static bool lostKeyBoard; // True if we've lost keyboard focus
	static bool lostMouse; // True if we've lost mouse focus
	void InitInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags); // Inits Vivid Input
	void DeInitInput(); // DeInits Vivid Input
	void pollInputs(); // Updates the state of the keyboard and mouse
	bool keyDown(char key); // Checks whether a certain key (indicated by char key) is pressed
	void SetMinKeyPressTime(char key, float time); // Sets the minimum amount of time between key presses for a given key
	// Checks if a mouse button is pressed. 0 = left mouse button, 1 = scroll wheel button, 2 = right mouse button
	bool mouseButtonDown(int button);
	float mouseDX(); // Returns the distance along the X axis that the mouse has moved since the last poll
	float mouseDY(); // Returns the distance along the Y axis that the mouse has moved since the last poll
	float mouseDZ(); // Returns the distance the scroll wheel has scrolled since the last poll
	//
	// Logging functionality
	//
	static std::ofstream* logFile; // Log file for logging
	// Opens the log file specified; overwrites it if it already exists, or creates a new one if it doesn't exist
	// Only opens the log file if logging is #defined
	void OpenLog(LPCSTR file);
	void CloseLog(); // Closes the log file and deletes the ofstream if logging is #defined
	// Writes a line in the log file if logging is #defined
	// WARNING: There isn't any error checking! Make sure the log file is allocated and open before calling this function
	void Log(LPCSTR msg);
	// Writes a number to the log file if logging is #defined
	// WARNING: There isn't any error checking! Make sure the log file is allocated and open before calling this function
	void Log(float num);
	void Log(int num);
}

#endif