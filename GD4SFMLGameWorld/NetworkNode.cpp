#include "NetworkNode.hpp"
#include "NetworkProtocol.hpp"
#include "CategoryID.hpp"

NetworkNode::NetworkNode()
	: SceneNode()
	, mPendingActions()
{
}

unsigned int NetworkNode::getCategory() const
{
	return static_cast<int>(CategoryID::Network);
}

void NetworkNode::notifyGameAction(GameActionID type, sf::Vector2f position)
{
	mPendingActions.push(Action(type, position));
}

bool NetworkNode::pollGameAction(Action& out)
{
	if (mPendingActions.empty())
	{
		return false;
	}
	else
	{
		out = mPendingActions.front();
		mPendingActions.pop();
		return true;
	}
}