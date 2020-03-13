/*
Charlie Duff
D00183790
*/
#pragma once
#include <sstream>
#include <SFML/System/Vector2.hpp>
#include "Animation.hpp"
namespace sf
{
	class Sprite;
	class Text;
}



template<typename T>
std::string toString(const T& value);

void centreOrigin(sf::Sprite& sprite);
void centreOrigin(sf::Text& text);
void centreOrigin(Animation& animation);

// Degree/radian conversion
float toDegree(float radian);
float toRadian(float degree);

// Vector operations
float length(sf::Vector2f vector);
sf::Vector2f unitVector(sf::Vector2f vector);

// Random number generation
int	randomInt(int exclusiveMax);

#include "Utility.inl"