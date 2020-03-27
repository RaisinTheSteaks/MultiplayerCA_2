/*
Charlie Duff
D00183790
*/
#include "DataTables.hpp"
#include "Ship.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "ShipID.hpp"
#include "ProjectileID.hpp"
#include "PickupID.hpp"
#include "ParticleID.hpp"
#include "PlayerID.hpp"
#include "CategoryID.hpp"
#include "Island.hpp"

#include <fstream>




//vector playerdata to store player keybindings and category, allows player 1 & 2 to have different keybindings, category, and textures 
std::vector<PlayerData> initializePlayerData()
{
	std::vector<PlayerData> data(static_cast<int>(PlayerID::TypeCount));
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::A] = ActionID::MoveLeft;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::D] = ActionID::MoveRight;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::W] = ActionID::MoveUp;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::S] = ActionID::MoveDown;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::Num1 ] = ActionID::FireLeft;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::Num2 ] = ActionID::FireRight;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::N] = ActionID::LaunchMissile;
	data[static_cast<int>(PlayerID::Player1)].categoryID = CategoryID::PlayerShip;

	data[static_cast<int>(PlayerID::Player2)].mKeyBinding[sf::Keyboard::Left] = ActionID::MoveLeft;
	data[static_cast<int>(PlayerID::Player2)].mKeyBinding[sf::Keyboard::Right] = ActionID::MoveRight;
	data[static_cast<int>(PlayerID::Player2)].mKeyBinding[sf::Keyboard::Up] = ActionID::MoveUp;
	data[static_cast<int>(PlayerID::Player2)].mKeyBinding[sf::Keyboard::Down] = ActionID::MoveDown;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::Numpad1] = ActionID::FireLeft;
	data[static_cast<int>(PlayerID::Player1)].mKeyBinding[sf::Keyboard::Numpad2] = ActionID::FireRight;
	data[static_cast<int>(PlayerID::Player2)].mKeyBinding[sf::Keyboard::M] = ActionID::LaunchMissile;
	data[static_cast<int>(PlayerID::Player2)].categoryID = CategoryID::Player2Ship;


	return data;
}
//vector island data to store island texture and category
std::vector<IslandData> initializeIslandData()
{
	std::vector<IslandData> data(static_cast<int>(IslandID::IslandCount));
	data[static_cast<int>(IslandID::Island)].texture = TextureID::Island;
	data[static_cast<int>(IslandID::Island)].textureRect = sf::IntRect(0, 0, 41, 41);
	data[static_cast<int>(IslandID::Island)].categoryID = CategoryID::Island1;
	return data;
}

std::vector<ShipData> initializeShipData()
{
	//TODO - Change this for better multiplayer
	float playerSpeed = 125.f;
	int playerFireRate = 3;
	std::vector<ShipData> data(static_cast<int>(ShipID::TypeCount));
	data[static_cast<int>(ShipID::Battleship)].hitpoints = 100;
	data[static_cast<int>(ShipID::Battleship)].speed = playerSpeed / 10;
	data[static_cast<int>(ShipID::Battleship)].fireInterval = sf::seconds(playerFireRate);
	data[static_cast<int>(ShipID::Battleship)].textureRect = sf::IntRect(10, 10, 60, 60);
	data[static_cast<int>(ShipID::Battleship)].texture = TextureID::ShipForward;
	data[static_cast<int>(ShipID::Battleship)].hasRollAnimation = false;
	data[static_cast<int>(ShipID::Battleship)].turnSpeed = 0.7f;
	data[static_cast<int>(ShipID::Battleship)].forward = TextureID::ShipForward;

	data[static_cast<int>(ShipID::Battleship2)].hitpoints = 100;
	data[static_cast<int>(ShipID::Battleship2)].speed = playerSpeed / 10;
	data[static_cast<int>(ShipID::Battleship2)].fireInterval = sf::seconds(playerFireRate);
	data[static_cast<int>(ShipID::Battleship2)].textureRect = sf::IntRect(10, 10, 60, 60);
	data[static_cast<int>(ShipID::Battleship2)].texture = TextureID::ShipForward;
	data[static_cast<int>(ShipID::Battleship2)].hasRollAnimation = false;
	data[static_cast<int>(ShipID::Battleship2)].turnSpeed = 0.7f;
	data[static_cast<int>(ShipID::Battleship2)].forward = TextureID::ShipForward;

#pragma region Aircraft
	data[static_cast<int>(ShipID::Raptor)].hitpoints = 20;
	data[static_cast<int>(ShipID::Raptor)].speed = 80.f;
	data[static_cast<int>(ShipID::Raptor)].fireInterval = sf::Time::Zero;
	data[static_cast<int>(ShipID::Raptor)].texture = TextureID::Entities;
	data[static_cast<int>(ShipID::Raptor)].textureRect = sf::IntRect(144, 0, 84, 64);

	data[static_cast<int>(ShipID::Raptor)].directions.push_back(Direction(+45.f, 80.f));
	data[static_cast<int>(ShipID::Raptor)].directions.push_back(Direction(-45.f, 160.f));
	data[static_cast<int>(ShipID::Raptor)].directions.push_back(Direction(+45.f, 80.f));
	data[static_cast<int>(ShipID::Raptor)].hasRollAnimation = false;

	data[static_cast<int>(ShipID::Avenger)].hitpoints = 40;
	data[static_cast<int>(ShipID::Avenger)].speed = 50.f;
	data[static_cast<int>(ShipID::Avenger)].fireInterval = sf::seconds(2);
	data[static_cast<int>(ShipID::Avenger)].texture = TextureID::Entities;
	data[static_cast<int>(ShipID::Avenger)].textureRect = sf::IntRect(228, 0, 60, 59);
	data[static_cast<int>(ShipID::Avenger)].directions.push_back(Direction(+45.f, 50.f));
	data[static_cast<int>(ShipID::Avenger)].directions.push_back(Direction(0.f, 50.f));
	data[static_cast<int>(ShipID::Avenger)].directions.push_back(Direction(-45.f, 100.f));
	data[static_cast<int>(ShipID::Avenger)].directions.push_back(Direction(0.f, 50.f));
	data[static_cast<int>(ShipID::Avenger)].directions.push_back(Direction(+45.f, 50.f));
	data[static_cast<int>(ShipID::Avenger)].hasRollAnimation = false;

#pragma endregion
	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileID::TypeCount));

	float bulletSpeed = 350.f;
	
	data[static_cast<int>(ProjectileID::AlliedBullet)].damage = 25;
	data[static_cast<int>(ProjectileID::AlliedBullet)].speed = bulletSpeed;
	data[static_cast<int>(ProjectileID::AlliedBullet)].texture = TextureID::Arrows;
	data[static_cast<int>(ProjectileID::AlliedBullet)].textureRect = sf::IntRect(0, 11, 21, 15);

	data[static_cast<int>(ProjectileID::EnemyBullet)].damage = 10;
	data[static_cast<int>(ProjectileID::EnemyBullet)].speed = bulletSpeed;
	data[static_cast<int>(ProjectileID::EnemyBullet)].texture = TextureID::Arrows;
	data[static_cast<int>(ProjectileID::EnemyBullet)].textureRect = sf::IntRect(0, 11, 21, 15);


	data[static_cast<int>(ProjectileID::Missile)].damage = 200;
	data[static_cast<int>(ProjectileID::Missile)].speed = 250.f;
	data[static_cast<int>(ProjectileID::Missile)].texture = TextureID::Entities;
	data[static_cast<int>(ProjectileID::Missile)].textureRect = sf::IntRect(160, 64, 15, 32);

	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupID::TypeCount));
	data[static_cast<int>(PickupID::HealthRefill)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::HealthRefill)].textureRect = sf::IntRect(0, 64, 40, 40);
	data[static_cast<int>(PickupID::HealthRefill)].action = [](Ship& a) {a.repair(25); };

	data[static_cast<int>(PickupID::MissileRefill)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::MissileRefill)].textureRect = sf::IntRect(40, 64, 40, 40);
	data[static_cast<int>(PickupID::MissileRefill)].action = std::bind(&Ship::collectMissiles, std::placeholders::_1, 3);

	data[static_cast<int>(PickupID::FireSpread)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::FireSpread)].textureRect = sf::IntRect(80, 64, 40, 40);
	data[static_cast<int>(PickupID::FireSpread)].action = std::bind(&Ship::increaseSpread, std::placeholders::_1);

	data[static_cast<int>(PickupID::FireRate)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::FireRate)].textureRect = sf::IntRect(120, 64, 40, 40);
	data[static_cast<int>(PickupID::FireRate)].action = std::bind(&Ship::increaseFireRate, std::placeholders::_1);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(static_cast<int>(ParticleID::ParticleCount));

	data[static_cast<int>(ParticleID::Propellant)].color = sf::Color(255, 255, 50);
	data[static_cast<int>(ParticleID::Propellant)].lifetime = sf::seconds(0.6f);

	data[static_cast<int>(ParticleID::Smoke)].color = sf::Color(50, 50, 50);
	data[static_cast<int>(ParticleID::Smoke)].lifetime = sf::seconds(4.f);

	return data;
}
