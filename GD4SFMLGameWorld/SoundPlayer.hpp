/*
Charlie Duff
D00183790
*/
#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SoundEffectID.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <list>

class SoundPlayer : private sf::NonCopyable
{
public:
	SoundPlayer();
	void play(SoundEffectID effect);
	void play(SoundEffectID effect, sf::Vector2f position);

	void removeStoppedSounds();
	void setListenPosition(sf::Vector2f position);
	sf::Vector2f getListenerPosition() const;

private:
	SoundBufferHolder mSoundsBuffer;
	std::list<sf::Sound> mSounds;
};