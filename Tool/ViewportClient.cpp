﻿// ViewportClient.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "ViewportClient.h"
#include "SystemExportFunc.h"
#include "ResourceExportFunc.h"
#include <filesystem>
#include "Buffer.h"
#include "FrustumCulling.h"
#include "BitmapBuffer.h"

// ViewportClient
namespace fs = std::experimental::filesystem::v1;

IMPLEMENT_DYNCREATE(ViewportClient, CFormView)

ViewportClient::ViewportClient()
	: CFormView(IDD_VIEWPORT),
	mScreenWidth(1600),
	mScreenHeight(600)
{

}

ViewportClient::~ViewportClient()
{
	ReleaseModel();
	ReleaseShader();
	ReleaseBuffers();
	Release3D();

	ReleaseResource();
	ReleaseSystem();
	(*(FrustumCulling::GetInst()))->DestroyInst();
}

void ViewportClient::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ViewportClient, CFormView)
END_MESSAGE_MAP()


// ViewportClient 진단

#ifdef _DEBUG
void ViewportClient::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ViewportClient::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
void ViewportClient::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	
	mHwnd = m_hWnd; 
	mHinstance = AfxGetInstanceHandle();

	InitializeD3D(mScreenWidth, mScreenHeight, VSYNC_ENABLED, mHwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	InitializeInput(mHinstance, AfxGetMainWnd()->m_hWnd);
	InitCamera();
	InitializeTimer();
	InitShader();
	InitBuffers();

	//InitModel();
	InitLight();
}

void ViewportClient::OnDraw(CDC* pDC)
{
	ID3D11DeviceContext* deviceContext = GetDeviceContext();
	float gray = 56.f / 255.f;
	BeginScene(gray, gray, gray, 1.0f);
	UpdateCamera();
	UpdateInput();
	UpdateTimer();
	UpdateBuffers(deviceContext);

	// 임시 회전 적용
	Buffer* cube = FindBuffer(L"LightCube");
	Buffer* grid = FindBuffer(L"Grid");
	Buffer* sky = FindBuffer(L"Sky");
	
	
	XMFLOAT3 rot = cube->GetRotation();
	cube->SetRotation(XMFLOAT3(rot.x, rot.y + GetElapsedTime(), rot.z));
	
	
	// 라이트 큐브
	FrustumCulling* f = (*(FrustumCulling::GetInst()));
	f->InitializeFrustum(SCREEN_FAR, GetViewMatrix(), GetProjectionMatrix());
	if (f->CullingSphere(cube->GetPosition(), 1.0f) == true)
		cube->RenderBuffers(deviceContext); 
	
	// 그리드
	grid->RenderBuffers(deviceContext);

	// 스카이
	sky->RenderBuffers(deviceContext);

	// 2D 텍스처
	Buffer* bitmap = FindBuffer(L"Bitmap");
	bitmap->RenderBuffers(deviceContext);
	
	// 텍스처 큐브
	//Buffer* textureCube = FindBuffer(L"TextureCube");
	//textureCube->RenderBuffers(deviceContext);

	//int fps = GetFPS();
	//RenderBuffers(deviceContext);
		
	/*
	ModelRender(deviceContext);
	ShaderRender(GetDeviceContext(), GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	*/

	Invalidate(false); 
	EndScene();
}


#endif
#endif //_DEBUG

// ViewportClient 메시지 처리기

void ViewportClient::InitCamera()
{
	float fieldOfView = XM_PI / 4.0f;
	float screenAspect = (float)mScreenWidth / (float)mScreenHeight;
	InitializeProjectionMatrix(fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_FAR);
	InitializeCamera(XMFLOAT3(0, 1, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(0, 10, -50));
	SetCameraRotation(XMFLOAT3(0, 0, 0));
	//SetCameraRotation(XMFLOAT3(90, 0, 180));
}

void ViewportClient::InitShader()
{
	ID3D11Device* device = GetDevice();

	WCHAR vs[] = L"../Shader/LightShader.vs";
	WCHAR ps[] = L"../Shader/LightShader.ps";
	InitializeShader(device, mHwnd, vs, ps, SHADERTYPE::LIGHT);
	
	WCHAR vs2[] = L"../Shader/TextureShader.vs";
	WCHAR ps2[] = L"../Shader/TextureShader.ps";
	InitializeShader(device, mHwnd, vs2, ps2, SHADERTYPE::TEXTURE);

	WCHAR vs3[] = L"../Shader/ColorShader.vs";
	WCHAR ps3[] = L"../Shader/ColorShader.ps";
	InitializeShader(device, mHwnd, vs3, ps3, SHADERTYPE::COLORVERTEX);

	WCHAR vs4[] = L"../Shader/SkyShader.vs";
	WCHAR ps4[] = L"../Shader/SkyShader.ps";
	InitializeShader(device, mHwnd, vs4, ps4, SHADERTYPE::SKY);
}

void ViewportClient::InitBuffers()
{
	ID3D11Device* device = GetDevice();
	ID3D11DeviceContext* devicecContext = GetDeviceContext();
	//AddBuffer(device, BUFFERTYPE::COLOR_CUBE, L"ColorCube");

	// 버퍼랑 셰이더를 클래스 하나로 합치는게 편할수도
	// Texture Cube
	AddBuffer(device, BUFFERTYPE::TEXTURE_CUBE, SHADERTYPE::TEXTURE, L"TextureCube");
	LoadTextureBuffer(device, devicecContext, L"TextureCube", L"../Content/Cube/seafloor.png");
	
	AddBuffer(device, BUFFERTYPE::GRID, SHADERTYPE::COLORVERTEX, L"Grid");
	AddBuffer(device, BUFFERTYPE::LIGHT_CUBE, SHADERTYPE::LIGHT, L"LightCube");
	LoadTextureBuffer(device, devicecContext, L"LightCube", L"../Content/Cube/seafloor.png");

	AddBuffer(device, BUFFERTYPE::SKY, SHADERTYPE::SKY, L"Sky");
	LoadTextureBuffer(device, devicecContext, L"Sky", L"../Content/Terrain/skymap.dds");

	AddBuffer(device, BUFFERTYPE::BITMAP, SHADERTYPE::TEXTURE, L"Bitmap");
	Buffer* bitmap = FindBuffer(L"Bitmap");
	BitmapBuffer * bit = dynamic_cast<BitmapBuffer*>(bitmap);
	LoadTextureBuffer(device, devicecContext, L"Bitmap", L"../Content/Cube/seafloor.png");
	bit->SetTexturePosition(devicecContext, XMFLOAT3(0, 0, 0), XMFLOAT2(3, 3));
}

void ViewportClient::InitModel()
{
	ID3D11Device* device = GetDevice();
	ID3D11DeviceContext* deviceContext = GetDeviceContext();

	fs::path contentPolderPath = fs::current_path().parent_path() /= "Content";
	wstring strPath = contentPolderPath.c_str();

	//LoadModel(device, deviceContext, strPath + L"\\Weapon\\Sword.FBX");
	LoadModel(device, deviceContext, strPath + L"\\Weapon\\Alisa.fbx");
}

void ViewportClient::InitLight()
{
	SetAmbientColor(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSpecularColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	SetLightDirection(XMFLOAT3(0.2f, -0.1f, 1.0f));
	SetSpecularPower(32.0f);
}
