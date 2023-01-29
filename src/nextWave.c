/*
	nextWave.c
	
	This file contains code for the next wave menu state.
*/

//fade next wave
void fadeNextWave(){
	//check that not already fading
	if(fadeState == NONE){
		//fade out
		fadeState = FADE_OUT;
		fadeProc = &startNextWaveMenu;
	};
};

//start next wave
void startNextWaveMenu(){
	//add points
	upgradePoints += 5;
	
	//set state
	state = NEXT_WAVE;
};

//start wave
void startWave(int n){
	//set pause to false
	pause = 0;
	
	//reset all non-player, non-terrain entities
	memset(bullets, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(enemies, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(enemyBullets, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(debris, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	memset(particles, 0, NUMBER_OF_PARTICLES * sizeof(Particle));

	//set wave
	wave = n;
	
	//create player
	initialisePlayer(&playerInstance, PLAYER_START_X, PLAYER_START_Y, PLAYER_START_Z);
	camera.position = playerInstance.position;
	camera.rotation = playerInstance.rotation;
	
	//create loop counter
	int i;
		
	//reset number of enemies
	numberOfEnemies = 0;
	int numberOfBombers = 0;
	int numberOfChasers = 0;
	int numberOfHeavyBombers = 0;
	
	//set number of enemies of each type
	if(wave < 4){
		numberOfBombers = 2 + wave;
	} else if(wave < 7){
		numberOfBombers = 2;
		numberOfChasers = wave - 3;
	} else if(wave < 10){
		numberOfBombers = 5;
		numberOfChasers = wave - 6;
	} else if(wave == 10){
		numberOfHeavyBombers = 1;
	} else if(wave < 15){
		numberOfChasers = wave - 5;
	} else if(wave < 20){
		numberOfBombers = wave;
		numberOfChasers = wave - 10;
	} else if(wave == 20){
		numberOfHeavyBombers = 2;
	} else if(wave < 30){
		numberOfBombers = wave;
		numberOfChasers = wave - 20;
	} else {
		numberOfBombers = 5 + wave % 10;
		numberOfChasers = 5 + wave % 10;
		numberOfHeavyBombers = wave / 10;
	};
	
	//spawn bombers
	for(i = 0; i < numberOfBombers; i++){
		initialiseEnemy(200 - rand() % 600, 300, 200 - rand() % 600, BOMBER);
	};
	
	//spawn chasers
	for(i = 0; i < numberOfChasers; i++){
		initialiseEnemy(200 - rand() % 600, 300, 200 - rand() % 600, CHASER);
	};
	
	//spawn heavy bombers
	for(i = 0; i < numberOfHeavyBombers; i++){
		initialiseEnemy(200 - rand() % 600, 400, 200 - rand() % 600, HEAVY_BOMBER);
	};
	
	//reset end wave timer
	endWaveTimer = 1.5f;
	
	//set state
	state = GAME;
	
	//get screen size
	RECT windowRect;
	GetClientRect(window.handle, &windowRect);
	GetWindowRect(window.handle, &window.frameRect);
	
	//set the cursor to the centre of the screen
	SetCursorPos((window.frameRect.left + window.frameRect.right) / 2, (window.frameRect.top + window.frameRect.bottom) / 2);
};

//start next wave
void startNextWave(){
	//next wave
	startWave(wave + 1);
};

//update next wave
void updateNextWave(){
	//update buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(nextWaveButtons[i].alive == 1){
			updateButton(&nextWaveButtons[i]);
		};
	};
};

//render next wave
void renderNextWave(){
	//draw menu bg
	drawMenuBG();
	
	//draw entry text
	char menuText[STR_LENGTH] = "Upgrades";
	drawString(menuText, (BUFFER_WIDTH - strlen(menuText) * 24) / 2, 440, 24, 255, 255, 255);
	
	//upgrade string
	char upgradeStr[STR_LENGTH];
	sprintf(upgradeStr, "Upgrade points: %d", upgradePoints);
	drawString(upgradeStr, (BUFFER_WIDTH - strlen(upgradeStr) * 16) / 2, 400, 16, 255, 255, 255);
	
	//render all text
	char damageUpgradeStr[STR_LENGTH] = "Damage";
	drawString(damageUpgradeStr, 20, 350, 16, 255, 255, 255);
	
	//render damage points
	int i;
	Pixel boxColour;
	boxColour.red = 255;
	boxColour.green = 255;
	boxColour.blue = 255;
	
	for(i = 0; i < 10; i++){
		if(i >= damage){
			boxColour.green = 0;
			boxColour.blue = 0;
		};
		drawRectangle(164 + i * 25, 346, 164 + i * 25 + 16, 370, boxColour.red, boxColour.green, boxColour.blue);
	};
	
	char batteryUpgradeStr[STR_LENGTH] = "Battery";
	drawString(batteryUpgradeStr, 20, 290, 16, 255, 255, 255);
	
	//draw battery rectangles
	boxColour.red = 255;
	boxColour.green = 255;
	boxColour.blue = 255;
	
	for(i = 0; i < 10; i++){
		if(i >= battery){
			boxColour.green = 0;
			boxColour.blue = 0;
		};
		drawRectangle(164 + i * 25, 286, 164 + i * 25 + 16, 310, boxColour.red, boxColour.green, boxColour.blue);
	};
	
	//life upgrade text
	char lifeUpgradeStr[STR_LENGTH];
	sprintf(lifeUpgradeStr, "Buy Life", lives);
	drawString(lifeUpgradeStr, 20, 230, 16, 255, 255, 255);
	
	//draw life upgrade rectangles
	boxColour.red = 255;
	boxColour.green = 255;
	boxColour.blue = 255;
	
	for(i = 0; i < MAX_LIVES; i++){
		if(i >= lives){
			boxColour.green = 0;
			boxColour.blue = 0;
		};
		drawRectangle(164 + i * 25, 226, 164 + i * 25 + 16, 250, boxColour.red, boxColour.green, boxColour.blue);
	};
	
	char buildingUpgradeStr[STR_LENGTH];
	sprintf(buildingUpgradeStr, "Repair Building (%d left)", numberOfBuildings);
	drawString(buildingUpgradeStr, 20, 170, 16, 255, 255, 255);
	
	char allBuildingsUpgradeStr[STR_LENGTH];
	sprintf(allBuildingsUpgradeStr, "Repair all Buildings");
	drawString(allBuildingsUpgradeStr, 20, 110, 16, 255, 255, 255);
	
	char buildingNumberStr[STR_LENGTH];
	sprintf(buildingNumberStr, "%d Pts", 50 - numberOfBuildings);
	strcpy(nextWaveButtons[5].text, buildingNumberStr);
	
	//render buttons
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(nextWaveButtons[i].alive == 1){
			renderButton(&nextWaveButtons[i]);
		};
	};
	
	//draw mouse 
	drawMouse();
};

//upgrade damage
void upgradeDamage(){
	//upgrade if enough points
	if(upgradePoints >= 5 && damage < 10){
		upgradePoints -= 5;
		damage += 1;
	};
};

//upgrade battery
void upgradeBattery(){
	//upgrade if enough points
	if(upgradePoints >= 5 && battery < 10){
		upgradePoints -= 5;
		battery += 1;
	};
};

//upgrade life
void upgradeLife(){
	//upgrade if enough points
	if(upgradePoints >= 15 && lives < MAX_LIVES){
		upgradePoints -= 15;
		lives += 1;
	};
};

//upgrade building
void upgradeBuilding(){
	//upgrade buildings
	if(upgradePoints >= 1 && numberOfBuildings < 50){
		//spend points
		upgradePoints -= 1;
		
		//make first building alive again
		int i;
		
		for(i = 0; i < 51; i++){
			if(terrain[i].alive == 0 && terrain[i].type == BUILDING){
				terrain[i].alive = 1;
				terrain[i].hp = 1;
				numberOfBuildings += 1;
				break;
			};
		};
	};
};
	
//upgrade all buildings
void upgradeAllBuildings(){
	//upgrade all buildings
	if(upgradePoints >= 50 - numberOfBuildings){
		//spend points
		upgradePoints -= (50 - numberOfBuildings);
		
		//set all buildings to alive
		int i;
		
		for(i = 0; i < 51; i++){
			if(terrain[i].alive == 0 && terrain[i].type == BUILDING){
				terrain[i].alive = 1;
				terrain[i].hp = 1;
				numberOfBuildings += 1;
			};
		};
	};		
};