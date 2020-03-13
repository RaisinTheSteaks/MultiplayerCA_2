/*
Charlie Duff
D00183790
*/
#include "Button.hpp"
#include "State.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

GUI::Button::Button(State::Context context)
	:mCallback()
	, mSprite(context.textures->get(TextureID::Buttons))
	, mText("", context.fonts->get(FontID::Alternate), 16)
	, mIsToggle(false)
	, mSounds(*context.sounds)
{
	changeTexture(ButtonID::Normal);
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
}

void GUI::Button::setCallback(Callback callback)
{
	mCallback = std::move(callback);
}

void GUI::Button::setText(const std::string& text)
{
	mText.setString(text);
	centreOrigin(mText);
}

void GUI::Button::setToggle(bool flag)
{
	mIsToggle = flag;
}

bool GUI::Button::isSelectable() const
{
	return true;
}

void GUI::Button::select()
{
	Component::select();
	changeTexture(ButtonID::Selected);
}

void GUI::Button::deselect()
{
	Component::deselect();
	changeTexture(ButtonID::Normal);
}

void GUI::Button::activate()
{
	Component::activate();
	mSounds.play(SoundEffectID::Button);

	//If we toggle then we should show the button as pressed/toggled
	if (mIsToggle)
	{
		changeTexture(ButtonID::Pressed);
	}
	if (mCallback)
	{
		mCallback();
	}

	if (!mIsToggle)
	{
		deactivate();
	}
}

void GUI::Button::deactivate()
{
	Component::deactivate();

	if (mIsToggle)
	{
		//Reset the texture
		if (isSelected())
		{
			changeTexture(ButtonID::Selected);
		}
		else
		{
			changeTexture(ButtonID::Normal);
		}
	}
}

void GUI::Button::handleEvent(const sf::Event& event)
{
}

void GUI::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
	target.draw(mText, states);
}

void GUI::Button::changeTexture(ButtonID button)
{
	sf::IntRect textureRect(0, 50 * static_cast<int>(button), 200, 50);
	mSprite.setTextureRect(textureRect);
}
