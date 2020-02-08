#include "pch.h"
#include "ResourceExportFunc.h"
#include "BufferMgr.h"
#include "Model.h"
#include "StringUtil.h"
#include "TextureUtil.h"
#include "Buffer.h"

EXTERN RESOURCE_DLL bool AddBuffer(ID3D11Device* device, const BUFFERTYPE bufferType, const SHADERTYPE shaderType, const TCHAR* key)
{
	return (*(BufferMgr::GetInst()))->AddBuffer(device, bufferType, shaderType, key);
}

EXTERN RESOURCE_DLL void UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	(*(BufferMgr::GetInst()))->UpdateBuffers(deviceContext);
}

EXTERN RESOURCE_DLL void RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	(*(BufferMgr::GetInst()))->RenderBuffers(deviceContext);
}

EXTERN RESOURCE_DLL void LoadTextureBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const TCHAR* key, const TCHAR* fileName)
{
	(*(BufferMgr::GetInst()))->LoadTextureBuffer(device, deviceContext, key, fileName);
}

EXTERN RESOURCE_DLL Buffer* FindBuffer(const TCHAR* key)
{
	return (*(BufferMgr::GetInst()))->FindBuffer(key);
}

EXTERN RESOURCE_DLL void ReleaseBuffers(void)
{
	(*(BufferMgr::GetInst()))->ReleaseBuffers();
}

// �� �ӽ�
EXTERN RESOURCE_DLL bool LoadModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring pPath)
{
	return (*(Model::GetInst()))->LoadModel(device, deviceContext, pPath);
}

EXTERN RESOURCE_DLL void RenderModel(ID3D11DeviceContext* deviceContext)
{
	(*(Model::GetInst()))->RenderModel(deviceContext);
}

EXTERN RESOURCE_DLL void ReleaseModel()
{
	(*(Model::GetInst()))->ReleaseModel();
}

// Resource
EXTERN RESOURCE_DLL void ReleaseResource()
{
	(*(BufferMgr::GetInst()))->DestroyInst();
	(*(Model::GetInst()))->DestroyInst();
	(*(StringUtil::GetInst()))->DestroyInst();
	(*(TextureUtil::GetInst()))->DestroyInst();
}


