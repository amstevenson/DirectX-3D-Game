
#ifndef BulletManagerH  
#define BulletManagerH	 

#include "Thing3DAbstract.h"
#include "Object3D.h"

class BulletManager{

	public:

		// array of bullets
		struct bullet
		{
			Object3D *bulletObj;
			bool	 isVisible;
			bool     isActive;
			
			float    startX;
			float    startZ;
		};

		// structure to house the pointer array
		// of bullets
		struct bulletStorage
		{
			bullet *bullets;
			int     amountOfBullets;
		};

		// variable container
		bulletStorage bulletManager;

	public:

		BulletManager();
		~BulletManager();	

		virtual void SetBullets(std::vector<Object3D *> bulletObjects);

		int GetSize(){return bulletManager.amountOfBullets;};

		virtual void SetBulletActive(int index, float toChangeX,
									 float toChangeY, float toChangeZ, float toChangeRY);

};


#endif