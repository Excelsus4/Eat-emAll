#pragma once
#include "stdafx.h"
#include "Device.h"
#include "Vertex.h"
#include "RectObject.h"
#include "Random.h"

Shader* shader;

ID3D11Buffer* vertexBuffer;

const int VNUM = 1;
const int VSIZE = 6;
Vertex vertices[VNUM*VSIZE];
RectObject player = RectObject(D3DXVECTOR3(50.0f, 50.0f, 0), 25.0f, D3DXVECTOR3(1, 1, 1), vertices);
float speed = 0.4f;
vector<RectObject*> foodVector;
vector<RectObject*> foodPool;

D3DXMATRIX W, V, P;

void InitScene() {
	shader = new Shader(L"../_Shaders/005_WVP.fx");

	random_device rd;
	Random::gen = new mt19937(rd());

	player.SetVertex();
	//for (int idx = 1; idx < VNUM; idx++) {
	//	foodVector.push_back(new RectObject(&vertices[idx * VSIZE]));
	//}
	//for (auto a : foodVector) {
	//	a->SetVertex();
	//}

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
}

void DestroyScene() {
	for (auto a : foodVector) {
		delete a;
	}

	vertexBuffer->Release();
	delete Random::gen;
	delete shader;
}

void Update() {
	// WVP
	D3DXMatrixIdentity(&W);
	D3DXMatrixIdentity(&V);
	D3DXMatrixIdentity(&P);

	//View
	D3DXVECTOR3 eye(0, 0, -1);
	D3DXVECTOR3 at(0, 0, 0);
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixLookAtLH(&V, &eye, &at, &up);

	//Projection
	D3DXMatrixOrthoOffCenterLH(&P, 0, (float)Width, 0, (float)Height, -1, 1);

	shader->AsMatrix("World")->SetMatrix(W);
	shader->AsMatrix("View")->SetMatrix(V);
	shader->AsMatrix("Projection")->SetMatrix(P);

	// Character Movement
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
		//ImGUI
		if (ImGui::Button("Create")) {

		}

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//DeviceContext->Draw(VNUM * VSIZE, 0);
		shader->Draw(0, 0, VNUM*VSIZE);
	}

	ImGui::Render();
	SwapChain->Present(0, 0);
}