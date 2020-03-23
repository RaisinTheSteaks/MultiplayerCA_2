/*
Charlie Duff
D00183790
*/
#include "PauseState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "Container.hpp"
#include "Button.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>

PauseState::PauseState(StateStack& stack, Context context, bool letsUpdatesTrhough)
	:State(stack, context)
	, mBackgroundSprite()
	, mPausedText()
	, mGUIContainer()
	, mLetsUpdatesThrough(letsUpdatesTrhough)
{
	sf::Font& font = context.fonts->get(FontID::Main);
	sf::Vector2f viewSize = context.window->getView().getSize();

	mPausedText.setFont(font);
	mPausedText.setString("Game Paused");
	mPausedText.setCharacterSize(70);
	centreOrigin(mPausedText);
	mPausedText.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	auto returnButton = std::make_shared<GUI::Button>(context);
	returnButton->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 75);
	returnButton->setText("Return");
	returnButton->setCallback([this]()
		{
			requestStackPop();
		});

	auto backToMenuButton = std::make_shared<GUI::Button>(context);
	backToMenuButton->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 125);
	backToMenuButton->setText("Back to menu");
	backToMenuButton->setCallback([this]()
		{
			requestStackClear();
			requestStackPush(StateID::Menu);
		});

	mGUIContainer.pack(returnButton);
	mGUIContainer.pack(backToMenuButton);
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
	window.draw(mGUIContainer);

	

}

bool PauseState::update(sf::Time dt)
{
	return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}
