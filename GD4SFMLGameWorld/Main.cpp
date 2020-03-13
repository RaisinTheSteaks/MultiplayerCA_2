/*
Charlie Duff
D00183790
*/
#include <stdexcept>
#include <iostream>
#include "Application.hpp"

int main()
{
	try 
	{
		Application theAmazingGame;
		theAmazingGame.run();
	}
	catch (std::exception& e)
	{
		std::cout << "\n EXCEPTION" << e.what() << std::endl;
	}
}