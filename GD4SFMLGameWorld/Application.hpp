/*
Charlie Duff
D00183790
*/
#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Player.hpp"
#include "StateStack.hpp"
#include "MusicPlayer.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

class Application
{
public:
	Application();
	void run();

private:
	void processInput();
	void update(sf::Time dt);
	void draw();

	void updateStatistics(sf::Time dt);
	void registerStates();

private:
	static const sf::Time TimePerFrame;

	sf::RenderWindow mWindow;
	TextureHolder mTextures;
	FontHolder mFonts;

	MusicPlayer mMusic;
	SoundPlayer mSoundPlayer;

	KeyBinding mKeyBinding1;
	KeyBinding mKeyBinding2;

	StateStack mStateStack;

	sf::Text mStatisticText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;

};