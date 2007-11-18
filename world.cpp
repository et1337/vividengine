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

#include "world.h"
Cell::Cell() {
	color.x = 0.0f; color.y = 0.0f; color.z = 0.0f; color.w = 1.0f;
}
// Returns true if the cell contains the specified mesh
bool Cell::Contains(Mesh* mesh) {
	// Iterate through the list and check if we've already added this mesh
	for(int i = 0; i < (int)meshes.size(); i++) {
		if(meshes[i] == mesh)
			return true; // Yes
	}
	return false; // No
}
// Returns true if the cell contains the specified light
bool Cell::Contains(Light* light) {
	// Iterate through the list and check if we've already added this light
	for(int i = 0; i < (int)lights.size(); i++) {
		if(lights[i] == light)
			return true; // Yes
	}
	return false; // No
}
// Adds the specified mesh to the cell
void Cell::AddMesh(Mesh* mesh) {
	if(!Contains(mesh)) {
		meshes.push_back(mesh);
		mesh->AddCell(this); // Add this cell to the mesh's list
	}
}
// Adds the specified light to the cell
void Cell::AddLight(Light* light) {
	if(!Contains(light)) {
		lights.push_back(light);
		light->AddCell(this); // Add this cell to the light's list
	}
}
// Clears the lists of lights and meshes
void Cell::Clear() {
	for(int i = 0; i < (int)meshes.size(); i++)
		meshes[i]->ClearCells(); // Clear the mesh's list

	for(int i = 0; i < (int)lights.size(); i++)
		lights[i]->ClearCells(); // Clear the light's list

	meshes.clear();
	lights.clear();
}
// Sets the ambient color of the cell
void Cell::SetAmbientColor(D3DXVECTOR3 nColor) {
	color.x = nColor.x; color.y = nColor.y; color.z = nColor.z; color.w = 1.0f;
}
// Gets the ambient color of the cell
D3DXVECTOR4 Cell::GetAmbientColor() {
	return color;
}
// Gets the list of lights in the cell
std::vector<Light*>* Cell::GetLights() {
	return &lights;
}
// Gets the list of meshes in the cell
std::vector<Mesh*>* Cell::GetMeshes() {
	return &meshes;
}

World::World() {}
World::World(float nWorldSizeX, float nWorldSizeY, float nWorldSizeZ, float nCellSizeX, float nCellSizeY, float nCellSizeZ) {
	SetSize(nWorldSizeX, nWorldSizeY, nWorldSizeZ, nCellSizeX, nCellSizeY, nCellSizeZ);
}
void World::SetSize(float nWorldSizeX, float nWorldSizeY, float nWorldSizeZ, float nCellSizeX, float nCellSizeY, float nCellSizeZ) {
	// Set the world and cell sizes
	worldSizeX = nWorldSizeX; worldSizeY = nWorldSizeY; worldSizeZ = nWorldSizeZ;
	cellSizeX = nCellSizeX; cellSizeY = nCellSizeY; cellSizeZ = nCellSizeZ;

	// Calculate the number of cells along the X, Y, and Z axes
	numCellsX = (int)(worldSizeX / cellSizeX);
	numCellsY = (int)(worldSizeY / cellSizeY);
	numCellsZ = (int)(worldSizeZ / cellSizeZ);

	// Calculate the total number of cells
	numCells = numCellsX * numCellsZ * numCellsY;

	// Initialize the cell list
	cells.clear();
	for(int i = 0; i < numCells; i++) {
		Cell cell;
		cells.push_back(cell);
	}
}
// Assigns lights and meshes to cells; called once every frame
void World::Update() {
	// Clear the cells
	for(int i = 0; i < (int)cells.size(); i++) {
		cells[i].Clear();
	}

	// Assign lights to cells
	std::list<Light*>::iterator j = Light::lights.begin();
	while (j != Light::lights.end()) {
		Light* light = *j;
		D3DXVECTOR3 pos = light->GetPosition();
		GetCell(pos)->AddLight(light);
		j++;
	}

	// Assign meshes to cells
	std::list<Mesh*>::iterator k = Mesh::meshes.begin();
	while(k != Mesh::meshes.end()) {
		Mesh* mesh = *k;
		D3DXVECTOR3 pos = mesh->transform.GetPosition();
		GetCell(pos)->AddMesh(mesh);
		k++;
	}
}
// Sets the ambient color of the cell at the position specified
void World::SetAmbientColor(D3DXVECTOR3 pos, D3DXVECTOR3 color) {
	GetCell(pos)->SetAmbientColor(color);
}
// Gets the list of cells in the world
std::vector<Cell>* World::GetCells() {
	return &cells;
}
// Gets the cell at the position specified
Cell* World::GetCell(D3DXVECTOR3 pos) {
	float ix = ((float)worldSizeX / 2.0f) + pos.x;
	ix = max(-worldSizeX / 2.0f, ix); ix = min(worldSizeX / 2.0f, ix);

	float iy = ((float)worldSizeY / 2.0f) + pos.y;
	iy = max(-worldSizeY / 2.0f, iy); iy = min(worldSizeY / 2.0f, iy);

	float iz = ((float)worldSizeZ / 2.0f) - pos.z;
	iz = max(-worldSizeZ / 2.0f, iz); iz = min(worldSizeZ / 2.0f, iz);

	int intx = (int)(ix / cellSizeX);
	int inty = (int)(iy / cellSizeY);
	int intz = (int)(iz / cellSizeZ);

	// Calculate the index
	int index = (intz * numCellsX) + intx + (inty * numCellsX * numCellsZ);

	// Make sure the index is inside the list's bounds
	if(index < (int)cells.size() && index >= 0) {
		return &cells[index];
	}

	// The index was outside the list's bounds; return null
	return NULL;
}