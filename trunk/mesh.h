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

#ifndef mesh_h
#define mesh_h
#include "vivid.h"
#include "transform.h"
#include "material.h"
#include "world.h"

struct Cell;

class Mesh {
public:
	static std::list<Mesh*> meshes; // Static list of meshes; used for caching and rendering
	Mesh(LPCSTR file); // Loads the x file specified
	Mesh();
	Mesh(const Mesh& vMesh);
	~Mesh();
	void LoadMesh(LPCSTR xfile); // Loads the x file specified
	void DrawSubset(DWORD index); // Draws the specified subset of the mesh
	DWORD GetNumFaces(); // Returns the number of faces (triangles) in the mesh
	DWORD GetNumVertices(); // Returns the number of vertices (points) in the mesh
	DWORD GetFVF(); // Returns the Flexible Vertex Format of the mesh
	int GetNumSubsets(); // Returns the number of subsets (materials) in the mesh
	Transform transform; // World transform
	std::vector<Material>* GetMaterials(); // Gets the list of materials in this mesh
	std::vector<Cell*>* GetCells(); // Gets the list of cells this mesh is inside
	D3DXVECTOR3 GetCenter(); // Gets the center of the mesh
	float GetRadius(); // Gets the distance from the center to the outermost vertex of the mesh
	bool IsAlpha(); // Returns true if this mesh has alpha information
	bool IsTranslucent(); // Returns true if this mesh needs access to the back buffer
	void AddCell(Cell* cell); // Adds a cell to the list of cells this mesh is inside
	void ClearCells(); // Clears the list of cells this mesh is inside
protected:
	ID3DXMesh* d3dmesh; // Actual mesh data
	LPCSTR filename; // Mesh file name; used for caching
	DWORD numSubsets; // Number of subsets (materials) in the mesh
	std::vector<Material> materials; // List of materials; loaded from X file
	std::vector<Cell*> cells; // List of cells this mesh is inside
	D3DXVECTOR3 center; // The center of the mesh
	float radius; // The distance from the center to the outermost vertex of the mesh
	bool alpha; // True if this mesh has alpha information
	bool translucent; // True if this mesh needs access to the back buffer
	void SetTo(Mesh* mesh);
};

#endif