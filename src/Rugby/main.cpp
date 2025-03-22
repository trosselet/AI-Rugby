#include "pch.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "GameManager.h"
#include "RugbyScene.h"

#include <cstdlib>
#include <crtdbg.h>

int main() 
{
    GameManager* pInstance = GameManager::Get();

	pInstance->CreateWindow(1280, 720, "Rugby 22");
	
	pInstance->LaunchScene<RugbyScene>();

	return 0;
}