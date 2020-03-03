#pragma once
#include <stdio.h>
#include "Vec2.h"
#include "Drawable.h"


static size_t const TileBaseValue = 2;

static size_t const AnimationLength = 200;

static Color const TileColor(Color::White);

static Color const WallTileColor(Color::Grey);



static char const WallCharacter = '#';



class Board : public Drawable, public Transformable
{
public:

	class Tile : public Rectangle
	{
	public:

		explicit Tile(bool isWall = false);


		virtual void draw(class RenderTarget& target, Transform addTransform = Transform()) const override;


		void setAsWall(bool isWall);

		void setValue(size_t newValue) { value = newValue; }

		// returns merged value
		int32_t mergeWith(Tile& other);


		// sets merged to false
		void confirmMerge();


		bool canMergeWith(Tile const& other) const;


		size_t getValue() const { return value * !isWall; }

		bool isAWall() const { return isWall; }

		bool wasMerged() const { return merged; };

	private:

		bool isWall = false;

		bool merged = false;

		size_t value = 0;
	};

public:

	Board();


	virtual void draw(class RenderTarget& target, Transform addTransform = Transform()) const override;


	bool update(size_t dt);

	void reset();

	void setDefaultShape(size_t n);


	int32_t shiftTo(Direction direction);

	bool canShiftTo(Direction direction) const;

	bool addNewTile();


	bool undo();


	bool saveTo(char const* src) const;

	bool loadFrom(char const* src);

private:

	Vec2i findIdxAfterShift(Vec2i const& idx, Direction dir) const;

	int32_t collapseTilesTo(Vec2i const& tileIdx, Direction direction);

	void fillPrevData();

private:

	Vector<Vector<Tile>> data;

	Vector<Vector<Tile>> prevData;

	Vector<Vector<Vec2i>> animationData;

	bool animate = false;

	size_t animationTime = 0;

	Vec2u size;
};