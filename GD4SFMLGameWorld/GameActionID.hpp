#pragma once
#include <SFML/System/Vector2.hpp>
enum class GameActionID
{
	//TODO - add custom GameActionID
	EnemyExplode,
};

struct Action
{
	Action()
	{ // leave uninitialized
	}

	Action(GameActionID type, sf::Vector2f position)
		: type(type)
		, position(position)
	{
	}

	GameActionID type;
	sf::Vector2f position;
};