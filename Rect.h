#ifndef __RECT_H__
#define __RECT_H__

#include "SDL\include\SDL_rect.h"
#include "Defs.h"

template<class TYPE>
struct Rect
{
	TYPE x, y, w, h;

	Rect Normalised()
	{
		Rect r = (*this);
		if (w < 0 && h < 0)
			r = { x + w, y + h, -w, -h };
		else if (w < 0)
			r = { x + w, y, -w, h };
		else if (h < 0)
			r = { x, y + h, w, -h };
		return r;
	}

	explicit operator SDL_Rect() const
	{
		return { (int)x, (int)y, (int)w, (int)h };
	}

	Rect operator *(scalar sc) const
	{
		Rect n = { x * sc, y * sc, w * sc, h * sc };
		return n;
	}

	Rect operator /(scalar sc) const
	{
		Rect n = { x / sc, y / sc, w / sc, h / sc };
		return n;
	}
};

typedef Rect<scalar> sRect;
typedef Rect<int> iRect;

#endif