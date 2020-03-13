/*
Charlie Duff
D00183790
*/
#pragma once
#include "SFML/System/NonCopyable.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "Command.hpp"
#include "CommandQueue.hpp"
#include "Utility.hpp"

#include <vector>
#include <memory>
#include <set>

class SceneNode : public sf::Transformable, public sf::Drawable, private sf::NonCopyable
{
public:
	typedef std::unique_ptr<SceneNode> Ptr;
	typedef std::pair<SceneNode*, SceneNode*> Pair;

public:
	SceneNode(CategoryID category = CategoryID::None);
	void attachChild(Ptr child);
	Ptr detachChild(const SceneNode& node);

	void update(sf::Time dt, CommandQueue& commands);

	sf::Vector2f getWorldPosition() const;
	sf::Transform getWorldTransform() const;

	virtual sf::FloatRect	getBoundingRect() const;

	void checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
	void checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);

	virtual unsigned int getCategory() const;
	void onCommand(const Command& command, sf::Time dt);
	virtual bool isDestroyed() const;
	virtual bool isMarkedForRemoval() const;

	void removeWrecks();

private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateChildren(sf::Time dt, CommandQueue& commands);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	std::vector<Ptr> mChildren;
	SceneNode* mParent;
	CategoryID mDefaultCategory;
};

float	distance(const SceneNode& lhs, const SceneNode& rhs);
bool	collision(const SceneNode& lhs, const SceneNode& rhs);

