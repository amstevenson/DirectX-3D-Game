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

#ifndef FlyingThing_H     //These are termed "guards", can you see what they do?
#define FlyingThing_H

#include "Thing3DAbstract.h"
#include "SDKmesh.h"
#include "../NigSound/NigSoundManager.h"
#include "../NigSound/NigSound.h" 


class FlyingThing3D : public Thing3DAbstract
{

//**********************************************************************//
	// Public instance variables.											//
	//**********************************************************************//
	public:
		CDXUTSDKMesh mesh;
		CDXUTSDKMesh g_MeshWing1;
		CDXUTSDKMesh g_MeshWing2;		
		CDXUTSDKMesh g_MeshSky;	

		float		 g_f_WingRotation;      
		float		 tailAngle;

		bool		 flyingThing;

	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//
	public:
		//******************************************************************//
		// Constructor.  Force the parent constructor to execute, then		//
		// create the default shaders.										//
		//******************************************************************//
		FlyingThing3D(ID3D11Device        *pRenderingDevice,
				ID3D11DeviceContext *pImmediateContext);
		~FlyingThing3D();									//Destructor.


	  //
	  // Method to create the flying thing
	  //
	  virtual void CreateFlyingThing(ID3D11Device         *pRenderingDevice,
							     	 ID3D11DeviceContext  *pImmediateContext,
								     LPCTSTR              SDKMeshFileName, 
								     int                  textureNumber);

	//**********************************************************************//
	// Implementation of abstract methods in parent class.					//
    //**********************************************************************//
	public:

      virtual void Create(ID3D11Device        *pRenderingDevice,
					      ID3D11DeviceContext *pImmediateContext);
	  virtual void RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                        ID3D11DeviceContext *pImmediateContext);
																						
	  virtual void DoBehaviour();

	  //
	  // render the mesh
	  //
	  virtual void RenderMesh (ID3D11DeviceContext *pContext, 
							   CDXUTSDKMesh        *toRender,
							   Thing3DVS_PS_Pair   *shaderPair);

	  //
	  // create the shaders that will be used with the meshes
	  //
	  virtual void CreateShadersForThing(ID3D11Device *pRenderingDevice);

	  //
	  // create the meshes that will be attached to the tiger/thing
	  //
	  virtual void CreateThingsAttachedMeshes(ID3D11Device *pRenderingDevice);

};

#endif