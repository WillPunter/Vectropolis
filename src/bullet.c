/*
	bullet.c
	
	This file includes code for initialising and updating bullets.
*/

//bullet functions
void initialiseBullet(double x, double y, double z, double xAngle, double yAngle, double zAngle){
	//search for empty slot
	int i;
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(bullets[i].alive == 0){
			bullets[i].alive = 1;
			bullets[i].position.x = x;
			bullets[i].position.y = y;
			bullets[i].position.z = z;
			bullets[i].size.x = 0.5;
			bullets[i].size.y = 0.5;
			bullets[i].size.z = 5;
			bullets[i].rotation.x = xAngle;
			bullets[i].rotation.y = yAngle;
			bullets[i].rotation.z = zAngle;
			bullets[i].model.mesh = &mesh_cube;
			bullets[i].model.scale = bullets[i].size;
			bullets[i].model.colour.red = 255;
			bullets[i].model.colour.green = 255;
			bullets[i].model.colour.blue = 0;
			bullets[i].updateProc = &updateBullet;
			break;
		};
	};
};

//update bullet
void updateBullet(Entity * bullet){
	//move
	Matrix4x4f rotation = createRotationMatrix(bullet->rotation);
	Vector4f moveDir = {0};
	moveDir.z = 1;
	moveDir = matrixVectorProduct(rotation, moveDir);
	
	bullet->velocity = scalarProduct(moveDir, 300);
	bullet->position = addVectors(bullet->position, scalarProduct(bullet->velocity, gameClock.deltaTime));
	
	if(vectorMagnitude(subtractVectors(bullet->position, playerInstance.position)) > 500){
		bullet->alive = 0;
	};
	
	bullet->model.rotation = bullet->rotation;
	
	//check for collision with terrain
	Entity * collidingTerrain;
	if(collidingTerrain = checkTerrainCollision(bullet)){
		//check if building
		if(collidingTerrain && collidingTerrain->type == BUILDING){
			//destroy building
			collidingTerrain->hp = 0;
		};
		
		//destroy self
		bullet->alive = 0;
	};
};