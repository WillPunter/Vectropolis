/*
	game.c
	
	This file provides all of the code related to the 
*/

//fade start new game
void fadeStartNewGame(){
	//check that not already fading
	if(fadeState == NONE){
		//fade out
		fadeState = FADE_OUT;
		fadeProc = &startNewArcadeGame;
	};
};

//start game
void startGame(){
	//attempt to open save
	if(!loadSave(&gameSave)){
		//make new directory
		char path[STR_LENGTH];
		sprintf(path, "%s/Saved Games/%s", getenv("USERPROFILE"), GAME_TITLE);
		_mkdir(path);
		
		//create new save
		memset(&gameSave, 0, sizeof(Save));
		gameSave.musicVolume = 0.5;
		gameSave.soundVolume = 0.5;
		gameSave.xSensitivity = 0.5;
		gameSave.ySensitivity = 0.5;
		writeSave(&gameSave);
	};
	
	//load bitmaps from included resource
	loadBitmapFromResource(IDR_FONTDATA, &bmp_font);
	loadBitmapFromResource(IDR_LIFEDATA, &bmp_life);
	loadBitmapFromResource(IDR_CURSORDATA, &bmp_cursor);
	
	//load meshes
	loadMeshFromResource(IDR_CUBEDATA, &mesh_cube);
	loadMeshFromResource(IDR_SPACESHIPDATA, &mesh_spaceship);
	loadMeshFromResource(IDR_ENEMYDATA, &mesh_enemy);
	loadMeshFromResource(IDR_CHASERDATA, &mesh_chaser);
	loadMeshFromResource(IDR_HEAVYBOMBERDATA, &mesh_heavyBomber);
	loadMeshFromResource(IDR_DEBRISDATA, &mesh_debris);
	
	//initialise sound engine
	if(initialiseSoundEngine() != S_OK){
		running = 0;
	};
	
	//load sounds
	loadSoundFromResource(IDR_SHOOTDATA, &sound_shoot);
	loadSoundFromResource(IDR_EXPLOSIONDATA, &sound_explosion);
	loadSoundFromResource(IDR_MENUMUSICDATA, &music_menu);
	loadSoundFromResource(IDR_GAMEMUSICDATA, &music_game);
	loadSoundFromResource(IDR_BUILDINGDESTROYDATA, &sound_buildingDestroy);
	loadSoundFromResource(IDR_ENEMYSHOOTDATA, &sound_enemyShoot);
	loadSoundFromResource(IDR_BOSSEXPLOSIONDATA, &sound_bossExplosion);
	loadSoundFromResource(IDR_SELECTDATA, &sound_select);
	
	//set state to menu
	startMenu();
	
	//initialise camera
	camera.scale.x = 1;
	camera.scale.y = 1;
	camera.scale.z = 1;
	camera.position.x = 0;
	camera.position.y = 0;
	camera.position.z = 0;
	camera.rotation.x = 0;
	camera.rotation.y = 0;
	camera.rotation.z = 0;
	
	menuCamera.position.x = 0;
	menuCamera.position.y = 100;
	menuCamera.position.z = -500;
	menuCamera.rotation.x = 0;
	menuCamera.rotation.y = -0.4;
	menuCamera.rotation.z = 0;
	menuCamera.scale.x = 1;
	menuCamera.scale.y = 1;
	menuCamera.scale.z = 1;
	
	//set fov
	window.fov = PI / 2;
	window.screenDistance = 1.f / tan(window.fov / 2.f);
	
	//initialise menu buttons
	char startButtonStr[STR_LENGTH] = "Start Game";
	initialiseButton(menuButtons, startButtonStr, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 3, 256, 24, 16, &fadeControlChoice, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char startButtonStr2[STR_LENGTH] = "Leaderboard";
	initialiseButton(menuButtons, startButtonStr2, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 3 - 42, 256, 24, 16, &fadeLeaderboard, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char startButtonStr3[STR_LENGTH] = "Settings";
	initialiseButton(menuButtons, startButtonStr3, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 3 - 84, 256, 24, 16, &fadeSettings, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char startButtonStr4[STR_LENGTH] = "Quit";
	initialiseButton(menuButtons, startButtonStr4, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 3 - 126, 256, 24, 16, &endGame, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	//initialise next wave buttons
	char nextWaveButtonStr[STR_LENGTH] = "Next Wave";
	initialiseButton(nextWaveButtons, nextWaveButtonStr, BUFFER_WIDTH / 2, 64, 256, 32, 16, &startNextWave, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char damageUpgradeStr[STR_LENGTH] = "5 Pts";
	initialiseButton(nextWaveButtons, damageUpgradeStr, BUFFER_WIDTH - 100, 360, 100, 32, 16, &upgradeDamage, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char batteryUpgradeStr[STR_LENGTH] = "5 Pts";
	initialiseButton(nextWaveButtons, batteryUpgradeStr, BUFFER_WIDTH - 100, 300, 100, 32, 16, &upgradeBattery, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char lifeUpgradeStr[STR_LENGTH] = "15 Pts";
	initialiseButton(nextWaveButtons, lifeUpgradeStr, BUFFER_WIDTH - 100, 240, 100, 32, 16, &upgradeLife, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char buildingUpgradeStr[STR_LENGTH] = "1 Pt";
	initialiseButton(nextWaveButtons, buildingUpgradeStr, BUFFER_WIDTH - 100, 180, 100, 32, 16, &upgradeBuilding, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char allBuildingUpgradeStr[STR_LENGTH];
	initialiseButton(nextWaveButtons, allBuildingUpgradeStr, BUFFER_WIDTH - 100, 120, 100, 32, 16, &upgradeAllBuildings, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	//initialise game over buttons
	char gameOverButtonStr[STR_LENGTH] = "Menu";
	initialiseButton(gameOverButtons, gameOverButtonStr, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 5, 280, 32, 16, &fadeMenu, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char gameOverButtonStr2[STR_LENGTH] = "Submit High-Score";
	initialiseButton(gameOverButtons, gameOverButtonStr2, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 5, 280, 32, 16, &fadeHighScore, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	//initialise new high score button
	char newHighScoreButtonStr[STR_LENGTH] = "Submit";
	initialiseButton(newHighScoreButtons, newHighScoreButtonStr, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 4, 256, 32, 16, &fadeSubmitHighScore, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	//initialise leaderboard buttons
	char leaderboardButtonStr[STR_LENGTH] = "Menu";
	initialiseButton(leaderboardButtons, leaderboardButtonStr, BUFFER_WIDTH / 2, 40, 256, 32, 16, &fadeMenu, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	//initialise settings buttons
	char settingsButtonStr[STR_LENGTH] = "Back";
	initialiseButton(settingsButtons, settingsButtonStr, BUFFER_WIDTH / 2, 80, 256, 32, 16, &fadeMenu, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char musicVolumeUpButtonStr[STR_LENGTH] = "+";
	initialiseButton(settingsButtons, musicVolumeUpButtonStr, 180 + BUFFER_WIDTH / 2 + 64, 360 + 8, 32, 32, 16, &musicVolumeUp, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char musicVolumeDownButtonStr[STR_LENGTH] = "-";
	initialiseButton(settingsButtons, musicVolumeDownButtonStr, 180 + BUFFER_WIDTH / 2 - 64, 360 + 8, 32, 32, 16, &musicVolumeDown, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char soundVolumeUpButtonStr[STR_LENGTH] = "+";
	initialiseButton(settingsButtons, soundVolumeUpButtonStr, 180 + BUFFER_WIDTH / 2 + 64, 312 + 8, 32, 32, 16, &soundVolumeUp, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char soundVolumeDownButtonStr[STR_LENGTH] = "-";
	initialiseButton(settingsButtons, soundVolumeDownButtonStr, 180 + BUFFER_WIDTH / 2 - 64, 312 + 8, 32, 32, 16, &soundVolumeDown, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char xSensitivityUpButtonStr[STR_LENGTH] = "+";
	initialiseButton(settingsButtons, xSensitivityUpButtonStr, 180 + BUFFER_WIDTH / 2 + 64, 264 + 8, 32, 32, 16, &xSensitivityUp, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char xSensitivityDownButtonStr[STR_LENGTH] = "-";
	initialiseButton(settingsButtons, xSensitivityDownButtonStr, 180 + BUFFER_WIDTH / 2 - 64, 264 + 8, 32, 32, 16, &xSensitivityDown, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char ySensitivityUpButtonStr[STR_LENGTH] = "+";
	initialiseButton(settingsButtons, ySensitivityUpButtonStr, 180 + BUFFER_WIDTH / 2 + 64, 216 + 8, 32, 32, 16, &ySensitivityUp, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char ySensitivityDownButtonStr[STR_LENGTH] = "-";
	initialiseButton(settingsButtons, ySensitivityDownButtonStr, 180 + BUFFER_WIDTH / 2 - 64, 216 + 8, 32, 32, 16, &ySensitivityDown, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	//initialise control choice buton
	char keyboardControlButtonStr[STR_LENGTH] = "Keyboard";
	initialiseButton(controlChoiceButtons, keyboardControlButtonStr, BUFFER_WIDTH / 4, 120, 256, 32, 16, &setKeyboardControl, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char mouseControlButtonStr[STR_LENGTH] = "Mouse";
	initialiseButton(controlChoiceButtons, mouseControlButtonStr, 3 * BUFFER_WIDTH / 4, 120, 256, 32, 16, &setMouseControl, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	char controlBackButtonStr[STR_LENGTH] = "Back";
	initialiseButton(controlChoiceButtons, controlBackButtonStr, BUFFER_WIDTH / 2, 60, 256, 32, 16, &fadeMenu, 50, 50, 50, 255, 255, 255, 255, 0, 0);	
	
	//initialise
	char resumeButtonStr[STR_LENGTH] = "Resume";
	initialiseButton(pauseButtons, resumeButtonStr, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 2 + 32, 256, 32, 16, &unpause, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	char exitButtomStr[STR_LENGTH] = "Menu";
	initialiseButton(pauseButtons, exitButtomStr, BUFFER_WIDTH / 2, BUFFER_HEIGHT / 2 - 32, 256, 32, 16, &fadeMenu, 50, 50, 50, 255, 255, 255, 255, 0, 0);
	
	//start main loop
	mainLoop();
};

//start new arcade game
void startNewArcadeGame(){
	//set all lives
	lives = 3;
	
	//reset instruction text timer
	instructionTextTimer = 5;
	
	//reset battery and damage
	damage = 1;
	battery = 1;

	//reset score
	score = 0;
	
	//reset upgrade points
	upgradePoints = 0;
	
	//reset all entities
	memset(terrain, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(bullets, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(enemies, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(enemyBullets, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(debris, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(lights, 0, NUMBER_OF_LIGHTS * sizeof(Light));
	memset(particles, 0, NUMBER_OF_PARTICLES * sizeof(Particle));
	
	//generate lights
	generateLights();
	
	//create buildings
	generateBuildings();
	
	//generate stars
	generateStars();
	
	//start first wave
	startWave(1);
};

//end arcade
void endArcadeGame(){
	//create loop counter
	int i;
	
	//find if high score has been reached
	int highScoreReached = 0;
	for(i = 0; i < NUMBER_OF_HIGH_SCORES; i++){
		if(score > gameSave.highScoreScores[i]){
			highScoreReached = 1;
			break;
		};
	};
	
	//check if high score was reached
	if(highScoreReached == 1){
		//new high score - add high score button
		gameOverButtons[0].alive = 0;
		gameOverButtons[1].alive = 1;
	} else {
		gameOverButtons[0].alive = 1;
		gameOverButtons[1].alive = 0;
	};
	
	//game over
	state = GAME_OVER;
};

//update game
void updateGame(){
	//check if paused
	if(pause){
		//check for unpause
		if(keys[VK_ESCAPE].isDown && keys[VK_ESCAPE].timePressed == 0){
			pause = 0;
		};
		
		//update pause buttons
		int i;
		
		for(i = 0; i < NUMBER_OF_BUTTONS; i++){
			if(pauseButtons[i].alive){
				updateButton(&pauseButtons[i]);
			};
		};
	} else {
		//update player instance
		if(playerInstance.updateProc){
			playerInstance.updateProc(&playerInstance);
		};
		
		//update all terrain
		int i;
		for(i = 0; i < NUMBER_OF_ENTITIES; i++){
			if(terrain[i].alive == 1 && terrain[i].updateProc){
				//update
				terrain[i].updateProc(&terrain[i]);
			};
		};
		
		//update all bullets
		for(i = 0; i < NUMBER_OF_ENTITIES; i++){
			if(bullets[i].alive == 1 && bullets[i].updateProc){
				//update
				bullets[i].updateProc(&bullets[i]);
			};
		};
		
		//update all enemies
		for(i = 0; i < NUMBER_OF_ENTITIES; i++){
			if(enemies[i].alive == 1 && enemies[i].updateProc){
				//update
				enemies[i].updateProc(&enemies[i]);
			};
		};
		
		//update all enemy bullets
		for(i = 0; i < NUMBER_OF_ENTITIES; i++){
			if(enemyBullets[i].alive == 1 && enemyBullets[i].updateProc){
				//update
				enemyBullets[i].updateProc(&enemyBullets[i]);
			};
		};
		
		//update all debris
		for(i = 0; i < NUMBER_OF_ENTITIES; i++){
			if(debris[i].alive == 1 && debris[i].updateProc){
				//update
				debris[i].updateProc(&debris[i]);
			};
		};
		
		//update all particles
		for(i = 0; i < NUMBER_OF_PARTICLES; i++){
			if(particles[i].alive == 1){
				//update
				updateParticle(&particles[i]);
			};
		};
		
		//check if game over
		if(numberOfBuildings <= 0){
			//set game over message
			strcpy(gameOverFeedbackText, "All your buildings were destroyed!");
			
			//go to game over
			fadeGameOver();
		} else if(numberOfEnemies <= 0){
			//decrement end wave timer
			endWaveTimer -= gameClock.deltaTime;
			
			//end of wave - go to wave interchange screen
			if(endWaveTimer < 0){
				//end wave
				fadeNextWave();
			};
		};
		
		//check for pause
		if(keys[VK_ESCAPE].isDown && keys[VK_ESCAPE].timePressed == 0){
			pause = 1;
		};
	};
	
	//play game music
	playMusic(&music_game);
};

//render game
void renderGame(){
	//reset depth buffer
	resetDepthBuffer();
	
	//transform camera
	//apply the camera transformation matrix to the model transformation matrix
	Matrix4x4f cameraTransform = createCameraRotationMatrix(scalarProduct(camera.rotation, -1));
	
	//transform lights
	transformLights(cameraTransform, camera.position);
	
	//render player Instance
	if(playerInstance.state == ALIVE){
		drawModel(&playerInstance.model, playerInstance.position, cameraTransform, camera.position);
	};
	
	//render all terrain
	int i;
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(terrain[i].alive == 1){
			//render
			drawModel(&terrain[i].model, terrain[i].position, cameraTransform, camera.position);
		};
	};
	
	//render all bullets
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(bullets[i].alive == 1){
			//render
			drawModel(&bullets[i].model, bullets[i].position, cameraTransform, camera.position);
		};
	};
	
	//render all enemies
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(enemies[i].alive == 1){
			//render
			drawModel(&enemies[i].model, enemies[i].position, cameraTransform, camera.position);
		};
	};
	
	//render all enemy bullets
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(enemyBullets[i].alive == 1){
			//render
			drawModel(&enemyBullets[i].model, enemyBullets[i].position, cameraTransform, camera.position);
		};
	};
	
	//render all debris
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(debris[i].alive == 1){
			//render
			drawModel(&debris[i].model, debris[i].position, cameraTransform, camera.position);
		};
	};
	
	//render particles
	for(i = 0; i < NUMBER_OF_PARTICLES; i++){
		if(particles[i].alive == 1){
			//draw particle
			renderParticle(particles[i].position, particles[i].colour, cameraTransform);
		};
	};
	
	//draw stars
	for(i = 0; i < NUMBER_OF_STARS; i++){
		//draw star as particle
		renderParticle(stars[i], starColours[i], cameraTransform);
	};
	
	//draw text
	char enemyString[STR_LENGTH];
	sprintf(enemyString, "Enemies: %d", numberOfEnemies); 
	drawString(enemyString, 20, BUFFER_HEIGHT - 64, 16, 255, 0, 0);
	
	char buildingString[STR_LENGTH];
	sprintf(buildingString, "Buildings: %d", numberOfBuildings); 
	drawString(buildingString, BUFFER_WIDTH - 240, BUFFER_HEIGHT - 64, 16, 255, 255, 0);
	
	char waveString[STR_LENGTH];
	sprintf(waveString, "Wave %d", wave); 
	drawString(waveString, 20, BUFFER_HEIGHT - 32, 16, 255, 255, 255);
	
	char scoreString[STR_LENGTH];
	sprintf(scoreString, "Score: %d", score); 
	drawString(scoreString, BUFFER_WIDTH - 240, BUFFER_HEIGHT - 32, 16, 255, 255, 255);
	
	//draw lives
	for(i = 0; i < lives; i++){
		//draw life
		drawBitmap(&bmp_life, BUFFER_WIDTH / 2 - (lives * 32) / 2 + i * 32, 50);
	};
	
	//draw health
	drawRectangle(20, 20, BUFFER_WIDTH / 2 - 20, 40, 255, 0, 0);
	drawRectangle(20, 20, 20 + (BUFFER_WIDTH / 2 - 40) * (double) playerInstance.hp / 10.f, 40, 0, 255, 0);
	
	//draw battery
	drawRectangle(BUFFER_WIDTH / 2 + 20, 20, BUFFER_WIDTH - 20, 40, 50, 50, 50);
	drawRectangle(BUFFER_WIDTH / 2 + 20, 20, BUFFER_WIDTH / 2 + 20 + currentBattery * (BUFFER_WIDTH / 2 - 40), 40, 255, 255, 255);
	
	//check if paused
	if(pause){
		//render all buttons
		for(i = 0; i < NUMBER_OF_BUTTONS; i++){
			if(pauseButtons[i].alive){
				renderButton(&pauseButtons[i]);
			};
		};
		
		//render pause text
		char pauseText[STR_LENGTH] = "Paused";
		drawString(pauseText, (BUFFER_WIDTH - 24 * strlen(pauseText)) / 2, 360, 24, 255, 255, 255);
	
		//draw mouse
		drawMouse();
	} else {
		//draw instruction text if necessary
		if(instructionTextTimer > 0){
			//set start y
			double startY = 360;
			
			//set y offset
			if(instructionTextTimer < 0.5){
				//instructionTextYOffset = (BUFFER_HEIGHT - startY + 64) - (BUFFER_HEIGHT - startY + 64) * 2 * instructionTextTimer;
			} else {
				instructionTextYOffset = 0;
			};
			
			//draw instruction text
			char instructionText[STR_LENGTH] = "Protect the Buildings";
			drawString(instructionText, (BUFFER_WIDTH - 16 * strlen(instructionText)) / 2, startY + instructionTextYOffset, 16, 255, 255, 255);
			
			char instructionText2[STR_LENGTH] = "Shoot Down All The Enemies";
			drawString(instructionText2, (BUFFER_WIDTH - 16 * strlen(instructionText2)) / 2, startY - 40 + instructionTextYOffset, 16, 255, 255, 255);
	
			//decrement instruction text timer
			instructionTextTimer -= gameClock.deltaTime;
		};
	};
};

//unpause
void unpause(){
	pause = 0;
};