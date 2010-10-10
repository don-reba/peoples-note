#pragma once

struct Rect : public RECT
{
	Rect();

	explicit Rect(const RECT & rect);

	LONG GetX()      const;
	LONG GetY()      const;
	LONG GetWidth()  const;
	LONG GetHeight() const;

	void Normalize();
};
