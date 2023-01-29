/*
	highScore.c
	
	This file contains all of the code related to the new high score state
*/

//fade high score
void fadeHighScore(){
	//check that not already fading
	if(fadeState == NONE){
		//fade to control choice
		fadeState = FADE_OUT;
		fadeProc = &startHighScore;
	};
};

//fade submit high score
void fadeSubmitHighScore(){
	//check that not already fading
	if(fadeState == NONE){
		//fade to control choice
		fadeState = FADE_OUT;
		fadeProc = &submitHighScore;
	};
};

//start high score
void startHighScore(){
	//set state to new high score
	state = NEW_HIGH_SCORE;
	
	//clear string
	memset(nameStr, 0, STR_LENGTH);
	nameStrIndex = 0;
};

//submit highscore
void submitHighScore(){
	//ask for data
	addScore(&gameSave, nameStr, score);
	startLeaderboard();
};

//update new high score
void updateNewHighScore(){
	//update buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(newHighScoreButtons[i].alive == 1){
			updateButton(&newHighScoreButtons[i]);
		};
	};
};

//render new high score
void renderNewHighScore(){
	//draw menu bg
	drawMenuBG();
	
	//draw title text
	char titleText[STR_LENGTH] = "New High Score";
	drawString(titleText, (BUFFER_WIDTH - strlen(titleText) * 24) / 2, 440, 24, 255, 255, 255);
	
	//draw instruction text
	char instructionText[STR_LENGTH] = "Please enter your name below:";
	drawString(instructionText, (BUFFER_WIDTH - strlen(instructionText) * 16) / 2, 380, 16, 255, 255, 255);
	
	//draw background rectangle
	drawRectangle(126, BUFFER_HEIGHT / 2 - 8, BUFFER_WIDTH - 126, BUFFER_HEIGHT / 2 + 24, 255, 255, 255);
	drawRectangle(128,BUFFER_HEIGHT / 2 - 6, BUFFER_WIDTH - 128, BUFFER_HEIGHT / 2 + 22, 0, 0, 0);
	
	//draw name
	drawString(nameStr, (BUFFER_WIDTH - strlen(nameStr) * 16) / 2, BUFFER_HEIGHT / 2, 16, 255, 255, 255);
	
	//decrease cursor timer
	cursorBlinkTimer -= gameClock.deltaTime;
	
	//correct blink timer 
	if(cursorBlinkTimer < 0){
		cursorBlinkTimer = 1;
	};
	
	//display cursor for 1/2 a second periodically
	if(cursorBlinkTimer < 0.5){
		//draw cursor
		drawRectangle((BUFFER_WIDTH - strlen(nameStr) * 16) / 2 + nameStrIndex * 16, BUFFER_HEIGHT / 2, (BUFFER_WIDTH - strlen(nameStr) * 16) / 2 + nameStrIndex * 16 + 1, BUFFER_HEIGHT / 2 + 16, 255, 255, 255);
	};
	
	//render buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(newHighScoreButtons[i].alive == 1){
			renderButton(&newHighScoreButtons[i]);
		};
	};
	
	//draw mouse 
	drawMouse();
};