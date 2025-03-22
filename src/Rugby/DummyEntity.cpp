#include "pch.h"
#include "DummyEntity.h"

#include <iostream>

void DummyEntity::OnCollision(Entity* other)
{
	std::cout << "DummyEntity::OnCollision" << std::endl;
}