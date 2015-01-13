
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
	delete g_meshData.indexes;
	delete g_meshData.vertices;
	delete &g_meshData;
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
	indexBufferDesc.ByteWidth = sizeof(USHORT) * g_meshData.numIndices;
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
	vertexBufferDesc.ByteWidth = sizeof( SimpleFlatThingVertex ) * g_meshData.numVertices; 

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
	//
	// Locally define the world matrices. If they are not local, then illegal memory acceses
	// will occur - a mismatch in terms of memory addresses
	//
	XMMATRIX matWorldlocal       = matWorld;
	XMMATRIX matViewLocal        = matView;
	XMMATRIX matProjectionLocal  = matProjection;

	XMMATRIX matWorldViewProjectionLocal = matWorldlocal * matViewLocal * matProjectionLocal;

	matWorld = matWorldlocal;

	//
	// update the world information
	//
	Thing3DVS_PS_Pair::CBMatrices      cbMatrices;
	cbMatrices.matWorld               = XMMatrixTranspose(matWorldlocal);
	cbMatrices.matWorldViewProjection = XMMatrixTranspose(matWorldViewProjectionLocal);
	pThing3DVS_PS_Pair->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	//
	// Update shader lighting
	//
	Thing3DVS_PS_Pair::CBLighting cbLighting;
	cbLighting.lightDiffuseColour = lightDiffuseColour;
	cbLighting.lightDiffuseColour = lightDiffuseColour;
	cbLighting.materialDiffuseColor = materialDiffuseColour;
	cbLighting.vecLightDir          = XMVector3Normalize(vecLightDirection);
	//pThing3DVS_PS_Pair->UpdateCBLighting(pImmediateContext, &cbLighting);

	if (!buffersAndtexturesSet) // Need to make this work
	{
	
		pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair->pSamLinear );
		//pThing3DVS_PS_Pair->SetupShadersAndConstantbuffers(pImmediateContext);

		//
		// Set the input layout
		//
		pImmediateContext->IASetInputLayout( pThing3DVS_PS_Pair->pVertexLayout );

		//
		// set the index and vertex buffers
		//
		UINT stride = sizeof(SimpleFlatThingVertex);
		UINT offset = 0;
		pImmediateContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
		pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );

		//
		// Set primitive topology
		//
		pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

    //
	// Draw the object
	//
	pImmediateContext->DrawIndexed( g_meshData.numIndices, 0, 0 );		
}

	


	
//**************************************************************************//
// Do something, up to you what.											//
//**************************************************************************//

void Object3D::DoBehaviour()
{

}

//*********************************************************************************//
// Set the object mesh from an .obj file										   //
// This is a work in progress, at present texture normals, vertices and indices	   //
// are returned.																   //
//*********************************************************************************//
void Object3D::SetObjMeshFromFile(LPSTR filename,
								  ID3D11Device        *pRenderingDevice)
{
	ObjectLoader *meshObject;
	
	//
	// Retrieve and initialise the object variables; for example, vertices and indices.
	//
	g_meshData = meshObject->LoadMesh(filename);
}