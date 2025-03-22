#pragma once

#include "Entity.h"
#include "StateMachine.h"
#include <vector>

class Ball;

class Player : public Entity
{
public:
	StateMachine<Player> mStateMachine;

	enum State
	{
		Idle,
		HasBall,
		ChaseBall,
		StayBehind,
		DoPass,
		ReceipePass,

		Count
	};

public:
	Player(float radius, const sf::Color& color);											//Constructor
	void OnUpdate() override;																//Update
	void OnCollision(Entity* collidedWith) override;										//Collision with 


	void TakeBall(Ball* ball);																//Take ball from the ground or a Player
	Ball* DropBall();																		//Drop the ball to a Player or do a pass
	bool BallHas() const;																	//Return true if Player have the ball


	Ball* GetBall() const { return mBall; }													//Return tha ball
	float GetOnwershipCD() const { return mOwnershipCooldown; }								//Return the invincible cooldown
	void SetOwnerShipCD(float timer) { mOwnershipCooldown = timer; }						//Set the invincible cooldown

	float GetTimeBeforePickingUp() const { return mTimeBeforePickingUp; }					//Return the time before being allowed to 
																							//have the ball
	void SetTimeBeforePickingUp(float timer) { mTimeBeforePickingUp = timer; }				//Set the time before being allowed to 
																							//have the ball

	void SetTeam(int team) { mTeam = team; }												//Set a team to the Player									
	int GetTeam() const { return mTeam; }													//Return the team of the Player
	int GetState() { return mStateMachine.GetCurrentState(); }								//Return Current State


	Player* NearestPlayer(const std::list<Player*>& players, const sf::Vector2f& position);	//Return the nearest Player
	bool IsTooCloseToTeammate(const std::list<Player*>& team);								//Return true if is too close to a mate
	const char* GetStateName(State state) const;											//Return a string of the state
	void ResetState();																		//Reset basic stats and setState to idle

	void UpdateOwnershipCooldown(float deltaTime);											//Decrease the invincibility

public:
	float mSpeedBoostDuration;
	float mDefaultSpeed;

private:
	Ball* mBall = nullptr;
	float mOwnershipCooldown;
	float mTimeBeforePickingUp;
	int mTeam;

private:
	friend class PlayerAction_Idle;
	friend class PlayerAction_ChaseBall;
	friend class PlayerAction_HasBall;
	friend class PlayerAction_StayBehind;
	friend class PlayerAction_DoPass;
	friend class PlayerAction_ReceipePass;
};