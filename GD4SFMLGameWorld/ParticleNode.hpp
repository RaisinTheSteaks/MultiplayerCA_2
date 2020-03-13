/*
Charlie Duff
D00183790
*/
#pragma once
#include "SceneNode.hpp"
#include "ResourceIdentifiers.hpp"
#include "Particle.hpp"
#include "ParticleID.hpp"

#include <SFML/Graphics/VertexArray.hpp>

#include <deque>

class ParticleNode : public SceneNode
{
public:
	ParticleNode(ParticleID type, const TextureHolder& textures);

	void addParticle(sf::Vector2f position);
	ParticleID getParticleType() const;
	virtual unsigned int getCategory() const;

private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	
	void addVertex(float worldX, float worldY, float textCoordx, float textCoordY, const sf::Color& color) const;
	void computeVertices() const;

private:
	std::deque<Particle> mParticles;
	const sf::Texture& mTexture;
	ParticleID mType;

	mutable sf::VertexArray mVertexArray;
	mutable bool mNeedsVertexUpdate;

};