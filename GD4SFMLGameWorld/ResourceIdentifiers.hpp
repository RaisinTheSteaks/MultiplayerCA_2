/*
Charlie Duff
D00183790
*/
#pragma once
#include "TextureID.hpp"
#include "FontID.hpp"
#include "ShaderID.hpp"
#include "SoundEffectID.hpp"

namespace sf
{
	class Texture;
	class Font;
	class Shader;
	class SoundBuffer;
}

template<typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, TextureID> TextureHolder;
typedef ResourceHolder<sf::Font, FontID> FontHolder;
typedef ResourceHolder<sf::Shader, ShaderID> ShaderHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffectID> SoundBufferHolder;