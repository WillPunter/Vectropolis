/*
	settings.c
	
	This file provides all of the code for the settings state.
*/

//fade settings
void fadeSettings(){
	//check that not already fading
	if(fadeState == NONE){
		//fade to settings
		fadeState = FADE_OUT;
		fadeProc = &startSettings;
	};
};

//start settings
void startSettings(){
	//set state to settings
	state = SETTINGS;
};

//update settings
void updateSettings(){
	//update buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(settingsButtons[i].alive == 1){
			updateButton(&settingsButtons[i]);
		};
	};
};

//render settings
void renderSettings(){
	//draw menu bg
	drawMenuBG();
	
	//draw background rectangle
	drawRectangle(30, 178, BUFFER_WIDTH - 30, 402, 255, 255, 255);
	drawRectangle(32, 180, BUFFER_WIDTH - 32, 400, 0, 0, 0);
	
	//draw title text
	char titleText[STR_LENGTH] = "Settings";
	drawString(titleText, (BUFFER_WIDTH - strlen(titleText) * 24) / 2, 440, 24, 255, 255, 255);
	
	//draw music text
	char musicText[STR_LENGTH] = "Music";
	drawString(musicText, 40, 360, 16, 255, 255, 255);
	
	//draw volume
	char musicVolumeStr[STR_LENGTH];
	sprintf(musicVolumeStr, "%d", (int) round(10.f * gameSave.musicVolume));
	drawString(musicVolumeStr, 180 + (BUFFER_WIDTH - strlen(musicVolumeStr) * 16) / 2, 360, 16, 255, 255, 255);
	
	//draw sound text
	char soundText[STR_LENGTH] = "Sound";
	drawString(soundText, 40, 312, 16, 255, 255, 255);
	
	//draw volume
	char soundVolumeStr[STR_LENGTH];
	sprintf(soundVolumeStr, "%d", (int) round(10.f * gameSave.soundVolume));
	drawString(soundVolumeStr, 180 + (BUFFER_WIDTH - strlen(soundVolumeStr) * 16) / 2, 312, 16, 255, 255, 255);
	
	//draw x sensitivity
	char hSensitivityText[STR_LENGTH] = "Horizontal Sensitivity";
	drawString(hSensitivityText, 40, 264, 16, 255, 255, 255);
	
	//draw x sensitivity
	char xSensitivityStr[STR_LENGTH];
	sprintf(xSensitivityStr, "%d", (int) round(10.f * gameSave.xSensitivity));
	drawString(xSensitivityStr, 180 + (BUFFER_WIDTH - strlen(xSensitivityStr) * 16) / 2, 264, 16, 255, 255, 255);
	
	//draw y sensitivity
	char vSensitivityText[STR_LENGTH] = "Vertical Sensitivity";
	drawString(vSensitivityText, 40, 216, 16, 255, 255, 255);
	
	//draw y sensitivity
	char ySensitivityStr[STR_LENGTH];
	sprintf(ySensitivityStr, "%d", (int) round(10.f * gameSave.ySensitivity));
	drawString(ySensitivityStr, 180 + (BUFFER_WIDTH - strlen(ySensitivityStr) * 16) / 2, 216, 16, 255, 255, 255);
	
	//render buttons
	int i;
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(settingsButtons[i].alive == 1){
			renderButton(&settingsButtons[i]);
		};
	};
	
	//draw mouse 
	drawMouse();
};