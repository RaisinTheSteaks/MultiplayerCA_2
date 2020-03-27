/*
Charlie Duff
D00183790
*/
#pragma once
#include "Command.hpp"
#include "ActionID.hpp"
#include "MissionStatusID.hpp"
#include "PlayerID.hpp"
#include "KeyBinding.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <map>

class CommandQueue;

class Player
{
public:
	Player(sf::TcpSocket* socket, sf::Uint8 identifier, const KeyBinding* binding);

	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);

	void handleRealtimeNetworkInput(CommandQueue& commands);
	// React to events or realtime state changes received over the network
	void handleNetworkEvent(ActionID action, CommandQueue& commands);
	void handleNetworkRealtimeChange(ActionID action, bool actionEnabled);

	void setMissionStatus(MissionStatusID status);
	MissionStatusID getMissionStatus() const;

	void disableAllRealtimeActions();
	bool isLocal() const;

private:
	void initializeActions();

private:
	const KeyBinding* mKeyBinding;
	std::map<ActionID, Command> mActionBinding;
	std::map<ActionID, bool> mActionProxies;
	MissionStatusID mCurrentMissionStatus;
	sf::Uint8 mIdentifier;
	sf::TcpSocket* mSocket;
	PlayerID mType;
	sf::Uint8 mScore;
};