/*
Charlie Duff
D00183790
*/
#include "SettingsState.hpp"
#include "Utility.hpp"

SettingState::SettingState(StateStack& stack, Context context)
	:State(stack, context)
	, mGUIContainer()
	, mTextureID(getTextureFromFile())
{
	mBackgroundSprite.setTexture(context.textures->get(TextureID::TitleScreen));
	//Build key bindings and button labels
	for (std::size_t x = 0; x < 2; ++x)
	{
		addButtonLabel(static_cast<int>(ActionID::MoveLeft), x, 0, "Move Left", context);
		addButtonLabel(static_cast<int>(ActionID::MoveRight), x, 1, "Move Right", context);
		addButtonLabel(static_cast<int>(ActionID::MoveUp), x, 2, "Move Up", context);
		addButtonLabel(static_cast<int>(ActionID::MoveDown), x, 3, "Move Down", context);
		addButtonLabel(static_cast<int>(ActionID::FireLeft), x, 4, "FireLeft", context);
		addButtonLabel(static_cast<int>(ActionID::FireRight), x, 5, "FireRight", context);
		addButtonLabel(static_cast<int>(ActionID::LaunchMissile), x, 6, "Missile", context);
	}

	updateLabels();


	auto spriteButton = std::make_shared<GUI::Button>(context);
	spriteButton->setPosition(900.f, 420.f);
	spriteButton->setText("Change Ship");
	spriteButton->setCallback([this]()
	{
			sf::Int32 nextSprite = (static_cast<sf::Int32>(mTextureID) + 1) % (static_cast<sf::Int32>(TextureID::FrigateForward) + 1);
			if (nextSprite == 0)
				nextSprite = static_cast<sf::Int32>(TextureID::ShipForward);
			mTextureID = static_cast<TextureID>(nextSprite);
			updateSpriteLabel();
	});
	mGUIContainer.pack(spriteButton);

	mSpriteLabel = std::make_shared<GUI::Label>(textureIDToString(mTextureID), *context.fonts);
	mSpriteLabel->setPosition(990, 480.f);
	mGUIContainer.pack(mSpriteLabel);

	auto backButton = std::make_shared<GUI::Button>(context);
	backButton->setPosition(80.f, 620.f);
	backButton->setText("Back");
	backButton->setCallback([this]()
		{
			writeTextureToFile(mTextureID);
			requestStackPop();
			
		});

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
	for (std::size_t action = 0; action < 2 * static_cast<int>(ActionID::ActionCount); ++action)
	{
		if (mBindingButtons[action]->isActive())
		{
			isKeyBinding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				// Player 1
				if (action < static_cast<int>(ActionID::ActionCount))
					getContext().keys1->assignKey(static_cast<ActionID>(action), event.key.code);

				// Player 2
				else
					getContext().keys2->assignKey(static_cast<ActionID>(action - static_cast<int>(ActionID::ActionCount)), event.key.code);
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
	for (std::size_t i = 0; i < static_cast<int>(ActionID::ActionCount); ++i)
	{
		auto action = static_cast<ActionID>(i);

		// Get keys of both players
		sf::Keyboard::Key key1 = getContext().keys1->getAssignedKey(action);
		sf::Keyboard::Key key2 = getContext().keys2->getAssignedKey(action);

		// Assign both key strings to labels
		mBindingLabels[i]->setText(toString(key1));
		mBindingLabels[i + static_cast<int>(ActionID::ActionCount)]->setText(toString(key2));
	}
}

void SettingState::updateSpriteLabel()
{
	mSpriteLabel->setText(textureIDToString(mTextureID));
}

void SettingState::addButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, Context context)
{
	// For x==0, start at index 0, otherwise start at half of array
	index += static_cast<int>(ActionID::ActionCount)* x;

	mBindingButtons[index] = std::make_shared<GUI::Button>(context);
	mBindingButtons[index]->setPosition(400.f * x + 80.f, 50.f * y + 300.f);
	mBindingButtons[index]->setText(text);
	mBindingButtons[index]->setToggle(true);

	mBindingLabels[index] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[index]->setPosition(400.f * x + 300.f, 50.f * y + 315.f);

	mGUIContainer.pack(mBindingButtons[index]);
	mGUIContainer.pack(mBindingLabels[index]);

}

std::string SettingState::textureIDToString(TextureID texture)
{
	if (mTextureID == TextureID::ShipForward)
	{
		return "Ship";
	}
	else if (mTextureID == TextureID::FrigateForward)
	{
		return "Frigate";
	}
}
