#pragma once
#include <string.h>
#include "Window.h"
#include "Clock.h"
#include "Board.h"


static const Vec2u WinSize(720, 560);


struct GameSave
{
	GameSave() = default;
	GameSave(size_t saveTime, size_t worldTime, size_t points);
	size_t saveTime = 0;
	size_t worldTime = 0;
	size_t points = 0;
};

struct GameSave_SaveTimeCmp
{ bool operator()(GameSave const& left, GameSave const& right) const { return left.saveTime == right.saveTime; } };



class Application
{
	enum class State : uint8_t { Playing, Loading, Quitting, Message, BoardSizeChoice, Count };

public:

	Application();

	int run();

private:

	void input();

	void playingInput(SDL_Event const& event);

	void boardSizeChoiceInput(SDL_Event const& event);

	void loadingInput(SDL_Event const& event);

	void messageInput(SDL_Event const& event);


	void reset();


	void save();

	void loadSaves();

	void load(size_t idx);


	void logic();



	void display();

	void displayHUD();

	void displayLoadingPanel();

	void displaySizeChoicePanel();

private:
	
	Window win;

	SimulationClock clock;

	Board board;


	Direction shiftDirection = Direction::Count;

	size_t points = 0;

	size_t prevPoints = 0;


	State state = State::BoardSizeChoice;

	MessageBox msgBox;

	Vector<GameSave> saves;


	char inputStr[32] = { 0 };

	size_t inputLen = 0;
};