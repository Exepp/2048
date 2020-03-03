#include "Board.h"
#include "Window.h"
#include "Application.h"


static Vec2i const Directions[uint8_t(Direction::Count)] = { {-1,0},{1,0},{0,-1},{0,1} };


Board::Tile::Tile(bool isWall)
{
	outlineColor = TileColor;
	setAsWall(isWall);
	value = TileBaseValue;
}

void Board::Tile::draw(RenderTarget & target, Transform addTransform) const
{
	Rectangle::draw(target, addTransform);
	addTransform.pos -= Vec2i(int32_t(getOrigin().x * size.x), int32_t(getOrigin().y * size.y)) / 2;

	if (value && !isWall)
	{
		Text valueTxt(nullptr, Color::Grey);
		valueTxt.scale({ 1.5f,2 });
		valueTxt.setPosition(Vec2i(size.x, size.y) / 2);
		valueTxt.setOrigin({ 0.5f, 0.5f });

		char temp[32];
		sprintf_s(temp, "%d", value);

		valueTxt.set(temp);
		valueTxt.draw(target, addTransform *= transform);
	}
}


void Board::Tile::setAsWall(bool isWall)
{
	this->isWall = isWall;
	if (isWall)
		fillColor = WallTileColor;
	else fillColor = TileColor;
}

int32_t Board::Tile::mergeWith(Tile & other)
{
	merged = value && other.value;
	value += other.value;
	other.value = 0;
	return int32_t(merged * value);
}

void Board::Tile::confirmMerge()
{
	merged = false;
}

bool Board::Tile::canMergeWith(Tile const& other) const
{
	return (!isWall && !other.isWall && !merged && !other.merged && (!value || !other.value || other.value == value));
}





Board::Board()
{
	setDefaultShape(4);
}

void Board::draw(RenderTarget & target, Transform addTransform) const
{
	addTransform *= transform;

	int32_t tileSize = size_t(float_t(WinSize.y / size.y) * 0.7f);
	int32_t tileBorderSize = int32_t(tileSize * 0.2);

	addTransform.pos -= (tileSize * Vec2i(size.x, size.y) + tileBorderSize * Vec2i(size.x - 1, size.y - 1)) / 2;

	if (animate)
	{
		for (size_t i = 0; i < size.y; ++i)
			for (size_t j = 0; j < size.x; ++j)
			{
				Transform temp = addTransform;
				temp.pos += Vec2i(j, i) * tileSize + Vec2i(tileBorderSize * j, tileBorderSize * i);

				Vec2i moveVec = animationData[i][j];
				if (moveVec.x || moveVec.y)
				{
					Tile tempTile;
					tempTile.size = { tileSize, tileSize };
					tempTile.setValue(0);
					tempTile.draw(target, temp);

				}
				else
				{
					Tile tempTile = prevData[i][j];
					tempTile.size = { tileSize, tileSize };
					tempTile.draw(target, temp);

				}
			}

		for (size_t i = 0; i < size.y; ++i)
			for (size_t j = 0; j < size.x; ++j)
				if (animationData[i][j].x || animationData[i][j].y)
				{
					Transform temp = addTransform;
					temp.pos += Vec2i(j, i) * tileSize + Vec2i(tileBorderSize * j, tileBorderSize * i);

					Tile tempTile = prevData[i][j];
					tempTile.size = { tileSize, tileSize };
					tempTile.setPosition(Vec2i(Vec2f(animationData[i][j]) * float_t(tileSize + tileBorderSize) * float_t(animationTime) / float_t(AnimationLength)));

					tempTile.draw(target, temp);
				}
	}
	else
		for (size_t i = 0; i < size.y; ++i)
			for (size_t j = 0; j < size.x; ++j)
			{
				Transform temp = addTransform;
				temp.pos += Vec2i(j, i) * tileSize + Vec2i(tileBorderSize * j, tileBorderSize * i);

				Tile tempTile = data[i][j];
				tempTile.size = { tileSize, tileSize };
				tempTile.draw(target, temp);
			}
}

bool Board::update(size_t dt)
{
	if (animate)
	{
		animationTime += dt;

		if (animationTime >= AnimationLength)
		{
			animate = false;
			animationTime = 0;
			for (size_t i = 0; i < size.y; ++i)
				for (size_t j = 0; j < size.x; ++j)
					animationData[i][j] = Vec2i();

			return true;
		}
	}
	return false;
}

void Board::reset()
{
	for (size_t i = 0; i < size.y; ++i)
		for (size_t j = 0; j < size.x; ++j)
		{
			data[i][j].setValue(0);
			animationData[i][j] = {};
		}
	addNewTile();
	fillPrevData();

	animationTime = 0;
	animate = false;
}

void Board::setDefaultShape(size_t n)
{
	data.clear();
	prevData.clear();
	animationData.clear();

	for (size_t i = 0; i < n; i++)
	{
		data.pushBack(Vector<Tile>());
		for (size_t j = 0; j < n; j++)
			data.back().pushBack(Tile());
	}
	prevData.resize(data.size());
	animationData.resize(data.size());
	for (size_t i = 0; i < data.size(); i++)
	{
		prevData[i].resize(data.front().size());
		animationData[i].resize(data.front().size());
	}

	size = { data.front().size(), data.size() };

	reset();
}

Vec2i Board::findIdxAfterShift(Vec2i const& idx, Direction dir) const
{
	Vec2i nextTileIdx = idx + Directions[uint8_t(dir)];

	if (nextTileIdx.clamped({ 0,0 }, { int32_t(size.x) - 1, int32_t(size.y) - 1 }) == nextTileIdx)
		nextTileIdx = findIdxAfterShift(nextTileIdx, dir);
	else
		return idx;

	if (data[idx.y][idx.x].isAWall())
		return idx;

	if (data[nextTileIdx.y][nextTileIdx.x].canMergeWith(data[idx.y][idx.x]))
		return nextTileIdx;

	return nextTileIdx - Directions[uint8_t(dir)];
}

int32_t Board::collapseTilesTo(Vec2i const& idx, Direction dir)
{
	int32_t points = 0;
	Vec2i targetIdx = idx + Directions[uint8_t(dir)];

	if (targetIdx.clamped({ 0,0 }, { int32_t(size.x) - 1, int32_t(size.y) - 1 }) == targetIdx)
	{
		points += collapseTilesTo(targetIdx, dir);

		if (data[idx.y][idx.x].getValue())
		{
			targetIdx = findIdxAfterShift(idx, dir);
			if (targetIdx != idx)
			{
				animationData[idx.y][idx.x] = targetIdx - idx;

				if (data[targetIdx.y][targetIdx.x].canMergeWith(data[idx.y][idx.x]))
					points += data[targetIdx.y][targetIdx.x].mergeWith(data[idx.y][idx.x]);
			}
		}
	}

	return points;
}

void Board::fillPrevData()
{
	for (int32_t i = 0; i < int32_t(size.y); ++i)
		for (int32_t j = 0; j < int32_t(size.x); ++j)
			prevData[i][j] = data[i][j];
}

int32_t Board::shiftTo(Direction direction)
{
	if (animate || !canShiftTo(direction))
		return -1;

	int32_t points = 0;

	fillPrevData();

	if (direction == Direction::Right)
		for (int32_t i = 0; i < int32_t(size.y); ++i)
			points += collapseTilesTo({ 0, i }, direction);
	else if (direction == Direction::Left)
		for (int32_t i = 0; i < int32_t(size.y); ++i)
			points += collapseTilesTo({ int32_t(size.x) - 1, i }, direction);
	else if (direction == Direction::Up)
		for (int32_t i = 0; i < int32_t(size.x); ++i)
			points += collapseTilesTo({ i, int32_t(size.y) - 1 }, direction);
	else if (direction == Direction::Down)
		for (int32_t i = 0; i < int32_t(size.x); ++i)
			points += collapseTilesTo({ i, 0 }, direction);

	for (int32_t i = 0; i < int32_t(size.y); ++i)
		for (int32_t j = 0; j < int32_t(size.x); ++j)
			data[i][j].confirmMerge();

	animate = true;

	return points;
}

bool Board::canShiftTo(Direction dir) const
{
	for (int32_t i = 0; i < int32_t(size.y); ++i)
		for (int32_t j = 0; j < int32_t(size.x); ++j)
			if (data[i][j].getValue() && findIdxAfterShift(Vec2i( j, i ), dir) != Vec2i(j, i))
				return true;
	return false;
}

bool Board::addNewTile()
{
	Vec2u* freeIndices = new Vec2u[size.x * size.y];
	size_t freeCount = 0;

	for (int32_t i = 0; i < int32_t(size.y); ++i)
		for (int32_t j = 0; j < int32_t(size.x); ++j)
			if (!data[i][j].getValue() && !data[i][j].isAWall())
				freeIndices[freeCount++] = Vec2u(j, i);

	if (freeCount)
	{
		Vec2u idx = freeIndices[rand() % freeCount];
		data[idx.y][idx.x].setValue(TileBaseValue);
	}

	delete[] freeIndices;
	return freeCount;
}

bool Board::undo()
{
	bool diff = false;
	for (int32_t i = 0; i < int32_t(size.y); ++i)
		for (int32_t j = 0; j < int32_t(size.x); ++j)
			if(diff |= (data[i][j].getValue() != prevData[i][j].getValue()))
				data[i][j] = prevData[i][j];

	return diff;
}

bool Board::saveTo(char const * src) const
{
	FILE* file = nullptr;
	fopen_s(&file, src, "w");
	if (file)
	{
		for (int32_t i = 0; i < int32_t(size.y); ++i)
		{
			for (int32_t j = 0; j < int32_t(size.x); ++j)
			{
				if (data[i][j].isAWall())
					fprintf(file, "#");
				else
					fprintf(file, "%d", data[i][j].getValue());
				if(j + 1 < int32_t(size.x))
					fprintf(file, " ");
			}
			if(i + 1 < int32_t(size.y))
				fprintf(file, "\n");
		}

		fclose(file);
		return true;
	}
	return false;
}

bool Board::loadFrom(char const * src)
{
	FILE* file = nullptr;
	fopen_s(&file, src, "r");

	if (file)
	{
		animationTime = 0;
		animate = false;

		data.clear();
		prevData.clear();
		animationData.clear();

		char c = 0;
		char c2 = 0;
		int32_t value = 0;

		data.pushBack(Vector<Tile>());

		while (!feof(file))
		{
			if (fscanf_s(file, " %d%c", &value, &c, 1))
			{
				Tile temp;
				temp.setValue(value);
				data.back().pushBack(temp);
				if (c == '\n')
					data.pushBack(Vector<Tile>());
			}
			else if (fscanf_s(file, " %c%c", &c, 1, &c2, 1) && (c == '#'))
			{
				Tile temp;
				temp.setAsWall(true);
				data.back().pushBack(temp);
				if (c2 == '\n')
					data.pushBack(Vector<Tile>());
			}
			else
			{
				fclose(file);
				setDefaultShape(4);
				return false;
			}
		}

		fclose(file);
	}
	else
	{
		setDefaultShape(4);
		return false;
	}

	size = { data.front().size(), data.size() };

	prevData.resize(size.y);
	animationData.resize(size.y);
	for (size_t i = 0; i < size.y; ++i)
	{
		prevData[i].resize(size.x);
		animationData[i].resize(size.x);
	}

	bool anyWithValue = false;

	for (int32_t i = 0; i < int32_t(size.y); ++i)
		for (int32_t j = 0; j < int32_t(size.x); ++j)
			if (data[i][j].getValue())
			{
				anyWithValue = true;
				break;
			}
	if (!anyWithValue)
		addNewTile();

	fillPrevData();

	return true;
}

