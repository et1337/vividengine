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

#ifndef world_h
#define world_h
#include "vivid.h"
#include "mesh.h"
#include <vector>
#include "light.h"

class Mesh;
class Light;

// The World is divided into Cells, which contain Meshes and Lights
struct Cell {
public:
	Cell();
	bool Contains(Mesh* mesh); // Returns true if the cell contains the specified mesh
	bool Contains(Light* light); // Returns true if the cell contains the specified light
	void AddMesh(Mesh* mesh); // Adds the specified mesh to the cell
	void AddLight(Light* light); // Adds the specified light to the cell
	void Clear(); // Clears the lists of lights and meshes
	void SetAmbientColor(D3DXVECTOR3 nColor); // Sets the ambient color of the cell
	D3DXVECTOR4 GetAmbientColor(); // Gets the ambient color of the cell
	std::vector<Light*>* GetLights(); // Gets the list of lights in the cell
	std::vector<Mesh*>* GetMeshes(); // Gets the list of meshes in the cell
private:
	D3DXVECTOR4 color; // Ambient color
	std::vector<Mesh*> meshes; // List of meshes in the cell
	std::vector<Light*> lights; // List of lights in the cell
};

class World {
public:
	World();
	World(float nWorldSizeX, float nWorldSizeY, float nWorldSizeZ, float nCellSizeX, float nCellSizeY, float nCellSizeZ);
	void Update(); // Assigns lights and meshes to cells; called once every frame
	void SetAmbientColor(D3DXVECTOR3 pos, D3DXVECTOR3 color); // Sets the ambient color of the cell at the position specified
	std::vector<Cell>* GetCells(); // Gets the list of cells in the world
private:
	Cell* GetCell(D3DXVECTOR3 pos); // Gets the cell at the position specified
	void SetSize(float nWorldSizeX, float nWorldSizeY, float nWorldSizeZ, float nCellSizeX, float nCellSizeY, float nCellSizeZ);
	float worldSizeX; // World size along the X axis
	float worldSizeY; // World size along the Y axis
	float worldSizeZ; // World size along the Z axis
	float cellSizeX; // Cell size along the X axis
	float cellSizeY; // Cell size along the Y axis
	float cellSizeZ; // Cell size along the Z axis
	int numCellsX; // Number of cells along the X axis
	int numCellsY; // Number of cells along the Y axis
	int numCellsZ; // Number of cells along the Z axis
	int numCells; // Total number of cells
	std::vector<Cell> cells; // List of cells in the world
};

#endif