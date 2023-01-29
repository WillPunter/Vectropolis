/*
	controlChoice.c
	
	This file provides code for the control choice state (where the player picks their controls).
*/

//fade control choice
void fadeControlChoice(){
	//check that not already fading
	if(fadeState == NONE){
		//fade to control choice
		fadeState = FADE_OUT;
		fadeProc = &startControlChoice;
	};
};

//set keyboard control
void setKeyboardControl(){
	controlType = KEYBOARD;
	fadeState = FADE_OUT;
	fadeProc = &startNewArcadeGame;
};

//set mouse control
void setMouseControl(){
	controlType = MOUSE;
	fadeState = FADE_OUT;
	fadeProc = &startNewArcadeGame;
};

//start control settings
void startControlChoice(){
	//set state to control choice
	state = CONTROL_CHOICE;
};

//update control
void updateControlChoice(){
	//update buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(controlChoiceButtons[i].alive == 1){
			updateButton(&controlChoiceButtons[i]);
		};
	};
};

//render control choice
void renderControlChoice(){
	//draw menu bg
	drawMenuBG();
	
	//draw entry text
	char titleText[STR_LENGTH] = "Choose Your Controls";
	drawString(titleText, (BUFFER_WIDTH - strlen(titleText) * 16) / 2, 440, 16, 255, 255, 255);
	
	//draw background rectangle
	drawRectangle(30, 158, BUFFER_WIDTH - 30, 402, 255, 255, 255);
	drawRectangle(32, 160, BUFFER_WIDTH - 32, 400, 0, 0, 0);
	
	//draw keyboard text
	char keyboardText[STR_LENGTH] = "Keyboard";
	drawString(keyboardText, BUFFER_WIDTH / 4 - strlen(keyboardText) * 16 / 2, 360, 16, 255, 255, 255);
	
	char keyboardText2[STR_LENGTH] = "-WASD to Turn";
	drawString(keyboardText2, 32, 320, 12, 255, 255, 255);
	
	char keyboardText3[STR_LENGTH] = "-Space to Shoot";
	drawString(keyboardText3, 32, 280, 12, 255, 255, 255);
	
	char keyboardText4[STR_LENGTH] = "-Left Control to Boost";
	drawString(keyboardText4, 32, 240, 12, 255, 255, 255);
	
	char keyboardText5[STR_LENGTH] = "-Esc to Pause";
	drawString(keyboardText5, 32, 200, 12, 255, 255, 255);
	
	//draw mouse text
	char mouseText[STR_LENGTH] = "Mouse";
	drawString(mouseText, 3 * BUFFER_WIDTH / 4 - strlen(mouseText) * 16 / 2, 360, 16, 255, 255, 255);
	
	char mouseText2[STR_LENGTH] = "-Move mouse to turn";
	drawString(mouseText2, BUFFER_WIDTH - 32 - 22 * 12, 320, 12, 255, 255, 255);
	
	char mouseText3[STR_LENGTH] = "-Left mouse to shoot";
	drawString(mouseText3, BUFFER_WIDTH - 32 - 22 * 12, 280, 12, 255, 255, 255);
	
	char mouseText4[STR_LENGTH] = "-Right mouse to boost";
	drawString(mouseText4, BUFFER_WIDTH - 32 - 22 * 12, 240, 12, 255, 255, 255);
	
	char mouseText5[STR_LENGTH] = "-Esc to Pause";
	drawString(mouseText5, BUFFER_WIDTH - 32 - 22 * 12, 200, 12, 255, 255, 255);
	
	//render buttons
	int i;
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(controlChoiceButtons[i].alive == 1){
			renderButton(&controlChoiceButtons[i]);
		};
	};
	
	//draw mouse
	drawMouse();
};