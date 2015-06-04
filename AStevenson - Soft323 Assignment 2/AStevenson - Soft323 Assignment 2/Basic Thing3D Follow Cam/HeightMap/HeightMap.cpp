//**************************************************************************//
// This class is dedicated to creating a Height Map object, that reads in   //
// and creates textures based on a grayscale bitmap image and a colour      //
// bitmap to define the colours that are rendered to the screen.            //
//                                                                          //
// partial credit to h ttp://www.braynzarsoft.net/index.php?p=D3D11HM#still //
// as parts of this were aided by following this amazing persons tutorial   //
//**************************************************************************//

#include "HeightMap.h"

//**************************************************************************//
// Constructor																//
//**************************************************************************//
HeightMap::HeightMap(ID3D11Device        *pRenderingDevice,
					     ID3D11DeviceContext *pImmediateContext) : Thing3DAbstract(
						 pRenderingDevice, pImmediateContext)
{
    // Inherits constructor from Thing3DAbstract (Init())
	// but in addition to this, the division value that is used for determining
	// the height of the hills is set here too
	mapScale = 10;

}

//**************************************************************************//
// Destructor																//
//**************************************************************************//
HeightMap::~HeightMap()
{
	
}

// 
// Loading in the height map
//
void HeightMap::HeightMapLoad(char* filename)
{
	hmInfo = loadFile(filename); // hmInfo is the local variable that
								 // contains all the information about files

	// global height/widths for readability
	globTerrainHeight = hmInfo.terrainheight;
	globTerrainWidth  = hmInfo.terrainwidth;
}

//
// Set the height mappings for the HeightMap
// This is required (in my case), because the map is
// initially invisible until it is rotated by a 180 degree angle
// (flipped over). This probably isnt a bug, but solving it simply
// requires loading in an inverted HeightMap.
// 
void HeightMap::SetInvertedMappings(char* filename)
{
	HeightMapInfo newPosYMap;
	newPosYMap = loadFile(filename);

	// Set the local height map information to have the new
	// mapping information for the Y coordinates.
	int rows  = newPosYMap.terrainheight;
	int cols  = newPosYMap.terrainwidth;
	int index = 0;

	for(USHORT j = 0; j < rows; j++)
	{
		for(USHORT i = 0; i < cols; i++)
		{
			// v is the array of vertices for the height map information
			// for which the y values of pos are used to calculate the
			// height.
			index = (rows * j) + i;

			v[index].Pos.y = newPosYMap.heightMap[index].y;
		}
	}
}

//
// Return a HeightMapInfo structure containing the information of the
// heightmap that has been passed to it.
//
HeightMap::HeightMapInfo HeightMap::loadFile(char* filename)
{
	HeightMapInfo localHMInfo;

	FILE *filePointer; // The current position in the file
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	int imageSize, index;
	unsigned char height;

	bool fileOpenFailure = false;

	// open the file
	filePointer = fopen(filename, "rb");
	if(filePointer == NULL)
		fileOpenFailure = true;

	if(!fileOpenFailure)
	{
		// Read bitmaps Header
		fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePointer);

		// Read the info header
		fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePointer);

		// Get the width and height (width and length) of the image
		int locTerrainWidth =  localHMInfo.terrainwidth  = bitmapInfoHeader.biWidth;
		int locTerrainHeight = localHMInfo.terrainheight = bitmapInfoHeader.biHeight;

		// For each pixel, there is a RBG value. Byte, byte, byte...
		imageSize = locTerrainWidth * locTerrainHeight * 3;

		// Initialise the array which stores the image data
		unsigned char* bitmapImage = new unsigned char[imageSize];

		// Set the file pointer to the beginning of the image data
		fseek(filePointer, bitmapFileHeader.bfOffBits, SEEK_SET);

		// Store image data in bitmapImage
		fread(bitmapImage, 1, imageSize, filePointer);

		// Close file
		fclose(filePointer);

		// Initialise the heightmap array (store the vertices of the terrain)
		localHMInfo.heightMap = new XMFLOAT3[locTerrainWidth * locTerrainHeight];

		// This variable is usually different. However only one value is needed for the height
		// so we will only be using one!
		int k = 0;

		// This number divides the terrain height, so that the mountainous points do not
		// appear to be overly spikey.
		float heightFactor = mapScale; // user defined (although it needs playing around with to perfect)

		// Read the image data into our heightMap Array
		for(int j = 0; j < locTerrainHeight; j++)
		{
			for(int i = 0; i < locTerrainWidth; i++)
			{
				height = bitmapImage[k];

				index = ( locTerrainWidth * j) + i;

				localHMInfo.heightMap[index].x = (float)i;
				localHMInfo.heightMap[index].y = (float)height / heightFactor;
				localHMInfo.heightMap[index].z = (float)j;

				k+=3;
			}
		}
		
		delete [] bitmapImage;
		bitmapImage = 0;

	}

	return localHMInfo;
}

void HeightMap::LoadColourMap(char* filename)
{
	HRESULT hr = S_OK;

	FILE *filePointer; // The current position in the file
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	int imageSize, index;
	unsigned char height;

	// open the file
	bool fileOpenFailure = false;

	// open the file
	filePointer = fopen(filename, "rb");
	if(filePointer == NULL)
		fileOpenFailure = true;

	// Read bitmaps Header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePointer);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePointer);

	// Get the width and height (width and length) of the image
	int textureWidth = hmInfo.textureWidth  = bitmapInfoHeader.biWidth;
	int textureHeight = hmInfo.textureHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = hmInfo.textureWidth * hmInfo.textureHeight * 3;

	// Allocate memory for the bitmap image data.
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Move to the beginning of the bitmap data.
	fseek(filePointer, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	fread(bitmapImage, 1, imageSize, filePointer);

	fclose(filePointer);

	// index for the file buffer
	int k = 0;

	// Initialise the pointer array of r/b/g colours
	hmInfo.textureColour = new XMFLOAT4[textureWidth * textureHeight];
	
	// go through the data recieved by reading the bitmap image
	// and allocate the red, green and blue values of the image
	// to the local Height Map information structure
	for(int j=0; j < textureHeight; j++)
	{
		for(int i=0; i < textureWidth; i++)
		{
			index = (textureHeight * j) + i;

			hmInfo.textureColour[index].x = (float)bitmapImage[k]      /5;
			hmInfo.textureColour[index].y = (float)bitmapImage[k + 1] / 5;
			hmInfo.textureColour[index].z = (float)bitmapImage[k + 2] / 3;

			k+=3;
		}
	}

	delete [] bitmapImage;
	bitmapImage = 0;

	V(hr);
}

//
// Initiate the scene by creating the vertices, creating the texture coordinates
// computing the normals(well...sort of... this misses out a few edges), setting
// the vertex/index buffers and finally drawing it all
//
void HeightMap::initScene( ID3D11Device        *pRenderingDevice,
					      ID3D11DeviceContext *pImmediateContext)
{
	int rows = globTerrainHeight;
	int cols = globTerrainWidth;

	// Create the grid
	numVertices = rows * cols;
	numFaces    = (rows - 1) * (cols - 1) * 2;

	v = std::vector<HeightMapVertex> (numVertices);

	int index;
	float height   = 0;

	for(DWORD i = 0; i < rows; ++i)
	{
		for(DWORD j = 0; j < cols; ++j)
		{
			index = cols * j + i;

			height = hmInfo.heightMap[index].y;

			v[index].Pos = hmInfo.heightMap[index];
			v[index].VecNormal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			
			v[index].colour.x = hmInfo.textureColour[index].x / 2;
			v[index].colour.y = hmInfo.textureColour[index].y / 4;
			v[index].colour.z = hmInfo.textureColour[index].z / 4;
			v[index].colour.w = 1.0f;


		}
	}

	// calculate indicies
	std::vector<DWORD> indices(numFaces * 3);

	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;


	for(DWORD i = 0; i < rows-1; i++)
	{
		for(DWORD j = 0; j < cols-1; j++)
		{
			// compute the indices for the two triangles that make up a quad

			indices[k] = i * cols + j; // Bottom Left of quad
			v[i * cols + j].TexUV = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			indices[k+1] = i * cols + j + 1; // Bottol right of quad
			v[i * cols + j + 1].TexUV = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k+2] = (i + 1)* cols + j; // Top left of quad
			v[(i + 1)*cols + j].TexUV = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k + 3] = (i + 1) * cols + j; // Top left of quad
			v[(i + 1)* cols + j].TexUV = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k + 4] = i * cols + j + 1; // Bottom right of quad
			v[i * cols + j + 1].TexUV = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k + 5] = (i + 1) * cols + j + 1; // Top right of quad
			v[(i + 1) * cols + j + 1].TexUV = XMFLOAT2(texVIndex + 1.0f, texVIndex + 0.0f);

			k += 6; // two triangles per quad, so +6

			texUIndex++;
		}
		texUIndex = 0;
		texVIndex ++;
	}
	
	std::vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Used to get vectors (sides) from the position of the verts
	float vecX, vecY, vecZ;

	//Two edges of our triangle
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//Compute face normals
	for(int i = 0; i < numFaces; ++i)
	{
		//Get the vector describing one edge of our triangle (edge 0,2)
		vecX = v[indices[(i*3)]].Pos.x - v[indices[(i*3)+2]].Pos.x;
		vecY = v[indices[(i*3)]].Pos.y - v[indices[(i*3)+2]].Pos.y;
		vecZ = v[indices[(i*3)]].Pos.z - v[indices[(i*3)+2]].Pos.z;		
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

		//Get the vector describing another edge of our triangle (edge 2,1)
		vecX = v[indices[(i*3)+2]].Pos.x - v[indices[(i*3)+1]].Pos.x;
		vecY = v[indices[(i*3)+2]].Pos.y - v[indices[(i*3)+1]].Pos.y;
		vecZ = v[indices[(i*3)+2]].Pos.z - v[indices[(i*3)+1]].Pos.z;		
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

		//Cross multiply the two edge vectors to get the un-normalized face normal
		XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
		tempNormal.push_back(unnormalized);			//Save unormalized normal (for normal averaging)
	}

	float sum[3];
	float length;
	int count = 0;
	index = 0;

	for(int z = 0; z < 10; z++) // repeat ten times for extra accuracy
	// Computing the normals
	for(DWORD i = 1; i < rows -1; ++i)
	{
		for(DWORD j = 1; j < cols -1; ++j)
		{
			// Initialise the sum
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (rows-1)) + (i-1);

				sum[0] += tempNormal[index].x;
				sum[1] += tempNormal[index].y;
				sum[2] += tempNormal[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (cols-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (rows-1)) + i;

				sum[0] += tempNormal[index].x;
				sum[1] += tempNormal[index].y;
				sum[2] += tempNormal[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (rows-1)))
			{
				index = (j * (cols-1)) + (i-1);

				sum[0] += tempNormal[index].x;
				sum[1] += tempNormal[index].y;
				sum[2] += tempNormal[index].z;
				count++;
			}

			// Upper right face.
			if((i < (cols-1)) && (j < (rows-1)))
			{
				index = (j * (cols-1)) + i;

				sum[0] += tempNormal[index].x;
				sum[1] += tempNormal[index].y;
				sum[2] += tempNormal[index].z;
				count++;
			}
			
			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * rows) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			v[index].VecNormal.x = (sum[0] / length);
			v[index].VecNormal.x = (sum[1] / length);
			v[index].VecNormal.x = (sum[2] / length);
		}
	}
	
	//**************************************************************************//
	//						Create vertex / index buffers						//
	//**************************************************************************//
	// Creating the index / vertex buffers
	HRESULT hr;

	//
	// First, since I dont use the Height Map information again after the
	// operations that proceed this, delete it
	//
	delete [] hmInfo.heightMap;

	//
	// Create the input layout
	//
	UINT numElements = ARRAYSIZE( thing3DAvstractVertexLayoutDescription );
	
	hr = pRenderingDevice->CreateInputLayout( thing3DAvstractVertexLayoutDescription, 
		                                      numElements, 
											  pThing3DVS_PS_Pair->pVertexShaderBuffer->GetBufferPointer(),
                                              pThing3DVS_PS_Pair->pVertexShaderBuffer->GetBufferSize(), 
											  &pVertexLayout );

	V(hr);

	//
    // Set the input layout
    //
	pImmediateContext->IASetInputLayout( pVertexLayout );

	//
	// Index buffer
	//
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage			= D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth		= sizeof(DWORD) * numFaces * 3;
	indexBufferDesc.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags	= 0;
	indexBufferDesc.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &indices[0];

	// create index buffer
	hr = pRenderingDevice->CreateBuffer(&indexBufferDesc, &initData, &hMIndexBuffer);

	//
	// Vertex buffer
	//
	D3D11_BUFFER_DESC vertexBufferDesc;

	vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth		= sizeof( HeightMapVertex ) * numVertices; // simpleflatthingvertex * numvertices
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	vertexBufferData.pSysMem = &v[0];

	hr = pRenderingDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &hMVertexBuffer);
}

void HeightMap::RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                      ID3D11DeviceContext *pImmediateContext) 											
{
	HRESULT hr;// might want to remove

	// stride for the index buffer?
	UINT stride = sizeof(HeightMapVertex); 
	UINT offset = 0;

	// locally define our world matrix
	XMMATRIX matWorldlocal       = matWorld;
	XMMATRIX matViewLocal        = matView;
	XMMATRIX matProjectionLocal  = matProjection;

	XMMATRIX matWorldViewProjectionLocal = matWorldlocal * matViewLocal * matProjectionLocal;

	matWorld = matWorldlocal;

	// update world information
	Thing3DVS_PS_Pair::CBMatrices        cbMatrices;
	cbMatrices.matWorld               =  XMMatrixTranspose(matWorldlocal);
	cbMatrices.matWorldViewProjection =  XMMatrixTranspose(matWorldViewProjectionLocal);
	pThing3DVS_PS_Pair->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	//
	// Set the input layout
	//
	pImmediateContext->IASetInputLayout( pThing3DVS_PS_Pair->pVertexLayout );
	pImmediateContext->IASetIndexBuffer( hMIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
	pImmediateContext->IASetVertexBuffers( 0, 1, &hMVertexBuffer, &stride, &offset );


	// update lighting
	Thing3DVS_PS_Pair::CBLighting cbLighting;
		
	cbLighting.LightAmbientColour   = LightAmbientColour;
	cbLighting.lightDiffuseColour   = lightDiffuseColour;
	cbLighting.materialDiffuseColor = materialDiffuseColour;
	cbLighting.vecLightDir          = XMVector3Normalize(vecLightDirection);
	pThing3DVS_PS_Pair->UpdateCBLighting(pImmediateContext, &cbLighting);
	pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair->pSamLinear );
	pThing3DVS_PS_Pair->SetupShadersAndConstantbuffers(pImmediateContext);
		
	pImmediateContext->PSSetShaderResources( 1, 1, &pThing3DVS_PS_Pair->pTextureResourceView );

	// drawing the faces!
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pImmediateContext->DrawIndexed( numFaces * 3, 0 , 0);
}

void HeightMap::Create(ID3D11Device        *pRenderingDevice,
					                        ID3D11DeviceContext *pImmediateContext) 											
{
	// left blank for now
}

void HeightMap::DoBehaviour()
{
	// left blank for now
}

void HeightMap::setScaleHeight(int scaleHeight)
{
	// set the height for the map scalings
	mapScale = scaleHeight;
}

XMFLOAT3 HeightMap::getAveragePosition()
{
	
	//
	// Taken from Frank Luna's book. 
	// Frustrum culling chapter! 
	//
	XMFLOAT3 vMinf3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	XMFLOAT3 vMaxf3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// Variables to store minumum and maximums of X, Y and Z's.
	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	//
	// Loop through and determine the minimum's and maximums of
	// the information provided (or in this case, the height map
	// information). 
	//
	for(UINT i = 0; i < numVertices; i++)
	{
		XMVECTOR P = XMLoadFloat3(&v[i].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	XMFLOAT3 center;
	XMStoreFloat3(&center, 0.5f*(vMin+vMax));
	
	// 
	//XMStoreFloat3(&v->extent, 0.5f*(vMax-vMin)); // This may be useful, but
												   // for now it is not required

	return center;

}

float HeightMap::getHeight(float x, float z)
{
    int left, top;
	
	// Add 0.5 to x and z. This is used in conjunction with floorf.
	// Floorf rounds down a value to the nearest whole. 
	// So 273.5 + 0.5 = 274    and with floorf = 274 
	//    273.4 + 0.5 = 273.9, and with floorf = 273 
	x+= 0.5; z+= 0.5;
	left = (int) floorf(x);
	top  = (int) floorf(z);

	// if we have any negative values, turn them into positives
	if(left < 0) left *= -1;
	if(top  < 0) top  *= -1;

    // Simply calculate the height by checking the indexes
	// of the width/height of the height map
	float returnHeight;
	returnHeight = v[top * globTerrainWidth + left].Pos.y;
	
	return returnHeight;
}
