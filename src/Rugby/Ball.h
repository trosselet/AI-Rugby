#pragma once
#include "Entity.h"

class Player;

class Ball : public Entity
{
public:
	Ball(float radius, const sf::Color& color);				//Constructor
	void OnUpdate() override;								//Update
	void OnCollision(Entity* collidedWith) override;		//Collision with
	void ResetOwner();										//Reset the Owner
	Player* GetOwner() const;								//Return the Ball owner
	Player* GetNextOwner() const;							//Return the Player which will get the ball after a pass
	void SetNextOwner(Player* player);						//Set the Player which will get the ball after a pass
	void SetOwner(Player* player);							//Set the Ball owner
	float GetSpeed() const;									//Return the mSpeed of the Ball

private:
	Player* mOwner;
	Player* mNextOwner;
};

