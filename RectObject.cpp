#pragma once
#include "stdafx.h"
#include "RectObject.h"
#include "Random.h"

RectObject::RectObject(D3DXVECTOR3 p, float s, D3DXVECTOR3 c, Vertex* v) :
	pos(p), size(s), color(c), vertices(v), alive(true)
{
}

RectObject::RectObject(Vertex* v):
	vertices(v), alive(false){
}

RectObject::~RectObject()
{
}

bool RectObject::CollisionCheck(const RectObject & target) const
{
	if (!target.alive)
		return false;

	if (pos.x + size > target.pos.x - size &&
		pos.x - size < target.pos.x + size &&
		pos.y + size > target.pos.y - size &&
		pos.y - size < target.pos.y + size) {
		return true;
	}
	return false;
}

void RectObject::Consume(const RectObject & food)
{
	// TOTAL AREA BASED MIXTURE SYSTEM
	float area = pow(food.size, 2);
	D3DXVECTOR3 foodPaint = food.color * area;

	float original = pow(size, 2);
	D3DXVECTOR3 myPaint = color * original;

	float newArea = area + original;
	D3DXVECTOR3 totalPaint = foodPaint + myPaint;

	size = sqrt(newArea);
	color = totalPaint / newArea;
}

void RectObject::SetVertex()
{
	vertices[0].Position = D3DXVECTOR3(pos.x - size, pos.y - size, 0.0f);
	vertices[1].Position = D3DXVECTOR3(pos.x - size, pos.y + size, 0.0f);
	vertices[2].Position = D3DXVECTOR3(pos.x + size, pos.y - size, 0.0f);
	vertices[3].Position = D3DXVECTOR3(pos.x + size, pos.y - size, 0.0f);
	vertices[4].Position = D3DXVECTOR3(pos.x - size, pos.y + size, 0.0f);
	vertices[5].Position = D3DXVECTOR3(pos.x + size, pos.y + size, 0.0f);

	vertices[0].Color = color;
	vertices[1].Color = color;
	vertices[2].Color = color;
	vertices[3].Color = color;
	vertices[4].Color = color;
	vertices[5].Color = color;
}

void RectObject::Translate(const D3DXVECTOR3 vector)
{
	pos += vector;
}

void RectObject::DisableVertex()
{
	size = 0;
	alive = false;
	SetVertex();
}

void RectObject::Randomize()
{
	uniform_real_distribution<float> colorDice(0, 1);
	uniform_real_distribution<float> sizeDice(10.0f, 40.0f);
	uniform_real_distribution<float> XposDice(0, Width);
	uniform_real_distribution<float> YposDice(0, Height);
	pos = D3DXVECTOR3(XposDice(*Random::gen), YposDice(*Random::gen), 0);
	size = sizeDice(*Random::gen);
	color = D3DXVECTOR3(colorDice(*Random::gen), colorDice(*Random::gen), colorDice(*Random::gen));

	alive = true;
	SetVertex();
}
