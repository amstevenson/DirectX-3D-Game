//**************************************************************************//
// PIXEL Shader file for the descendent of AbstractThing3D.  It is very		//
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

cbuffer CBLighting : register (b1)
{
	float4 MaterialDiffuseColor;    // Material's diffuse color
	float4 vecLightDir;             // Light's direction in world space
	float4 LightDiffuse;			// Light's diffuse color
	float4 LightAmbientColour;      // Light's's ambient color

	bool hasTexture;
}


//**************************************************************************//
// Textures and Texture Samplers.  These variables can't (it seems) go in	//
// constant buffers; I think the size of a CB is too restricted.			//
//																			//
// Two shader variables; add more as required.								//
//**************************************************************************//
Texture2D ThingTexture : register( t0 );					
Texture2D TileTexture  : register( t1 );					



//**************************************************************************//
// Texture sampler.  A simple texture sampler is all we need here.			//
//**************************************************************************//
SamplerState MeshTextureSampler : register( s0 );
SamplerState ObjectTextureSampler : register(s1);

float4 ObjectL_PS(VS_OUTPUT input) : SV_Target
{
	
	input.vecNormal = normalize(input.vecNormal);	

	//Set diffuse color of material
	float4 diffuse = MaterialDiffuseColor;

	//If material has a diffuse texture map, set it now
	if(hasTexture)
		diffuse = TileTexture.Sample( MeshTextureSampler, input.TextureUV );

	float3 finalColor;

	finalColor = diffuse * LightAmbientColour;
	finalColor += saturate(dot(vecLightDir, input.vecNormal) * LightDiffuse * diffuse);
	
	return float4(finalColor, diffuse.a);	
}

//**************************************************************************//
// The pixel shader.  This shader outputs the pixel's color by modulating   //
// the texture's color with diffuse material color.  As above, this is "per //
// pixel lighting, which gives better results than "per vertex lighting",	//
// but is slower.															//
//**************************************************************************//
float4 Thing3D_PS( VS_OUTPUT In ) : SV_Target
{ 
    float lighting = saturate( dot( vecLightDir, In.vecNormal ) );
	lighting = max( lighting, LightAmbientColour );

	
	// Lookup mesh texture and modulate it with diffuse
    return ThingTexture.Sample(MeshTextureSampler, In.TextureUV) * lighting;
}


//**************************************************************************//
//					Nigels Object Loader Pixel Shaders						//
//**************************************************************************//
float4 Thing3D_NoLighting_PS( VS_OUTPUT In) : SV_Target
{
    return ThingTexture.Sample( MeshTextureSampler, In.TextureUV ) * MaterialDiffuseColor;
}

//**************************************************************************//
// The pixel shader fot the flat thing. This uses a different texture, and	//
// there is no lighting.													//
//**************************************************************************//
float4 FlatThing3D_PS( VS_OUTPUT In ) : SV_Target
{ 

	// Lookup mesh texture
    return TileTexture.Sample(MeshTextureSampler, In.TextureUV) * MaterialDiffuseColor;
}

//And that's all folks.


//***************************************************************************************//
// Textured pixel shader, adapted from:h ttp://www.xbdev.net/directx3dx/specialX/Fur/    //
// so all credit goes to the fellow/lady who wrote it; although I had to get it to work  //
// with this application (it makes the floor and things look nice).					     //
//***************************************************************************************//
float4 Thing3D_Textured_PS( VS_OUTPUT In): SV_TARGET
{
      float4 thingColour = ThingTexture.Sample( MeshTextureSampler,  In.TextureUV );
      float4 thingFinalColour = thingColour;
     
      // directional lighting
      float4 thingAmbient = {0.3, 0.3, 0.3, 0.0};
      thingAmbient = thingAmbient * thingFinalColour;

      float4 thingDiffuse = thingFinalColour;
      thingFinalColour = thingAmbient + thingDiffuse * dot(vecLightDir, In.vecNormal);

      // return final colour
      thingFinalColour.a = thingColour.a;

      return thingFinalColour;       // Use texture colour
}

//
// Pixel shader for terrain
// Tutorial from http:// www.rastertek.com/tertut04.html
//
float4 Terrain_PS(VS_OUTPUT input) : SV_TARGET
{
	float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

	// Texture coordinate position defines the pixel colour
	textureColor = TileTexture.Sample(MeshTextureSampler, input.TextureUV);

	// default output colour
	color = LightAmbientColour;

	// Invert the light direction for calculations.
    lightDir = -vecLightDir;

	// Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.vecNormal, lightDir));

    if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (LightDiffuse * lightIntensity);
    }

	// Saturate the final light color.
    color = saturate(color);

   // Multiply the texture pixel and the final light color to get the result.
    color = color * textureColor;

	// Combine the color map value into the final color.
    color = saturate(color * input.TerrainColour * 2.0f);

    return color;
}