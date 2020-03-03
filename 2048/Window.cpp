#include "Window.h"
#include <stdio.h>
#include <string.h>

#include "Assert.h"
#include "Application.h"


RenderTarget::RenderTarget(SDL_Renderer* renderer, WindowProperties const & prop) : renderer(renderer)
{
	screen = SDL_CreateRGBSurface(0, prop.size.x, prop.size.y, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, prop.size.x, prop.size.y);

	ASSERT((charset = SDL_LoadBMP("./cs8x8.bmp")), "SDL_ERROR: Charset loading");
	SDL_SetColorKey(charset, true, 0x000000);
}

RenderTarget::~RenderTarget()
{
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
}

void RenderTarget::draw(SDL_Surface * sprite, Transform const& transform)
{
	if (sprite)
	{
		Vec2i realSize(int32_t(sprite->w * transform.scale.x), int32_t(sprite->h * transform.scale.y));
		SDL_Rect dest;

		dest.x = int(transform.pos.x - realSize.x * transform.origin.x);
		dest.y = int(transform.pos.y - realSize.y * transform.origin.y);
		dest.w = realSize.x;
		dest.h = realSize.y;

		SDL_BlitScaled(sprite, NULL, screen, &dest);
	}
}

void RenderTarget::draw(char const * str, Color const & color, Transform transform)
{
	if (!str)
		return;


	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = int(8 * transform.scale.x);
	d.h = int(8 * transform.scale.y);

	Vec2i realSize(strlen(str) * d.w, d.h);
	Vec2i startPos = transform.pos;

	while (*str) 
	{
		int c = *str & 255;
		if (c == '\n')
		{
			transform.pos.y += d.h + 3;
			transform.pos.x = startPos.x;
			str++;
			continue;
		}

		s.x = (c % 16) * 8;
		s.y = (c / 16) * 8;


		d.x = int32_t(transform.pos.x - realSize.x * transform.origin.x);
		d.y = int32_t(transform.pos.y - realSize.y * transform.origin.y);

		SDL_SetSurfaceColorMod(charset, color.r, color.g, color.b);
		SDL_BlitScaled(charset, &s, screen, &d);


		d.w = int(8 * transform.scale.x);
		d.h = int(8 * transform.scale.y);

		transform.pos.x += d.w;
		str++;
	};
}

void RenderTarget::drawPixel(Vec2i const & pos, Color const & color)
{
	int bpp = screen->format->BytesPerPixel;
	if (pos.x >= 0 && pos.y >= 0 && pos.x < int32_t(WinSize.x) && pos.y < int32_t(WinSize.y))
	{
		Uint8 *p = (Uint8 *)screen->pixels + pos.y * screen->pitch + pos.x * bpp;
		*(Uint32 *)p = color.sdlLike(screen);
	}
}




void RenderTarget::clear(Color const& color)
{
	SDL_FillRect(screen, NULL, color.sdlLike(screen));
}

void RenderTarget::display()
{
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	//SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}




Window::Window(WindowProperties const & prop) : properties(prop)
{
	ASSERT(!SDL_Init(SDL_INIT_EVERYTHING), "SDL_ERROR: Init");
	ASSERT(!SDL_CreateWindowAndRenderer(prop.size.x, prop.size.y, prop.flags, &window, &renderer), "SDL_ERROR: Window creation");

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, prop.size.x, prop.size.y);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	target = new RenderTarget(renderer, prop);

	setTitle(prop.title);
	setCursorVisible(prop.cursorVisible);
}

Window::~Window()
{
	delete target;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::clear(Color const& color)
{
	target->clear(color);
}

void Window::draw(Drawable const & drawable)
{
	drawable.draw(*target);
}

void Window::display()
{
	target->display();
}

bool Window::pollEvent(SDL_Event & event)
{
	return SDL_PollEvent(&event);
}

void Window::setTitle(char const * title)
{
	ASSERT((strlen(title) < sizeof(WindowProperties::title)), "setTitle_ERROR: Title lenght is too long");

	strcpy_s(properties.title, title);
	SDL_SetWindowTitle(window, properties.title);
}

void Window::setCursorVisible(bool visible)
{
	if(visible)
		SDL_ShowCursor(SDL_ENABLE);
	else
		SDL_ShowCursor(SDL_DISABLE);
	properties.cursorVisible = visible;
}