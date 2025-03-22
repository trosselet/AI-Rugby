#pragma once

#include "Action.h"
#include "Player.h"

class PlayerAction_Idle : public Action<Player>					//Player is in Idle State
{
public:
	void Start(Player* pPlayer) override {}
	void Update(Player* pPlayer) override;
	void End(Player* pPlayer) override {}
};

class PlayerAction_ChaseBall : public Action<Player>			//Player is in ChaseBall State
{
public:
	void Start(Player* pPlayer) override;
	void Update(Player* pPlayer) override;
	void End(Player* pPlayer) override;
};

class PlayerAction_HasBall : public Action<Player>				//Player is in HasBall State
{
private:
	float mOwnership = 4.0f;

public:
	void Start(Player* pPlayer) override;
	void Update(Player* pPlayer) override;
	void End(Player* pPlayer) override;
};

class PlayerAction_StayBehind : public Action<Player>			//Player is in StayBehind State
{

public:

	void Start(Player* pPlayer) override;
	void Update(Player* pPlayer) override;
	void End(Player* pPlayer) override;
};

class PlayerAction_DoPass : public Action<Player>				//Player is in DoPass State
{

	float mTimeBeforPickUpBall = 1.0f;
public:
	void Start(Player* pPlayer) override;
	void Update(Player* pPlayer) override;
	void End(Player* pPlayer) override;
};

class PlayerAction_ReceipePass : public Action<Player>			//Player is in ReceivePass State
{
public:
	void Start(Player* pPlayer) override;
	void Update(Player* pPlayer) override;
	void End(Player* pPlayer) override;
};