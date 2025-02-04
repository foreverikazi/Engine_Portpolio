#include "pch.h"
#include "TextureUtil.h"
#include "../include/FreeImage.h"
#include <atlstr.h>

TextureUtil::TextureUtil() : 
	mDiffuseRV(nullptr),
	mTexture(nullptr)
{

}

TextureUtil::~TextureUtil()
{
	if (mDiffuseRV)
	{
		mDiffuseRV->Release();
		mDiffuseRV = nullptr;
	}

	if (mTexture)
	{
		mTexture->Release();
		mTexture = nullptr;
	}
}

ID3D11ShaderResourceView* TextureUtil::LoadTextureUtil(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wstring fileName)
{
	FIBITMAP *dib(0);
	unsigned char* bits(0);
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	string path = converter.to_bytes(fileName.c_str());

	fif = FreeImage_GetFileType(path.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(path.c_str());

	if (fif == FIF_UNKNOWN)
		return nullptr;

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, path.c_str());

	//if the image failed to load, return failure
	if (!dib)	return nullptr;

	//이미지 형태가 32 비트가 아니면 32로 변환. dx11에 24 format 이 없는 듯하다..
	if (FreeImage_GetBPP(dib) != 32) {
		dib = FreeImage_ConvertTo32Bits(dib);
	}
	FreeImage_FlipVertical(dib);

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	unsigned int width = FreeImage_GetWidth(dib);
	unsigned int height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if ((bits == 0) || (width == 0) || (height == 0))
		return nullptr;


	//텍스처의 구조체를 설정합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 빈 텍스처를 생성합니다.
	ID3D11Texture2D*			texture = nullptr;
	HRESULT hResult = device->CreateTexture2D(&textureDesc, nullptr, &texture);
	if (FAILED(hResult)) {
		return nullptr;
	}

	//이미지 사이즈
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	// 이미지 데이터를 텍스처에 복사합니다.
	deviceContext->UpdateSubresource(texture, 0, nullptr, bits, rowPitch, 0);

	// 셰이더 리소스 뷰 구조체를 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스처의 셰이더 리소스 뷰를 만듭니다.
	ID3D11ShaderResourceView* texData = nullptr;
	hResult = device->CreateShaderResourceView(texture, &srvDesc, &texData);
	if (FAILED(hResult)) {
		return nullptr;
	}

	// 이 텍스처에 대해 밉맵을 생성합니다.
	deviceContext->GenerateMips(texData);

	texture->Release();
	FreeImage_Unload(dib);
	return texData;
}