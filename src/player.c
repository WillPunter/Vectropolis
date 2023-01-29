/*
	player.c
	
	This file includes code for initialising and updating the player.
*/

//player functions
void initialisePlayer(Entity * player, double x, double y, double z){
	player->position.x = x;
	player->position.y = y;
	player->position.z = z;
	player->rotation.x = 0;
	player->rotation.y = 0;
	player->rotation.z = 0;
	player->size.x = 2;
	player->size.y = 1;
	player->size.z = 2;
	player->reloadTime = 0.1;
	player->reloadTimer = 0;
	player->model.mesh = &mesh_spaceship;
	player->model.scale.x = 1;
	player->model.scale.y = 1;
	player->model.scale.z = 1;
	player->state = ALIVE;
	player->hp = 10;
	player->deathTimer = 3.0f;
	player->speed = 50;
	player->updateProc = &updatePlayer;
	player->model.colour.red = 0;
	player->model.colour.green = 0;
	player->model.colour.blue = 255;
	currentBattery = 1;
};

//update player
void updatePlayer(Entity * player){
	//check that not dead
	if(player->state == ALIVE){
		//check control type
		if(controlType == KEYBOARD){
			//rotate player ship
			if(keys['S'].isDown){
				player->rotation.x -= 4 * gameClock.deltaTime * gameSave.ySensitivity;
			} else if(keys['W'].isDown){
				player->rotation.x += 4 * gameClock.deltaTime * gameSave.ySensitivity;
			};
		
			if(keys['A'].isDown){
				player->rotation.y -= 4 * gameClock.deltaTime * gameSave.xSensitivity;
			} else if(keys['D'].isDown){
				player->rotation.y += 4 * gameClock.deltaTime * gameSave.xSensitivity;
			};
		} else {
			//rotate with mouse
			if(window.isFocused){
				player->rotation.y += deltaX * gameClock.deltaTime * gameSave.xSensitivity;
				player->rotation.x += -deltaY * gameClock.deltaTime * gameSave.ySensitivity;
			};
		};
		
		//get move direction vector
		Matrix4x4f dir = createRotationMatrix(camera.rotation);
		Vector4f moveDir = {0};
		moveDir.z = 1;
		moveDir = matrixVectorProduct(dir, moveDir);
		
		//set player velocity
		double speed = player->speed;
		
		//check if boost should be applied
		if(((GetAsyncKeyState(VK_LCONTROL) && controlType == KEYBOARD) || (GetAsyncKeyState(VK_RBUTTON) && controlType == MOUSE)) && currentBattery > 0){
			//boost is applied - speed up
			speed *= 2;
			currentBattery -= gameClock.deltaTime / ((double) battery * 2.f);
		};
		
		//velocity
		player->velocity = scalarProduct(moveDir, speed);
		
		//move
		player->position = addVectors(player->position, scalarProduct(player->velocity, gameClock.deltaTime));
		
		//move camera towards player
		//delta positon
		Vector4f deltaPosition = subtractVectors(subtractVectors(player->position, scalarProduct(moveDir, 8)), camera.position);
		Vector4f angleOffset = {0};
		angleOffset.x = -0.3;
		Vector4f deltaAngle = subtractVectors(addVectors(player->rotation, angleOffset), camera.rotation);
		camera.position = addVectors(camera.position, scalarProduct(deltaPosition, 0.075));
		camera.rotation = addVectors(camera.rotation, scalarProduct(deltaAngle, 0.075));
		
		//shoot
		if(player->reloadTimer > 0){
			player->reloadTimer -= gameClock.deltaTime;
		}
		
		if(((GetAsyncKeyState(VK_SPACE) && controlType == KEYBOARD) || (GetAsyncKeyState(VK_LBUTTON) && controlType == MOUSE)) && player->reloadTimer <= 0 && currentBattery > 0){
			//reset reload timer
			player->reloadTimer = player->reloadTime;
			
			//decrement battery
			currentBattery -= 0.1 / (double) battery;
			
			//creaet bullet at position + offset
			Vector4f bulletPos = addVectors(player->position, scalarProduct(moveDir, 4));
			initialiseBullet(bulletPos.x, bulletPos.y, bulletPos.z, player->rotation.x, player->rotation.y, player->rotation.z);
			
			//play gunshot sound
			playSound(&sound_shoot);
		} else if(!(((GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_SPACE)) && controlType == KEYBOARD) || ((GetAsyncKeyState(VK_RBUTTON) || GetAsyncKeyState(VK_LBUTTON)) && controlType == MOUSE))){
			//recharge battery
			currentBattery += (double) gameClock.deltaTime / 3;
			
			//ensure that bettery never exceeds 1
			if(currentBattery > 1){
				currentBattery = 1;
			};
		};
		
		//check collision with terrain
		Entity * collidingBuilding;
		if(player->hp > 0){
			if(collidingBuilding = checkTerrainCollision(player)){
				//destroy building
				if(collidingBuilding->type == BUILDING){
					//set building health to zero
					collidingBuilding->hp = 0;
				};
			
				//set hp to 0
				player->hp = 0;
			};
		};
		
		//check for collision with enemy bullets
		int i;
		for(i = 0; i < NUMBER_OF_ENTITIES; i++){
			if(enemyBullets[i].alive == 1){
				//check distance
				if(checkBasicEntityCollision(player, &enemyBullets[i], 2.f)){
					player->hp -= 2;
					enemyBullets[i].alive = 0;
				};
			};
		};
		
		//check for collision with enemies
		for(i = 0; i < NUMBER_OF_ENTITIES; i++){
			if(enemies[i].alive == 1){
				//check distance
				if(checkAdvancedEntityCollision(player, &enemies[i], 1.f)){
					player->hp = 0;
				};
			};
		};
		
		//set model position
		player->model.rotation = player->rotation;
		
		//check hp
		if(player->hp <= 0){
			//die
			player->state = DEAD;
			
			//play explosion sound
			playSound(&sound_explosion);
			
			//generate debris
			int i;
			for(i = 0; i < 10; i++){
				initialiseDebris(player->position.x, player->position.y, player->position.z, 0, 0, 255);
			};
			
			//generate particles
			for(i = 0; i < 150; i++){
				initialiseParticle(5, player->position.x, player->position.y, player->position.z, 25 - (rand() % 50), 25 - (rand() % 50), 25 - (rand() % 50), 255, 200, 0);
				initialiseParticle(5, player->position.x, player->position.y, player->position.z, 25 - (rand() % 50), 25 - (rand() % 50), 25 - (rand() % 50), 0, 0, 200);
			};
		};
	} else {
		//reset velocity
		player->velocity.x = 0;
		player->velocity.y = 0;
		player->velocity.z = 0;
		
		//subtract
		player->deathTimer -= gameClock.deltaTime;
		if(player->deathTimer <= 0){
			//decrement lives
			lives -= 1;
			
			if(lives <= 0){
				//set game over text
				strcpy(gameOverFeedbackText, "You are out of lives");
				
				//fade game over
				fadeGameOver();
			} else {
				//reset player position
				initialisePlayer(&playerInstance, PLAYER_START_X, PLAYER_START_Y, PLAYER_START_Z);
			};
		};
	};
};