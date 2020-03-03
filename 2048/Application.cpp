#include <stdio.h>
#include <time.h>
#include "Application.h"


GameSave::GameSave(size_t saveTime, size_t worldTime, size_t points) : worldTime(worldTime), points(points), saveTime(saveTime)
{}





Application::Application() : win({ Vec2u(720, 560), "Pawel Glomski 172026" })
{
	if (board.loadFrom("shape"))
		state = State::Playing;

	board.setPosition(Vec2i(WinSize / 2u));
}

int Application::run()
{
	clock.restart();

	while (state != State::Quitting)
	{
		clock.nextStep();
		
		input();
		
		logic();
		
		display();
	}

	return 0;
}

void Application::input()
{
	SDL_Event event;

	while (win.pollEvent(event))
	{
		messageInput(event);
		boardSizeChoiceInput(event);
		playingInput(event);
		loadingInput(event);

		if (event.type == SDL_QUIT)
			state = State::Quitting;
	}
}

void Application::playingInput(SDL_Event const & event)
{
	if(state == State::Playing)
		if (event.type == SDL_KEYDOWN)
			if (event.key.keysym.sym == SDLK_LEFT) shiftDirection = Direction::Left;
			else if (event.key.keysym.sym == SDLK_RIGHT) shiftDirection = Direction::Right;
			else if (event.key.keysym.sym == SDLK_UP) shiftDirection = Direction::Up;
			else if (event.key.keysym.sym == SDLK_DOWN) shiftDirection = Direction::Down;
			else if (event.key.keysym.sym == SDLK_u) { if (board.undo()) points = prevPoints; }
			else if (event.key.keysym.sym == SDLK_n) reset();
			else if (event.key.keysym.sym == SDLK_ESCAPE) state = State::Quitting;
			else if (event.key.keysym.sym == SDLK_l) { state = State::Loading; loadSaves(); }
			else if (event.key.keysym.sym == SDLK_s) save();
}

void Application::boardSizeChoiceInput(SDL_Event const & event)
{
	if (state == State::BoardSizeChoice)
		if (event.type == SDL_KEYDOWN)
			if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9)
			{
				inputStr[inputLen++] = '0' + (event.key.keysym.sym - SDLK_0);
				inputStr[inputLen] = '\0';
			}
			else if (event.key.keysym.sym == SDLK_BACKSPACE && inputLen > 0)
				inputStr[--inputLen] = '\0';
			else if (event.key.keysym.sym == SDLK_ESCAPE) state = State::Quitting;
			else if (event.key.keysym.sym == SDLK_RETURN)
			{
				if (inputLen == 0 || inputStr[0] == '0')
					state = State::Quitting;
				else
				{
					size_t size = 0;
					sscanf_s(inputStr, "%u", &size);
					board.setDefaultShape(size);
					inputStr[inputLen = 0] = '\0';

					state = State::Playing;
				}
			}
}

void Application::loadingInput(SDL_Event const & event)
{
	if (state == State::Loading)
		if (event.type == SDL_KEYDOWN)
			if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9)
			{
				inputStr[inputLen++] = '0' + (event.key.keysym.sym - SDLK_0);
				inputStr[inputLen] = '\0';
			}
			else if (event.key.keysym.sym == SDLK_BACKSPACE && inputLen > 0)
				inputStr[--inputLen] = '\0';
			else if (event.key.keysym.sym == SDLK_ESCAPE) state = State::Playing;
			else if (event.key.keysym.sym == SDLK_RETURN)
			{
				if (inputLen == 0)
					state = State::Playing;
				else
				{
					size_t idx = 0;
					sscanf_s(inputStr, "%u", &idx);
					load(idx - 1);
					inputStr[inputLen = 0] = '\0';
				}
			}
}

void Application::messageInput(SDL_Event const & event)
{
	if (state == State::Message)
		if (event.type == SDL_KEYDOWN)
			if (event.key.keysym.sym == SDLK_RETURN)
			{
				if (msgBox.type == MessageBox::Type::Error)
					state = State::Quitting;
				else
					state = State::Playing;
				msgBox.type = MessageBox::Type::Count;
			}
}

void Application::reset()
{
	points = 0;
	prevPoints = 0;
	clock.restart();
	board.reset();
}

void Application::save()
{
	loadSaves();

	FILE* file = nullptr;
	fopen_s(&file, "saveList", "a");

	if (file)
	{
		size_t now = size_t(time(NULL));
		char buffer[32] = { 0 };
		sprintf_s(buffer, "%u", now);
		
		if (board.saveTo(buffer))
		{
			if (!saves.find({ now, 0,0 }, GameSave_SaveTimeCmp()))
			{
				fprintf_s(file, "%u %u %u\n", now, clock.getWorldTime(), points);

				msgBox.set("Saving completed");
				state = State::Message;
			}
			fclose(file);
			return;
		}
		fclose(file);
	}

	msgBox.set("Saving failed", MessageBox::Type::Warning);
	state = State::Message;
}

void Application::loadSaves()
{
	FILE* file = nullptr;
	fopen_s(&file, "saveList", "r");

	int32_t result = 0;

	if (file)
	{
		size_t saveTime = 0;
		size_t worldTime = 0;
		size_t sPoints = 0;
		
		saves.clear();
		for (size_t i = 0; (result = fscanf_s(file, " %u %u %u", &saveTime, &worldTime, &sPoints)) > 0; ++i)
			saves.pushBack({saveTime, worldTime, sPoints});

		fclose(file);
	}

	if (result != EOF)
	{
		msgBox.set("Couldn't load list of saves", MessageBox::Type::Warning);
		state = State::Message;
	}
}

void Application::load(size_t idx)
{
	loadSaves();

	if (idx < saves.size())
	{
		char savePath[32] = { 0 };
		sprintf_s(savePath, "%u", saves[idx].saveTime);

		if (board.loadFrom(savePath))
		{
			msgBox.set("Loading completed");
			state = State::Message;

			points = saves[idx].points;
			clock.setWorldTime(saves[idx].worldTime);

			return;
		}
	}

	msgBox.set("Loading failed", MessageBox::Type::Warning);
	state = State::Message;
	return;
}

void Application::logic()
{
	if (shiftDirection != Direction::Count)
	{
		int32_t value = board.shiftTo(shiftDirection);
		if (value > 0)
		{
			prevPoints = points;
			points += value;
		}
		shiftDirection = Direction::Count;
	}

	if (board.update(clock.getDeltaTime()))
	{
		board.addNewTile();
		for (uint8_t i = 0; i < uint8_t(Direction::Count); ++i)
			if (board.canShiftTo(Direction(i)))
				return;

		msgBox.set("Game Over");
		state = State::Message;
	}
}

void Application::display()
{
	win.clear();
	if(state != State::Loading && state != State::BoardSizeChoice)
		win.draw(board);
	displayHUD();
	win.display();
}

void Application::displayHUD()
{
	if (state == State::Loading)
		displayLoadingPanel();
	else if (state == State::BoardSizeChoice)
		displaySizeChoicePanel();
	else
	{
		char tStr[32];

		sprintf_s(tStr, "Game time: %u sec", clock.getWorldTime() / 1000);
		win.draw(Text(tStr));

		sprintf_s(tStr, "Points: %u", points);
		Text pointsText(tStr);
		pointsText.setPosition({ 200, 0 });
		win.draw(pointsText);
	}

	win.draw(msgBox);
}

void Application::displayLoadingPanel()
{
	Text txt;
	char buffer[64];

	txt.set("Nr");
	win.draw(txt);

	txt.set("Save time");
	txt.setPosition({ 50, 0 });
	win.draw(txt);

	txt.move({ 0,30 });

	for (size_t i = 0; i < saves.size(); ++i)
	{
		txt.move({ -50, 0 });
		sprintf_s(buffer, "%u", i + 1);
		txt.set(buffer);
		win.draw(txt);

		sprintf_s(buffer, "%u", saves[i].saveTime);
		txt.move({ 50, 0 });
		txt.set(buffer);
		win.draw(txt);

		txt.move({ 0,11 });
	}

	txt.setPosition({ 0, int32_t(WinSize.y) });
	txt.setOrigin({ 0.f, 1.f });
	sprintf_s(buffer, "Save to load: %s", inputStr);
	txt.set(buffer);
	win.draw(txt);
}

void Application::displaySizeChoicePanel()
{
	Text txt;
	char buffer[64];

	txt.setPosition(Vec2i(WinSize) / 2);
	txt.setOrigin({ 0.5f, 0.5f });
	sprintf_s(buffer, "Board size: %s", inputStr);
	txt.set(buffer);
	win.draw(txt);
}