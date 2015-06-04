//**************************************************************************//
// VERTEX Shader file for the descendent of AbstractThing3D.  It is very	//
// heavily based on the shader provided in Microsoft's "SimpleSample".		//
//																			//
// This (poggled) code is copyright of Dr Nigel Barlow, lecturer in			//
// computing, University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.	//
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
//																			//
// Poggeling ©Nigel Barlow nigel@soc.plymouth.ac.uk.						//
// "Simple Sample" ©Microsoft Corporation. All rights reserved.				//
//**************************************************************************//

#include "Thing3D_Common.hlsli"


//**************************************************************************//
// First constant buffer, goes at position 0.  We will need to know the		//
// position in the sample that uses this code.  And the constant buffer's	//
// structure must be identical to the structure defined in the program that	//
// uses this shader.														//
//**************************************************************************//
cbuffer CBMatrices: register( b0 )
{
	matrix matWorld;                  // World matrix for object
	matrix matWorldViewProjection;    // World * View * Projection matrix
}

cbuffer CBPerTailWag : register( b0 )
{
	matrix		matWorldTailWag;	
	matrix		matWorldViewProjectionTailWag;	
	float4		g_tailAngle;	
};



//**************************************************************************//
// Vertex shader input structure.	The semantics (the things after the		//
// colon) look a little weird.  The semantics are used (so Microsoft tell	//
// us) used by the compiler to link shader inputs and outputs. 				//
//																			//
// For this to work, you must ensure that the vertex structure you use in	//
// any program that uses this shader is the same as below, vertex position,	//
// normal vector and the texture UV coordinates, in that order!				//
//**************************************************************************//
struct VS_INPUT
{
	float4 Position   : POSITION;	
    float3 vecNormal  : NORMAL;		
    float2 TextureUV  : TEXCOORD0;
	float4 TerrainColour : COLOR;
};

//**************************************************************************//
// This shader computes standard transform and lighting.   Calculating		//
// lighting in the vertex xhader is termed "per vertex shading" and, as the //
// term suggests, performs one lighting calculation on each verted.			//
// Performing the lighting calculations in the pixel shader is more accurate//
// and gives better results, but it slower.									//
//**************************************************************************//
VS_OUTPUT Thing3D_VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
	//**********************************************************************//
	// Multiply every vertex vy the WVP matrix (we do it "properly here		//
	// unlike the cubes sample.												//
	//**********************************************************************//
    output.Position = mul(input.Position, matWorldViewProjection);
    
	//**********************************************************************//
	// Whatever we do to the tiger, we must also do to its normal vector.	//
	//**********************************************************************//
    output.vecNormal = normalize(mul(input.vecNormal, (float3x3)matWorld)); 
        
    
	//**********************************************************************//
	// And finally, just copu the texture Us and Vs to the output structure	//
	//**********************************************************************//
    output.TextureUV = input.TextureUV; 
    

    return output;    
}

VS_OUTPUT Terrain_VS(VS_INPUT input)
{

	VS_OUTPUT output;

	// change position to be four units
	//input.Position.w = -1.0f;

	//output.Position = mul(input.Position, matWorld);
	output.Position = mul(input.Position, matWorldViewProjection);

	// store the texture coordinates
	output.TextureUV = input.TextureUV;

	// calculate the normal vector against the world matrix
	output.vecNormal = mul(input.vecNormal, (float3x3)matWorld);

	// normalise the normal - as it were
	output.vecNormal - normalize(output.vecNormal);

	// send the colour map colour to the pixel shader
	output.TerrainColour = input.TerrainColour;

	return output;

}


//And that's all folks.

//**************************************************************************//
//							Tail Wag - Y direction.							//				
//							Learnt by studying Nigels code					//
//**************************************************************************//
VS_OUTPUT Thing3D_TailWag_VS (VS_INPUT input)
{
    VS_OUTPUT output;
    
	//**********************************************************************//
	// Deform the mesh here.  We must do the deform in world space, i.e.	//
	// before we project the tiger.											//
	//**********************************************************************//
	float tailAngle = g_tailAngle.x;
	float tailBase  = 0.53;				//The tiger is created with its tail pointing
										//straight down the Z axis, so the tail 
	if (input.Position.z > tailBase )	//begins at z= tailBase
	{
		float distanceFromBum = tailBase - input.Position.z;
		
		input.Position.y += tailAngle * distanceFromBum;  //Wag from side to side.
	}

	//**********************************************************************//
	// Multiply every vertex vy the WVP matrix (we do it "properly here		//
	// unlike the cubes sample.												//
	//**********************************************************************//
    output.Position = mul(input.Position, matWorldViewProjectionTailWag);
    

	//**********************************************************************//
	// Whatever we do to the tiger, we must also do to its normal vector.	//
	//**********************************************************************//
	float3 vNormalWorldSpace;
	vNormalWorldSpace = normalize(mul(input.vecNormal, (float3x3) matWorldTailWag)); // normal (world space)
	output.vecNormal    = vNormalWorldSpace;
 
    
 	//**********************************************************************//
	// And finally, just copu the texture Us and Vs to the output structure	//
	//**********************************************************************//
    output.TextureUV = input.TextureUV; 
    
    return output;    
}

//***************************************************************************************//
// Textured vertex shader, adapted from:http:// www.xbdev.net/directx3dx/specialX/Fur/   //
// so all credit goes to the fellow/lady who wrote it; although I had to get it to work  //
// with this application (it makes the floor and things look nice).						 //
//																						 //
// I am not sure if this even does anything yet...but I am keeping it for possible later //
// use.																				     //
//***************************************************************************************//
VS_OUTPUT Thing3D_CreateFur_VS(VS_INPUT input)
{
    VS_OUTPUT output;

	float FurLength = 0;
	float UVScale   = 1.0f;
	float Layer     = 0; // 0 to 1 for the level
	float3 vGravity = float3(0,-2.0,0);
	float4 vecLightDir = float4(0.8,0.8,1,0);


    //This single line is responsible for creating the layers!  This is it! Nothing
    //more nothing less!
    float3 P = input.Position.xyz + (input.vecNormal * FurLength);
     
    //Modify our normal so it faces the correct direction for lighting if we
    //want any lighting
    float3 normal = normalize(mul(input.vecNormal, (float3x3)matWorld));

    // Couple of lines to give a swaying effect!
    // Additional Gravit/Force Code
    vGravity = mul(vGravity, (float3x3)matWorld);
    float k =  pow(Layer, 3);  // We use the pow function, so that only the tips of the hairs bend
                                 // As layer goes from 0 to 1, so by using pow(..) function is still
                                 // goes form 0 to 1, but it increases faster! exponentially
    P = P + vGravity*k;
    // End Gravity Force Addit Code
 
    output.TextureUV = input.TextureUV * UVScale; // Pass long texture data
    // UVScale??  Well we scale the fur texture alpha coords so this effects the fur thickness
    // thinness, sort of stretches or shrinks the fur over the object!
     
    output.Position = mul(float4(P, 1.0f), matWorldViewProjection); // Output Vertice Position Data
    output.vecNormal = normal; // Output Normal
    
	return output;
}
