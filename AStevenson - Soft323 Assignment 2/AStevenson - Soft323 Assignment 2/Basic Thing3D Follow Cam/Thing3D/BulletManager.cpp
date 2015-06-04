
//******************************************************************************//
// This class manages bullets that are loaded before runtime.					//
// Bullets are given a start x and z position and have states toggled			//
//******************************************************************************//

#include "BulletManager.h"

BulletManager::BulletManager()
{

}

BulletManager::~BulletManager()
{
	delete bulletManager.bullets;
}

void BulletManager::SetBullets(std::vector<Object3D *> bulletObjects)
{
	// add the objects to the structure and make sure that all of them
	// are not active
	bulletManager.bullets = new bullet[bulletObjects.size()];
	bulletManager.amountOfBullets = bulletObjects.size();

	for(int i = 0; i < bulletObjects.size(); i++)
	{
		bulletManager.bullets[i].bulletObj = bulletObjects[i];
		bulletManager.bullets[i].isActive  = false;
	}
}

void BulletManager::SetBulletActive(int index, float toChangeX, float toChangeY, float toChangeZ, float toChangeRY)
{
	Object3D *toChangeObj = bulletManager.bullets[index].bulletObj;

	// change the information of the object
	toChangeObj->x  = toChangeX;
	toChangeObj->y  = toChangeY + 1.5f; // + 1.5 for cyborg difference
										// I should be using vMin - vMax (Frank Luna) for the Y axis of the
										// Cyborg here, to find the middle.
	toChangeObj->z  = toChangeZ;
	toChangeObj->ry = toChangeRY + 80;  // + 80 to place the bullet straight infront

	// store the starting x and z values, in order to disable the rendering process
	// if a bullet reaches a certain distance
	bulletManager.bullets[index].startX = toChangeX;
	bulletManager.bullets[index].startZ = toChangeZ;

	// change visibility
	bulletManager.bullets[index].isVisible = true;
	bulletManager.bullets[index].isActive  = true;

	// set speed
	toChangeObj->speed = 1.2;
}
