#pragma once
#include "stdafx.h"
#include "Vertex.h"

class RectObject
{
public:
	RectObject(D3DXVECTOR3 p, float s, D3DXVECTOR3 c, Vertex* v);
	RectObject(Vertex* v);
	virtual ~RectObject();

	bool CollisionCheck(const RectObject& target) const;
	void Consume(const RectObject& food);

	void SetVertex();
	void Translate(const D3DXVECTOR3 vector);
	void DisableVertex();
	bool isAlive() const { return alive; };
private:
	D3DXVECTOR3 pos;
	float size;
	D3DXVECTOR3 color;
	Vertex* vertices;
	bool alive;
};

