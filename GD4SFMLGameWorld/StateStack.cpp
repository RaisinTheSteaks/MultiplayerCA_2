/*
Charlie Duff
D00183790
*/
#include "StateStack.hpp"
#include <cassert>

StateStack::StateStack(State::Context context)
	: mStack(),
	mPendingList(),
	mContext(context),
	mFactories()
{
}

void StateStack::update(sf::Time dt)
{
	//Iterate from top to bottom, stop as soon as update returns false
	for (auto itr = mStack.rbegin(); itr != mStack.rend(); ++itr)
	{
		if (!(*itr)->update(dt))
		{
			break;
		}
	}
	applyPendingChanges();
}

void StateStack::draw()
{
	//Draw all active states from bottom to top
	for (State::Ptr& state : mStack)
	{
		state->draw();
	}
}

void StateStack::handleEvent(const sf::Event& event)
{
	//Iterate from top to bottom, stop as soon handleEvent() returns false
	for (auto itr = mStack.rbegin(); itr != mStack.rend(); ++itr)
	{
		if (!(*itr)->handleEvent(event))
		{
			break;
		}
	}
	applyPendingChanges();
}

void StateStack::pushState(StateID stateID)
{
	mPendingList.push_back(PendingChange(StateStackActionID::Push, stateID));
}

void StateStack::popState()
{
	mPendingList.push_back(PendingChange(StateStackActionID::Pop));
}

void StateStack::clearStates()
{
	mPendingList.push_back(PendingChange(StateStackActionID::Clear));
}

bool StateStack::isEmpty() const
{
	return mStack.empty();
}

State::Ptr StateStack::createState(StateID stateID)
{
	auto found = mFactories.find(stateID);
	assert(found != mFactories.end());

	return found->second();
}

void StateStack::applyPendingChanges()
{
	for (PendingChange change : mPendingList)
	{
		switch (change.action)
		{
		case StateStackActionID::Push:
			mStack.push_back(createState(change.stateID));
			break;
		case StateStackActionID::Pop:
			mStack.pop_back();
			break;

		case StateStackActionID::Clear:
			mStack.clear();
			break;
		}
	}
	mPendingList.clear();
}

StateStack::PendingChange::PendingChange(StateStackActionID action, StateID stateID) :
	action(action), stateID(stateID)
{
}
