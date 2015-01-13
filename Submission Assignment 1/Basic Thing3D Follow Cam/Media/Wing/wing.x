xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template FrameTransformMatrix {
 <F6F23F41-7686-11cf-8F52-0040333594A3>
 Matrix4x4 frameMatrix;
}

template Frame {
 <3D82AB46-62DA-11cf-AB39-0020AF71E433>
 [...]
}

Header {
 1;
 0;
 1;
}




   Mesh Body1 {
    89;
    0.751136;-0.067471;-0.006818;,
    1.243014;-0.058432;-0.006818;,
    1.177114;-0.058432;-0.125258;,
    0.751136;0.067471;-0.006818;,
    1.177114;0.058432;-0.125258;,
    1.243014;0.058432;-0.006818;,
    0.751136;-0.067471;-0.006818;,
    0.997075;-0.058432;-0.211961;,
    0.751136;0.067471;-0.006818;,
    0.997075;0.058432;-0.211961;,
    0.751136;-0.067471;-0.006818;,
    0.751136;-0.058432;-0.243697;,
    0.751136;0.067471;-0.006818;,
    0.751136;0.058432;-0.243697;,
    0.751136;-0.067471;-0.006818;,
    0.505198;-0.058432;-0.211961;,
    0.751136;0.067471;-0.006818;,
    0.505198;0.058432;-0.211961;,
    0.751136;-0.067471;-0.006818;,
    0.325158;-0.058432;-0.125258;,
    0.751136;0.067471;-0.006818;,
    0.325158;0.058432;-0.125258;,
    0.751136;-0.067471;-0.006818;,
    0.259259;-0.058432;-0.006818;,
    0.751136;0.067471;-0.006818;,
    0.259259;0.058432;-0.006818;,
    0.751136;-0.067471;-0.006818;,
    0.325158;-0.058432;0.111621;,
    0.751136;0.067471;-0.006818;,
    0.325158;0.058432;0.111621;,
    0.751136;-0.067471;-0.006818;,
    0.505198;-0.058432;0.198325;,
    0.751136;0.067471;-0.006818;,
    0.505198;0.058432;0.198325;,
    0.751136;-0.067471;-0.006818;,
    0.751136;-0.058432;0.230061;,
    0.751136;0.067471;-0.006818;,
    0.751136;0.058432;0.230061;,
    0.751136;-0.067471;-0.006818;,
    0.997075;-0.058432;0.198325;,
    0.751136;0.067471;-0.006818;,
    0.997075;0.058432;0.198325;,
    0.751136;-0.067471;-0.006818;,
    1.177114;-0.058432;0.111621;,
    0.751136;0.067471;-0.006818;,
    1.177114;0.058432;0.111621;,
    0.751136;-0.067471;-0.006818;,
    1.243014;-0.058432;-0.006818;,
    0.751136;0.067471;-0.006818;,
    1.243014;0.058432;-0.006818;,
    1.603092;-0.033736;-0.006818;,
    1.488952;-0.033736;-0.211961;,
    1.177114;-0.033736;-0.362136;,
    0.751136;-0.033736;-0.417104;,
    0.325158;-0.033736;-0.362136;,
    0.013321;-0.033736;-0.211961;,
    -0.100820;-0.033736;-0.006818;,
    0.013321;-0.033736;0.198325;,
    0.325158;-0.033736;0.348500;,
    0.751136;-0.033736;0.403468;,
    1.177114;-0.033736;0.348500;,
    1.488952;-0.033736;0.198325;,
    1.603092;-0.033736;-0.006818;,
    1.734892;0.000000;-0.006818;,
    1.603093;0.000000;-0.243697;,
    1.243014;0.000000;-0.417104;,
    0.751136;0.000000;-0.480576;,
    0.259259;0.000000;-0.417104;,
    -0.100820;0.000000;-0.243697;,
    -0.232618;0.000000;-0.006818;,
    -0.100820;0.000000;0.230061;,
    0.259259;0.000000;0.403468;,
    0.751136;0.000000;0.466940;,
    1.243014;0.000000;0.403468;,
    1.603093;0.000000;0.230061;,
    1.734892;0.000000;-0.006818;,
    1.603092;0.033736;-0.006818;,
    1.488952;0.033736;-0.211961;,
    1.177114;0.033736;-0.362136;,
    0.751136;0.033736;-0.417104;,
    0.325158;0.033736;-0.362136;,
    0.013321;0.033736;-0.211961;,
    -0.100820;0.033736;-0.006818;,
    0.013321;0.033736;0.198325;,
    0.325158;0.033736;0.348500;,
    0.751136;0.033736;0.403468;,
    1.177114;0.033736;0.348500;,
    1.488952;0.033736;0.198325;,
    1.603092;0.033736;-0.006818;;

    120;
    3;0,2,1;,
    3;3,5,4;,
    3;6,7,2;,
    3;8,4,9;,
    3;10,11,7;,
    3;12,9,13;,
    3;14,15,11;,
    3;16,13,17;,
    3;18,19,15;,
    3;20,17,21;,
    3;22,23,19;,
    3;24,21,25;,
    3;26,27,23;,
    3;28,25,29;,
    3;30,31,27;,
    3;32,29,33;,
    3;34,35,31;,
    3;36,33,37;,
    3;38,39,35;,
    3;40,37,41;,
    3;42,43,39;,
    3;44,41,45;,
    3;46,47,43;,
    3;48,45,49;,
    3;1,51,50;,
    3;1,2,51;,
    3;2,7,51;,
    3;51,7,52;,
    3;7,53,52;,
    3;7,11,53;,
    3;11,15,53;,
    3;53,15,54;,
    3;15,55,54;,
    3;15,19,55;,
    3;19,23,55;,
    3;55,23,56;,
    3;23,57,56;,
    3;23,27,57;,
    3;27,31,57;,
    3;57,31,58;,
    3;31,59,58;,
    3;31,35,59;,
    3;35,39,59;,
    3;59,39,60;,
    3;39,61,60;,
    3;39,43,61;,
    3;43,47,61;,
    3;61,47,62;,
    3;50,51,63;,
    3;63,51,64;,
    3;51,65,64;,
    3;51,52,65;,
    3;52,53,65;,
    3;65,53,66;,
    3;53,67,66;,
    3;53,54,67;,
    3;54,55,67;,
    3;67,55,68;,
    3;55,69,68;,
    3;55,56,69;,
    3;56,57,69;,
    3;69,57,70;,
    3;57,71,70;,
    3;57,58,71;,
    3;58,59,71;,
    3;71,59,72;,
    3;59,73,72;,
    3;59,60,73;,
    3;60,61,73;,
    3;73,61,74;,
    3;61,75,74;,
    3;61,62,75;,
    3;63,77,76;,
    3;63,64,77;,
    3;64,65,77;,
    3;77,65,78;,
    3;65,79,78;,
    3;65,66,79;,
    3;66,67,79;,
    3;79,67,80;,
    3;67,81,80;,
    3;67,68,81;,
    3;68,69,81;,
    3;81,69,82;,
    3;69,83,82;,
    3;69,70,83;,
    3;70,71,83;,
    3;83,71,84;,
    3;71,85,84;,
    3;71,72,85;,
    3;72,73,85;,
    3;85,73,86;,
    3;73,87,86;,
    3;73,74,87;,
    3;74,75,87;,
    3;87,75,88;,
    3;76,77,5;,
    3;5,77,4;,
    3;77,9,4;,
    3;77,78,9;,
    3;78,79,9;,
    3;9,79,13;,
    3;79,17,13;,
    3;79,80,17;,
    3;80,81,17;,
    3;17,81,21;,
    3;81,25,21;,
    3;81,82,25;,
    3;82,83,25;,
    3;25,83,29;,
    3;83,33,29;,
    3;83,84,33;,
    3;84,85,33;,
    3;33,85,37;,
    3;85,41,37;,
    3;85,86,41;,
    3;86,87,41;,
    3;41,87,45;,
    3;87,49,45;,
    3;87,88,49;;

    MeshNormals {
     89;
     0.018373;-0.999779;-0.010223;,
     0.051727;-0.998246;-0.028781;,
     0.037551;-0.998282;-0.044978;,
     0.018373;0.999779;-0.010223;,
     0.037551;0.998282;-0.044978;,
     0.051727;0.998246;-0.028781;,
     0.013447;-0.999520;-0.027922;,
     0.025780;-0.995367;-0.092630;,
     0.013447;0.999520;-0.027922;,
     0.025780;0.995367;-0.092630;,
     0.004921;-0.999261;-0.038133;,
     0.000000;-0.995971;-0.089680;,
     0.004921;0.999261;-0.038133;,
     0.000000;0.995971;-0.089680;,
     -0.004921;-0.999261;-0.038133;,
     -0.025780;-0.995367;-0.092630;,
     -0.004921;0.999261;-0.038133;,
     -0.025780;0.995367;-0.092630;,
     -0.013447;-0.999520;-0.027922;,
     -0.037551;-0.998282;-0.044978;,
     -0.013447;0.999520;-0.027922;,
     -0.037551;0.998282;-0.044978;,
     -0.018373;-0.999779;-0.010223;,
     -0.051748;-0.998660;-0.000000;,
     -0.018373;0.999779;-0.010223;,
     -0.051748;0.998660;-0.000000;,
     -0.018373;-0.999779;0.010223;,
     -0.037551;-0.998282;0.044978;,
     -0.018373;0.999779;0.010223;,
     -0.037551;0.998282;0.044978;,
     -0.013447;-0.999520;0.027922;,
     -0.025780;-0.995367;0.092630;,
     -0.013447;0.999520;0.027922;,
     -0.025780;0.995367;0.092630;,
     -0.004921;-0.999261;0.038132;,
     0.000000;-0.995971;0.089680;,
     -0.004921;0.999261;0.038132;,
     0.000000;0.995971;0.089680;,
     0.004921;-0.999261;0.038132;,
     0.025780;-0.995367;0.092630;,
     0.004921;0.999261;0.038132;,
     0.025780;0.995367;0.092630;,
     0.013447;-0.999520;0.027922;,
     0.037551;-0.998282;0.044978;,
     0.013447;0.999520;0.027922;,
     0.037551;0.998282;0.044978;,
     0.018373;-0.999779;0.010223;,
     0.051727;-0.998246;0.028781;,
     0.018373;0.999779;0.010223;,
     0.051727;0.998246;0.028781;,
     0.157850;-0.983550;-0.087827;,
     0.135457;-0.977676;-0.160625;,
     0.076228;-0.959430;-0.271448;,
     0.000000;-0.950841;-0.309679;,
     -0.076229;-0.959429;-0.271449;,
     -0.135458;-0.977676;-0.160625;,
     -0.158464;-0.987365;-0.000000;,
     -0.135458;-0.977676;0.160625;,
     -0.076229;-0.959430;0.271448;,
     0.000000;-0.950841;0.309679;,
     0.076228;-0.959430;0.271448;,
     0.135457;-0.977676;0.160625;,
     0.157850;-0.983550;0.087827;,
     0.873845;0.000000;-0.486204;,
     0.645732;-0.000000;-0.763564;,
     0.271018;-0.000000;-0.962574;,
     0.000000;0.000000;-1.000000;,
     -0.271018;-0.000000;-0.962574;,
     -0.645733;0.000000;-0.763563;,
     -1.000000;0.000000;-0.000000;,
     -0.645734;0.000000;0.763563;,
     -0.271018;-0.000000;0.962574;,
     0.000000;0.000000;1.000000;,
     0.271018;0.000000;0.962574;,
     0.645733;0.000000;0.763563;,
     0.873846;0.000000;0.486203;,
     0.157850;0.983550;-0.087827;,
     0.135457;0.977676;-0.160625;,
     0.076228;0.959430;-0.271448;,
     0.000000;0.950841;-0.309679;,
     -0.076229;0.959429;-0.271449;,
     -0.135458;0.977676;-0.160625;,
     -0.158464;0.987365;-0.000000;,
     -0.135458;0.977676;0.160625;,
     -0.076229;0.959430;0.271448;,
     0.000000;0.950841;0.309679;,
     0.076228;0.959430;0.271448;,
     0.135457;0.977676;0.160625;,
     0.157850;0.983550;0.087827;;
     
     120;
     3;0,2,1;,
     3;3,5,4;,
     3;6,7,2;,
     3;8,4,9;,
     3;10,11,7;,
     3;12,9,13;,
     3;14,15,11;,
     3;16,13,17;,
     3;18,19,15;,
     3;20,17,21;,
     3;22,23,19;,
     3;24,21,25;,
     3;26,27,23;,
     3;28,25,29;,
     3;30,31,27;,
     3;32,29,33;,
     3;34,35,31;,
     3;36,33,37;,
     3;38,39,35;,
     3;40,37,41;,
     3;42,43,39;,
     3;44,41,45;,
     3;46,47,43;,
     3;48,45,49;,
     3;1,51,50;,
     3;1,2,51;,
     3;2,7,51;,
     3;51,7,52;,
     3;7,53,52;,
     3;7,11,53;,
     3;11,15,53;,
     3;53,15,54;,
     3;15,55,54;,
     3;15,19,55;,
     3;19,23,55;,
     3;55,23,56;,
     3;23,57,56;,
     3;23,27,57;,
     3;27,31,57;,
     3;57,31,58;,
     3;31,59,58;,
     3;31,35,59;,
     3;35,39,59;,
     3;59,39,60;,
     3;39,61,60;,
     3;39,43,61;,
     3;43,47,61;,
     3;61,47,62;,
     3;50,51,63;,
     3;63,51,64;,
     3;51,65,64;,
     3;51,52,65;,
     3;52,53,65;,
     3;65,53,66;,
     3;53,67,66;,
     3;53,54,67;,
     3;54,55,67;,
     3;67,55,68;,
     3;55,69,68;,
     3;55,56,69;,
     3;56,57,69;,
     3;69,57,70;,
     3;57,71,70;,
     3;57,58,71;,
     3;58,59,71;,
     3;71,59,72;,
     3;59,73,72;,
     3;59,60,73;,
     3;60,61,73;,
     3;73,61,74;,
     3;61,75,74;,
     3;61,62,75;,
     3;63,77,76;,
     3;63,64,77;,
     3;64,65,77;,
     3;77,65,78;,
     3;65,79,78;,
     3;65,66,79;,
     3;66,67,79;,
     3;79,67,80;,
     3;67,81,80;,
     3;67,68,81;,
     3;68,69,81;,
     3;81,69,82;,
     3;69,83,82;,
     3;69,70,83;,
     3;70,71,83;,
     3;83,71,84;,
     3;71,85,84;,
     3;71,72,85;,
     3;72,73,85;,
     3;85,73,86;,
     3;73,87,86;,
     3;73,74,87;,
     3;74,75,87;,
     3;87,75,88;,
     3;76,77,5;,
     3;5,77,4;,
     3;77,9,4;,
     3;77,78,9;,
     3;78,79,9;,
     3;9,79,13;,
     3;79,17,13;,
     3;79,80,17;,
     3;80,81,17;,
     3;17,81,21;,
     3;81,25,21;,
     3;81,82,25;,
     3;82,83,25;,
     3;25,83,29;,
     3;83,33,29;,
     3;83,84,33;,
     3;84,85,33;,
     3;33,85,37;,
     3;85,41,37;,
     3;85,86,41;,
     3;86,87,41;,
     3;41,87,45;,
     3;87,49,45;,
     3;87,88,49;;
    }

    MeshTextureCoords {
     89;
     0.000000;1.000000;,
     0.000000;0.833333;,
     0.083333;0.833333;,
     0.000000;0.000000;,
     0.083333;0.166667;,
     0.000000;0.166667;,
     0.083333;1.000000;,
     0.166667;0.833333;,
     0.083333;0.000000;,
     0.166667;0.166667;,
     0.166667;1.000000;,
     0.250000;0.833333;,
     0.166667;0.000000;,
     0.250000;0.166667;,
     0.250000;1.000000;,
     0.333333;0.833333;,
     0.250000;0.000000;,
     0.333333;0.166667;,
     0.333333;1.000000;,
     0.416667;0.833333;,
     0.333333;0.000000;,
     0.416667;0.166667;,
     0.416667;1.000000;,
     0.500000;0.833333;,
     0.416667;0.000000;,
     0.500000;0.166667;,
     0.500000;1.000000;,
     0.583333;0.833333;,
     0.500000;0.000000;,
     0.583333;0.166667;,
     0.583333;1.000000;,
     0.666667;0.833333;,
     0.583333;0.000000;,
     0.666667;0.166667;,
     0.666667;1.000000;,
     0.750000;0.833333;,
     0.666667;0.000000;,
     0.750000;0.166667;,
     0.750000;1.000000;,
     0.833333;0.833333;,
     0.750000;0.000000;,
     0.833333;0.166667;,
     0.833333;1.000000;,
     0.916667;0.833333;,
     0.833333;0.000000;,
     0.916667;0.166667;,
     0.916667;1.000000;,
     1.000000;0.833333;,
     0.916667;0.000000;,
     1.000000;0.166667;,
     0.000000;0.666667;,
     0.083333;0.666667;,
     0.166667;0.666667;,
     0.250000;0.666667;,
     0.333333;0.666667;,
     0.416667;0.666667;,
     0.500000;0.666667;,
     0.583333;0.666667;,
     0.666667;0.666667;,
     0.750000;0.666667;,
     0.833333;0.666667;,
     0.916667;0.666667;,
     1.000000;0.666667;,
     0.000000;0.500000;,
     0.083333;0.500000;,
     0.166667;0.500000;,
     0.250000;0.500000;,
     0.333333;0.500000;,
     0.416667;0.500000;,
     0.500000;0.500000;,
     0.583333;0.500000;,
     0.666667;0.500000;,
     0.750000;0.500000;,
     0.833333;0.500000;,
     0.916667;0.500000;,
     1.000000;0.500000;,
     0.000000;0.333333;,
     0.083333;0.333333;,
     0.166667;0.333333;,
     0.250000;0.333333;,
     0.333333;0.333333;,
     0.416667;0.333333;,
     0.500000;0.333333;,
     0.583333;0.333333;,
     0.666667;0.333333;,
     0.750000;0.333333;,
     0.833333;0.333333;,
     0.916667;0.333333;,
     1.000000;0.333333;;
    }

    

    MeshMaterialList {
     1;
     120;
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0;

     Material Material04 {
      0.800000;0.800000;0.800000;1.000000;;
      0.835938;
      1.000000;1.000000;1.000000;;
      0.000000;0.000000;0.000000;;
      TextureFilename {
       "WingTex.bmp";
      }
     }
    }

   }
 
