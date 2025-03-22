#pragma once
#include "Condition.h"
#include "Player.h"

class PlayerCondition_HasBall : public Condition<Player>				//If Player have ball
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_EnemyProx : public Condition<Player>				//If there is an Enemy in a certain range of the ball holder
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_IsInvincible : public Condition<Player>			//If OwnerShip cooldown is > 0
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_AllyHasBall : public Condition<Player>			//If an Ally have the ball
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_EnemyHasBall : public Condition<Player>			//If an Enemy have ball
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_AllowDoPass : public Condition<Player>			//If player can do a pass
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_AllyReceip : public Condition<Player>				//If an ally is receiving the ball
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_IsNearestTeamPlayer : public Condition<Player>	//If is the nearest player of the team
{
public:
	bool OnTest(Player* owner) override;
};
