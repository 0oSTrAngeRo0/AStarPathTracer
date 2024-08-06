#pragma once

class MeshData;
class SMikkTSpaceContext;

class MikkTSpaceTangentGenerator {
public:
	static bool GenerateTangent(MeshData& mesh);
private:
	static int GetNumFaces(const SMikkTSpaceContext* pContext);
	static int GetNumVerticesOfFace(const SMikkTSpaceContext* pContext, const int iFace);
	static void GetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert);
	static void GetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert);
	static void GetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert);
	static void SetTSpaceBasic(
		const SMikkTSpaceContext* pContext, 
		const float fvTangent[], 
		const float fSign, 
		const int iFace, 
		const int iVert
	);
};