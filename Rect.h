#ifndef __RECT_H__
#define __RECT_H__

#include "Defs.h"
#include "SDL\include\SDL_rect.h"

template<class TYPE>
struct Rect
{
	TYPE x, y, w, h;

	Rect(TYPE x, TYPE y, TYPE w, TYPE h) : x(x), y(y), w(w), h(h) {}
	Rect() : x(0), y(0), w(0), h(0){}
	explicit Rect(const SDL_Rect & r) : x(r.x), y(r.y), w(r.w), h(r.w){}

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