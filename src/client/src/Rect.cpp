#include "stdafx.h"
#include "Rect.h"

#include <algorithm>

using namespace std;

Rect::Rect()
{
}

Rect::Rect(const RECT & rect)
	: RECT (rect)
{
}

LONG Rect::GetX() const
{
	return left;
}

LONG Rect::GetY() const
{
	return top;
}

LONG Rect::GetWidth() const
{
	return abs(right - left);
}

LONG Rect::GetHeight() const
{
	return abs(bottom - top);
}

void Rect::Normalize()
{
	if (right < left)
		swap(left, right);
	if (bottom < top)
		swap(top, bottom);
}
