#include "pch.h"
#include "RugbyScene.h"
#include "Debug.h"
#include "Player.h"
#include "Ball.h"
#include "Entity.h"
#include <random>
#include <assert.h>
#include <iostream>

void RugbyScene::OnInitialize()
{
	int width = GetWindowWidth();
	int height = GetWindowHeight();

	float playerRadius = 15.f;
	mScore[LEFT] = 0;
	mScore[RIGHT] = 0;

	mGoalAreas[LEFT] = { 0, 0, width / 10, height };
	mGoalAreas[RIGHT] = { width - width / 10, 0, width, height };

	for (int i = 0; i < 6; i++)
	{
		float yPosition = (height / 6) * (i);

		Player* pLeft = CreateEntity<Player>(playerRadius, sf::Color::Green);
		pLeft->SetPosition(playerRadius * 2, yPosition);
		pLeft->SetTeam(LEFT);
		Debug::DrawCircle(pLeft->GetPosition().x, pLeft->GetPosition().y, playerRadius * 1.25f, sf::Color::White);
		mPlayersLeft.push_back(pLeft);

		Player* pRight = CreateEntity<Player>(playerRadius, sf::Color::Red);
		pRight->SetPosition(width - playerRadius * 2, yPosition);
		pRight->SetTeam(RIGHT);
		mPlayersRight.push_back(pRight);
	}

	pBall = CreateEntity<Ball>(5, sf::Color::Yellow);
	pBall->SetPosition(width / 2, height / 2);

	ResetBall(0);
}

void RugbyScene::OnUpdate()
{
	if (!isDebuging)
	{
		isShowingIfEnemyProx = false;
		isShowingPlayerSpeed = false;
		isShowingState = false;
		isAllDebug = false;
	}
	else
	{
		std::string debugText = "Debug : D | Reset : R | Speed : V | Range : S | State : E | Pass : A | All : F";
		Debug::DrawText(GetWindowWidth() / 2, GetWindowHeight() - 15, debugText, 0.5f, 0.5f, sf::Color::White);
	}

	for (int i = 0; i < 2; i++)
	{
		sf::Color color = sf::Color::Color(82, 129, 26, 204);
		const CCDD& area = mGoalAreas[i];
		Debug::Get()->DrawFilledRectangle(area.xMin, area.yMin, area.xMax - area.xMin, area.yMax - area.yMin, color);
	}

	Debug::Get()->DrawFilledRectangle(0, 0, GetWindowWidth(), GetWindowHeight(), sf::Color::Color(82, 129, 26, 100));

	if (isShowingIfEnemyProx)
	{
		for (Player* player : mPlayersLeft)
		{
			Debug::DrawNotFilledCircle(player->GetPosition().x, player->GetPosition().y, 60, sf::Color::White);
		}

		for (Player* player : mPlayersRight)
		{
			Debug::DrawNotFilledCircle(player->GetPosition().x, player->GetPosition().y, 60, sf::Color::White);
		}
	}

	int count = 0;
	int countReceiv = 0;
	int countStayBehind = 0;
	int countDoPass = 0;

	for (Player* player : mPlayersLeft)
	{
		if (player->GetState() == Player::State::HasBall)
		{
			count++;
		}
		if (player->GetState() == Player::State::ReceipePass)
		{
			countReceiv++;
		}
		if (player->GetState() == Player::State::DoPass)
		{
			countDoPass++;
		}
	}

	for (Player* player : mPlayersRight)
	{
		if (player->GetState() == Player::State::HasBall)
		{
			count++;
		}
		if (player->GetState() == Player::State::ReceipePass)
		{
			countReceiv++;
		}
		if (player->GetState() == Player::State::DoPass)
		{
			countDoPass++;
		}
	}

	assert(count <= 1);
	assert(countReceiv <= 1);
	assert(countDoPass <= 1);

	std::string scoreText = std::to_string(mScore[LEFT]) + " | " + std::to_string(mScore[RIGHT]);
	Debug::DrawText(GetWindowWidth() / 2, 10, scoreText, 0.5f, 0.5f, sf::Color::White);

	sf::Vector2f ballPos = pBall->GetPosition();

	if (ballPos.x <= mGoalAreas[LEFT].xMax)
	{
		if (IsBallControlledBy(mPlayersRight))
		{
			mScore[RIGHT]++;
			ResetBall(0);
		}
	}
	else if (ballPos.x >= mGoalAreas[RIGHT].xMin)
	{
		if (IsBallControlledBy(mPlayersLeft))
		{
			mScore[LEFT]++;
			ResetBall(1);
		}
	}
}

bool RugbyScene::IsBallControlledBy(const std::list<Player*>& players)
{
	float closestDistance = 100;
	Player* closestPlayer = nullptr;

	for (Player* player : players)
	{
		float distance = std::hypot(player->GetPosition().x - pBall->GetPosition().x,
			player->GetPosition().y - pBall->GetPosition().y);

		if (distance < closestDistance)
		{
			closestDistance = distance;
			closestPlayer = player;
		}
	}
	return closestDistance <= 5.f;
}

void RugbyScene::OnEvent(const sf::Event& event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Right)
		{
			TrySelectEntity(mPlayersLeft, event.mouseButton.x, event.mouseButton.y);
			TrySelectEntity(mPlayersRight, event.mouseButton.x, event.mouseButton.y);
		}
		else if (event.mouseButton.button == sf::Mouse::Left && pEntitySelected)
		{
			pEntitySelected->GoToPosition(event.mouseButton.x, event.mouseButton.y, 300);
		}
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::A)
		{
			if (isDebuging)
			{
				for (Player* player : mPlayersLeft)
				{
					if (player->BallHas())
					{
						player->DropBall();
						Player* nearestTeammate = player->NearestPlayer(mPlayersLeft, player->GetPosition());
						pBall->SetNextOwner(nearestTeammate);
						if (nearestTeammate != nullptr)
						{
							sf::Vector2f teammatePos = nearestTeammate->GetPosition();
							pBall->GoToPosition(teammatePos.x, teammatePos.y, pBall->GetSpeed());
						}
					}
				}

				for (Player* player : mPlayersRight)
				{
					if (player->BallHas())
					{
						player->DropBall();
						Player* nearestTeammate = player->NearestPlayer(mPlayersRight, player->GetPosition());
						pBall->SetNextOwner(nearestTeammate);
						if (nearestTeammate != nullptr)
						{
							sf::Vector2f teammatePos = nearestTeammate->GetPosition();
							pBall->GoToPosition(teammatePos.x, teammatePos.y, pBall->GetSpeed());
						}
					}
				}
			}
		}

		if (event.key.code == sf::Keyboard::D)
		{
			isDebuging = !isDebuging;
		}

		if (event.key.code == sf::Keyboard::R && isDebuging)
		{
			ResetBall(0);
		}

		if (event.key.code == sf::Keyboard::S && isDebuging)
		{
			isShowingIfEnemyProx = !isShowingIfEnemyProx;
		}
		
		if (event.key.code == sf::Keyboard::V && isDebuging)
		{
			isShowingPlayerSpeed = !isShowingPlayerSpeed;
		}

		if (event.key.code == sf::Keyboard::E && isDebuging)
		{
			isShowingState = !isShowingState;
		}

		if (event.key.code == sf::Keyboard::F && isDebuging)
		{
			isAllDebug = !isAllDebug;
			isShowingIfEnemyProx = !isShowingIfEnemyProx;
			isShowingPlayerSpeed = !isShowingPlayerSpeed;
			isShowingState = !isShowingState;
		}
	}
}
void RugbyScene::TrySelectEntity(std::list<Player*>& players, int x, int y)
{
	for (Player* player : players)
	{
		if (player->IsInside(x, y))
		{
			pEntitySelected = player;
			break;
		}
	}
}

void RugbyScene::ResetPlayers()
{
	int width = GetWindowWidth();
	int height = GetWindowHeight();

	float playerRadius = 20.f;

	float startXLeft = playerRadius * 2;
	float startXRight = width - playerRadius * 2;

	float ySpacing = playerRadius * 5;

	int i = 0;
	for (Player* player : mPlayersLeft)
	{
		float yPosition = ySpacing * (i + 1);
		player->SetPosition(startXLeft, yPosition);
		player->GoToPosition(startXLeft, yPosition);
		player->ResetState();;
		i++;
	}

	i = 0;
	for (Player* player : mPlayersRight)
	{
		float yPosition = ySpacing * (i + 1);
		player->SetPosition(startXRight, yPosition);
		player->GoToPosition(startXRight, yPosition);
		player->ResetState();
		i++;
	}
}

void RugbyScene::ResetBall(int team)
{
	int playerBall = rand() % (mPlayersLeft.size());

	std::list<Player*>::iterator it;

	if (team == 0)
	{
		it = std::next(mPlayersLeft.begin(), playerBall);
		pBall->SetPosition((*it)->GetPosition().x, (*it)->GetPosition().y);
		(*it)->TakeBall(pBall);
	}
	else
	{
		it = std::next(mPlayersRight.begin(), playerBall);
		pBall->SetPosition((*it)->GetPosition().x, (*it)->GetPosition().y);
		(*it)->TakeBall(pBall);
	}

	ResetPlayers();
}

std::list<Player*>& RugbyScene::GetLeftTeam()
{
	return mPlayersLeft;
}

std::list<Player*>& RugbyScene::GetRightTeam()
{
	return mPlayersRight;
}

sf::Vector2f RugbyScene::GetBallPosition()
{
	return pBall->GetPosition();
}

Ball* RugbyScene::GetBall()
{
	return pBall;
}

Player* RugbyScene::GetBallHolder()
{
	for (Player* player : mPlayersLeft)
	{
		if (player->BallHas())
		{
			return player;
		}
	}

	for (Player* player : mPlayersRight)
	{
		if (player->BallHas())
		{
			return player;
		}
	}

	return nullptr;
}