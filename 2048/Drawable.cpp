#include "Drawable.h"
#include "Window.h"

#include <string.h>



void Pixel::draw(RenderTarget & target, Transform addTransform) const
{
	target.drawPixel(Vec2i(pos), color);
}

void Line::draw(RenderTarget & target, Transform addTransform) const
{
	Vec2f dir(Vec2f(end - start));
	int32_t length = int32_t(dir.length());
	dir.normalize();

	for (Pixel pixel(start, color); (pixel.pos - Vec2f(start)).length() <= length; pixel.pos += dir)
		pixel.draw(target);
}

void Rectangle::draw(RenderTarget & target, Transform addTransform) const
{
	addTransform *= transform;
	addTransform.origin = transform.origin;

	Line line;
	line.color = outlineColor;

	Vec2i realSize = Vec2i(int32_t(size.x * addTransform.scale.x), int32_t(size.y * addTransform.scale.y));
	Vec2i topLeft = addTransform.pos - Vec2i(int32_t(realSize.x * addTransform.origin.x), int32_t(realSize.y * addTransform.origin.y));

	line.start = topLeft;
	line.end = topLeft + Vec2i(realSize.x, 0);
	line.draw(target);

	line.end = topLeft + Vec2i(0, realSize.y);
	line.draw(target);

	line.start = topLeft + realSize + Vec2i(0, 0);
	line.end = topLeft + Vec2i(realSize.x, 0);
	line.draw(target);

	line.end = topLeft + Vec2i(0, realSize.y);
	line.draw(target);


	line.color = fillColor;
	line.start = topLeft + Vec2i(1, 1);
	line.end = topLeft + Vec2i(realSize.x - 1, 1);

	for (; line.start.y <= topLeft.y + realSize.y - 1; ++line.start.y, ++line.end.y)
		line.draw(target);
}


Text::Text(char const * text, Color const & color) : color(color)
{
	set(text);
}

Text::~Text()
{
	free();
}

void Text::draw(RenderTarget & target, Transform addTransform) const
{
	addTransform.origin = transform.origin;
	target.draw(str, color, addTransform *= transform);
}

void Text::set(char const * text)
{
	if (text)
	{
		size_t strSize = strlen(text) + 1;
		alloc(strSize);
		strcpy_s(str, strSize, text);
	}
	else free();
}

void Text::alloc(uint32_t size)
{
	free();
	str = new char[size];
}

void Text::free()
{
	if (str)
	{
		delete[] str;
		str = nullptr;
	}
}


#include "Application.h"


MessageBox::MessageBox()
{
	size = Vec2i(WinSize) / 3;
	fillColor = Color::Black;
	outlineColor = Color::Red;
	setOrigin({ 0.5f, 0.5f });
	setPosition(Vec2i(WinSize) / 2);

	text.setOrigin({ 0.5f, 0.5f });
	text.setPosition(size / 2);
}

void MessageBox::set(char const * text, Type type)
{
	this->text.set(text);
	this->type = type;
}

void MessageBox::draw(RenderTarget & target, Transform addTransform) const
{
	if (type == Type::Count)
		return;

	Rectangle::draw(target, addTransform);

	addTransform *= transform;
	addTransform.pos -= Vec2i(int32_t(size.x * getOrigin().x), int32_t(size.y * getOrigin().y));

	Text title;
	title.setOrigin({ 0.5f, -1.f });
	title.setPosition({ size.x / 2, 0 });

	if (type == Type::Info)
		title.set("Information");
	else if (type == Type::Warning)
		title.set("Warning");
	else if (type == Type::Error)
		title.set("Error");

	title.draw(target, addTransform);
	text.draw(target, addTransform);

	title.setOrigin({ 0.5f, 2.f });
	title.setPosition({ size.x / 2, size.y });
	title.set("Press Enter");

	title.draw(target, addTransform);
}
