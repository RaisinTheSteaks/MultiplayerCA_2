/*
Charlie Duff
D00183790
*/
#include "GameState.hpp"

GameState::GameState(StateStack& stack, Context context)
	:State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds)
	, mPlayer(*context.player)
	, mPlayer2(*context.player2)
{
	mPlayer.setMissionStatus(MissionStatusID::MissionRunning);
	mPlayer2.setMissionStatus(MissionStatusID::MissionRunning);
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
	if (!mWorld.hasAlivePlayer1())
	{
		mPlayer.setMissionStatus(MissionStatusID::MissionFailure);
		requestStackPush(StateID::GameOver);
	}

	else if (!mWorld.hasAlivePlayer2())
	{
		mPlayer2.setMissionStatus(MissionStatusID::MissionFailure);
		requestStackPush(StateID::GameOver);
	}


	else if (mWorld.hasPlayerReachedEnd())
	{
		mPlayer.setMissionStatus(MissionStatusID::MissionSuccess);
		mPlayer2.setMissionStatus(MissionStatusID::MissionSuccess);
		requestStackPush(StateID::GameOver);
	}

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);
	mPlayer2.handleRealtimeInput(commands);
	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);
	mPlayer2.handleEvent(event, commands);

	//Pause if esc is pressed
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		requestStackPush(StateID::Pause);
	}
	return true;
}
