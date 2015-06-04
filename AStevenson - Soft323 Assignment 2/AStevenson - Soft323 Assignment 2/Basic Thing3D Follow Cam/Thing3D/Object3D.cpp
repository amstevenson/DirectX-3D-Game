
#include "Object3D.h"

//**************************************************************************//
// This code is copyright of Dr Nigel Barlow, lecturer in computing,		//
// University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.			//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//																			//
//	3:	You should correct at least 10% of the typig abd spekking errirs.   //
//**************************************************************************//

//**************************************************************************//
// A class that is heavily based on Nigel's FlatThing3D. It creates an		//
// instance of an object that is derived from an object(.obj) file.			//
// However, in this edition (the second submission) it uses subsets and     //
// uses textures.															//
//**************************************************************************//

//**************************************************************************//
// Constructors.															//
//**************************************************************************//

Object3D::Object3D(ID3D11Device        *pRenderingDevice,
					     ID3D11DeviceContext *pImmediateContext) 
					   : Thing3DAbstract(pRenderingDevice, pImmediateContext)
{
	Init();
}



//**************************************************************************//
// Used by constructors.													//
//**************************************************************************//
void Object3D::Init()
{
   x  = y  = z  = speed = 0.0f;
   ry = rz              = 0.0f;
   rx					= XMConvertToRadians(270.0f);		//Orientation for background.
   sx = sy = sz         = 1.0f;
   initVecDir			= XMVectorSet(1, 0, 0, 0);
   frameTime			= 0.001;


   materialDiffuseColour = XMFLOAT4(1, 1, 1, 1);
}

//**************************************************************************//
// Destructor. 
//**************************************************************************//

Object3D::~Object3D()
{
}	

//***************************************************************************//
// Creates an object from a .obj file. At present the information that		 //
// populates this object is derived from the structure that is returned from //
// the call to the object loader - which requires the setObjMeshFromFile	 //
// method to be called.														 //
//***************************************************************************//
void Object3D::Create(ID3D11Device        *pRenderingDevice,
					     ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr;

	

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
	// create the index buffer
	//
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * g_meshData.numIndices; // ushort * numindices
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;

	//
	// set the index buffers data
	//
	initData.pSysMem = &g_meshData.indexes[0];
	hr = pRenderingDevice->CreateBuffer( &indexBufferDesc, &initData, &pIndexBuffer );

	//
	// create the vertex buffer
	//
    D3D11_BUFFER_DESC vertexBufferDesc;

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	// Note to self: if SimpleFlatThingVertex is replaced by the structure within ObjectLoader.h
	// A pyramid is created...quirky indeed.
	vertexBufferDesc.ByteWidth = sizeof( SimpleFlatThingVertex ) * g_meshData.numVertices; // simpleflatthingvertex * numvertices

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;

	//
	// set the vertex buffers data
	//
	vertexBufferData.pSysMem = &g_meshData.vertices[0];
	hr = pRenderingDevice->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &pVertexBuffer );

	V( hr ) 
}

//**************************************************************************//
// Implementation of an abstract method in the parent class.  Render ourself//
// for whatever we are in this particular implementation, which is a mesh.	//
//**************************************************************************//
void Object3D::RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                        ID3D11DeviceContext *pImmediateContext) 											
{
	HRESULT hr;// might want to remove

	// stride for the index buffer?
	UINT stride = sizeof(SimpleFlatThingVertex); 
	UINT offset = 0;

	// locally define our world matrix
	XMMATRIX matWorldlocal       = matWorld;
	XMMATRIX matViewLocal        = matView;
	XMMATRIX matProjectionLocal  = matProjection;

	XMMATRIX matWorldViewProjectionLocal = matWorldlocal * matViewLocal * matProjectionLocal;

	matWorld = matWorldlocal;

	// update world information
	Thing3DVS_PS_Pair::CBMatrices      cbMatrices;
	cbMatrices.matWorld               = XMMatrixTranspose(matWorldlocal);
	cbMatrices.matWorldViewProjection = XMMatrixTranspose(matWorldViewProjectionLocal);
	pThing3DVS_PS_Pair->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	// Draw the subsets
	if(g_meshData.materialsUsedAmount > 0)
	for(int i = 0; i < g_meshData.materialsUsedAmount -1; i++)
	{
		//
		// Set the input layout
		//
		pImmediateContext->IASetInputLayout( pThing3DVS_PS_Pair->pVertexLayout );
		pImmediateContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
		pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );

		// try and match the instance of the material that is used, with the 
		// actual material surface that is stored independantly.
		// For example: 0 - 1000 faces will use "inside"
		//				1001-2000 will use "outside"
		// but there needs to be a way to use a material more than once, and
		// this is probably the bad way of implementing that...
		int materialToUseIndex = 0;


		for(int j = 0; j < g_meshData.totalMaterials; j++)
		{
			if(g_meshData.materialUsed[i] == g_meshData.materials[j].materialName)
			{
				materialToUseIndex = j;
			}
		}

		Thing3DVS_PS_Pair::CBLighting cbLighting;
		
		cbLighting.LightAmbientColour   = LightAmbientColour;
		cbLighting.lightDiffuseColour   = lightDiffuseColour;
		cbLighting.materialDiffuseColor = g_meshData.materials[materialToUseIndex].difColour;
		cbLighting.hasTexture			= g_meshData.materials[materialToUseIndex].hasTexture;
		cbLighting.vecLightDir          = XMVector3Normalize(vecLightDirection);
		pThing3DVS_PS_Pair->UpdateCBLighting(pImmediateContext, &cbLighting);

		pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair->pSamLinear );

		pThing3DVS_PS_Pair->SetupShadersAndConstantbuffers(pImmediateContext);
			
		if(g_meshData.materials[materialToUseIndex].hasTexture)
		{
			pThing3DVS_PS_Pair->SetTexture(pImmediateContext, g_meshData.materials[materialToUseIndex].textureResource, 
				g_meshData.materials[materialToUseIndex].textureIndex);

			pImmediateContext->PSSetShaderResources( g_meshData.materials[materialToUseIndex].textureIndex, 1, &pThing3DVS_PS_Pair->pTextureResourceView );
		}

		int indexStart = g_meshData.materialIndexStart[i];
		int indexDrawAmount =  g_meshData.materialIndexStart[i+1] - g_meshData.materialIndexStart[i];

		//
		// Draw the object
		//
		pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		pImmediateContext->DrawIndexed( indexDrawAmount, indexStart, 0 );
	}
	else{ // there was no mtl file

		pImmediateContext->IASetInputLayout( pThing3DVS_PS_Pair->pVertexLayout );
		pImmediateContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
		pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );

		int indexStart = 0;
		int indexDrawAmount =  g_meshData.numVertices;

		//
		// Draw the object
		//
		pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		pImmediateContext->DrawIndexed( g_meshData.numIndices, 0, 0 );
	}

	V(hr); // might want to remove 
}

	


	
//**************************************************************************//
// Do something, up to you what.											//
//**************************************************************************//

void Object3D::DoBehaviour()
{
	MoveForward();
	speed *= 0.995;		

	//if( (speed > -0.5 && speed < 0.5) && height > 0)
		//height *= 0.99;
}

//*********************************************************************************//
// Set the object mesh from an .obj file										   //
// This is a work in progress, at present texture normals, vertices and indices	   //
// are returned.																   //
//*********************************************************************************//
void Object3D::SetObjMeshFromFile(std::wstring filename,
								  std::wstring filePath,
								  ID3D11Device        *pRenderingDevice)
{
	ObjectLoader *meshObject;
	
	//
	// Retrieve and initialise the object variables; for example, vertices and indices.
	//
	g_meshData = meshObject->LoadMesh(filename, filePath, pRenderingDevice);
}

XMFLOAT3 Object3D::GetAveragePosition()
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
	for(UINT i = 0; i < g_meshData.numVertices; i++)
	{
		XMVECTOR P = XMLoadFloat3(&g_meshData.vertices[i].Pos);
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

XMFLOAT3 Object3D::GetMinimumPosition()
{
	
	//
	// Taken from Frank Luna's book. 
	// Frustrum culling chapter! 
	//
	XMFLOAT3 vMinf3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	XMFLOAT3 vMaxf3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// Variables to store minumum and maximums of X, Y and Z's.
	XMVECTOR vMin = XMLoadFloat3(&vMinf3);

	//
	// Loop through and determine the minimum's and maximums of
	// the information provided (or in this case, the height map
	// information). 
	//
	for(UINT i = 0; i < g_meshData.numVertices; i++)
	{
		XMVECTOR P = XMLoadFloat3(&g_meshData.vertices[i].Pos);
		vMin = XMVectorMin(vMin, P);
	}

	XMFLOAT3 minimum;

	minimum.x = XMVectorGetX(vMin);
	minimum.y = XMVectorGetY(vMin);
	minimum.z = XMVectorGetZ(vMin);

	return minimum;
}