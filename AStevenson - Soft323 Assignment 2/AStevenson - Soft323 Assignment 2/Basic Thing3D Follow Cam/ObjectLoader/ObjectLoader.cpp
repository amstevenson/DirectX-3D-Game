//******************************************************************************//
//			Extracts the object variables from a file that has the				//
//			.obj extention. It also extracts the material textures,				//
//			start/end subset indexes and detects different types of faces		//
//******************************************************************************//

#include "ObjectLoader.h"

//**************************************************************************//
// Constructor																//
//**************************************************************************//
ObjectLoader::ObjectLoader()
{
	g_numIndices = 0;
	meshSubsets = 0;
}

//**************************************************************************//
// Destructor																//
//**************************************************************************//
ObjectLoader::~ObjectLoader()
{

}


//**************************************************************************//
// Trim the start of a c++ string											//
//**************************************************************************//
std::wstring ObjectLoader::TrimStart(std::wstring s) 
{
        s.erase(s.begin(), std::find_if(s.begin(), 
			    s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

//**************************************************************************//
// Trim the end of a c++ string												//
//**************************************************************************//
std::wstring ObjectLoader::TrimEnd(std::wstring s) 
{
        s.erase(std::find_if(s.rbegin(), s.rend(), 
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}


ObjectLoader::meshInformation ObjectLoader::LoadMesh(std::wstring filename,
													 std::wstring filePath,
													 ID3D11Device *pRenderingDevice)
{
	
	// vertex / indices information for the file
	std::wifstream          fileStream;
	std::wstring            line;
	
	// v, vt, vn 
	std::vector <XMFLOAT3>  vectorVertices(0);
	std::vector <XMFLOAT2>  vertexTextures(0);
	std::vector <XMFLOAT3>  vertexNormals(0);

	// index information
	std::vector <UINT>    vectorIndices(0);
	std::vector <UINT>	  vectorNormalIndices(0);
	std::vector <UINT>    vectorTextureIndices(0);

	// for counting the amount of vertices
	int vIndex  = 0; // taking note of the index (for use with materials)
	//int indices = 0;

	bool hasVertexTexture = false;
	bool hasVertexNormal  = false;
	bool hasTexCoords     = false;

	// "g" "matlib" etc
	int subsetCount = 0;
	std::vector<int> subsetIndexStart;
	std::vector<int> subsetTexture;
	std::vector<int> materialIndexStart;

	// mesh materials for "use" and storing the file name for the material library
	std::wstring meshMatLib;
	std::wstring meshMaterialsTemp;
	std::vector<std::wstring> meshMaterials;

	//
	// Create the mesh that will be returned later on
	//
	meshInformation mesh;

	// open file
	fileStream.open(filePath + filename);

	while(std::getline(fileStream, line))
	{
		
		line = TrimStart(line);
		
		//
		// Read in the vertices
		//
		if (line.compare(0, 2, L"v ") == 0)  //"v space"
		{
			WCHAR first[5];
			XMFLOAT3 vertex;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%2s%f%f%f", first, &vertex.x, &vertex.y, &vertex.z); 

			vectorVertices.push_back(vertex);
		}

		//
		// Read in the vertex textures
		//
		else if (line.compare(0, 2, L"vt") == 0)
		{
			// This part does not work yet, there are conflicts with already existing shaders.
			// I will come back to this. 
			WCHAR first[5];
			XMFLOAT2 vertexTexture;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f", first, &vertexTexture.x, &vertexTexture.y);

			vertexTextures.push_back(vertexTexture);

			//
			// We have vertex textures
			// 
			hasVertexTexture = true;
		}

		//
		// Read in the vertex normals
		//
		else if (line.compare(0, 2, L"vn") == 0)
		{
			WCHAR first[5];
			XMFLOAT3 vertexNormal;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f%f", first, &vertexNormal.x,
									  &vertexNormal.y,&vertexNormal.z);

			vertexNormals.push_back(vertexNormal);

			//
			// We have vertex normals
			//
			hasVertexNormal = true;
		}

		else if (line.compare(0, 1, L"g") == 0)
		{
			subsetIndexStart.push_back(vIndex);		//Start index for this subset
			subsetCount++;


		}//"g space"? 

		//
		// Read in the face
		//
		else if (line.compare(0, 2, L"f ") == 0)  //"f space"
		{
			
			int currentIndexCount = 0; // for each triangle point, count and estimate
									   // what type of face to store the index information for
			int numSlashes        = 0;
			int numDecimals       = 0; 
			std::wstring decimalAsString;
			int pos, tex, normal  = 0;
			UINT tempIndices[3];

			// take note of where we are...
			// Number of decimals and slashes differ for each type of face
			// i/i/i; S = 2; D = 3
			// i//i;  S = 2; D = 2
			// i/i;   S = 1; D = 1
			// i;		     D = 1
			for(int i = 2; i < line.size(); i++) // start at 2 to skip the beggining "the f and the space afterwards" - "f "
			{
				//
				// Get character
				//
				char lineChar = line.at(i);
				int derp = numDecimals;

				if(lineChar == '/')
				{
					// we are at a '/'
					numSlashes ++;

					if(line.at(i -1) != '/')
					{
						// store the decimal value for the specific index
						WCHAR oldStyleStr[20];
						wcscpy(oldStyleStr, decimalAsString.c_str());
						swscanf(oldStyleStr, L"%d", &tempIndices[numDecimals]);

						int temp = tempIndices[numDecimals];

						numDecimals ++;
					}
					else
					{
						// then do nothing (a pos/norm face)
					}

					// clear the string for the next decimal
					decimalAsString.clear();
				}
				else
				{
					// add the current char to the string (append it)
					decimalAsString += lineChar;
				}
				
				// new storage
				if(lineChar == ' ' || i == line.size() -1) 
				{

					// store the final value before calculating the faces indices
					WCHAR oldStyleStr[20];
					wcscpy(oldStyleStr, decimalAsString.c_str());
					swscanf(oldStyleStr, L"%d", &tempIndices[numDecimals]);

					numDecimals ++;

					decimalAsString.clear();

					//
					// Store the information about the face
					// 
					// Note: I use the booleans "hasVertexNormal/Texture" at the bottom
					// of this loader
					//
					if(numDecimals == 3 && numSlashes == 2)		 // Pos / Tex / Normal
					{
						vectorIndices.push_back((tempIndices[0])        -1);
						vectorTextureIndices.push_back((tempIndices[1]) -1);
						vectorNormalIndices.push_back((tempIndices[2])  -1);

						vIndex ++;
					}
					else if(numDecimals == 2 && numSlashes == 2) // pos / norm
					{
						vectorIndices.push_back((tempIndices[0])        -1);
						// leave out texture
						vectorNormalIndices.push_back((tempIndices[2])  -1);

						vIndex++;
					}
					else if(numDecimals == 2 && numSlashes == 1) // Pos / Tex
					{
						vectorIndices.push_back((tempIndices[0])        -1);
						vectorTextureIndices.push_back((tempIndices[1]) -1);
						// leave out normals

						vIndex ++;
					}
					else if(numDecimals == 1 && numSlashes == 0)
					{
						vectorIndices.push_back((tempIndices[0])        -1);
						// leave out texture
						// leave out normals

						vIndex ++;
					}

					// finish off by resetting the amount of slashes and decimals
					numDecimals = 0;
					numSlashes  = 0;
				}
			}
		}

		else if (line.compare(0, 6, L"mtllib") == 0)
		{
			WCHAR first[6];

			std::wstring whole;
			std::wstring matFileName;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			
			whole = oldStyleStr;
			matFileName = whole.substr(7, whole.length());

			meshMatLib = matFileName;

		}//"mtllib "? 

		else if (line.compare(0, 6, L"usemtl") == 0)
		{
			std::wstring whole;
			std::wstring meshMaterial;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			
			whole = oldStyleStr;
			meshMaterial = whole.substr(7, whole.length());

			// keeping track of the names
			meshMaterials.push_back(meshMaterial);

			// keeping track of materials used
			materialIndexStart.push_back(vIndex);

		}// usemtl 


	} // end of read file

	subsetIndexStart.push_back(vIndex );   // last index for groups "g"
	materialIndexStart.push_back(vIndex ); // last index for usemtl s
	
	// push back the last material for the same reason as applying the last index
	std::wstring lastMaterialNameused = meshMaterials[meshMaterials.size() -1];
	meshMaterials.push_back(lastMaterialNameused);

	fileStream.close(); // close the stream

	// next...materials file 
	fileStream.open(filePath + meshMatLib);

	int matCount = 0;	//keep track of materials
	bool kdset = false;

	std::vector<SurfaceMaterial> tempMaterials;
	std::vector<std::wstring> materialTextureNames;

	while(std::getline(fileStream, line))
	{
		line = TrimStart(line);

		// Kd - Diffuse
		if (line.compare(0, 2, L"Kd") == 0)
		{
			WCHAR space[5];
			float mDiffX, mDiffY, mDiffZ;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f%f", space, &mDiffX, &mDiffY, &mDiffZ );

			tempMaterials[matCount -1].difColour.x = mDiffX;
			tempMaterials[matCount -1].difColour.y = mDiffY;
			tempMaterials[matCount -1].difColour.z = mDiffZ;

		}  

		// Ka - Ambient
		else if (line.compare(0, 2, L"Ka") == 0)
		{
			WCHAR space[5];
			float mAmbX, mAmbY, mAmbZ;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f%f", space, &mAmbX, &mAmbY, &mAmbZ );

			tempMaterials[matCount -1].ambColour.x = mAmbX;
			tempMaterials[matCount -1].ambColour.y = mAmbY;
			tempMaterials[matCount -1].ambColour.z = mAmbZ;

		}  

		else if (line.compare(0, 5, L"illum") == 0)
		{
			WCHAR space[5];
			float tempIllum;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%5s%f", space, &tempIllum);

			tempMaterials[matCount -1].illum = tempIllum;
		}  

		// Ks - Specular Colour
		else if (line.compare(0, 2, L"Ks") == 0)
		{
			WCHAR space[5];
			float ks1, ks2, ks3;
			ks1 = ks2 = ks3 = 0;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f%f", space, &ks1, &ks2, &ks3);

			tempMaterials[matCount -1].specularColour.x = ks1;
			tempMaterials[matCount -1].specularColour.y = ks2;
			tempMaterials[matCount -1].specularColour.z = ks3;
		}  

		// Ks - Specular Colour
		else if (line.compare(0, 1, L"d") == 0)
		{
			WCHAR space[5];
			float tempTransparency = 0;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%2s%f", space, &tempTransparency);

			tempMaterials[matCount -1].transparency = tempTransparency;
			tempMaterials[matCount -1].transparent = true;
		} 

		else if (line.compare(0, 6, L"map_Kd") == 0)
		{
			std::wstring whole;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			
			whole = oldStyleStr;

			std::wstring texturePath = filePath + whole.substr(7, whole.length());

			//check if this texture has already been loaded
			bool alreadyLoaded = false;
			for(int i = 0; i < materialTextureNames.size(); ++i)
			{
				if(texturePath == materialTextureNames[i])
				{
					alreadyLoaded = true;
					tempMaterials[matCount-1].textureIndex = i;
					tempMaterials[matCount-1].hasTexture = true;
				}
			}

			if(!alreadyLoaded)
			{
			// load and store a new texture
			ID3D11ShaderResourceView* tempTexture;
			HRESULT hr = D3DX11CreateShaderResourceViewFromFile( pRenderingDevice,  texturePath.c_str(),
											NULL, NULL, &tempTexture, NULL );

				if(SUCCEEDED(hr))
				{
					materialTextureNames.push_back(texturePath);

					// store the texture resource
					tempMaterials[matCount -1].textureIndex = materialTextureNames.size();
					tempMaterials[matCount - 1].textureResource = tempTexture;
					tempMaterials[matCount - 1].hasTexture = true;
				}
			}
		} 

		// Create a new material (first step in this process of storing material information)
		else if (line.compare(0, 6, L"newmtl") == 0)
		{
			std::wstring whole;
			std::wstring matName;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			
			whole = oldStyleStr;

			// Create a new material
			SurfaceMaterial tempMaterial;
			tempMaterials.push_back(tempMaterial);

			tempMaterials[matCount].materialName = whole.substr(7, whole.length());
			tempMaterials[matCount].transparent = false;
			tempMaterials[matCount].hasTexture = false;
			tempMaterials[matCount].textureIndex = 0;
			kdset = false;

			matCount++; // index is referenced after call, therefore ++ here

		} 

	}

	// store how many materials there are in total
	mesh.totalMaterials = matCount;

	int numGroups = subsetIndexStart.size();
	mesh.subsetAmount = numGroups;
	mesh.subsetIndexStart = new int[numGroups];
	for (int i = 0; i < numGroups; i++)
	{
		int debugStartIndex = subsetIndexStart[i];

		mesh.subsetIndexStart[i] = subsetIndexStart[i];
	}

	// need to keep track of what materials are being used, and where they are being used
	int numMaterialsUsed = materialIndexStart.size();
	mesh.materialsUsedAmount = numMaterialsUsed;
	mesh.materialIndexStart = new int[numMaterialsUsed];
	mesh.materialUsed		= new std::wstring[numMaterialsUsed];

	for(int i = 0; i < numMaterialsUsed; i++)
	{
		mesh.materialIndexStart[i] = materialIndexStart[i];
		mesh.materialUsed[i] = meshMaterials[i];
	}

	//
	// Allocate mesh vertex positions
	//
	mesh.numVertices = (UINT) vectorVertices.size();
	mesh.vertices    = new SimpleVertex[mesh.numVertices];
	for (int i = 0; i < mesh.numVertices; i++)
	{
		mesh.vertices[i].Pos.x = vectorVertices[i].x;
		mesh.vertices[i].Pos.y = vectorVertices[i].y;
		mesh.vertices[i].Pos.z = vectorVertices[i].z;

		if(hasVertexNormal)
		{
			mesh.vertices[i].VecNormal.x = vertexNormals[vectorNormalIndices[i]].x;
			mesh.vertices[i].VecNormal.y = vertexNormals[vectorNormalIndices[i]].y;
			mesh.vertices[i].VecNormal.z = vertexNormals[vectorNormalIndices[i]].z;
		}
		else
		{
			mesh.vertices[i].VecNormal.x = 0;
			mesh.vertices[i].VecNormal.y = 0;
			mesh.vertices[i].VecNormal.z = 0;
		}

		if(hasVertexTexture)
		{
			mesh.vertices[i].TexUV.x = vertexTextures[vectorTextureIndices[i]].x;
			mesh.vertices[i].TexUV.y = vertexTextures[vectorTextureIndices[i]].y;
		}
		else
		{
			mesh.vertices[i].TexUV.x = 0;
			mesh.vertices[i].TexUV.y = 0;
		}
	}

	//
	// Allocate mesh indices
	//
	mesh.numIndices = (UINT) vectorIndices.size();
	mesh.indexes    = new UINT[mesh.numIndices];
	for (int i = 0; i < mesh.numIndices; i++)
		mesh.indexes[i] = vectorIndices[i];

	// Allocate material information
	UINT numMaterials = tempMaterials.size();
	mesh.materials = new SurfaceMaterial[numMaterials];
	
	for(int i = 0; i < numMaterials; i++)
		mesh.materials[i] = tempMaterials[i];
	
	// return the structure that contains all of the material and vertex information
	return mesh;
}
