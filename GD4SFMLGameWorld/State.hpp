/*
Charlie Duff
D00183790
*/
#pragma once
#include "StateID.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"

#include <memory>

class Player;
class StateStack;

namespace sf
{
	class RenderWindow;
}

class State
{
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context
	{
		Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& font, Player& player, Player& player2, MusicPlayer& music, SoundPlayer& sounds);

		sf::RenderWindow* window;
		TextureHolder* textures;
		FontHolder* fonts;
		Player* player;
		Player* player2;
		MusicPlayer* music;
		SoundPlayer* sounds;
	};

public:
	State(StateStack& stack, Context context);
	//All parent classes should have a virtual destructor, otherwise we end up with memory leaks - only
	//the child part of the class will get destructed without the virtual destructor
	virtual ~State();

	virtual void draw() = 0;
	virtual bool update(sf::Time dt) = 0;
	virtual bool handleEvent(const sf::Event& event) = 0;

protected:
	void requestStackPush(StateID stateID);
	void requestStackPop();
	void requestStackClear();

	Context getContext() const;

private:
	StateStack* mStack;
	Context mContext;
};