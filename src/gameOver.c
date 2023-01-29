/*
	gameOver.c
	
	This file contains all of the code for the game over state.
*/

//fade game over
void fadeGameOver(){
	//check that not already fading
	if(fadeState == NONE){
		//fade out
		fadeState = FADE_OUT;
		fadeProc = &endArcadeGame;
	};
};

//update game over
void updateGameOver(){
	//update buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(gameOverButtons[i].alive == 1){
			updateButton(&gameOverButtons[i]);
		};
	};
};

//render game over
void renderGameOver(){
	//draw menu background
	drawMenuBG();
	
	//draw entry text
	char menuText[STR_LENGTH] = "Game Over";
	drawString(menuText, (BUFFER_WIDTH - strlen(menuText) * 24) / 2, 400, 24, 255, 255, 255);
	
	//draw game over text
	drawString(gameOverFeedbackText, (BUFFER_WIDTH - strlen(gameOverFeedbackText) * 16) / 2, 300, 16, 255, 255, 255);
	
	//render buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(gameOverButtons[i].alive == 1){
			renderButton(&gameOverButtons[i]);
		};
	};
	
	//draw mouse 
	drawMouse();
};