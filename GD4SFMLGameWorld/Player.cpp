/*
Charlie Duff
D00183790
*/
#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Ship.hpp"
#include "ActionID.hpp"
#include "DataTables.hpp"
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
	ShipMover(float rotation, float acceleration) :
		rotation(rotation),
		acceleration(acceleration)
	{
	}

	void operator() (Ship& Ship, sf::Time) const
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
		float curRot = Ship.getRotation();
		float pi = 3.14159265;
		sf::Vector2f velocity;
		
		if (acceleration < 0)
		{
			velocity.y = cos(curRot*pi / 180) * 1;
			velocity.x = sin(curRot*pi / 180) * -1;
		}
		else if (acceleration > 0)
		{
			velocity.y = cos(curRot*pi / 180)*-1;
			velocity.x = sin(curRot*pi / 180) * 1;
		}

		//Trying to get a slow deceleration
		/*else if (acceleration== 0)
		{
			velocity = Ship.getDirectionVec() - (Ship.getDirectionVec()*0.1f);
		}
*/
		if (rotation > 0)
		{
			Ship.setRotation(Ship.getRotation() + Ship.getTurnSpeed());
			Ship.getBoundingRect();

		}
		else if (rotation < 0)
		{
			Ship.setRotation(Ship.getRotation() - Ship.getTurnSpeed());
			Ship.getBoundingRect();
		}
		//std::cout << "Curr X [" << velocity.x << "] Curr Y [" << velocity.y << "]\n";

		Ship.accelerate(velocity*Ship.getMaxSpeed());
		Ship.setDirectionVec(velocity);
	}
	float rotation, acceleration;

};

Player::Player(PlayerID type) : mCurrentMissionStatus(MissionStatusID::MissionRunning), mType(type)
{
	//using key bindings from vector playerdata rather than player class, allows for multiple players to have different key bindings
	mKeyBinding = Table[static_cast<int>(mType)].mKeyBinding;

	///Moved to the DataTables.cpp
	//// Set initial key bindings
	//mKeyBinding[sf::Keyboard::A] = ActionID::MoveLeft;
	//mKeyBinding[sf::Keyboard::D] = ActionID::MoveRight;
	//mKeyBinding[sf::Keyboard::W] = ActionID::MoveUp;
	//mKeyBinding[sf::Keyboard::S] = ActionID::MoveDown;
	//mKeyBinding[sf::Keyboard::Space] = ActionID::Fire;
	//mKeyBinding[sf::Keyboard::M] = ActionID::LaunchMissile;

	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's Ship
	for (auto& pair : mActionBinding)
		pair.second.category = static_cast<int>(Table[static_cast<int>(mType)].categoryID);
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = mKeyBinding.find(event.key.code);

		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
		{
			commands.push(mActionBinding[found->second]);
		}
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	for (auto pair : mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
		{
			commands.push(mActionBinding[pair.second]);
		}
	}
}

void Player::assignKey(ActionID action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	mKeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(ActionID action) const
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

void Player::setMissionStatus(MissionStatusID status)
{
	mCurrentMissionStatus = status;
}

MissionStatusID Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	//Changed to provide direction of steering and acceleration vs deceleration
	mActionBinding[ActionID::MoveLeft].action = derivedAction<Ship>(ShipMover(-1, 0));
	mActionBinding[ActionID::MoveRight].action = derivedAction<Ship>(ShipMover(1, 0));
	mActionBinding[ActionID::MoveUp].action = derivedAction<Ship>(ShipMover(0, 1));
	mActionBinding[ActionID::MoveDown].action = derivedAction<Ship>(ShipMover(0, -1));

	mActionBinding[ActionID::Fire].action = derivedAction<Ship>([](Ship& a, sf::Time) { a.fire(); });
	mActionBinding[ActionID::LaunchMissile].action = derivedAction<Ship>([](Ship& a, sf::Time) { a.launchMissile(); });
}

bool Player::isRealtimeAction(ActionID action)
{
	switch (action)
	{
	case ActionID::MoveLeft:
	case ActionID::MoveRight:
	case ActionID::MoveDown:
	case ActionID::MoveUp:
	case ActionID::Fire:
		return true;

	default:
		return false;
	}
}