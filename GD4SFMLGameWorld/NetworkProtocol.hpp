#pragma once

#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>

const unsigned short ServerPort = 50000;

namespace Server
{
	//Packets that originate from the server
	enum class PacketType
	{
		BroadcastMessage, //This takes an std::string and is used to send a message to all clients, which they can show on the screen for some seconds
		SpawnSelf, //The takes an sf::Int32 value for the aircraft identifier, and two float values for its initial position. These are used to spawn the client's player one aircraft
		InitialState, //This takes two float values, the world height, then sends an sf::Int32 with the count of the aircraft in the world, then for each aircraft it sends sf::Int32 identifier and two float values with the position of the aircraft
		///InitialState doesn't take in scrolling speed value now
		PlayerEvent, //The takes two sf::Int32, the aircraft identifier and the action identifier as declared in ActionID.hpp. 
		PlayerRealtimeChange, //The same as PlayerEvent but for real time actions. This means we are changing an ongoing state so add a Boolean value to the parameters
		PlayerConnect, //The as SpawnSelf but indicates that an aircraft from a different machine is entering the world.
		PlayerDisconnect, //Sends one sf::Int32 with identifier for aircraft that has diconnected
		AcceptCoopPartner, //This is used to tell the client that it can spawn a second player. It needs sf::Int32 with id for new plane and two float values with the position of the aircraft
		SpawnPickup, //sf::Int32 with type of pickup, 2 floats for position
		UpdateClientState, //This takes a float with the current scrolling distance on the server, an sf::Int32 with the aircraft count. For each aircraft send identifier and two float values for position
		GameOver //This no arguments. It is just used to tell the client that the game is over
		//we changed this from MissionSuccess to GameOver
	};
}

namespace Client
{
	// Packets originated in the client
	enum class PacketType
	{
		PlayerEvent, //This takes two sf::Int32 variables, an aircraft identifier, and the event to be triggered as defined in ActionID. It is used to request the server to trigger an event on the requested aircraft
		PlayerRealtimeChange, //This is the same as Playerevent but additionally takes a boolean to state whether the action is active or not
		RequestCoopPartner, //Takes no parameters. It is sent when the user presses the Return key to request a local partner. Its counterpart is AcceptCoopPartner
		PositionUpdate, //Takes an sf::Int32 variable with the number of local aircraft, and for each aircraft it packs another sf::Int32 variable for the identifer and two float values for the position
		//Position x, position y, rotation, health
		GameEvent, //This informs the server of a specific happening in the client's game logic, such as enemy explosions
		Quit //Takes no parameters - it just informs the server that the game state is closing so it can remove its aricraft
	};
}