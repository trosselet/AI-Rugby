#pragma once

#include "GameManager.h"
#include <list>

class Entity;
class Player;
class Ball;

namespace sf
{
	class Event;
}

struct CCDD
{
	int xMin;
	int yMin;
	int xMax;
	int yMax;
};

class RugbyScene : public Scene
{
public:
	enum Side
	{
		LEFT,
		RIGHT,
		BALL
	};
	enum Tag
	{
		PLAYER,
	};

private:
	Entity* pEntitySelected;
	std::list<Player*> mPlayersLeft;
	std::list<Player*> mPlayersRight;
	Ball* pBall;
	int mScore[2];

	bool isDebuging = false;
	bool isShowingIfEnemyProx = false;
	bool isShowingPlayerSpeed = false;
	bool isShowingState = false;
	bool isAllDebug = false;

public:
	CCDD mGoalAreas[2];

public:
	void OnInitialize() override;									//Initialize
	void OnEvent(const sf::Event& event) override;					//Event
	void OnUpdate() override;										//Update
	std::list<Player*>& GetLeftTeam();								//Return List of the Left Team
	std::list<Player*>& GetRightTeam();								//Return List of the Right Team
	sf::Vector2f GetBallPosition();									//Return Position of the ball
	Ball* GetBall();												//Return the Ball object
	Player* GetBallHolder();										//Return the Player with the ball
	bool GetShowingState() { return isShowingState; }				//Return the Debug visualisation of states
	bool GetShowingSpeed() { return isShowingPlayerSpeed; }			//Return the Debug visualisation of speed
	float GetScreenWidth() { return GetWindowWidth(); }				//Return the WindowWidth
	float GetScreenHeight() { return GetWindowHeight(); }			//Return the WindowHeight

private:
	bool IsBallControlledBy(const std::list<Player*>& players);		//Return true if Ball is controlled by the team in attribute
	void TrySelectEntity(std::list<Player*>& players, int x, int y);
	void ResetPlayers();											//Reset Player position and stats
	void ResetBall(int team);										//Reset Ball position and stats
};