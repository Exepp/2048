#pragma once
#include "sdl-2.0.7/include/SDL.h"
#include "Vec2.h"
#include "Drawable.h"


struct WindowProperties
{
	Vec2u size;
	char title[32];
	SDL_WindowFlags flags = SDL_WindowFlags(0);
	bool cursorVisible = true;
};


class RenderTarget
{
	friend class Window;

public:

	RenderTarget(SDL_Renderer* renderer, WindowProperties const & prop);

	~RenderTarget();


	void draw(SDL_Surface* sprite, Transform const& transform);

	void draw(char const* str, Color const & color, Transform transform = Transform());

	void drawPixel(Vec2i const& pos, Color const& color);

private:

	void clear(Color const& color);

	void display();

private:

	SDL_Renderer* renderer = nullptr;
	SDL_Surface* screen = nullptr;
	SDL_Surface* charset = nullptr;
	SDL_Texture* scrtex = nullptr;
};




class Window
{
public:

	explicit Window(WindowProperties const & prop);

	Window(Window const &) = delete;

	Window(Window &&) = default;

	Window& operator=(Window const& ) = delete;

	Window& operator=(Window &&) = delete;

	~Window();


	void clear(Color const& color = Color::Black);

	void draw(Drawable const& drawable);

	void display();

	
	bool pollEvent(SDL_Event& event);


	void setTitle(char const * title);

	void setCursorVisible(bool visible = true);

private:

	SDL_Window* window = nullptr;

	SDL_Renderer* renderer = nullptr;

	RenderTarget* target = nullptr;

	WindowProperties properties;
};