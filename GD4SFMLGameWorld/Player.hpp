/*
Charlie Duff
D00183790
*/
#pragma once
#include "Command.hpp"
#include "ActionID.hpp"
#include "MissionStatusID.hpp"
#include "PlayerID.hpp"

#include <SFML/Window/Event.hpp>
#include <map>

class CommandQueue;

class Player
{
public:
	Player(PlayerID type);

	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);

	void assignKey(ActionID action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(ActionID action) const;

	void setMissionStatus(MissionStatusID status);
	MissionStatusID getMissionStatus() const;

private:
	void initializeActions();
	static bool isRealtimeAction(ActionID action);

private:
	std::map<sf::Keyboard::Key, ActionID> mKeyBinding;
	std::map<ActionID, Command> mActionBinding;
	MissionStatusID mCurrentMissionStatus;
	PlayerID mType;
};