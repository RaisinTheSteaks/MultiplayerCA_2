/*
Charlie Duff
D00183790
*/
#include "SettingsState.hpp"

SettingState::SettingState(StateStack& stack, Context context)
	:State(stack, context)
	, mGUIContainer()
{
	mBackgroundSprite.setTexture(context.textures->get(TextureID::TitleScreen));
	//Build key bindings and button labels
	addButtonLabel(ActionID::MoveLeft, 300.f, "Move Left", context);
	addButtonLabel(ActionID::MoveRight, 350.f, "Move Right", context);
	addButtonLabel(ActionID::MoveUp, 400.f, "Move Up", context);
	addButtonLabel(ActionID::MoveDown, 450.f, "Move Down", context);
	addButtonLabel(ActionID::Fire, 500.f, "Fire", context);
	addButtonLabel(ActionID::LaunchMissile, 550.f, "Missile", context);

	updateLabels();

	auto backButton = std::make_shared<GUI::Button>(context);
	backButton->setPosition(80.f, 620.f);
	backButton->setText("Back");
	backButton->setCallback(std::bind(&SettingState::requestStackPop, this));

	mGUIContainer.pack(backButton);
	
}

void SettingState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool SettingState::update(sf::Time dt)
{
	return true;
}

bool SettingState::handleEvent(const sf::Event& event)
{
	bool isKeyBinding = false;

	//Iterate through all key binding buttons to see they are being pressed, waiting for the user to enter a key
	for (std::size_t action = 0; action < static_cast<int>(ActionID::ActionCount); ++action)
	{
		if (mBindingButtons[action]->isActive())
		{
			isKeyBinding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				getContext().player->assignKey(static_cast<ActionID>(action), event.key.code);
				mBindingButtons[action]->deactivate();
			}
			break;
		}
	}

	//If the keybindings have been updated we need to update the labels
	if (isKeyBinding)
	{
		updateLabels();
	}
	else
	{
		mGUIContainer.handleEvent(event);
	}
	return false;
}

void SettingState::updateLabels()
{
	Player& player = *getContext().player;
	for (std::size_t i = 0; i < static_cast<int>(ActionID::ActionCount); ++i)
	{
		sf::Keyboard::Key key = player.getAssignedKey(static_cast<ActionID>(i));
		mBindingLabels[i]->setText(toString(key));
	}
}

void SettingState::addButtonLabel(ActionID action, float y, const std::string& text, Context context)
{
	mBindingButtons[static_cast<int>(action)] = std::make_shared<GUI::Button>(context);
	mBindingButtons[static_cast<int>(action)]->setPosition(80.f, y);
	mBindingButtons[static_cast<int>(action)]->setText(text);
	mBindingButtons[static_cast<int>(action)]->setToggle(true);

	mBindingLabels[static_cast<int>(action)] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[static_cast<int>(action)]->setPosition(300.f, y + 15.f);

	mGUIContainer.pack(mBindingButtons[static_cast<int>(action)]);
	mGUIContainer.pack(mBindingLabels[static_cast<int>(action)]);


}
