/*
Charlie Duff
D00183790
*/
#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Ship.hpp"
#include "LayerID.hpp"
#include "CommandQueue.hpp"
#include "ShipID.hpp"
#include "Pickup.hpp"
#include "PostEffect.hpp"
#include "BloomEffect.hpp"
#include "SoundNode.hpp"
#include "SoundPlayer.hpp"
#include "Gun.hpp"
#include "Island.hpp"
#include "IslandID.hpp"
#include "NewEffect.hpp"

#include "SFML/System/NonCopyable.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <array>


//Forward declaration
namespace sf
{
	class RenderTarget;
}


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds);
	void update(sf::Time dt);
	void draw();
	CommandQueue& getCommandQueue();
	bool hasAlivePlayer() const;
	bool hasAlivePlayer1() const;
	bool hasAlivePlayer2() const;
	bool hasPlayerReachedEnd() const;
	void updateSounds();

private:
	void loadTextures();
	void buildScene();
	void adaptPlayerPosition();
	void adaptPlayerVelocity();
	void handleCollisions();


	void spawnEnemies();
	void addEnemies();
	void addEnemy(ShipID type, float relX, float relY);

	sf::FloatRect getBattlefieldBounds() const;
	sf::FloatRect getViewBounds() const;

	void destroyEntitiesOutsideView();

	void guideMissiles();

	struct SpawnPoint
	{
		SpawnPoint(ShipID type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
		{
		}

		ShipID type;
		float x;
		float y;
	};

private:
	sf::RenderTarget& mTarget;
	sf::RenderTexture mSceneTexture;
	sf::View mCamera;
	TextureHolder mTextures;
	FontHolder& mFonts;
	SoundPlayer& mSounds;

	SceneNode mSceneGraph;
	std::array<SceneNode*, static_cast<int>(LayerID::LayerCount)> mSceneLayers;
	CommandQueue mCommandQueue;

	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	std::array<sf::Vector2f, 2>mSpawnPositions;
	float mScrollSpeed;
	Ship* mPlayerShip;
	/*
	Joshua Corcoran
	D00190830
	_____________
	Going to use this array to store all guns to be used in the game. 
	Will use a similar approach to store all players in later iterations of the game
	Will change the approach to not use magic numbers, but instead, the number of players in the game
	May possibly change the number of guns on each ship
	*/
	std::array<Gun*,2>mPlayerGuns;
	Ship* mPlayerShip2;
	std::array<Island*,3>mIsland;
	std::vector<SpawnPoint>	mEnemySpawnPoints;
	std::vector<Ship*> mActiveEnemies;

	BloomEffect	mBloomEffect;
	NewEffect mNewEffect;
};