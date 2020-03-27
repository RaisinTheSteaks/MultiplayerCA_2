/*
Charlie Duff
D00183790
*/
#include "Application.hpp"
#include "Utility.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"
#include "SettingsState.hpp"
#include "GameOverState.hpp"
#include "MultiplayerGameState.hpp"

const sf::Time Application::TimePerFrame = sf::seconds(1.f / 60.f);

Application::Application()
	: mWindow(sf::VideoMode(1280, 960), "Sea-8-2", sf::Style::Close)
	, mTextures()
	, mFonts()
	, mKeyBinding1(2)
	, mKeyBinding2(1)
	, mMusic()
	, mSoundPlayer()
	, mStateStack(State::Context(mWindow, mTextures, mFonts, mMusic, mSoundPlayer,mKeyBinding1,mKeyBinding2))
	, mStatisticText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false);

	mFonts.load(FontID::Main, "Media/Sansation.ttf");
	mFonts.load(FontID::Alternate, "Media/Alternative.ttf");
	mTextures.load(TextureID::TitleScreen, "Media/Textures/triremeBackground.jpg");
	mTextures.load(TextureID::Buttons, "Media/Textures/Buttons.png");
	mStatisticText.setFont(mFonts.get(FontID::Main));
	mStatisticText.setPosition(5.f, 5.f);
	mStatisticText.setCharacterSize(20);

	registerStates();
	mStateStack.pushState(StateID::Title);
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processInput();
			update(TimePerFrame);

			//Check if the statestack is empty
			if (mStateStack.isEmpty())
			{
				mWindow.close();
			}
		}
		updateStatistics(elapsedTime);
		draw();
	}
}

void Application::processInput()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		mStateStack.handleEvent(event);

		if (event.type == sf::Event::Closed)
		{
			mWindow.close();
		}
	}
}

void Application::update(sf::Time dt)
{
	mStateStack.update(dt);
}

void Application::draw()
{
	mWindow.clear();
	mStateStack.draw();

	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticText);
	mWindow.display();
}

void Application::updateStatistics(sf::Time dt)
{
	mStatisticsUpdateTime += dt;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticText.setString("Frames/Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time/Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Application::registerStates()
{
	mStateStack.registerState<TitleState>(StateID::Title);
	mStateStack.registerState<MenuState>(StateID::Menu);
	mStateStack.registerState<GameState>(StateID::Game);
	mStateStack.registerState<MultiplayerGameState>(StateID::HostGame, true);
	mStateStack.registerState<MultiplayerGameState>(StateID::JoinGame, false);
	mStateStack.registerState<PauseState>(StateID::Pause);
	mStateStack.registerState<PauseState>(StateID::NetworkPause, true);
	mStateStack.registerState<SettingState>(StateID::Settings);
	mStateStack.registerState<GameOverState>(StateID::GameOver, "Mission Failed!");
}
