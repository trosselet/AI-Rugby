#include "pch.h"
#include "Ball.h"
#include "Player.h"
#include <iostream>
#include <assert.h>

Ball::Ball(float radius, const sf::Color& color) : Entity(radius, color)
{
    this->SetTag(-5);
    SetSpeed(300.0f);
    mOwner = nullptr;
    mNextOwner = nullptr;
}

void Ball::OnCollision(Entity* collidedWith)
{
}

void Ball::OnUpdate()
{
    if (mOwner)
    {
        SetPosition(mOwner->GetPosition().x, mOwner->GetPosition().y);
    }

    if (!(mOwner) && mNextOwner)
    {
        GoToPosition(mNextOwner->GetPosition().x, mNextOwner->GetPosition().y);
    }

    assert(mOwner || mNextOwner);
}

void Ball::ResetOwner()
{
    mOwner = nullptr;
}

Player* Ball::GetOwner() const
{
    return mOwner;
}

Player* Ball::GetNextOwner() const
{
    return mNextOwner;
}

void Ball::SetNextOwner(Player* player)
{
    if (player != nullptr && this != nullptr)
    {
        mOwner = nullptr;
        mNextOwner = player;
    }
}

void Ball::SetOwner(Player* player)
{
    if (player != nullptr && this != nullptr)
    {
        mNextOwner = nullptr;
        mOwner = player;
    }
}

float Ball::GetSpeed() const
{
    return mSpeed;
}
