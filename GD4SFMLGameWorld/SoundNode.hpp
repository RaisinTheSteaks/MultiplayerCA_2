/*
Charlie Duff
D00183790
*/
#pragma once
#include "SceneNode.hpp"
#include "ResourceIdentifiers.hpp"

class SoundPlayer;

class SoundNode : public SceneNode
{
public:
	explicit SoundNode(SoundPlayer& player);
	void playSound(SoundEffectID sound, sf::Vector2f position);

	virtual unsigned int getCategory() const;

private:
	SoundPlayer& mSounds;
};