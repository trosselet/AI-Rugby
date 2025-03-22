#include "pch.h"
#include "PlayerAction.h"

#include "GameManager.h"
#include "RugbyScene.h"
#include "Ball.h"
#include <iostream>

void PlayerAction_Idle::Update(Player* pPlayer)
{
    RugbyScene* pScene = pPlayer->GetScene<RugbyScene>();
    const std::list<Player*>* team = nullptr;

    if (pPlayer->GetTeam() == RugbyScene::LEFT)
    {
        team = &pScene->GetLeftTeam();
    }
    else
    {
        team = &pScene->GetRightTeam();
    }

    if (team && pPlayer->IsTooCloseToTeammate(*team))
    {
        sf::Vector2f currentPos = pPlayer->GetPosition();
        pPlayer->GoToPosition(currentPos.x + 20.0f, currentPos.y + 20.0f, pPlayer->mSpeed);
    }
}


void PlayerAction_ChaseBall::Start(Player* pPlayer)
{
}

void PlayerAction_ChaseBall::Update(Player* pPlayer)
{
    RugbyScene* pScene = pPlayer->GetScene<RugbyScene>();
    Ball* ball = pScene->GetBall();
    if (ball != nullptr)
    {
        sf::Vector2f targetPos = ball->GetPosition();
        pPlayer->GoToPosition(targetPos.x, targetPos.y, pPlayer->mSpeed);

        if (pPlayer->IsColliding(ball) && pPlayer->GetTimeBeforePickingUp() <= 0)
        {
            pPlayer->TakeBall(ball);
        }
        else
        {
            const std::list<Player*>& opposingTeam = (pPlayer->GetTeam() == RugbyScene::LEFT) ? pScene->GetRightTeam() : pScene->GetLeftTeam();
             
            for (Player* opponent : opposingTeam)
            {
                if (opponent->BallHas() && pPlayer->IsColliding(opponent) && opponent->mOwnershipCooldown <= 0 && pPlayer->mTimeBeforePickingUp <= 0)
                {
                    pPlayer->TakeBall(opponent->DropBall());
                    break;
                }
            }
        }
    }
}

void PlayerAction_ChaseBall::End(Player* pPlayer)
{
}

void PlayerAction_HasBall::Start(Player* pPlayer)
{
    pPlayer->SetOwnerShipCD(2.0f);
    pPlayer->SetSpeed(pPlayer->mDefaultSpeed * 1.25f);
    pPlayer->mSpeedBoostDuration = 2.0f;
}

void PlayerAction_HasBall::Update(Player* pPlayer)
{
    RugbyScene* pScene = pPlayer->GetScene<RugbyScene>();

    pPlayer->UpdateOwnershipCooldown(GameManager::Get()->GetDeltaTime());

    if (pPlayer->mSpeedBoostDuration > 0)
    {
        pPlayer->mSpeedBoostDuration -= GameManager::Get()->GetDeltaTime();
        if (pPlayer->mSpeedBoostDuration <= 0)
        {
            pPlayer->SetSpeed(pPlayer->mDefaultSpeed);
        }
    }

    int team = pPlayer->GetTeam();
    const CCDD* goal = nullptr;

    if (team == RugbyScene::LEFT)
    {
        goal = &pScene->mGoalAreas[RugbyScene::RIGHT];
    }
    else
    {
        goal = &pScene->mGoalAreas[RugbyScene::LEFT];
    }

    if (goal != nullptr)
    {
        float goalX = (goal->xMin + goal->xMax) / 2;
        float goalY = (goal->yMin + goal->yMax) / 2;

        sf::Vector2f targetPos(goalX, goalY);

        std::list<Player*> opponents;
        if (team == RugbyScene::LEFT)
        {
            opponents = pScene->GetRightTeam();
        }
        else
        {
            opponents = pScene->GetLeftTeam();
        }

        sf::Vector2f avoidanceVector(0.f, 0.f);
        float avoidanceRadius = 200.f;
        float avoidanceStrength = 75.0f;

        for (Player* opponent : opponents)
        {
            sf::Vector2f toOpponent = pPlayer->GetPosition() - opponent->GetPosition();
            float distance = std::sqrt(toOpponent.x * toOpponent.x + toOpponent.y * toOpponent.y);

            if (distance < avoidanceRadius)
            {
                if (distance > 0.f)
                {
                    toOpponent /= distance;
                }
                avoidanceVector += toOpponent * (avoidanceRadius - distance) / avoidanceRadius;
            }
        }

        sf::Vector2f moveDirection = targetPos - pPlayer->GetPosition();

        float moveLength = std::sqrt(moveDirection.x * moveDirection.x + moveDirection.y * moveDirection.y);
        if (moveLength > 0.f)
        {
            moveDirection /= moveLength;
        }

        sf::Vector2f finalDirection = moveDirection;
        if (avoidanceRadius > 0)
        {
            finalDirection += avoidanceVector * (avoidanceStrength / avoidanceRadius);
        }

        float finalLength = std::sqrt(finalDirection.x * finalDirection.x + finalDirection.y * finalDirection.y);
        if (finalLength > 0.f)
        {
            finalDirection /= finalLength;
        }

        sf::Vector2f finalTarget = pPlayer->GetPosition() + finalDirection * 10.f;

        pPlayer->GoToPosition(finalTarget.x, finalTarget.y, pPlayer->mSpeed);
    }
}



void PlayerAction_HasBall::End(Player* pPlayer)
{
    pPlayer->SetOwnerShipCD(2.0f);
    pPlayer->SetSpeed(pPlayer->mDefaultSpeed);
}

void PlayerAction_StayBehind::Start(Player* pPlayer)
{
    pPlayer->GoToPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y);
}

void PlayerAction_StayBehind::Update(Player* pPlayer)
{
    RugbyScene* pScene = pPlayer->GetScene<RugbyScene>();
    Player* ballHolder = nullptr;
    const std::list<Player*>* team = nullptr;

    if (pPlayer->GetTeam() == RugbyScene::LEFT)
    {
        team = &pScene->GetLeftTeam();
    }
    else
    {
        team = &pScene->GetRightTeam();
    }

    for (Player* teammate : *team)
    {
        if (teammate->BallHas())
        {
            ballHolder = teammate;
            break;
        }
    }

    if (ballHolder != nullptr)
    {
        sf::Vector2f ballHolderPos = ballHolder->GetPosition();
        sf::Vector2f delta = pPlayer->GetPosition() - ballHolderPos;

        float distanceToKeep = 40.0f;
        float currentDistance = delta.x * delta.x + delta.y * delta.y;

        float targetX = ballHolderPos.x;
        float offsetX = 150.0f;
        if (pPlayer->GetTeam() == RugbyScene::LEFT)
        {
            targetX -= offsetX;
        }
        else
        {
            targetX += offsetX;
        }

        float offsetY = 50.0f;
        int playerIndex = std::distance(team->begin(), std::find(team->begin(), team->end(), pPlayer));
        float targetY = ballHolderPos.y + playerIndex * offsetY;

        if (currentDistance > distanceToKeep * distanceToKeep)
        {
            pPlayer->GoToPosition(targetX, targetY, pPlayer->mSpeed);
        }
    }
}

void PlayerAction_StayBehind::End(Player* pPlayer)
{
    pPlayer->GoToPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y);
}

void PlayerAction_DoPass::Start(Player* pPlayer)
{
    pPlayer->SetTimeBeforePickingUp(1.0f);
}

void PlayerAction_DoPass::Update(Player* pPlayer)
{
    RugbyScene* pScene = pPlayer->GetScene<RugbyScene>();
    mTimeBeforPickUpBall -= GameManager::Get()->GetDeltaTime();

    for (Player* player : pScene->GetLeftTeam())
    {
        if (player->BallHas())
        {
            player->DropBall();

            Player* nearestTeammate = player->NearestPlayer(pScene->GetLeftTeam(), player->GetPosition());
            if (nearestTeammate != nullptr)
            {
                if (nearestTeammate->GetPosition().x < player->GetPosition().x)
                {
                    pScene->GetBall()->SetNextOwner(nearestTeammate);
                }
                else
                {
                    pScene->GetBall()->SetNextOwner(nullptr);
                }
            }
        }
    }

    for (Player* player : pScene->GetRightTeam())
    {
        if (player->BallHas())
        {
            player->DropBall();

            Player* nearestTeammate = player->NearestPlayer(pScene->GetRightTeam(), player->GetPosition());
            if (nearestTeammate != nullptr)
            {
                if (nearestTeammate->GetPosition().x > player->GetPosition().x)
                {
                    pScene->GetBall()->SetNextOwner(nearestTeammate);
                }
                else
                {
                    pScene->GetBall()->SetNextOwner(nullptr);
                }
            }
        }
    }
}


void PlayerAction_DoPass::End(Player* pPlayer)
{
}

void PlayerAction_ReceipePass::Start(Player* pPlayer)
{
}

void PlayerAction_ReceipePass::Update(Player* pPlayer)
{
    RugbyScene* pScene = pPlayer->GetScene<RugbyScene>();

    sf::Vector2f ballPos = pScene->GetBallPosition();
    pPlayer->GoToPosition(ballPos.x, ballPos.y, pPlayer->mSpeed);

    if (pPlayer->IsColliding(pScene->GetBall()))
    {
        pPlayer->TakeBall(pScene->GetBall());
    }
}

void PlayerAction_ReceipePass::End(Player* pPlayer)
{
}