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

		struct SurfaceMaterial
		{
			// name
			std::wstring materialName;

			// colours
			XMFLOAT4 difColour;
			XMFLOAT4 ambColour;
			int		 illum;
			XMFLOAT4 specularColour;

			// textures
			//int texArrayIndex;
			bool hasTexture;
			int						  textureIndex;
			ID3D11ShaderResourceView* textureResource;

			// transparency
			int      transparency;
			bool transparent;

		};

		//
		// The mesh information structure will contain all of the information needed to
		// create the object and make it appear using DirectX11. 
		//
		struct meshInformation
		{
			SimpleVertex    *vertices;
			UINT            *indexes;

			// materials
			SurfaceMaterial *materials;
			int				totalMaterials;

			// counter information
			UINT            numVertices;
			UINT            numIndices;

			// occurance of groups
			int				*subsetIndexStart;
			UINT			subsetAmount;

			// occurance of materials
			int				*materialIndexStart;
			UINT			materialsUsedAmount;
			std::wstring    *materialUsed; // to find surface material
		};



		//std::vector <SurfaceMaterial> materials;


	public:

		//**********************************************************************//
		// Constructor.															//
		//**********************************************************************//
		ObjectLoader();

		~ObjectLoader();	//Destructor.

		//**********************************************************************//
		// Load the .obj file													//
		//**********************************************************************//
		meshInformation LoadMesh(std::wstring filename, std::wstring filePath, ID3D11Device *pRenderingDevice);


	private:

		std::wstring TrimStart(std::wstring s);
		std::wstring TrimEnd(std::wstring s); 

};


#endif