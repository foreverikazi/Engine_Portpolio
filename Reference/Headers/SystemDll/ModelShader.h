#pragma once
#define SYSTEM_EXPORT
#include "SystemDef.h"
#include "Shader.h"

EXTERN class SYSTEM_DLL ModelShader : public Shader
{
public :
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, const TCHAR* vsFilename, const TCHAR* psFilename);
	bool SetMatrixShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX worldMat, const XMMATRIX viewMat, const XMMATRIX projMat);
	bool SetLightShaderParameters(ID3D11DeviceContext* deviceContext, const XMFLOAT4 ambientColor, const XMFLOAT4 diffuseColor, const XMFLOAT3 lightDirection, const XMFLOAT4 specularColor, const float specularPower);
	bool SetCameraShaderParameters(ID3D11DeviceContext* deviceContext, const XMFLOAT3 cameraPosition);
	bool UpdateShader(ID3D11DeviceContext* deviceContext);
	void ReleaseShader();

public :
	ModelShader();
	virtual ~ModelShader();

private :
	ID3D11SamplerState* mSampleState;
	ID3D11Buffer*		mLightBuffer;
	ID3D11Buffer*		mCameraBuffer;
};

