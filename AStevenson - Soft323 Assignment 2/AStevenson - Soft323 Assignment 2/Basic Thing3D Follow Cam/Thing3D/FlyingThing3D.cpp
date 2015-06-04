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
// A class that implements and renders a FlyingThing3D.						//
// In this instance the main character of the game will be an instance of	//
// this particular object.													//
//**************************************************************************//

#include "FlyingThing3D.h"
#include <exception>	//Std stuff; no ".h"

//******************************************************************//
// Sounds.															//
//******************************************************************//

Thing3DVS_PS_Pair *pThing3DVS_PS_Pair_No_Lighting;
Thing3DVS_PS_Pair *pThing3DVS_PS_Pair_Tail_Wag;

NigSoundManager soundManager;

DWORD sound1State;
NigSound wingFlap(&soundManager); 

//**************************************************************************//
// Constructor.  Force the parent constructor to execute, then create the   //
// default shaders.															//
//**************************************************************************//
FlyingThing3D::FlyingThing3D(ID3D11Device        *pRenderingDevice,
			     ID3D11DeviceContext *pImmediateContext) 
			   : Thing3DAbstract(pRenderingDevice, pImmediateContext)
{
}


//**************************************************************************//
// Destructor.   We should release some Dx stuff here.   It is NOT complete!//
//**************************************************************************//

FlyingThing3D::~FlyingThing3D()
{
	//
	// Delete the shaders
	//
	delete pThing3DVS_PS_Pair_No_Lighting;

	//
	// Delete meshes
	//
	g_MeshSky.Destroy();
	g_MeshWing1.Destroy();
	g_MeshWing2.Destroy();

	//
	// Delete sound manager variables
	//
	soundManager.~NigSoundManager();
}						


//**************************************************************************//
// Create method.  It uses the VERTEX format that was defined in			//
// AbstractThing3D.	  However we could certainly devine our own custom		//
// format here.																//
//**************************************************************************//

void FlyingThing3D::Create(ID3D11Device        *pRenderingDevice,
					 ID3D11DeviceContext *pImmediateContext) 
{
	ShowMessage(L"Thing3D; call my CreateMesh() instead of create()");
}

void FlyingThing3D::CreateFlyingThing(ID3D11Device         *pRenderingDevice,
					     ID3D11DeviceContext  *pImmediateContext,
					     LPCTSTR              SDKMeshFileName, 
						 int                  textureNumber)
{
	// initialise wing and tail points
	g_f_WingRotation		=  (float) sin (timeGetTime() / 200.0);
	tailAngle			    =  (float) sin (timeGetTime() / 300.0);

	HRESULT hr = 0;	// needed for V macro.

	//**********************************************************************//
	// Load the mesh.  Then put the first texture into video memory.		//
	//The same Texture2D variable	is used for other textures.				//
	//**********************************************************************//
	this->textureNumber = textureNumber;

	// 'mesh' is treated as the focal point for renderForMyImplementation
	// all other matrices revolve around it
	V( mesh.Create( pRenderingDevice, SDKMeshFileName, true ) );

	SDKMESH_SUBSET			  *pSubset    = NULL;
	D3D11_PRIMITIVE_TOPOLOGY  PrimType;
 
	pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair->pSamLinear );
 	pSubset  = mesh.GetSubset( 0, 0 );
	PrimType = CDXUTSDKMesh::GetPrimitiveType11( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
    pImmediateContext->IASetPrimitiveTopology( PrimType );
	
	pThing3DVS_PS_Pair->pTextureResourceView = mesh.GetMaterial( pSubset->MaterialID )->pDiffuseRV11;
 	pImmediateContext->PSSetShaderResources( textureNumber, 1, &pThing3DVS_PS_Pair->pTextureResourceView );
	
	//
	// create the flappy flap sound
	//
	V(soundManager.init());
	V(wingFlap.createSoundFromFile(L"Media\\Sound\\flap.wav", 1));
	sound1State = 0;

	//
	// create the meshes
	//
	CreateThingsAttachedMeshes(pRenderingDevice);

	//
	// create the shaders
	//
	CreateShadersForThing(pRenderingDevice);
}

//**************************************************************************//
// Create all of the meshes that will be used in conjunction with the thing //
// I have placed the sky in as well...simply because I can make it always in//
// the things view; of course, this may change in the future.				//
//**************************************************************************//
void FlyingThing3D::CreateThingsAttachedMeshes(ID3D11Device *pRenderingDevice)
{
	HRESULT hr = 0;

	V( g_MeshWing1.Create( pRenderingDevice, L"Media\\Wing\\wing.sdkmesh", true) );
	V( g_MeshWing2.Create( pRenderingDevice, L"Media\\Wing\\wing.sdkmesh", true) );
	V( g_MeshSky.Create  ( pRenderingDevice, L"Media\\Skybox\\skysphere.sdkmesh", true) );
}

//**************************************************************************//
// Initialise all of the shaders that will be used by the thing;
// tail wagging, no lighting...perhaps some others in the future.
//**************************************************************************//
void FlyingThing3D::CreateShadersForThing(ID3D11Device *pRenderingDevice)
{
	Thing3DVS_PS_Pair *pNoLightingShader = new Thing3DVS_PS_Pair();
	pNoLightingShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pNoLightingShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_NoLighting_PS");

 
	pNoLightingShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	pThing3DVS_PS_Pair_No_Lighting = pNoLightingShader;

	// deleting causes so many errors...why I wonder? Come back to this
	// delete pNoLightingShader;

	// the tail wag one
	
	Thing3DVS_PS_Pair *pTailWagShader = new Thing3DVS_PS_Pair();
	pTailWagShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_TailWag_VS");

	pTailWagShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_Textured_PS");

 
	pTailWagShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	pThing3DVS_PS_Pair_Tail_Wag = pTailWagShader;
	

}

//**************************************************************************//
// Implementation of an abstract method in the parent class.  Render ourself//
// for whatever we are in this particular implementation, which is a mesh.	//
//**************************************************************************//
void FlyingThing3D::RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                    ID3D11DeviceContext *pImmediateContext) 											
{
	HRESULT hr = 0;

	//**********************************************************************//
	// There is some issue with the overloaded multiply of an XMMatrix that //
	// the good ond D3DXMATRIX never used to have.   If you use the			//
	// overloaded multiply on an object's instance variables it goes bang!	//
	// Answers to Nigel please!  There may be a prize...					//
	//**********************************************************************//

	XMMATRIX matWorldlocal       = matWorld;
	XMMATRIX matViewLocal        = matView;
	XMMATRIX matProjectionLocal  = matProjection;

	XMMATRIX matWorldViewProjectionLocal = matWorldlocal * matViewLocal * matProjectionLocal;

	//******************************************************************//    
	// Update shader variables.  We must update these for every mesh	//
	// that we draw (well, actually we need only update the position	//
	// for each mesh, think hard about this - Nigel						//
	//																	//
    // We pass the parameters to it in a constant buffer.  The buffer	//
	// we define in this module MUST match the constant buffer in the	//
	// shader.															//
	//																	//
	// It would seem that the constant buffer we pass to the shader must//
	// be global, well defined on the heap anyway.  Not a local variable//
	// it would seem.													//
	//******************************************************************// 
	//******************************************************************//    
	//							(Flying) THING							//
	//******************************************************************//

	//
	// for non tail wag things
	//
	Thing3DVS_PS_Pair::CBMatrices      cbMatrices;
	cbMatrices.matWorld               = XMMatrixTranspose(matWorldlocal);
	cbMatrices.matWorldViewProjection = XMMatrixTranspose(matWorldViewProjectionLocal);
	pThing3DVS_PS_Pair->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	Thing3DVS_PS_Pair::CBLighting cbLighting;
	cbLighting.lightDiffuseColour   = lightDiffuseColour;
	cbLighting.lightDiffuseColour   = lightDiffuseColour;
	cbLighting.materialDiffuseColor = materialDiffuseColour;
	cbLighting.vecLightDir          = XMVector3Normalize(vecLightDirection);
	pThing3DVS_PS_Pair->UpdateCBLighting(pImmediateContext, &cbLighting);

	pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair->pSamLinear );
	pThing3DVS_PS_Pair->SetupShadersAndConstantbuffers(pImmediateContext);

	//******************************************************************//    
	//							CREATE BODY MOTION						//
	//******************************************************************//
		
	if(speed > 0.5 && speed > -0.5) tailAngle = (float) sin (timeGetTime() / 300.0); 

	Thing3DVS_PS_Pair::CBMatrices CBTailWag;
	CBTailWag.matWorld                = XMMatrixTranspose(matWorldlocal);
	CBTailWag.matWorldViewProjection = XMMatrixTranspose(matWorldViewProjectionLocal);
	CBTailWag.bodyMotion.x           = tailAngle;
	pThing3DVS_PS_Pair_Tail_Wag->UpdateCBMatrices(pImmediateContext, &CBTailWag);								

	pThing3DVS_PS_Pair_Tail_Wag->UpdateCBLighting(pImmediateContext, &cbLighting);
	pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair_Tail_Wag->pSamLinear );
	pThing3DVS_PS_Pair_Tail_Wag->SetupShadersAndConstantbuffers(pImmediateContext);

	RenderMesh(pImmediateContext, &mesh, pThing3DVS_PS_Pair_Tail_Wag);

	
	//******************************************************************//    
	//							SKY										//
	//******************************************************************//
	
	XMMATRIX matSkyTranslate = XMMatrixTranslation(0, -3, 0);
	XMMATRIX matSkyScale	 = XMMatrixScaling(3, 1, 3);
	XMMATRIX matSkyRotate    = XMMatrixRotationY(-ry);

	XMMATRIX matSkyWorld = matSkyRotate* matSkyTranslate * matSkyScale;

	matSkyWorld *= matWorldlocal;

	// note to self: use all local XMMATRIX's, else illegal memory accesses occur
	XMMATRIX matSkyWorldViewProjection = matSkyWorld * matViewLocal * matProjectionLocal;

	cbMatrices.matWorldViewProjection = XMMatrixTranspose(matSkyWorldViewProjection);
	pThing3DVS_PS_Pair_No_Lighting->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	//
	// Render the sky; uses a shader that creates no lighting (the sky is most beautiful
	// when it is bright and shiny(?)).
	//
	RenderMesh(pImmediateContext, &g_MeshSky, pThing3DVS_PS_Pair_No_Lighting);
	

	//******************************************************************//    
	//							WING 1									//
	//******************************************************************//
	wingFlap.updateStatus();

	if(speed > 0.5 && speed > -0.5 || height > 0)
	{
		g_f_WingRotation			= (float) sin(timeGetTime() / 160.0);
		
		// play wing flap
		hr = wingFlap.play();
	}

	XMMATRIX matTigerWing1Translate = XMMatrixTranslation(0.2,0,0);
	XMMATRIX matTigerWing1Rotate    = XMMatrixRotationZ(g_f_WingRotation);
	XMMATRIX matWing1World			= matTigerWing1Translate * matTigerWing1Rotate;

	matWing1World *= matWorldlocal;

	XMMATRIX matWingWorldViewProjection;
	matWingWorldViewProjection =  matWing1World *  matViewLocal *  matProjectionLocal;

	cbMatrices.matWorldViewProjection = XMMatrixTranspose(matWingWorldViewProjection);
	pThing3DVS_PS_Pair->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	//
	// Render Wing 1
	//
	RenderMesh(pImmediateContext, &g_MeshWing1, pThing3DVS_PS_Pair);

	//******************************************************************//    
	//							WING 2									//
	//******************************************************************//
	
	XMMATRIX matTigerWing2Translate = XMMatrixTranslation(-1.8, 0, 0);
	XMMATRIX matTigerWing2Rotate    = XMMatrixRotationZ(-g_f_WingRotation);

	XMMATRIX matWing2World			= matTigerWing2Translate * matTigerWing2Rotate;
	
	matWing2World *= matWorldlocal;

	XMMATRIX matWing2WorldViewProjection;
	matWing2WorldViewProjection = matWing2World * matViewLocal *  matProjectionLocal;

	cbMatrices.matWorldViewProjection = XMMatrixTranspose(matWing2WorldViewProjection);
	pThing3DVS_PS_Pair->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	//
	// Render Wing 2
	//
	RenderMesh(pImmediateContext, &g_MeshWing2, pThing3DVS_PS_Pair);

}


//**************************************************************************//
// Render a CDXUTSDKMesh, using the Device Context specified.				//
//**************************************************************************//
void FlyingThing3D::RenderMesh (ID3D11DeviceContext *pContext, 
				 CDXUTSDKMesh        *toRender,
				 Thing3DVS_PS_Pair   *shaderPair)
{
	//Get the mesh
    //IA setup
	pContext->IASetInputLayout( shaderPair->pVertexLayout );
    UINT Strides[1];
    UINT Offsets[1];
    ID3D11Buffer* pVB[1];
    pVB[0] = toRender->GetVB11( 0, 0 );
    Strides[0] = ( UINT )toRender->GetVertexStride( 0, 0 );
    Offsets[0] = 0;
    pContext->IASetVertexBuffers( 0, 1, pVB, Strides, Offsets );
    pContext->IASetIndexBuffer( toRender->GetIB11( 0 ), toRender->GetIBFormat11( 0 ), 0 );

    // Set the shaders
	pContext->VSSetShader( shaderPair->pVertexShader, NULL, 0 );
	pContext->PSSetShader( shaderPair->pPixelShader,  NULL, 0 );
    
	for( UINT subset = 0; subset < toRender->GetNumSubsets( 0 ); ++subset )
    {
		//Render
		SDKMESH_SUBSET* pSubset = NULL;
		D3D11_PRIMITIVE_TOPOLOGY PrimType;
        
		// Get the subset
        pSubset = toRender->GetSubset( 0, subset );

        PrimType = CDXUTSDKMesh::GetPrimitiveType11( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
        pContext->IASetPrimitiveTopology( PrimType );

		//**************************************************************************//
		// At the moment we load a texture into video memory every frame, which is	//
		// HORRIBLE, we need to create more Texture2D variables.					//
		//**************************************************************************//
        ID3D11ShaderResourceView* pDiffuseRV = toRender->GetMaterial( pSubset->MaterialID )->pDiffuseRV11;
        pContext->PSSetShaderResources( 0, 1, &pDiffuseRV );

        pContext->DrawIndexed( ( UINT )pSubset->IndexCount, 0, ( UINT )pSubset->VertexStart );
    }


	//******************************************************************************//
	// Restore the original texture if we put another in its place.					//
	//******************************************************************************//
	/*if (mustRestoreOriginalTexture)
	{
        PrimType = CDXUTSDKMesh::GetPrimitiveType11( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
		pSubset = mesh.GetSubset( 0, 0 );
		pImmediateContext->IASetPrimitiveTopology( PrimType );
		pDiffuseRV= mesh.GetMaterial( pSubset->MaterialID )->pDiffuseRV11;
		pImmediateContext->PSSetShaderResources( textureNumber, 1, &pDiffuseRV );
	} 
	
	// come back to this
	*/
}


//**************************************************************************//
// Do something, up to you what.	All this does is move the object forward//
// and slow it down some, to simulate slow the object a wee bit of friction.//
//**************************************************************************//
void FlyingThing3D::DoBehaviour() 
{
	MoveForward();
	speed *= 0.995;		

	if( (speed > -0.5 && speed < 0.5) && height > 0)
		height *= 0.998;
}
