//******************************************************************************//
// An object loader for creating a struct that contains all(well...some)		//
// of the information inside of an .obj file.									//
// Based off the object loader sample which was created by Dr Nigel Barlow		//
// (email: nigel@soc.plymouth.ac.uk)											//
// 																				//
//******************************************************************************//

#include "ObjectLoader.h"

//**************************************************************************//
// Constructor																//
//**************************************************************************//
ObjectLoader::ObjectLoader()
{
	g_numIndices = 0;
	meshSubsets = 0;
}

//**************************************************************************//
// Destructor																//
//**************************************************************************//
ObjectLoader::~ObjectLoader()
{

}


//**************************************************************************//
// Trim the start of a c++ string											//
//**************************************************************************//
std::wstring ObjectLoader::TrimStart(std::wstring s) 
{
        s.erase(s.begin(), std::find_if(s.begin(), 
			    s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

//**************************************************************************//
// Trim the end of a c++ string												//
//**************************************************************************//
std::wstring ObjectLoader::TrimEnd(std::wstring s) 
{
        s.erase(std::find_if(s.rbegin(), s.rend(), 
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}


ObjectLoader::meshInformation ObjectLoader::LoadMesh(LPSTR filename)
{
	std::wifstream          fileStream;
	std::wstring            line;
	std::vector <XMFLOAT3>  vectorVertices(0);
	std::vector <USHORT>    vectorIndices(0);
	std::vector <XMFLOAT2>  vertexTextures(0);
	std::vector <XMFLOAT3>  vertexNormals(0);
	bool hasVertexTexture = false;
	bool hasVertexNormal  = false;

	fileStream.open(filename);

	while(std::getline(fileStream, line))
	{
		line = TrimStart(line);
		
		//
		// Read in the vertices
		//
		if (line.compare(0, 2, L"v ") == 0)  //"v space"
		{
			WCHAR first[5];
			XMFLOAT3 vertex;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%2s%f%f%f", first, &vertex.x, &vertex.y, &vertex.z); 

			vectorVertices.push_back(vertex);
		}

		//
		// Read in the vertex textures
		//
		if (line.compare(0, 2, L"vt") == 0)
		{
			// This part does not work yet, there are conflicts with already existing shaders.
			// I will come back to this. 
			WCHAR first[5];
			XMFLOAT2 vertexTexture;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f", first, &vertexTexture.x, &vertexTexture.y);

			vertexTextures.push_back(vertexTexture);

			//
			// We have vertex textures
			// 
			hasVertexTexture = true;
		}

		//
		// Read in the vertex normals
		//
		if (line.compare(0, 2, L"vn") == 0)
		{
			WCHAR first[5];
			XMFLOAT3 vertexNormal;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f%f", first, &vertexNormal.x,
									  &vertexNormal.y,&vertexNormal.z);

			vertexNormals.push_back(vertexNormal);

			//
			// We have vertex normals
			//
			hasVertexNormal = true;
		}

		//
		// Read in the face
		//
		if (line.compare(0, 2, L"f ") == 0)  //"f space"
		{
			WCHAR first[5];
			WCHAR slash1[5];
			WCHAR slash2[5];
			WCHAR slash3[5];
			WCHAR slash4[5];
			WCHAR slash5[5];
			WCHAR slash6[5];

			UINT v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3; 

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%2s%d%1s%d%1s%d%d%1s%d%1s%d%d%1s%d%1s%d", first, 
										&v1, slash1, &vt1, slash2, &vn1, 
				                        &v2, slash3, &vt2, slash4, &vn2, 
										&v3, slash5, &vt3, slash6, &vn3); 

			vectorIndices.push_back(v1-1);	// Check this carefully; see below
			vectorIndices.push_back(v2-1);
			vectorIndices.push_back(v3-1);
		}
	}

	//
	// Create the mesh that will be returned later on
	//
	meshInformation mesh;

	//
	// Allocate mesh vertex positions
	//
	mesh.numVertices = (USHORT) vectorVertices.size();
	mesh.vertices    = new SimpleVertex[mesh.numVertices];
	for (int i = 0; i < mesh.numVertices; i++)
	{
		mesh.vertices[i].Pos.x = vectorVertices[i].x;
		mesh.vertices[i].Pos.y = vectorVertices[i].y;
		mesh.vertices[i].Pos.z = vectorVertices[i].z;
	}

	//
	// Allocate mesh vertex normals
	//
	if(hasVertexNormal == true)
	{
		USHORT numVertexNormals = (USHORT) vertexNormals.size();
		for(int i = 0; i < numVertexNormals; i++)
		{
			mesh.vertices[i].VecNormal.x = vertexNormals[i].x;
			mesh.vertices[i].VecNormal.y = vertexNormals[i].y;
			mesh.vertices[i].VecNormal.z = vertexNormals[i].z;
		}

		mesh.hasVertNorms = true;
	}

	//
	// Allocate mesh vertex textures
	//
	/*
	if(hasVertexTexture == true)
	{
		USHORT numVertexTextures = (USHORT) vertexTextures.size();
		for(int i = 0; i < numVertexTextures; i++)
		{
			mesh.vertices[i].TexUV.x = vertexTextures[i].x;
			mesh.vertices[i].TexUV.y = vertexTextures[i].y;
		}

		mesh.hasTexCoords = true;
	}
	*/

	/* 
	 need to address the above...causes all sorts of memory(heap) errors
	 almost definitely something to do with the shader
	 I believe that I need to create another structure that works with the shaders, so that
	 the shaders do not conflict with one another...
	 */

	//
	// Allocate mesh indices
	//
	mesh.numIndices = (USHORT) vectorIndices.size();
	mesh.indexes    = new USHORT[mesh.numIndices];
	for (int i = 0; i < mesh.numIndices; i++)
		mesh.indexes[i] = vectorIndices[i];
	
	return mesh;
}
