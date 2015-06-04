#ifndef HeightMapH 
#define HeightMapH	

#include <iostream>
#include <istream>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <xnamath.h>

#include <vector>

#include <algorithm> 
#include <functional> 
#include <cctype>

#include "../Thing3D/Thing3DAbstract.h"
#include "../ObjectLoader/ObjectLoader.h"

class HeightMap : public Thing3DAbstract
{

	//
	// Public variables
	//
	public:

		UINT numFaces;
		UINT numVertices;

		// global variables for the HeightMap
		int globTerrainWidth;		// Width of heightmap
		int globTerrainHeight;		// Height (Length) of heightmap
		int mapScale;

		//
		// The center is used. It has the middle positions
		// of the collective x y and z vertices
		//
		XMFLOAT3		center;
		XMFLOAT3		extent;

		ID3D11Buffer			   *hMVertexBuffer;
		ID3D11Buffer			   *hMIndexBuffer;
		ID3D11ShaderResourceView   *ptxtResourceViewInShader;
		ID3D11InputLayout		   *pVertexLayout;

		struct HeightMapInfo{	   // Heightmap structure
			XMFLOAT3  *heightMap;	// Array to store terrain's vertex positions
			XMFLOAT4  *textureColour;
			//XMFLOAT2  *tex;
			//XMFLOAT3  *VecNormal;
			int		  terrainwidth;
			int		  terrainheight;
			int		  textureWidth;
			int       textureHeight;
		};

		HeightMapInfo hmInfo;

		struct HeightMapVertex
		{
			XMFLOAT3 Pos;	
			XMFLOAT3 VecNormal;
			XMFLOAT2 TexUV;
			XMFLOAT4 colour;
		};

		// Reusing a simple mesh. Structure with Pos, Tex and Norm
		std::vector<HeightMapVertex> v;


	//
	// Public methods
	//
	public:

		HeightMap(ID3D11Device        *pRenderingDevice,
			      ID3D11DeviceContext *pImmediateContext);  // Constructor

		~HeightMap();	//Destructor.

		//void Init();

		void HeightMapLoad(char* filename);

		void LoadColourMap(char* filename);

		void initScene( ID3D11Device        *pRenderingDevice,
					   ID3D11DeviceContext *pImmediateContext);

        virtual void Create(ID3D11Device        *pRenderingDevice,
					        ID3D11DeviceContext *pImmediateContext);

	   virtual void RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                         ID3D11DeviceContext *pImmediateContext);	//Draw yourself 
																						//for whatever you are
	   virtual void DoBehaviour();

	   // Frank Luna's code for getting the height for a specific section of the map
	   // and has been slightly edited by Nigel too
	   virtual float getHeight(float x, float z);

	   virtual XMFLOAT3 getAveragePosition();

	   void HeightMap::SetInvertedMappings(char* filename);

	   void setScaleHeight(int mapScaleSize);

	private:

		virtual HeightMapInfo loadFile(char* filename);
};

#endif