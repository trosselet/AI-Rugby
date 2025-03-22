#include "pch.h"
#include "PlayerCondition.h"

#include <iostream>
#include "RugbyScene.h"

bool PlayerCondition_HasBall::OnTest(Player* owner)
{
    RugbyScene* pScene = owner->GetScene<RugbyScene>();
    if (owner == pScene->GetBallHolder())
    {
        return true;
    }
    return false;
}

bool PlayerCondition_EnemyProx::OnTest(Player* owner)
{
    RugbyScene* pScene = owner->GetScene<RugbyScene>();
    Player* nearestEnemy = nullptr;

    if (owner->GetTeam() == RugbyScene::LEFT)
    {
        nearestEnemy = owner->NearestPlayer(pScene->GetRightTeam(), owner->GetPosition());
    }
    else if (owner->GetTeam() == RugbyScene::RIGHT)
    {
        nearestEnemy = owner->NearestPlayer(pScene->GetLeftTeam(), owner->GetPosition());
    }

    if (nearestEnemy != nullptr)
    {
        sf::Vector2f enemyPos = nearestEnemy->GetPosition();

        bool isClose = std::abs(owner->GetPosition().x - enemyPos.x) < 60 && std::abs(owner->GetPosition().y - enemyPos.y) < 60;
            
        if (isClose)
        {
            if (owner->GetTeam() == RugbyScene::LEFT)
            {
                if (enemyPos.x > owner->GetPosition().x)
                {
                    return true;
                }
            }
            else if (owner->GetTeam() == RugbyScene::RIGHT)
            {
                if (enemyPos.x < owner->GetPosition().x)
                {
                    return true;
                }
            }
        }
    }

    return false;
}


bool PlayerCondition_IsInvincible::OnTest(Player* owner)
{
    if (owner->GetOnwershipCD() >= 0)
    {
        return true;
    }
    return false;
}

bool PlayerCondition_AllyHasBall::OnTest(Player* owner)
{
    RugbyScene* pScene = owner->GetScene<RugbyScene>();

    if (owner->GetTeam() == RugbyScene::LEFT)
    {
        for (auto& player : pScene->GetLeftTeam())
        {
            if (player == pScene->GetBallHolder())
            {
                return true;
            }
        }
    }

    if (owner->GetTeam() == RugbyScene::RIGHT)
    {
        for (auto& player : pScene->GetRightTeam())
        {
            if (player == pScene->GetBallHolder())
            {
                return true;
            }
        }
    }
    return false;
}


bool PlayerCondition_EnemyHasBall::OnTest(Player* owner)
{
    RugbyScene* pScene = owner->GetScene<RugbyScene>();

    if (owner->GetTeam() == RugbyScene::LEFT)
    {
        for (auto& player : pScene->GetRightTeam())
        {
            if (owner->BallHas())
            {
                return true;
            }
        }
    }
    else if (owner->GetTeam() == RugbyScene::RIGHT)
    {
        for (auto& player : pScene->GetLeftTeam())
        {
            if (owner->BallHas())
            {
                return true;
            }
        }
    }
    return false;
}


bool PlayerCondition_AllowDoPass::OnTest(Player* owner)
{
    if (owner->BallHas() && owner->GetOnwershipCD() <= 0)
    {
        return true;
    }

    return false;
}

bool PlayerCondition_AllyReceip::OnTest(Player* owner)
{
    RugbyScene* pScene = owner->GetScene<RugbyScene>();

    if (owner->GetTeam() == RugbyScene::LEFT)
    {
        for (auto& player : pScene->GetLeftTeam())
        {
            if (player->GetState() == Player::ReceipePass)
            {
                return true;
            }
        }
    }
    else if (owner->GetTeam() == RugbyScene::RIGHT)
    {
        for (auto& player : pScene->GetRightTeam())
        {
            if (player->GetState() == Player::ReceipePass)
            {
                return true;
            }
        }
    }
    return false;
}

bool PlayerCondition_IsNearestTeamPlayer::OnTest(Player* owner)
{
    RugbyScene* pScene = owner->GetScene<RugbyScene>();

    if (owner->GetTeam() == RugbyScene::LEFT)
    {
        for (auto& player : pScene->GetLeftTeam())
        {
            if (player->BallHas())
            {
                if (player->NearestPlayer(pScene->GetLeftTeam(), player->GetPosition()) == owner)
                {
                    return true;
                }
            }
        }
    }
    else if (owner->GetTeam() == RugbyScene::RIGHT)
    {
        for (auto& player : pScene->GetRightTeam())
        {
            if (player->BallHas())
            {
                if (player->NearestPlayer(pScene->GetRightTeam(), player->GetPosition()) == owner)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
