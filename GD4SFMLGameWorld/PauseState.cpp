/*
Charlie Duff
D00183790
*/
#include "PauseState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>

PauseState::PauseState(StateStack& stack, Context context)
	:State(stack, context)
	, mBackgroundSprite()
	, mPausedText()
	, mInstructionText()
{
	sf::Font& font = context.fonts->get(FontID::Main);
	sf::Vector2f viewSize = context.window->getView().getSize();

	mPausedText.setFont(font);
	mPausedText.setString("Game Paused");
	mPausedText.setCharacterSize(70);
	centreOrigin(mPausedText);
	mPausedText.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	mInstructionText.setFont(font);
	mInstructionText.setString("Press Backspace to return to the main menu");
	centreOrigin(mInstructionText);
	mInstructionText.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);
	getContext().music->setPaused(true);
}

PauseState::~PauseState()
{
	getContext().music->setPaused(false);
}

void PauseState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(mPausedText);
	window.draw(mInstructionText);

	

}

bool PauseState::update(sf::Time dt)
{
	return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
	if (event.type != sf::Event::KeyPressed)
	{
		return false;
	}

	//If escape return to the game
	if (event.key.code == sf::Keyboard::Escape)
	{
		requestStackPop();
	}

	if (event.key.code == sf::Keyboard::BackSpace)
	{
		requestStackClear();
		requestStackPush(StateID::Menu);
	}
	return false;
}
