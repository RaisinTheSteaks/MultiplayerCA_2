/*
Charlie Duff
D00183790
*/
#include "GameState.hpp"


GameState::GameState(StateStack& stack, Context context)
	:State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds, false)
	, mPlayer(nullptr, 1, context.keys1)
{
	mWorld.addShip(1);
	mPlayer.setMissionStatus(MissionStatusID::MissionRunning);
	context.music->play(MusicID::MissionTheme);
}

void GameState::draw()
{
	mWorld.draw();
}

bool GameState::update(sf::Time dt)
{
	mWorld.update(dt);
	//checks if player 1/2 is alive
	if (!mWorld.hasAlivePlayer())
	{
		mPlayer.setMissionStatus(MissionStatusID::MissionFailure);
		requestStackPush(StateID::GameOver);
	}

	else if (mWorld.hasPlayerReachedEnd())
	{
		mPlayer.setMissionStatus(MissionStatusID::MissionSuccess);
		requestStackPush(StateID::GameOver);
	}

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);

	//Pause if esc is pressed
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		requestStackPush(StateID::Pause);
	}
	return true;
}
