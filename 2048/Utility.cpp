#include "Utility.h"
#include "SDL-2.0.7/include/SDL.h"

Color const Color::Red = { 0xff,0,0 };
Color const Color::Green = { 0,0xff,0 };
Color const Color::Blue = { 0,0,0xff };
Color const Color::Black = { 0,0,0 };
Color const Color::White = { 0xff,0xff,0xff };
Color const Color::Grey = { 0x60,0x60,0x60 };

int Color::sdlLike(SDL_Surface const * screen) const
{
	return SDL_MapRGB(screen->format, r, g, b);
}