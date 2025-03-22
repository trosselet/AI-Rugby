#include "pch.h"
#include <iostream>

#include "PlayerAction.h"
#include "PlayerCondition.h"

#include "RugbyScene.h"
#include "Player.h"
#include "Ball.h"
#include "Debug.h"

Player::Player(float radius, const sf::Color& color) : Entity(radius, color), mStateMachine(this, State::Count)
{
    mDefaultSpeed = 100.0f;
    this->mBall = nullptr;
    SetSpeed(mDefaultSpeed);
    mOwnershipCooldown = 6.0f;
    mTimeBeforePickingUp = 1.0f;
    mSpeedBoostDuration = 0.0f;
    mTeam = -1;

    SetTag(RugbyScene::Tag::PLAYER);

    //IDLE
    {
        Behaviour<Player>* pIdle = mStateMachine.CreateBehaviour(State::Idle);
        pIdle->AddAction(new PlayerAction_Idle());

        //-> HasBall
        {
            auto transition = pIdle->CreateTransition(State::HasBall);

			auto condition = transition->AddCondition<PlayerCondition_HasBall>();
        }

        // -> ChaseBall
        {
            auto transition = pIdle->CreateTransition(State::ChaseBall);

			auto condition = transition->AddCondition<PlayerCondition_AllyHasBall>();
			condition->expected = false;

            auto condition2 = transition->AddCondition<PlayerCondition_HasBall>();
            condition2->expected = false;

            auto condition3 = transition->AddCondition<PlayerCondition_AllyReceip>();
            condition3->expected = false;
        }
        
		// -> STAYBEHIND
		{
			auto transition = pIdle->CreateTransition(State::StayBehind);

            auto condition1 = transition->AddCondition<PlayerCondition_AllyHasBall>();

			auto condition2 = transition->AddCondition<PlayerCondition_HasBall>();
			condition2->expected = false;
		}
    }
    
    //HasBall
    {
        Behaviour<Player>* pHasBall = mStateMachine.CreateBehaviour(State::HasBall);
        pHasBall->AddAction(new PlayerAction_HasBall());

		//-> IDLE
		{
			auto transition = pHasBall->CreateTransition(State::Idle);

			auto condition1 = transition->AddCondition<PlayerCondition_HasBall>();
			condition1->expected = false;
		}
        
        //-> DoPass
        {
            auto transition = pHasBall->CreateTransition(State::DoPass);

            auto condition1 = transition->AddCondition<PlayerCondition_HasBall>();

            auto condition2 = transition->AddCondition<PlayerCondition_EnemyProx>();

            auto condition3 = transition->AddCondition<PlayerCondition_AllowDoPass>();
        }
    }
    
	//ChaseBall
	{
		Behaviour<Player>* pChaseBall = mStateMachine.CreateBehaviour(State::ChaseBall);
		pChaseBall->AddAction(new PlayerAction_ChaseBall());

		//-> IDLE
		{
			auto transition = pChaseBall->CreateTransition(State::Idle);

			auto condition = transition->AddCondition<PlayerCondition_AllyHasBall>();
		}

		//-> HasBall
		{
			auto transition = pChaseBall->CreateTransition(State::HasBall);

			auto condition = transition->AddCondition<PlayerCondition_HasBall>();
		}
	}

	//StayBehind
	{
		Behaviour<Player>* pStayBehind = mStateMachine.CreateBehaviour(State::StayBehind);
		pStayBehind->AddAction(new PlayerAction_StayBehind());

		
		//-> RececipePass
		{
			auto transition = pStayBehind->CreateTransition(State::ReceipePass);

			auto condition1 = transition->AddCondition<PlayerCondition_AllyHasBall>();
			condition1->expected = false;

			auto condition2 = transition->AddCondition<PlayerCondition_IsNearestTeamPlayer>();
		}

        //-> IDLE
        {
            auto transition = pStayBehind->CreateTransition(State::Idle);

            auto condition = transition->AddCondition<PlayerCondition_AllyHasBall>();
            condition->expected = false;
        }
	}

	//DoPass
    {
        Behaviour<Player>* pDoPass = mStateMachine.CreateBehaviour(State::DoPass);
        pDoPass->AddAction(new PlayerAction_DoPass());

        //-> IDLE
        {
            auto transition = pDoPass->CreateTransition(State::Idle);

			auto condition1 = transition->AddCondition<PlayerCondition_HasBall>();
			condition1->expected = false;
        }
    }

	//ReceipePass
	{
		Behaviour<Player>* pReceipePass = mStateMachine.CreateBehaviour(State::ReceipePass);
		pReceipePass->AddAction(new PlayerAction_ReceipePass());

		//-> HasBall
		{
			auto transition = pReceipePass->CreateTransition(State::HasBall);

			auto condition = transition->AddCondition<PlayerCondition_HasBall>();
		}
	}

    mStateMachine.SetState(State::Idle);
}

void Player::OnUpdate()
{
    RugbyScene* pScene = this->GetScene<RugbyScene>();

    float screenWidth = pScene->GetScreenWidth();
    float screenHeight = pScene->GetScreenHeight();

    float constrainedX = std::clamp(this->GetPosition().x, 15.0f, screenWidth - 15.0f);
    float constrainedY = std::clamp(this->GetPosition().y, 15.0f, screenHeight - 15.0f);

    SetPosition(constrainedX, constrainedY);

    if (mTimeBeforePickingUp > 0.0f)
    {
        mTimeBeforePickingUp -= GameManager::Get()->GetDeltaTime();;
        if (mTimeBeforePickingUp < 0.0f)
        {
            mTimeBeforePickingUp = 0.0f;
        }
    }

    if (this->BallHas())
    {
        std::cout << "Cooldown: " << mOwnershipCooldown << std::endl;
    }

    const sf::Vector2f& position = GetPosition();
    const char* stateName = GetStateName((Player::State)mStateMachine.GetCurrentState());
    std::string playerSpeed = "Speed : " + std::to_string(mSpeed);

    if (pScene->GetShowingState())
    {
        Debug::DrawText(position.x, position.y - 50, stateName, 0.5f, 0.5f, sf::Color::White);
    }

    if (pScene->GetShowingSpeed())
    {
        Debug::DrawText(position.x, position.y + 50, playerSpeed, 0.5f, 0.5f, sf::Color::Magenta);
    }

    mStateMachine.Update();
}

void Player::OnCollision(Entity* collidedWith)
{
    
    if (!(collidedWith->IsTag(-5)))
    {
        Player* player = dynamic_cast<Player*>(collidedWith);

        sf::Vector2f delta = GetPosition() - collidedWith->GetPosition();
        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        float minDistance = GetRadius() + collidedWith->GetRadius();

        if (distance < minDistance)
        {
            if (distance > 0.f)
                delta /= distance;

            sf::Vector2f correction = delta * (minDistance - distance) / 2.f;
            SetPosition(GetPosition().x + correction.x, GetPosition().y + correction.y);

            collidedWith->SetPosition(collidedWith->GetPosition().x - correction.x, collidedWith->GetPosition().y - correction.y);
        }
        if (this->IsColliding(player))
        {
            if (player->BallHas() && player->mOwnershipCooldown <= 0 && player->mTeam != this->mTeam && mTimeBeforePickingUp <= 0)
            {
                this->TakeBall(player->DropBall());
            }
        }
    }
    else
    {
        Ball* ball = dynamic_cast<Ball*>(collidedWith);
        if (this->BallHas() && mTimeBeforePickingUp <= 0)
        {
            this->TakeBall(ball);
        }
    }
}


void Player::TakeBall(Ball* ball)
{
    this->mBall = ball;
    this->mBall->SetOwner(this);
}

Ball* Player::DropBall()
{
    Ball* temp = this->mBall;
    mTimeBeforePickingUp = 1.0f;
    this->mBall = nullptr;
    return temp;
}

bool Player::BallHas() const
{
    return this->mBall != nullptr && this->mBall->GetOwner() == this;
}

Player* Player::NearestPlayer(const std::list<Player*>& players, const sf::Vector2f& position)
{
    Player* nearestPlayer = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Player* player : players)
    {
        if (player != this)
        {
            if (player->GetTimeBeforePickingUp() <= 0)
            {
                sf::Vector2f delta = player->GetPosition() - position;
                float distance = delta.x * delta.x + delta.y * delta.y;

                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestPlayer = player;
                }
            }
        }
    }

    return nearestPlayer;
}

const char* Player::GetStateName(State state) const
{
    switch (state)
    {
    case Idle: return "Idle";
    case HasBall: return "HasBall";
    case ChaseBall: return "ChaseBall";
    case StayBehind: return "StayBehind";
    case DoPass: return "DoPass";
    case ReceipePass: return "ReceipePass";
    default: return "Unknown";
    }
}

void Player::ResetState()
{
    mTimeBeforePickingUp = 0.0f;
    mOwnershipCooldown = 0.0f;
    mBall = nullptr;
    mStateMachine.SetState(State::Idle);
}

bool Player::IsTooCloseToTeammate(const std::list<Player*>& team)
{
    for (Player* teammate : team)
    {
        if (teammate == this) continue;
        sf::Vector2f delta = this->GetPosition() - teammate->GetPosition();
        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (distance < 30.0f)
            return true;
    }
    return false;
}

void Player::UpdateOwnershipCooldown(float deltaTime)
{
    if (mOwnershipCooldown > 0.0f)
    {
        mOwnershipCooldown -= deltaTime;
        if (mOwnershipCooldown < 0.0f)
        {
            mOwnershipCooldown = 0.0f;
        }
    }
}