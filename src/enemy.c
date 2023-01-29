/*
	enemy.c
	
	This file includes functions for initialising and updating enemies, including all of the various types.
	There are also various other functions used inside the update functions for the various enemy types.
*/

//enemy functions
void initialiseEnemy(double x, double y, double z, int type){
	//search for empty slot
	int i;
	
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		//check if slot is empty
		if(enemies[i].alive == 0){
			//set properties
			numberOfEnemies += 1;
			enemies[i].alive = 1;
			enemies[i].position.x = x;
			enemies[i].position.y = y;
			enemies[i].position.z = z;
			enemies[i].size.x = 4;
			enemies[i].size.y = 2;
			enemies[i].size.z = 6;
			enemies[i].model.scale.x = 1;
			enemies[i].model.scale.y = 1;
			enemies[i].model.scale.z = 1;
			enemies[i].reloadTime = 5;
			enemies[i].reloadTimer = enemies[i].reloadTime;
			enemies[i].speed = 10;
			enemies[i].type = type;
			enemies[i].model.colour.red = 255;
			enemies[i].model.colour.green = 0;
			enemies[i].model.colour.blue = 0;
			
			//check type
			switch(type){
				//bomber
				case BOMBER:{
					enemies[i].model.mesh = &mesh_enemy;
					enemies[i].updateProc = &updateBomber;
					enemies[i].speed = 15;
					enemies[i].maxHp = 10;
					enemies[i].angularSpeed = PI / 2.f;
					enemies[i].range = 120;
					enemies[i].altitude = 100;
					break;
				};
				
				//chaser
				case CHASER:{
					enemies[i].model.mesh = &mesh_chaser;
					enemies[i].updateProc = &updateChaser;
					enemies[i].speed = 25.f;
					enemies[i].reloadTime = 0.1;
					enemies[i].angularSpeed = 2 * PI;
					enemies[i].reloadTimer = enemies[i].reloadTime;
					enemies[i].maxHp = 5;
					enemies[i].range = 150;
					break;
				};
				
				//heavy bomber
				case HEAVY_BOMBER:{
					enemies[i].model.mesh = &mesh_heavyBomber;
					enemies[i].updateProc = &updateBomber;
					enemies[i].speed = 20;
					enemies[i].maxHp = 300;
					enemies[i].reloadTime = 0.1;
					enemies[i].angularSpeed = PI / 4;
					enemies[i].range = 300;
					enemies[i].size.x = 30;
					enemies[i].size.y = 10;
					enemies[i].size.z = 50;
					enemies[i].altitude = 200;
					break;
				};
			};
			
			//set health to maximum health
			enemies[i].hp = enemies[i].maxHp;	
			
			break;
		};
	};
};

//check enemy bullet collision
void checkEnemyBulletCollision(Entity * enemy){
	//check for collision with bullets
	int i;
	
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(bullets[i].alive == 1){
			//check distance
			if(checkBasicEntityCollision(enemy, &bullets[i], 2.f)){
				enemy->hp -= damage;
				bullets[i].alive = 0;
			};
		};
	};
};

//check enemy alive
void checkEnemyAlive(Entity * enemy){
	//kill self if hp <= 0
	if(enemy->hp <= 0){
		//set alive to zero
		enemy->alive = 0;
		numberOfEnemies -= 1;
		
		//increment score
		if(enemy->type == BOMBER){
			score += 10;
			
			//play explosion sound
			playSound(&sound_explosion);
		} else if(enemy->type == CHASER){
			score += 25;
			
			//play explosion sound
			playSound(&sound_explosion);
		} else if(enemy->type == HEAVY_BOMBER){
			score += 100;
			
			//play explosion sound
			playSound(&sound_bossExplosion);
		};
		
		//decide number of debris
		int debrisCount = 5;
		int particleCount = 100;
		
		if(enemy->type == HEAVY_BOMBER){
			debrisCount = 25;
			particleCount = 150;
		};
		
		//generate debris
		int i;
		for(i = 0; i < debrisCount; i++){
			initialiseDebris(enemy->position.x, enemy->position.y, enemy->position.z, 255, 0, 0);
		};
		
		//generate particles
		for(i = 0; i < particleCount; i++){
			initialiseParticle(5, enemy->position.x, enemy->position.y, enemy->position.z, 25 - (rand() % 50), 25 - (rand() % 50), 25 - (rand() % 50), 255, 200, 0);
		};
	};
};

//update enemy position
void updateEnemyPosition(Entity * enemy){
	//move self
	enemy->position = addVectors(enemy->position, scalarProduct(enemy->velocity, gameClock.deltaTime));
	
	//set rotation
	enemy->model.rotation = enemy->rotation;
};

//find nearest build
Entity * findNearestBuilding(Entity * entity){
	//set target
	Entity * target = 0;
	double distance = 999999;
	double newDistance;
	
	//iterate through buildings
	int i;
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		//check if terrain is active and a building
		if(terrain[i].alive == 1 && terrain[i].type == BUILDING && (newDistance = vectorMagnitude(subtractVectors(entity->position, terrain[i].position))) < distance){
			target = &terrain[i];
			distance = newDistance;
		};
	};
	
	//return target
	return target;
};

//get shortest angle
double getShortestAngle(double a1, double a2){
	//calculate change in angle
	double deltaAngle = standardiseAngle(a2 - a1);
	
	//check if other direction is quicker
	if(fabs(deltaAngle) > PI){
		//quicker to turn in other direction since other direction is less than PI radians
		deltaAngle = -standardiseAngle(2 * PI - deltaAngle);
	};
	
	return deltaAngle;
};

//rotate towards target
void rotateTowardsTarget(double * angle, double targetAngle, double angularSpeed){
	//rotate in correct direction
	
	//calculate change in angle
	double deltaAngle = getShortestAngle(*angle, targetAngle);
	double deltaSign = deltaAngle / fabs(deltaAngle);
	
	//rotate
	*angle += deltaSign * angularSpeed * gameClock.deltaTime;
	
	//check for sign change (indicating rotation has overshot target angle)
	double newDelta = getShortestAngle(*angle, targetAngle);
	double newSign = newDelta / fabs(newDelta);
	
	if(newSign != deltaSign){
		*angle = targetAngle;
	};
};

//chase target
Vector4f chaseTarget(Entity * enemy, Vector4f targetOffset){
	//rotate towards player
	Vector4f targetVector = normaliseVector(subtractVectors(addVectors(enemy->target->position, targetOffset), enemy->position));
	Vector4f targetAngle = directionToAngle(targetVector);
	
	//rotate in the x
	rotateTowardsTarget(&enemy->rotation.x, targetAngle.x, enemy->angularSpeed);
	
	//rotate in the y
	rotateTowardsTarget(&enemy->rotation.y, targetAngle.y, enemy->angularSpeed);
	
	//rotate in the z
	rotateTowardsTarget(&enemy->rotation.z, targetAngle.z, enemy->angularSpeed);
	
	//rotation
	Matrix4x4f rotation = createRotationMatrix(enemy->rotation);
	Vector4f direction = {0};
	direction.z = 1;	
	direction = matrixVectorProduct(rotation, direction);
	
	//set velocity
	enemy->velocity = scalarProduct(direction, enemy->speed);
	
	//return direction
	return direction;
};

//update bomber
void updateBomber(Entity * enemy){
	//search for nearest building
	if(!enemy->target || enemy->target->alive == 0){
		//set target
		enemy->target = findNearestBuilding(enemy);
	} else {
		//chase target
		Vector4f offset = {0};
		offset.y = enemy->altitude;
		Vector4f direction = chaseTarget(enemy, offset);
		
		//reload
		enemy->reloadTimer -= gameClock.deltaTime;
		if(enemy->reloadTimer < 0){
			enemy->reloadTimer = 0;
		};
		
		//fire bombs
		Vector4f targetVector = subtractVectors(enemy->target->position, enemy->position);
		if(vectorMagnitude(targetVector) < enemy->range && enemy->reloadTimer == 0){
			//shoot
			enemy->reloadTimer = enemy->reloadTime;
			
			//get diretion angle
			Vector4f targetAngle = directionToAngle(targetVector);
			Matrix4x4f rotation = createRotationMatrix(targetAngle);
			Vector4f direction2 = {0};
			direction2.z = 1;	
			direction2 = matrixVectorProduct(rotation, direction2);
			Vector4f original = normaliseVector(targetVector);
			
			//create bullet
			initialiseEnemyBullet(enemy->position.x, enemy->position.y, enemy->position.z, targetAngle.x, targetAngle.y, targetAngle.z, 1, 1, 2, 200);
		};
	};
	
	//update enemy position
	updateEnemyPosition(enemy);
	
	//check bullet collision
	checkEnemyBulletCollision(enemy);
	
	//check alive
	checkEnemyAlive(enemy);
	
	//set model colour
	enemy->model.colour.red = 200 + (55.f * (double) (enemy->maxHp - enemy->hp) / enemy->maxHp);
};

//update chaser
void updateChaser(Entity * enemy){
	//chase target
	Vector4f offset;
	offset.x = 0;
	offset.y = 0;
	offset.z = 0;
	enemy->target = &playerInstance;
	Vector4f direction = chaseTarget(enemy, offset);
	
	//update enemy position
	updateEnemyPosition(enemy);
	
	//check bullet collision
	checkEnemyBulletCollision(enemy);
	
	//check alive
	checkEnemyAlive(enemy);
	
	//shoot
	if(vectorMagnitude(subtractVectors(playerInstance.position, enemy->position)) < enemy->range){
		//decrement reload
		enemy->reloadTimer -= gameClock.deltaTime;
		
		//shoot
		if(enemy->reloadTimer <= 0){
			//reset reload
			enemy->reloadTimer = enemy->reloadTime;
			
			//calculate firing position and shoot
			Vector4f firingPosition = addVectors(enemy->position, scalarProduct(direction, 3));
			initialiseEnemyBullet(firingPosition.x, firingPosition.y, firingPosition.z, enemy->rotation.x, enemy->rotation.y, enemy->rotation.z, 0.5, 0.5, 1, 2 * playerInstance.speed + 5);
		};
	} else {
		//reset reload
		enemy->reloadTimer = enemy->reloadTime;
	};
	
	//set enemy colour
	enemy->model.colour.red = 200 + (55.f * (double) (enemy->maxHp - enemy->hp) / enemy->maxHp);
};