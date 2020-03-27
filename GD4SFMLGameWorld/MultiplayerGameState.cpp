#include "MultiplayerGameState.hpp"
#include "MusicPlayer.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <fstream>
#include <iostream>


sf::IpAddress getAddressFromFile()
{
	{ // Try to open existing file (RAII block)
		std::ifstream inputFile("ip.txt");
		std::string ipAddress;
		if (inputFile >> ipAddress)
			return ipAddress;
	}

	// If open/read failed, create new file
	std::ofstream outputFile("ip.txt");
	std::string localAddress = "127.0.0.1";
	outputFile << localAddress;
	return localAddress;
}

MultiplayerGameState::MultiplayerGameState(StateStack& stack, Context context, bool isHost) 
	: State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds, true)
	, mWindow(*context.window)
	, mTextureHolder(*context.textures)
	, mConnected(false)
	, mGameServer(nullptr)
	, mActiveState(true)
	, mHasFocus(true)
	, mHost(isHost)
	, mGameStarted(false)
	, mClientTimeout(sf::seconds(2.f))
	, mTimeSinceLastPacket(sf::seconds(0.f))
	, mLobbyState(true)
	, mTimer(sf::seconds(900))
	
{
	mBroadcastText.setFont(context.fonts->get(FontID::Alternate));
	mBroadcastText.setPosition(1024.f / 2, 100.f);

	mPlayerInvitationText.setFont(context.fonts->get(FontID::Alternate));
	mPlayerInvitationText.setCharacterSize(20);
	mPlayerInvitationText.setFillColor(sf::Color::White);
	mPlayerInvitationText.setString("Press Enter to start the game!");
	centreOrigin(mPlayerInvitationText);
	mPlayerInvitationText.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 2.f);

	// We reuse this text for "Attempt to connect" and "Failed to connect" messages
	mFailedConnectionText.setFont(context.fonts->get(FontID::Alternate));
	mFailedConnectionText.setString("Attempting to connect...");
	mFailedConnectionText.setCharacterSize(35);
	mFailedConnectionText.setFillColor(sf::Color::White);
	centreOrigin(mFailedConnectionText);
	mFailedConnectionText.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 2.f);

	int min = static_cast<int>(mTimer.asSeconds()) / 60;
	int sec = static_cast<int>(mTimer.asSeconds()) % 60;
	std::string time = std::to_string(min) + ":" + std::to_string(sec);
	mTimerText.setFont(context.fonts->get(FontID::Alternate));
	mTimerText.setString(time);
	mTimerText.setCharacterSize(30);
	mTimerText.setFillColor(sf::Color::White);
	centreOrigin(mTimerText);
	mTimerText.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 12.f);

	// Render a "establishing connection" frame for user feedback
	mWindow.clear(sf::Color::Black);
	mWindow.draw(mFailedConnectionText);
	mWindow.display();
	mFailedConnectionText.setString("Could not connect to the remote server!");
	centreOrigin(mFailedConnectionText);

	sf::IpAddress ip;
	if (isHost)
	{
		mGameServer.reset(new GameServer(sf::Vector2f(mWindow.getSize())));
		ip = "127.0.0.1";
	}
	else
	{
		ip = getAddressFromFile();
	}

	if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done)
	{
		mConnected = true;



	}
	else
		mFailedConnectionClock.restart();

	mSocket.setBlocking(false);

	// Play game theme
	context.music->play(MusicID::MissionTheme);
	context.music->setVolume(3.f);
}

void MultiplayerGameState::draw()
{
	if (mConnected)
	{
		mWorld.draw();

		// Broadcast messages in default view
		// TODO - camera set to player, smaller than the world size
		mWindow.setView(mWindow.getDefaultView());
		mWindow.draw(mTimerText);

		if (!mBroadcasts.empty())
			mWindow.draw(mBroadcastText);

		if (mHost && mLobbyState && mPlayerInvitationTime < sf::seconds(0.5f))
			mWindow.draw(mPlayerInvitationText);

		
	}
	else
	{
		mWindow.draw(mFailedConnectionText);
	}
}

bool MultiplayerGameState::update(sf::Time dt)
{
	if (mConnected)
	{
		mWorld.update(dt);

		
		// Remove players whose aircrafts were destroyed
		bool foundLocalShip = false;
		for (auto itr = mPlayers.begin(); itr != mPlayers.end(); )
		{
			// Check if there are no more local planes for remote clients
			if (std::find(mLocalPlayerIdentifiers.begin(), mLocalPlayerIdentifiers.end(), itr->first) != mLocalPlayerIdentifiers.end())
			{
				foundLocalShip = true;
			}

			if (!mWorld.getShip(itr->first))
			{
				itr = mPlayers.erase(itr);

				// No more players left: Mission failed
				if (mPlayers.empty())
					requestStackPush(StateID::GameOver);
			}
			else
			{
				++itr;
			}
		}

		/*if (!foundLocalShip && mGameStarted)
		{
			requestStackPush(StateID::GameOver);
		}
*/
		// Only handle the realtime input if the window has focus and the game is unpaused
		if (mActiveState && mHasFocus && !mLobbyState)
		{
			CommandQueue& commands = mWorld.getCommandQueue();
			for (auto& pair : mPlayers)
				pair.second->handleRealtimeInput(commands);
		}

		// Always handle the network input
		CommandQueue& commands = mWorld.getCommandQueue();
		for (auto& pair : mPlayers)
			pair.second->handleRealtimeNetworkInput(commands);

		// Handle messages from server that may have arrived
		sf::Packet packet;
		if (mSocket.receive(packet) == sf::Socket::Done)
		{
			mTimeSinceLastPacket = sf::seconds(0.f);
			sf::Int32 packetType;
			packet >> packetType;
			handlePacket(packetType, packet);
		}
		else
		{
			// Check for timeout with the server
			if (mTimeSinceLastPacket > mClientTimeout)
			{
				mConnected = false;

				mFailedConnectionText.setString("Lost connection to server");
				centreOrigin(mFailedConnectionText);

				mFailedConnectionClock.restart();
			}
		}

		updateBroadcastMessage(dt);

		// Time counter for blinking 2nd player text
		mPlayerInvitationTime += dt;
		if (mPlayerInvitationTime > sf::seconds(1.f))
			mPlayerInvitationTime = sf::Time::Zero;

		

		// Events occurring in the game
		//TODO - Add Action class to handle gameAction
		//TODO - Add pollGameAction into World class
		Action gameAction;
		while (mWorld.pollGameAction(gameAction))
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PacketType::GameEvent);
			packet << static_cast<sf::Int32>(gameAction.type);
			packet << gameAction.position.x;
			packet << gameAction.position.y;

			mSocket.send(packet);
		}

		// Regular position updates
		if (mTickClock.getElapsedTime() > sf::seconds(1.f / 20.f))
		{

			sf::Packet positionUpdatePacket;
			positionUpdatePacket << static_cast<sf::Int32>(Client::PacketType::PositionUpdate);
			positionUpdatePacket << static_cast<sf::Int32>(mLocalPlayerIdentifiers.size());

			//TODO - check if int32 is best optimized choice for hp
			for (sf::Int32 identifier : mLocalPlayerIdentifiers)
			{
				if (Ship* ship = mWorld.getShip(identifier))
					positionUpdatePacket << identifier << ship->getPosition().x << ship->getPosition().y 
					<< ship->getRotation() << static_cast<sf::Int32>(ship->getHitpoints());
			}

			mSocket.send(positionUpdatePacket);
			mTickClock.restart();
		}

		mTimeSinceLastPacket += dt;
	}

	// Failed to connect and waited for more than 5 seconds: Back to menu
	else if (mFailedConnectionClock.getElapsedTime() >= sf::seconds(5.f))
	{
		requestStackClear();
		requestStackPush(StateID::Menu);
	}

	if (!mLobbyState)
	{
		sf::Time timer = mTimer - now();
		int min = static_cast<int>(timer.asSeconds()) / 60;
		int sec = static_cast<int>(timer.asSeconds()) % 60;
		std::string time = std::to_string(min) + ":" + std::to_string(sec);
		mTimerText.setString(time);
	}

	return true;
}

void MultiplayerGameState::printScoreBoard()
{
	std::ofstream outStream("scoreBoard.txt");
	if (outStream.is_open())
	{
		outStream.clear();
		outStream << "ScoreBoard\n________________\n";
		for (std::vector<ScoreKeeper>::iterator it = mScoreBoard.begin(); it != mScoreBoard.end(); ++it)
		{
			outStream << "[" << it->shipID << "," << it->score << "]\n";
		}
		outStream.close();
	}
	else
	{
		std::cout << "Cannot open file" << std::endl;
	}
}

bool MultiplayerGameState::handleEvent(const sf::Event& event)
{
	CommandQueue& commands = mWorld.getCommandQueue();
	// Forward event to all players
	if (!mLobbyState)
	{
		for (auto& pair : mPlayers)
			pair.second->handleEvent(event, commands);
	}


	if (event.type == sf::Event::KeyPressed)
	{
		// Enter pressed, add second player co-op (only if we are one player)
		if (event.key.code == sf::Keyboard::Return && mLobbyState && mHost)
		{
			/*std::cout << "Pressed" << std::endl;
			mLobbyState = false;*/
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PacketType::RequestStartGame);

			mSocket.send(packet);
			

		}

		// Escape pressed, trigger the pause screen
		else if (event.key.code == sf::Keyboard::Escape)
		{
			disableAllRealtimeActions();
			requestStackPush(StateID::NetworkPause);
		}
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		mHasFocus = true;
	}
	else if (event.type == sf::Event::LostFocus)
	{
		mHasFocus = false;
	}

	return true;
}

void MultiplayerGameState::onActivate()
{
	mActiveState = true;
}

void MultiplayerGameState::onDestroy()
{
	if (!mHost && mConnected)
	{
		// Inform server this client is dying
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PacketType::Quit);
		mSocket.send(packet);
	}
}

void MultiplayerGameState::disableAllRealtimeActions()
{
	mActiveState = false;

	//TODO - Add disableAllRealtimeActions to Player class
	for (sf::Int32 identifier : mLocalPlayerIdentifiers)
		mPlayers[identifier]->disableAllRealtimeActions();
}

void MultiplayerGameState::updateBroadcastMessage(sf::Time elapsedTime)
{
	if (mBroadcasts.empty())
		return;

	// Update broadcast timer
	mBroadcastElapsedTime += elapsedTime;
	if (mBroadcastElapsedTime > sf::seconds(2.5f))
	{
		// If message has expired, remove it
		mBroadcasts.erase(mBroadcasts.begin());

		// Continue to display next broadcast message
		if (!mBroadcasts.empty())
		{
			mBroadcastText.setString(mBroadcasts.front());
			centreOrigin(mBroadcastText);
			mBroadcastElapsedTime = sf::Time::Zero;
		}
	}
}

void MultiplayerGameState::handlePacket(sf::Int32 packetType, sf::Packet& packet)
{
	switch (packetType)
	{
#pragma region BroadcastMessage
		// Send message to all clients
		case static_cast<int>(Server::PacketType::BroadcastMessage) :
		{
			std::string message;
			packet >> message;
			mBroadcasts.push_back(message);

			// Just added first message, display immediately
			if (mBroadcasts.size() == 1)
			{
				mBroadcastText.setString(mBroadcasts.front());
				centreOrigin(mBroadcastText);
				mBroadcastElapsedTime = sf::Time::Zero;
			}
		} break;

#pragma endregion

#pragma region SpawnSelf
		
	// Sent by the server to order to spawn player 1 airplane on connect
	case static_cast<int>(Server::PacketType::SpawnSelf):
	{
		//TODO - check if int32 is best for ship id
		sf::Int32 shipIdentifier;
		sf::Vector2f shipPosition;
		//TODO - receive a struct type of Spawnpoint that contains pos x, pos y, and direction.
		packet >> shipIdentifier >> shipPosition.x >> shipPosition.y;

		ScoreKeeper sk = { shipIdentifier,0 };
		mScoreBoard.push_back(sk);
		std::cout << "Adding self: " << shipIdentifier << " to scoreboard" << std::endl;

		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PacketType::SendTexture);
		packet << shipIdentifier;
		packet << static_cast<sf::Int32>(getTextureFromFile());
		mSocket.send(packet);

		//TODO - add addShip into World class
		Ship* ship = mWorld.addShip(shipIdentifier);
		ship->setPosition(shipPosition);
		ship->setTexture(getTextureFromFile());
		//TODO - modify Player to handle 2 different key set (John Screencast)
		mPlayers[shipIdentifier].reset(new Player(&mSocket, shipIdentifier, getContext().keys1));
		mLocalPlayerIdentifiers.push_back(shipIdentifier);

		mGameStarted = true;
	} break;

#pragma endregion

#pragma region PlayerConnect
	case static_cast<int>(Server::PacketType::PlayerConnect):
	{
		sf::Int32 shipIdentifier;
		sf::Vector2f shipPosition;
		packet >> shipIdentifier >> shipPosition.x >> shipPosition.y;

		ScoreKeeper sk = {shipIdentifier,0};

		mScoreBoard.push_back(sk);
		std::cout << "Adding " << shipIdentifier << " to scoreboard" << std::endl;
		//TODO - add addShip into World class
		Ship* ship = mWorld.addShip(shipIdentifier);
		ship->setPosition(shipPosition);

		//TODO - modify Player to handle 2 different key set (John Screencast)
		mPlayers[shipIdentifier].reset(new Player(&mSocket, shipIdentifier, nullptr));
	} break;
#pragma endregion
	
#pragma region PlayerDisconnect
	case static_cast<int>(Server::PacketType::PlayerDisconnect):
	{
		sf::Int32 shipIdentifier;
		packet >> shipIdentifier;

		//TODO - add removeShip into World class
		mWorld.removeShip(shipIdentifier);
		mPlayers.erase(shipIdentifier);
	} break;
#pragma endregion

#pragma region InitialState
	case static_cast<int>(Server::PacketType::InitialState):
	{
		sf::Int32 shipCount;
		//float worldHeight;
		//packet >> worldHeight;

		//TODO - add setWorldHeight into World class
		//mWorld.setWorldHeight(worldHeight);
		

		packet >> shipCount;
		for (sf::Int32 i = 0; i < shipCount; ++i)
		{
			sf::Int32 shipIdentifier;
			sf::Int32 hitpoints;
			sf::Vector2f shipPosition;
			packet >> shipIdentifier >> shipPosition.x >> shipPosition.y >> hitpoints;

			Ship* ship = mWorld.addShip(shipIdentifier);
			ship->setPosition(shipPosition);

			//TODO - check if setHitPoints is pre-existing method or new method
			ship->setHitpoints(hitpoints);
			//TODO - change Player constructor to handle socket
			mPlayers[shipIdentifier].reset(new Player(&mSocket, shipIdentifier, nullptr));
		}
	} break;
#pragma endregion
	
#pragma region AcceptCoopPartner
	case static_cast<int>(Server::PacketType::AcceptCoopPartner):
	{
		//TODO -read spawn position x and y
		sf::Int32 shipIdentifier;
		packet >> shipIdentifier;

		mWorld.addShip(shipIdentifier);

		//TODO - change context to handle keys2
		mPlayers[shipIdentifier].reset(new Player(&mSocket, shipIdentifier, getContext().keys2));
		mLocalPlayerIdentifiers.push_back(shipIdentifier);
	} break;
#pragma endregion 

#pragma region PlayerEvent
	// Player event (like missile fired) occurs
	case static_cast<int>(Server::PacketType::PlayerEvent):
	{
		sf::Int32 shipIdentifier;
		sf::Int32 action;
		packet >> shipIdentifier >> action;

		std::cout << action << std::endl;
		auto itr = mPlayers.find(shipIdentifier);
		if (itr != mPlayers.end())
			itr->second->handleNetworkEvent(static_cast<ActionID>(action), mWorld.getCommandQueue());
	} break;
#pragma endregion

#pragma region PlayerRealtimeChange
	// Player's movement or fire keyboard state changes
	case static_cast<int>(Server::PacketType::PlayerRealtimeChange):
	{
		sf::Int32 shipIdentifier;
		sf::Int32 action;
		bool actionEnabled;
		packet >> shipIdentifier >> action >> actionEnabled;

		//TODO - add handleNetworkRealtimeChange to Player class
		auto itr = mPlayers.find(shipIdentifier);
		if (itr != mPlayers.end())
			itr->second->handleNetworkRealtimeChange(static_cast<ActionID>(action), actionEnabled);
	} break;
#pragma endregion

#pragma region GameOver
	case static_cast<int>(Server::PacketType::GameOver) :
	{
		printScoreBoard();
		requestStackPush(StateID::GameOver);
	} break;

	case static_cast<int>(Server::PacketType::TextureInfo) :
	{
		sf::Int32 shipIdentifier;
		sf::Int32 texture;
		packet >> shipIdentifier >> texture;

		mWorld.getShip(shipIdentifier)->setTexture(static_cast<TextureID>(texture));

	} break;

	case static_cast<int>(Server::PacketType::StartGame) :
	{
		
		mLobbyState = false;
		mClock.restart();
	} break;
	// Pickup created
#pragma endregion

#pragma region SpawnPickup
	case static_cast<int>(Server::PacketType::SpawnPickup):
	{
		sf::Int32 type;
		sf::Vector2f position;
		packet >> type >> position.x >> position.y;
		//TODO - add createPickup to World class
		mWorld.createPickup(position, static_cast<PickupID>(type));
	} break;
#pragma endregion

#pragma region UpdateScoreBoard
	case static_cast<int>(Server::PacketType::UpdateScoreBoard):
	{

		sf::Int32 shipID;
		//sf::Uint8 score;

		packet >> shipID;
		std::cout << "Recieved Killer SHIP: " + shipID << "[]" <<std::endl;
		std::cout << mScoreBoard.size() << " elements in scoreboard" << std::endl;
		for (auto it = begin(mScoreBoard); it != end(mScoreBoard); ++it)
		{
			std::cout << "EachShip in the scoreboard: " + it->shipID << std::endl;
			if (it->shipID == shipID)
			{
				it->score++;
				std::cout << "Ship new SCORE: " + it->score<< std::endl;
			}
		}

	}break;
#pragma endregion

#pragma region UpdateClientState
	case static_cast<int>(Server::PacketType::UpdateClientState):
	{
		//float currentWorldPosition;
		sf::Int32 shipCount;
		packet >> shipCount;

		//float currentViewPosition = mWorld.getViewBounds().top + mWorld.getViewBounds().height;

		
		//Used to track number of ships across state
		mLastShipCount = shipCount;
		
		float currentViewPosition = mWorld.getViewBounds().top + mWorld.getViewBounds().height;


		// Set the world's scroll compensation according to whether the view is behind or too advanced
		//mWorld.setWorldScrollCompensation(currentViewPosition / currentWorldPosition);

		for (sf::Int32 i = 0; i < shipCount; ++i)
		{
			sf::Vector2f shipPosition;
			sf::Int32 shipIdentifier;
			float shipRotation;
			sf::Int32 hitPoints;
			packet >> shipIdentifier >> shipPosition.x >> shipPosition.y >> shipRotation >> hitPoints;
			Ship* ship = mWorld.getShip(shipIdentifier);
			bool isLocalPlane = std::find(mLocalPlayerIdentifiers.begin(), mLocalPlayerIdentifiers.end(), shipIdentifier) != mLocalPlayerIdentifiers.end();
			if (ship && !isLocalPlane)
			{
				sf::Vector2f interpolatedPosition = ship->getPosition() + (shipPosition - ship->getPosition()) * 0.1f;
				ship->setPosition(interpolatedPosition);
				ship->setRotation(shipRotation);
				if (hitPoints > 0)
					ship->setHitpoints(hitPoints);
				else
				{
					sf::Packet outPacket;
					outPacket << static_cast<int>(Client::PacketType::GameEvent);
					outPacket << static_cast<int>(GameActionID::UpdateScoreboard);
					outPacket << 0.f << 0.f;
					outPacket << ship->mLastHitByID;
					mSocket.send(outPacket);
					ship->destroy();
				}
			}
		}
	} break;
#pragma endregion
	}
}

sf::Time MultiplayerGameState::now() const
{
	return mClock.getElapsedTime();
}
