/*
Charlie Duff
D00183790
*/
#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Ship.hpp"
#include "ActionID.hpp"
#include "DataTables.hpp"
#include "NetworkProtocol.hpp"

#include <SFML/Network/Packet.hpp>

#include <map>
#include <string>
#include <algorithm>
#include <iostream>

namespace
{
	const std::vector<PlayerData> Table = initializePlayerData();
}

struct ShipMover
{
	ShipMover(float rotation, float acceleration, int identifier) :
		rotation(rotation),
		acceleration(acceleration),
		shipID(identifier)
	{
	}

	void operator() (Ship& ship, sf::Time) const
	{

		if (ship.getIdentifier() == shipID)
		{
			float yVel = 0.f;
			//Citation
			/*
			Joshua  Corcoran
			D00190830
			____________
			Code adapted from sfml forum on moving in direction of rotation
			https://en.sfml-dev.org/forums/index.php?topic=544.0
			*/

			/*
			Calculating the radians of the current rotation, and then cos/sin (ing) the angle to find the distance you need to move in each angle
			*/
			float curRot = ship.getRotation();
			float pi = 3.14159265;
			sf::Vector2f velocity;

			if (acceleration < 0)
			{
				velocity.y = cos(curRot * pi / 180) * 1;
				velocity.x = sin(curRot * pi / 180) * -1;
			}
			else if (acceleration > 0)
			{
				velocity.y = cos(curRot * pi / 180) * -1;
				velocity.x = sin(curRot * pi / 180) * 1;
			}

			//Trying to get a slow deceleration
			/*else if (acceleration== 0)
			{
				velocity = Ship.getDirectionVec() - (Ship.getDirectionVec()*0.1f);
			}
			*/
			if (rotation > 0)
			{
				ship.setRotation(ship.getRotation() + ship.getTurnSpeed());
				ship.getBoundingRect();

			}
			else if (rotation < 0)
			{
				ship.setRotation(ship.getRotation() - ship.getTurnSpeed());
				ship.getBoundingRect();
			}
			//std::cout << "Curr X [" << velocity.x << "] Curr Y [" << velocity.y << "]\n";

			ship.accelerate(velocity * ship.getMaxSpeed());
			ship.setDirectionVec(velocity);
		}
		
	}
	float rotation, acceleration;
	int shipID;
};

struct ShipFireTrigger
{
	ShipFireTrigger(int identifier)
		: shipID(identifier)
	{
	}

	void operator() (Ship& ship, sf::Time) const
	{
		if (ship.getIdentifier() == shipID)
			ship.fire();
	}

	int shipID;
};

Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding) 
	: mKeyBinding(binding)
	, mCurrentMissionStatus(MissionStatusID::MissionRunning)
	, mIdentifier(identifier)
	, mSocket(socket)
{
	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's Ship
	for (auto& pair : mActionBinding)
		pair.second.category = static_cast<int>(CategoryID::PlayerShip);
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{

		// Check if pressed key appears in key binding, trigger command if so
		ActionID action;
		if (mKeyBinding && mKeyBinding->checkAction(event.key.code, action) && !isRealtimeAction(action))
		{
			// Network connected -> send event over network
			if (mSocket)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Client::PacketType::PlayerEvent);
				packet << mIdentifier;
				packet << static_cast<sf::Int32>(action);
				mSocket->send(packet);
			}
			else
			{
				commands.push(mActionBinding[action]);
			}
		}
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Check if this is a networked game and local player or just a single player game
	if ((mSocket && isLocal()) || !mSocket)
	{
		// Lookup all actions and push corresponding commands to queue
		std::vector<ActionID> activeActions = mKeyBinding->getRealtimeActions();
		for (ActionID action : activeActions)
		{
			commands.push(mActionBinding[action]);
		}
	}
}

void Player::handleRealtimeNetworkInput(CommandQueue& commands)
{
	if (mSocket && !isLocal())
	{
		// Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
		for (auto pair : mActionProxies)
		{
			if (pair.second && isRealtimeAction(pair.first))
				commands.push(mActionBinding[pair.first]);
		}
	}
}

void Player::handleNetworkEvent(ActionID action, CommandQueue& commands)
{
	commands.push(mActionBinding[action]);
}

void Player::handleNetworkRealtimeChange(ActionID action, bool actionEnabled)
{
	mActionProxies[action] = actionEnabled;
}

void Player::setMissionStatus(MissionStatusID status)
{
	mCurrentMissionStatus = status;
}

MissionStatusID Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::disableAllRealtimeActions()
{
	for (auto& action : mActionProxies)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PacketType::PlayerRealtimeChange);
		packet << mIdentifier;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;
		mSocket->send(packet);
	}
}

bool Player::isLocal() const
{
	// No key binding means this player is remote
	return mKeyBinding != nullptr;
}

void Player::initializeActions()
{
	//Changed to provide direction of steering and acceleration vs deceleration
	mActionBinding[ActionID::MoveLeft].action = derivedAction<Ship>(ShipMover(-1, 0, mIdentifier));
	mActionBinding[ActionID::MoveRight].action = derivedAction<Ship>(ShipMover(1, 0, mIdentifier));
	mActionBinding[ActionID::MoveUp].action = derivedAction<Ship>(ShipMover(0, 1, mIdentifier));
	mActionBinding[ActionID::MoveDown].action = derivedAction<Ship>(ShipMover(0, -1, mIdentifier));

	mActionBinding[ActionID::Fire].action = derivedAction<Ship>(ShipFireTrigger(mIdentifier));
	//mActionBinding[ActionID::LaunchMissile].action = derivedAction<Ship>([](Ship& a, sf::Time) { a.launchMissile(); });
}

