#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include "GameActionID.hpp"

#include <vector>
#include <memory>
#include <map>


class GameServer
{
public:
	explicit GameServer(sf::Vector2f battlefieldSize);
	~GameServer();

	void notifyPlayerSpawn(sf::Int32 shipIdentifier);
	void notifyPlayerRealtimeChange(sf::Int32 shipIdentifier, sf::Int32 action, bool actionEnabled);
	void notifyPlayerEvent(sf::Int32 shipIdentifier, sf::Int32 action);

private:
	// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
	struct RemotePeer
	{
		RemotePeer();

		sf::TcpSocket			socket;
		sf::Time				lastPacketTime;
		std::vector<sf::Int32>	shipIdentifiers;
		bool					ready;
		bool					timedOut;
	};

	// Structure to store information about current aircraft state
	struct ShipInfo
	{
		sf::Vector2f				position;
		sf::Int32					hitpoints;
		float						rotation;
		std::map<sf::Int32, bool>	realtimeActions;
	};

	// Unique pointer to remote peers
	typedef std::unique_ptr<RemotePeer> PeerPtr;


private:
	void setListening(bool enable);
	void executionThread();
	void tick();
	sf::Time now() const;

	void handleIncomingPackets();
	void handleIncomingPacket(sf::Packet& packet, RemotePeer & receivingPeer, bool& detectedTimeout);

	void handleIncomingConnections();
	void handleDisconnections();

	void informWorldState(sf::TcpSocket& socket);
	void broadcastMessage(const std::string& message);
	void sendToAll(sf::Packet& packet);
	void updateClientState();

private:
	sf::Thread mThread;
	sf::Clock mClock;
	sf::TcpListener	mListenerSocket;
	bool mListeningState;
	sf::Time mClientTimeoutTime;

	std::size_t	mMaxConnectedPlayers;
	std::size_t	mConnectedPlayers;

	float mWorldHeight;
	sf::FloatRect mBattleFieldRect;
	//float mBattleFieldScrollSpeed;

	std::size_t	mShipCount;
	std::map<sf::Int32, ShipInfo> mShipInfo;

	std::vector<PeerPtr> mPeers;
	sf::Int32 mShipIdentifierCounter;
	bool mWaitingThreadEnd;

	bool mLobbyState;
	sf::Time mGameTimer;
	//sf::Time mTimeForNextSpawn;
};