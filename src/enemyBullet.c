/*
	enemyBullet.c
	
	This file includes code for initialising and updating enemy bullets
*/

//enemy bullet functions  
void initialiseEnemyBullet(double x, double y, double z, double xAngle, double yAngle, double zAngle, double width, double height, double length, double speed){
	//search for empty slot
	int i;
	
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		//check if slot is empty
		if(enemyBullets[i].alive == 0){
			//slot found
			enemyBullets[i].alive = 1;
			enemyBullets[i].position.x = x;
			enemyBullets[i].position.y = y;
			enemyBullets[i].position.z = z;
			enemyBullets[i].rotation.x = xAngle;
			enemyBullets[i].rotation.y = yAngle;
			enemyBullets[i].rotation.z = zAngle;
			enemyBullets[i].size.x = width;
			enemyBullets[i].size.y = height;
			enemyBullets[i].size.z = length;
			enemyBullets[i].speed = speed;
			enemyBullets[i].model.mesh = &mesh_cube;
			enemyBullets[i].model.scale = enemyBullets[i].size;
			enemyBullets[i].model.colour.red = 255;
			enemyBullets[i].model.colour.green = 100;
			enemyBullets[i].model.colour.blue = 0;
			enemyBullets[i].updateProc = &updateEnemyBullet;
			playSound(&sound_enemyShoot);
			break;
		};
	};
};

//update enemy bullet
void updateEnemyBullet(Entity * enemyBullet){
	//move
	Matrix4x4f rotation = createRotationMatrix(enemyBullet->rotation);
	Vector4f moveDir = {0};
	moveDir.z = 1;
	moveDir = matrixVectorProduct(rotation, moveDir);
	
	enemyBullet->velocity = scalarProduct(moveDir, enemyBullet->speed);
	enemyBullet->position = addVectors(enemyBullet->position, scalarProduct(enemyBullet->velocity, gameClock.deltaTime));
	
	//check for collision with terrain
	Entity * collidingTerrain;
	if(collidingTerrain = checkTerrainCollision(enemyBullet)){
		//check if building
		if(collidingTerrain && collidingTerrain->type == BUILDING){
			//destroy building
			collidingTerrain->hp = 0;
		};
		
		//destroy self
		enemyBullet->alive = 0;
	};
	
	//check if out of bounds
	if(fabs(enemyBullet->position.x) > 1000 || fabs(enemyBullet->position.y) > 1000 || fabs(enemyBullet->position.z) > 1000){
		enemyBullet->alive = 0;
	};
	
	enemyBullet->model.rotation = enemyBullet->rotation;
};