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
#include "NetworkProtocol.hpp"
#include "GameActionID.hpp"

#include "SFML/System/NonCopyable.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <array>

class NetworkNode;
//Forward declaration
namespace sf
{
	class RenderTarget;
}


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked);
	void update(sf::Time dt);
	void draw();
	CommandQueue& getCommandQueue();
	bool hasAlivePlayer() const;
	bool hasPlayerReachedEnd() const;
	void updateSounds();
	sf::FloatRect getViewBounds() const;
	Ship* getShip(int identifier) const;
	Ship* addShip(int identifier);
	void removeShip(int identifier);

	void createPickup(sf::Vector2f position, PickupID type);

	//void setCurrentBattleFieldPosition(float lineY);
	void setWorldHeight(float height);

	bool pollGameAction(Action& out);

private:
	void loadTextures();
	void buildScene();
	void placeIslands(float widthChunk, float heightChunk);
	void adaptPlayerPosition();
	void adaptPlayerVelocity();
	void handleCollisions();

	sf::FloatRect getBattlefieldBounds() const;
	

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
	std::vector<Ship*> mPlayerShip;
	std::array<Island*,18>mIsland;
	std::vector<SpawnPoint>	mEnemySpawnPoints;
	std::vector<Ship*> mActiveEnemies;

	BloomEffect	mBloomEffect;
	bool mNetworkedWorld;
	NetworkNode* mNetworkNode;
	NewEffect mNewEffect;
};