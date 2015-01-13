//******************************************************************************//
// An object loader for creating a struct that contains all(well...some)		//
// of the information inside of an .obj file.									//
// Based off the object loader sample which was created by Dr Nigel Barlow		//
// (email: nigel@soc.plymouth.ac.uk)											//
// This class has been constructed by Adam Stevenson							//
//******************************************************************************//

#ifndef ObjectLoaderH  
#define ObjectLoaderH	 

#include <iostream>
#include <istream>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <xnamath.h>

#include <algorithm> 
#include <functional> 
#include <cctype>

#include <vector>
#include <sstream>
#include <fstream>

class ObjectLoader
{
	public:

		int g_numIndices;		// Need to record the number of indices
								// for drawing mesh.

	public:

		int meshSubsets;

		//**************************************************************************//
		// A simple structure to hold a single vertex.								//
		//**************************************************************************//
		struct SimpleVertex
		{
			XMFLOAT3 Pos;	
			XMFLOAT3 VecNormal;
			XMFLOAT2 TexUV;
		};

		//
		// The mesh information structure will contain all of the information needed to
		// create the object and make it appear using DirectX11. 
		//
		struct meshInformation
		{
			SimpleVertex *vertices;
			USHORT       *indexes;

			// if we have vertex tex coords or normals
			//XMFLOAT3     *vertNorm;
			//XMFLOAT2     *vertTexCoord;

			// the amount of vertices and indices
			USHORT       numVertices;
			USHORT       numIndices;

			// store whether we have any vertex normals or tex coords
			bool hasTexCoords;
			bool hasVertNorms;
		};


	public:

		//**********************************************************************//
		// Constructor.															//
		//**********************************************************************//
		ObjectLoader();

		~ObjectLoader();	//Destructor.

		//**********************************************************************//
		// Load the .obj file													//
		//**********************************************************************//
		meshInformation LoadMesh(LPSTR filename);


	private:

		std::wstring TrimStart(std::wstring s);
		std::wstring TrimEnd(std::wstring s); 

};


#endif