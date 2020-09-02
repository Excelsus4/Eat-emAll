#pragma once
#include "stdafx.h"
#include "Device.h"
#include "Vertex.h"
#include "RectObject.h"
#include "Random.h"

ID3D11Buffer* vertexBuffer;
ID3D11InputLayout* inputLayout;
D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const int VNUM = 12;
const int VSIZE = 6;
Vertex vertices[VNUM*VSIZE];
RectObject player = RectObject(D3DXVECTOR3(-0.7f, -0.7f, 0), 0.05f, D3DXVECTOR3(1, 1, 1), vertices);
float speed = 0.0001f;
vector<RectObject*> foodVector;

void InitScene() {
	random_device rd;
	Random::gen = new mt19937(rd());

	player.SetVertex();
	for (int idx = 1; idx < VNUM; idx++) {
		foodVector.push_back(new RectObject(&vertices[idx * VSIZE]));
	}
	for (auto a : foodVector) {
		a->SetVertex();
	}

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex) * VNUM * VSIZE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Create Input Layout
	{
		HRESULT hr = Device->CreateInputLayout(
			layoutDesc, ARRAYSIZE(layoutDesc), VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(), &inputLayout
		);
	}
}

void DestroyScene() {
	for (auto a : foodVector) {
		delete a;
	}
	delete Random::gen;

	inputLayout->Release();
	vertexBuffer->Release();
}

void Update() {
	if (Key->Press('A') == true)
		player.Translate(D3DXVECTOR3(-speed, 0, 0));
	else if (Key->Press('D') == true)
		player.Translate(D3DXVECTOR3(+speed, 0, 0));
	
	if (Key->Press('W') == true)
		player.Translate(D3DXVECTOR3(0, +speed, 0));
	else if (Key->Press('S') == true)
		player.Translate(D3DXVECTOR3(0, -speed, 0));

	//Check Collision
	for (auto a : foodVector) {
		if (player.CollisionCheck(*a)) {
			player.Consume(*a);
			a->DisableVertex();
		}
	}
	//always
	player.SetVertex();

	DeviceContext->UpdateSubresource(
		vertexBuffer, 0, NULL, vertices, sizeof(Vertex) * VNUM * VSIZE, 0);
}

void Render() {
	D3DXCOLOR bgColor = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1);
	DeviceContext->ClearRenderTargetView(RTV, (float*)bgColor);
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		DeviceContext->IASetInputLayout(inputLayout);

		DeviceContext->Draw(VNUM * VSIZE, 0);
	}
	SwapChain->Present(0, 0);
}