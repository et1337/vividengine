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

#include "mesh.h"

std::list<Mesh*> Mesh::meshes;

Mesh::Mesh(LPCSTR file) {
	d3dmesh = 0; // Zero all the members
	filename = 0;
	numSubsets = 0;
	radius = 0.0f;
	alpha = false;
	translucent = false;
	LoadMesh(file); // Load x file
	meshes.push_back(this); // Add this object to the static rendering list
}
Mesh::Mesh() {
	d3dmesh = 0; // Zero all the members
	filename = 0;
	numSubsets = 0;
	radius = 0.0f;
	alpha = false;
	translucent = false;
	meshes.push_back(this); // Add this object to the static rendering list
}
Mesh::~Mesh() {
	std::list<Mesh*>::iterator i = meshes.begin(); // Iterate through all the meshes
	while(i != meshes.end()) {
		if(*i == this) {
			meshes.erase(i); // Erase this object from the list so we don't have a stray pointer in the mesh list
			break;
		}
		i++;
	}
	materials.clear();
	vvd::Release<ID3DXMesh*>(d3dmesh); // Release the mesh data
}
Mesh::Mesh(const Mesh& mesh) {
	d3dmesh = mesh.d3dmesh;
	if(d3dmesh)
		d3dmesh->AddRef(); // Make sure d3dmesh isn't deleted when the other mesh is deleted
	filename = mesh.filename;
	numSubsets = mesh.numSubsets;
	transform = mesh.transform;
	materials.clear();
	materials = mesh.materials;
	cells = mesh.cells;
	radius = mesh.radius;
	center = mesh.center;
	alpha = mesh.alpha;
	translucent = mesh.translucent;
}
// Gets the list of materials in this mesh
std::vector<Material>* Mesh::GetMaterials() {
	return &materials;
}
// Gets the list of cells this mesh is inside
std::vector<Cell*>* Mesh::GetCells() {
	return &cells;
}
// Adds a cell to the list of cells this mesh is inside
void Mesh::AddCell(Cell* cell) {
	cells.push_back(cell);
}
// Clears the list of cells this mesh is inside
void Mesh::ClearCells() {
	cells.clear();
}
// Gets the center of the mesh
D3DXVECTOR3 Mesh::GetCenter() {
	return center;
}
// Gets the distance from the center to the outermost vertex of the mesh
float Mesh::GetRadius() {
	D3DXVECTOR3 scale = transform.GetScale();
	return radius * max(max(scale.x, scale.y), scale.z);
}
// Returns true if this mesh has alpha information
bool Mesh::IsAlpha() {
	return alpha;
}
// Returns true if this mesh needs access to the back buffer
bool Mesh::IsTranslucent() {
	return translucent;
}
// Loads the specified X file
void Mesh::LoadMesh(LPCSTR xfile) {
	// Logging stuff
	vvd::Log("");
	std::string msg = "Vivid: Loading X file: ";
	msg += xfile;
	vvd::Log(msg.c_str());

	IDirect3DDevice9* device = vvd::GetDevice();

	// Iterate through meshes to see if this file has already been loaded
	std::list<Mesh*>::iterator i = meshes.begin();
	while(i != meshes.end()) {
		Mesh* mesh = *i;
		if(!(0 == mesh->filename)) {
			if(strcmp(mesh->filename, xfile) == 0 && mesh != this) {
				// This mesh file has already been loaded; use the pre-loaded data
				SetTo(mesh);
				msg = "Vivid: Successfully loaded precached mesh: ";
				msg += xfile;
				vvd::Log(msg.c_str());
				return;
			}
		}
		i++;
	}
	filename = xfile;
	// This file hasn't been loaded, so load the X file
	ID3DXBuffer* adjBuffer; // The adjacency buffer is used for mesh optimization
	ID3DXBuffer* mtrlBuffer; // The material buffer holds the filenames for all the materials
	HRESULT hr = D3DXLoadMeshFromX( // Load X file
		filename,
		D3DXMESH_MANAGED,
		vvd::GetDevice(),
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numSubsets,
		&d3dmesh);

	if(FAILED(hr))
	{
		msg = "Vivid: Failed to load mesh: ";
		msg += filename;
		vvd::Log(msg.c_str());
		exit(1);
	}

	// Weld the vertices
	D3DXWELDEPSILONS epsilons;
	epsilons.Normal = 0.001f;
	epsilons.Position = 0.1f;
	D3DXWeldVertices(d3dmesh, D3DXWELDEPSILONS_WELDPARTIALMATCHES, &epsilons, (DWORD*)adjBuffer->GetBufferPointer(), (DWORD*)adjBuffer->GetBufferPointer(), 0, 0);

	// Optimize the mesh
	hr = d3dmesh->OptimizeInplace(	
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT  |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);

	if(FAILED(hr)) {
		msg = "Vivid: Failed to optimize mesh: ";
		msg += xfile;
		vvd::Log(msg.c_str());
		exit(1);
	} else {
		msg = "Vivid: Successfully optimized mesh: ";
		msg += xfile;
		vvd::Log(msg.c_str());
	}

	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	d3dmesh->GetDeclaration(decl);

	bool hasTangentData = false;
	bool hasNormalData = false;
	for(int i = 0; i < MAX_FVF_DECL_SIZE; i++) {
		if(decl[i].Type == D3DDECLTYPE_UNUSED)
			break;

		if(decl[i].Usage == D3DDECLUSAGE_TANGENT) {
			hasTangentData = true;
		} else if(decl[i].Usage = D3DDECLUSAGE_NORMAL) {
			hasNormalData = true;
		}
	}

	if(!hasTangentData) {
		if(!hasNormalData) {
			D3DXComputeTangentFrameEx(d3dmesh, D3DDECLUSAGE_TEXCOORD, 0,
				D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_TANGENT, 0,
				D3DDECLUSAGE_NORMAL, 0,
				D3DXTANGENT_CALCULATE_NORMALS | D3DXTANGENT_GENERATE_IN_PLACE,
				(DWORD*)adjBuffer->GetBufferPointer(), 0.01f, 0.25f, 0.01f, &d3dmesh, NULL);
		} else {
			D3DXComputeTangentFrameEx(d3dmesh, D3DDECLUSAGE_TEXCOORD, 0,
				D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_TANGENT, 0,
				D3DDECLUSAGE_NORMAL, 0,
				D3DXTANGENT_GENERATE_IN_PLACE,
				(DWORD*)adjBuffer->GetBufferPointer(), 0.01f, 0.25f, 0.01f, &d3dmesh, NULL);
		}
	}

	vvd::Release<ID3DXBuffer*>(adjBuffer); // Release the adjacency buffer; we don't need it

	// Are there any materials in this mesh?
	if( mtrlBuffer != 0 && numSubsets != 0 )
	{
		// Yes, so get the material buffer
		D3DXMATERIAL* nmtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(int i = 0; i < (int)numSubsets; i++)
		{
			// Check if this material has a material filename
			if( nmtrls[i].pTextureFilename != 0 )
			{
				// Yes, so load the material
				Material material(nmtrls[i].pTextureFilename);
				if(material.IsAlpha()) {
					alpha = true;
				}
				if(material.IsTranslucent()) {
					translucent = true;
				}
				materials.push_back(material);
			}
			else
			{
				// No material for the ith subset, load a blank one
				Material material;
				materials.push_back(material);
			}
		}
	}

	// Calculate radius and center vector
	BYTE* v = 0;
	d3dmesh->LockVertexBuffer(0, (void**)&v);

	if(FAILED(D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		d3dmesh->GetNumVertices(),
		D3DXGetFVFVertexSize(d3dmesh->GetFVF()),
		&center,
		&radius))) {

		msg = "Failed to compute bounding sphere for mesh ";
		msg += xfile;
		vvd::Log(msg.c_str());
		exit(1);

	}

	d3dmesh->UnlockVertexBuffer();

	msg = "Vivid: Successfully loaded mesh: ";
	msg += xfile;
	vvd::Log(msg.c_str());
}
// Draws the specified subset of the mesh
void Mesh::DrawSubset(DWORD index) {
	d3dmesh->DrawSubset(index);
}
// Returns the number of faces (triangles) in the mesh
DWORD Mesh::GetNumFaces() {
	return d3dmesh->GetNumFaces();
}
// Returns the number of vertices (points) in the mesh
DWORD Mesh::GetNumVertices() {
	return d3dmesh->GetNumVertices();
}
// Returns the Flexible Vertex Format of the mesh
DWORD Mesh::GetFVF() {
	return d3dmesh->GetFVF();
}
// Returns the number of subsets (materials) in the mesh
int Mesh::GetNumSubsets() {
	return (int)numSubsets;
}
void Mesh::SetTo(Mesh* mesh) {
	vvd::Release<ID3DXMesh*>(d3dmesh);
	d3dmesh = mesh->d3dmesh;
	if(d3dmesh)
		d3dmesh->AddRef(); // Make sure d3dmesh isn't deleted when the other mesh is deleted
	filename = mesh->filename;
	numSubsets = mesh->numSubsets;
	transform = mesh->transform;
	materials = mesh->materials;
	center = mesh->center;
	radius = mesh->radius;
	alpha = mesh->alpha;
	translucent = mesh->translucent;
	cells = mesh->cells;
}