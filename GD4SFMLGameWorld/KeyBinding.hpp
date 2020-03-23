#pragma once
#include <SFML/Window/Keyboard.hpp>
#include "ActionID.hpp"

#include <map>
#include <vector>
class KeyBinding
{
public:
	typedef ActionID Action;


public:
	explicit KeyBinding(int controlPreconfiguration);

	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;

	bool checkAction(sf::Keyboard::Key key, Action& out) const;
	std::vector<Action> getRealtimeActions() const;

private:
	void initializeActions();


private:
	std::map<sf::Keyboard::Key, Action>	mKeyMap;
};

bool isRealtimeAction(ActionID action);