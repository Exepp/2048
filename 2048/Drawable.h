#pragma once
#include "Vec2.h"
#include "Utility.h"


struct Transform
{
	Transform() = default;

	Transform(Vec2i pos, Vec2f origin, Vec2f scale) : pos(pos), origin(origin), scale(scale) {}

	// origin is omitted
	Transform& operator*=(Transform const& other)
	{
		pos += other.pos;
		scale.x *= other.scale.x;
		scale.y *= other.scale.y;
		return *this;
	}



	Vec2i pos;

	// as ratio of current size
	Vec2f origin;

	Vec2f scale = Vec2f(1.f, 1.f);

};



struct Drawable
{
	virtual void draw(class RenderTarget& target, Transform addTransform = Transform()) const = 0;
};



class Transformable
{
public:

	void setPosition(Vec2i const& newPos) { transform.pos = newPos; }

	void setOrigin(Vec2f const& newOrigin) { transform.origin = newOrigin; }

	void setScale(Vec2f const& newScale) { transform.scale = newScale; }

	
	void move(Vec2i const& moveVec) { setPosition(transform.pos + moveVec); }

	void scale(Vec2f const& scaleVec) { setScale({ transform.scale.x * scaleVec.x, transform.scale.y * scaleVec.y }); }



	Vec2i const& getPosition() const { return transform.pos; }

	Vec2f const& getOrigin() const { return transform.origin; }
	
	Vec2f const& getScale() const { return transform.scale; }

protected:

	Transform transform;
};



struct Pixel : public Drawable
{
	Pixel() = default;
	Pixel(Vec2i const& pos, Color const& color) : pos(pos), color(color) {}

	void draw(RenderTarget& target, Transform addTransform = Transform()) const override;


	Vec2f pos;

	Color color;
};



struct Line : public Drawable
{
	void draw(RenderTarget& target, Transform addTransform = Transform()) const override;



	Vec2i start;

	Vec2i end;

	Color color;
};



class Rectangle : public Drawable, public Transformable
{
public:

	void draw(RenderTarget& target, Transform addTransform = Transform()) const override;


public:

	Vec2i size;

	Color fillColor;

	Color outlineColor;

};


class Text : public Drawable, public Transformable
{
public:

	Text(char const* text = nullptr, Color const & color = Color::White);

	Text(Text const&) = delete;

	~Text();


	void draw(RenderTarget& target, Transform addTransform = Transform()) const override;


	void set(char const* text);

private:

	void alloc(uint32_t size);

	void free();

public:

	Color color = Color::White;

private:

	char* str = nullptr;
};


struct MessageBox : public Rectangle
{
	enum class Type { Info, Warning, Error, Count };



	MessageBox();


	void set(char const* text, Type type = Type::Info);


	void draw(RenderTarget& target, Transform addTransform = Transform()) const override;



	Text text;

	Type type = Type::Count;

};