/*
Charlie Duff
D00183790
*/
#include "SoundPlayer.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>

namespace
{
	const float ListenerZ = 300.f;
	const float Attenuation = 8.f;
	const float MinDistance2D = 200.f;
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer()
	:mSoundsBuffer()
	, mSounds()
{
	mSoundsBuffer.load(SoundEffectID::AlliedGunfire, "Media/Sound/ArrowShot_Many.wav");
	mSoundsBuffer.load(SoundEffectID::EnemyGunfire, "Media/Sound/ArrowShot_Many.wav");
	mSoundsBuffer.load(SoundEffectID::Explosion1, "Media/Sound/Explosion1.wav");
	mSoundsBuffer.load(SoundEffectID::Explosion2, "Media/Sound/Explosion2.wav");
	mSoundsBuffer.load(SoundEffectID::LaunchMissile, "Media/Sound/LaunchMissile.wav");
	mSoundsBuffer.load(SoundEffectID::CollectPickup, "Media/Sound/CollectPickup.wav");
	mSoundsBuffer.load(SoundEffectID::Button, "Media/Sound/Button.wav");
	mSoundsBuffer.load(SoundEffectID::Scream, "Media/Sound/breakingWood.wav");

	sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::play(SoundEffectID effect)
{
	play(effect, getListenerPosition());
}

void SoundPlayer::play(SoundEffectID effect, sf::Vector2f position)
{
	mSounds.push_back(sf::Sound());
	sf::Sound& sound = mSounds.back();

	sound.setBuffer(mSoundsBuffer.get(effect));
	sound.setPosition(position.x, -position.y, 0.f);
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);

	sound.play();
}

void SoundPlayer::removeStoppedSounds()
{
	mSounds.remove_if([](const sf::Sound& s)
	{
		return s.getStatus() == sf::Sound::Stopped;
	});
}

void SoundPlayer::setListenPosition(sf::Vector2f position)
{
	sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::getListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}
