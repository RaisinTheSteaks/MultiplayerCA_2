#pragma once
#include "State.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "GameServer.hpp"
#include "NetworkProtocol.hpp"

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>

struct ScoreKeeper
{
	ScoreKeeper(sf::Uint32 shipID, sf::Uint8 score) : shipID(shipID), score(score)
	{

	}

	void operator()(sf::Uint32 shipID, sf::Uint8 score)
	{
		shipID = shipID;
		score = score;
	}

	sf::Int32 shipID;
	sf::Uint8 score;
};

class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context context, bool isHost);

	virtual void draw();
	virtual bool update(sf::Time dt);
	void printScoreBoard();
	virtual bool handleEvent(const sf::Event& event);
	virtual void onActivate();
	void onDestroy();

	void disableAllRealtimeActions();


private:
	void updateBroadcastMessage(sf::Time elapsedTime);
	void handlePacket(sf::Int32 packetType, sf::Packet& packet);


private:
	typedef std::unique_ptr<Player> PlayerPtr;


private:
	World mWorld;
	sf::RenderWindow& mWindow;
	TextureHolder& mTextureHolder;

	std::map<int, PlayerPtr> mPlayers;
	std::vector<sf::Int32> mLocalPlayerIdentifiers;
	sf::TcpSocket mSocket;
	bool mConnected;
	std::unique_ptr<GameServer> mGameServer;
	sf::Clock mTickClock;

	std::vector<std::string> mBroadcasts;
	sf::Text mBroadcastText;
	sf::Time mBroadcastElapsedTime;

	sf::Text mPlayerInvitationText;
	sf::Time mPlayerInvitationTime;

	sf::Text mFailedConnectionText;
	sf::Clock mFailedConnectionClock;

	bool mActiveState;
	bool mHasFocus;
	bool mHost;
	bool mGameStarted;
	sf::Time mClientTimeout;
	sf::Time mTimeSinceLastPacket;

	//last ship count is used to store the number of players in the game, gets updated when handling the updateClientState packet
	sf::Int32 mLastShipCount;
	std::vector<ScoreKeeper> mScoreBoard;

};