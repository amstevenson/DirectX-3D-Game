

#include "Thing3DAbstract.h"
#include "../ObjectLoader/ObjectLoader.h"


#ifndef Object3D_H     //These are termed "guards", can you see what they do?
#define Object3D_H	

class Object3D : public Thing3DAbstract
{
	public:

		// instance variables
		ObjectLoader::meshInformation	g_meshData;

		ID3D11Buffer					*pVertexBuffer;
		ID3D11Buffer					*pIndexBuffer;
		ID3D11ShaderResourceView		*ptxtResourceViewInShader;
		ID3D11InputLayout				*pVertexLayout;

		//
		// This may not be required
		//
		struct SimpleFlatThingVertex
		{
			XMFLOAT3 pos;
			XMFLOAT3 normal;
			XMFLOAT2 texUV;
		};

	//**********************************************************************//
	// Implementation of abstract methods in parent class.					//
    //**********************************************************************//
	public:
      virtual void Create(ID3D11Device        *pRenderingDevice,
					      ID3D11DeviceContext *pImmediateContext);
	  virtual void RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                         ID3D11DeviceContext *pImmediateContext);	//Draw yourself 
																						//for whatever you are
	  virtual void DoBehaviour();


	//**********************************************************************//
	// Constructors and destructors.										//
	//**********************************************************************//
	public:
	  Object3D(ID3D11Device        *pRenderingDevice,
			      ID3D11DeviceContext *pImmediateContext);
      ~Object3D();									//Destructor.

	//**********************************************************************//
	// Accessors															//
	//**********************************************************************//
	public:

		void  SetObjMeshFromFile(LPSTR filename,
			ID3D11Device        *pRenderingDevice);

    //**********************************************************************//
	// Protected and private methods.										//
	//**********************************************************************//
	protected:
	  void Init(); // Used by constructors.	


};



#endif