/*
Charlie Duff
D00183790
*/
#pragma once
#include "SceneNode.hpp"
#include "Particle.hpp"
#include "ParticleID.hpp"

class ParticleNode;

class EmitterNode : public SceneNode
{
public:
	explicit EmitterNode(ParticleID type);

private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void emitParticles(sf::Time dt);

private:
	sf::Time mAccumulatedTime;
	ParticleID mType;
	ParticleNode* mParticleSystem;
};