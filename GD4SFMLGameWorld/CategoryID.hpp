/*
Charlie Duff
D00183790
*/
#pragma once

//Entity/SceneNode category, used to dispatch commands
enum class CategoryID
{
	None = 0,
	SceneAirLayer = 1 << 0,
	PlayerShip = 1 << 1,
	Player2Ship = 1 << 2,
	AlliedShip = 1 << 3,
	EnemyShip = 1 << 4,
	Pickup = 1 << 5,
	AlliedProjectile = 1 << 6,
	Allied2Projectile = 1 << 7,
	EnemyProjectile = 1 << 8,
	ParticleSystem = 1 << 9,
	SoundEffect = 1 << 10,
	Gun = 1 << 11,
	Island1 = 1 << 12,
	Network = 1 << 13,
	

	Ship = PlayerShip | Player2Ship | AlliedShip | EnemyShip,
	Projectile = AlliedProjectile | Allied2Projectile | EnemyProjectile,
	Island = Island1
};