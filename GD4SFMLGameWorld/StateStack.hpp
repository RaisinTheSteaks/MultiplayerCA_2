
/*
Charlie Duff
D00183790
*/
#pragma once
#include "State.hpp"
#include "StateID.hpp"
#include "ResourceIdentifiers.hpp"
#include "StateStackActionID.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
#include <utility>
#include <functional>
#include <map>

namespace sf
{
	class Event;
}

class StateStack : private sf::NonCopyable
{
public:
	explicit StateStack(State::Context context);

	template<typename>
	void registerState(StateID stateID);

	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);

	void pushState(StateID stateID);
	void popState();
	void clearStates();

	bool isEmpty() const;

private:
	State::Ptr createState(StateID stateID);
	void applyPendingChanges();

private:
	struct PendingChange
	{
		explicit PendingChange(StateStackActionID action, StateID stateID = StateID::None);

		StateStackActionID action;
		StateID stateID;
	};

private:
	std::vector<State::Ptr> mStack;
	std::vector<PendingChange> mPendingList;

	State::Context mContext;
	std::map < StateID, std::function<State::Ptr()>> mFactories;
};

template<typename T>
void StateStack::registerState(StateID stateID)
{
	mFactories[stateID] = [this]()
	{
		return State::Ptr(new T(*this, mContext));
	};
}

