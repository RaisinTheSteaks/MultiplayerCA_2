/*
Charlie Duff
D00183790
*/
#include "State.hpp"
#include "StateStack.hpp"

State::State(StateStack& stack, Context context) : mStack(&stack), mContext(context)
{
}

State::~State()
{
}


void State::requestStackPush(StateID stateID)
{
	mStack->pushState(stateID);
}

void State::requestStackPop()
{
	mStack->popState();
}

void State::requestStackClear()
{
	mStack->clearStates();
}

State::Context State::getContext() const
{
	return mContext;
}

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& font, MusicPlayer& music, SoundPlayer& sounds, KeyBinding& keys1, KeyBinding& keys2) :
	window(&window), textures(&textures), fonts(&font), music(&music), sounds(&sounds), keys1(&keys1), keys2(&keys2)
{
}

void State::onActivate()
{
}

void State::onDestroy()
{
}
