#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Loader.h"
#include "Camera.h"
#include "GameDummy.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class Graphics
{
private:
	struct constBufferPerFrame
	{

	} cbPerFrame;

	struct constBufferPerObject
	{
		DirectX::XMFLOAT4X4 WVP;
		DirectX::XMFLOAT4X4 World;
	} cbPerObject;

	DirectX::XMMATRIX world;

	Loader* loader;
	GameDummy* game;
	Camera* camera;

	VertexType* vertices;
	UINT* indices;
	NormalType* normals;

	int nObjects;
	int nVertices;
	int nIndices;
	int nNormals;

	DirectX::XMFLOAT3 playerPosition;
	float size;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* cbPerFrameBuffer;
	ID3D11Buffer* cbPerObjectBuffer;

	ID3D11InputLayout* rVertexLayout;
	ID3D11VertexShader* rVS;
	ID3D11PixelShader* rPS;

	D3D11_VIEWPORT viewport;
	IDXGISwapChain* rSwapChain;
	ID3D11Device* rDevice;
	ID3D11DeviceContext* rDeviceContext;
	ID3D11RenderTargetView* rBackbufferRTV;
	ID3D11DepthStencilView* rDepthStencilView;
	ID3D11Texture2D* rDepthStencilBuffer;

private:
	HRESULT CreateDirect3DContext(HWND &wndHandle);
	void SetViewport(int width, int height);
	HRESULT CreateDepthBuffer(int width, int height);
	HRESULT CreateShaders();
	void InitVertices();
	void SetVertices(const ObjectType& obj);
	void CreateBuffers();
	void CreateCamera();

public:
	Graphics();
	Graphics(const Graphics &obj);
	~Graphics();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, int width, int height, float screenNear, float screenFar, bool fullscreen);

	void Update();
	void Render();

	void SwapFBBuffer();
	void ReleaseCOM();
};

#endif