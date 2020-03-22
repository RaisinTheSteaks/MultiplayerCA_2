#pragma once
#include "SceneNode.hpp"
#include "NetworkProtocol.hpp"
#include "GameActionID.hpp"

#include <queue>

class NetworkNode : public SceneNode
{
public:
	NetworkNode();

	void					notifyGameAction(GameActionID type, sf::Vector2f position);
	bool					pollGameAction(Action& out);

	virtual unsigned int	getCategory() const;


private:
	std::queue<Action>	mPendingActions;
};