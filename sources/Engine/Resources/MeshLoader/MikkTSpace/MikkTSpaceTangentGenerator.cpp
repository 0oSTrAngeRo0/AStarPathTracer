#include "Engine/Resources/MeshLoader/mikktspace.h"

#include "Engine/Resources/MeshLoader/MikkTSpaceTangentGenerator.h"
#include "Engine/Resources/MeshLoader/MeshResourceUtilities.h"

bool MikkTSpaceTangentGenerator::GenerateTangent(MeshData& mesh) {
	SMikkTSpaceInterface interface;
	interface.m_getNumFaces = GetNumFaces;
	interface.m_getNumVerticesOfFace = GetNumVerticesOfFace;
	interface.m_getPosition = GetPosition;
	interface.m_getNormal = GetNormal;
	interface.m_getTexCoord = GetTexCoord;
	interface.m_setTSpaceBasic = SetTSpaceBasic;
	//interface.m_setTSpace = SetTSpace;
	interface.m_setTSpace = nullptr;

	SMikkTSpaceContext context;
	context.m_pInterface = &interface;
	context.m_pUserData = &mesh;
	return genTangSpaceDefault(&context);
}

int MikkTSpaceTangentGenerator::GetNumFaces(const SMikkTSpaceContext* pContext) {
	MeshData& mesh = *static_cast<MeshData*>(pContext->m_pUserData);
	return mesh.indices.size();
}

int MikkTSpaceTangentGenerator::GetNumVerticesOfFace(const SMikkTSpaceContext* pContext, const int iFace) {
	return 3;
}

void MikkTSpaceTangentGenerator::GetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert) {
	MeshData& mesh = *static_cast<MeshData*>(pContext->m_pUserData);
	uint32_t index = mesh.indices[iFace][iVert];
	glm::vec3 value = mesh.positions[index];
	fvPosOut[0] = value.x;
	fvPosOut[1] = value.y;
	fvPosOut[2] = value.z;
}

void MikkTSpaceTangentGenerator::GetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert) {
	MeshData& mesh = *static_cast<MeshData*>(pContext->m_pUserData);
	uint32_t index = mesh.indices[iFace][iVert];
	glm::vec3 value = mesh.normals[index];
	fvNormOut[0] = value.x;
	fvNormOut[1] = value.y;
	fvNormOut[2] = value.z;
}

void MikkTSpaceTangentGenerator::GetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert) {
	MeshData& mesh = *static_cast<MeshData*>(pContext->m_pUserData);
	uint32_t index = mesh.indices[iFace][iVert];
	glm::vec2 value = mesh.uvs[index];
	fvTexcOut[0] = value.x;
	fvTexcOut[1] = value.y;
}

void MikkTSpaceTangentGenerator::SetTSpaceBasic(
	const SMikkTSpaceContext* pContext,
	const float fvTangent[],
	const float fSign,
	const int iFace,
	const int iVert
) {
	glm::vec4 tangent(fvTangent[0], fvTangent[1], fvTangent[2], fSign);

	MeshData& mesh = *static_cast<MeshData*>(pContext->m_pUserData);
	uint32_t index = mesh.indices[iFace][iVert];
	mesh.tangents[index] = tangent;
}
