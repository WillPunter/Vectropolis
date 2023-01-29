/*
	menu.c
	
	This file provides all of the code related to the menu state.
*/

//fade menu
void fadeMenu(){
	//check that not already fading
	if(fadeState == NONE){
		//fade to menu
		fadeState = FADE_OUT;
		fadeProc = &startMenu;
	};
};

//generate stars
void generateStars(){
	//create stars
	int i;
	for(i = 0; i < NUMBER_OF_STARS; i++){
		//randomise positions (at least 10000 units away from the player)
		stars[i].x = 1000 + rand() % 10000;
		stars[i].y = 1000 + rand() % 10000;
		stars[i].z = 1000 + rand() % 10000;
		
		//randomise directions
		if(rand() % 2 == 1){
			stars[i].x = -stars[i].x;
		};
		
		if(rand() % 2 == 1){
			stars[i].y = -stars[i].y;
		};
		
		if(rand() % 2 == 1){
			stars[i].z = -stars[i].z;
		};
		
		//randomise colours
		starColours[i].red = 200 + rand() % 56;
		starColours[i].green = 200 + rand() % 56;
		starColours[i].blue = 200 + rand() % 56;
	};
};

//initialise menu scene
void initialiseMenuScene(){
	//generate lights
	generateLights();
	
	//create buildings
	generateBuildings();
	
	//generate stars
	generateStars();
};

//start menu
void startMenu(){
	//save changes
	writeSave(&gameSave);
	
	//generate menu scene
	initialiseMenuScene();
	
	//set state to menu
	state = MENU;
};

//update menu
void updateMenu(){
	//update buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(menuButtons[i].alive == 1){
			updateButton(&menuButtons[i]);
		};
	};
	
	//play game music
	if(musicOn){
		playMusic(&music_menu);
	};
};

//render menu
void renderMenu(){
	//draw menu bg
	drawMenuBG();
	
	//draw entry text
	char menuText[STR_LENGTH] = GAME_TITLE;
	drawString(menuText, (BUFFER_WIDTH - strlen(menuText) * 32) / 2, 380, 32, 255, 255, 255);
	
	//render buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(menuButtons[i].alive == 1){
			renderButton(&menuButtons[i]);
		};
	};
	
	//draw mouse 
	drawMouse();
};

//end game
void endGame(){
	running = 0;
};