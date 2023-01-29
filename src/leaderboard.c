/*
	leaderboard.c
	
	Thie file contains all of the code for the leaderboard state.
*/

//fade leaderboard
void fadeLeaderboard(){
	//check that not already fading
	if(fadeState == NONE){
		//fade to leaderboard
		fadeState = FADE_OUT;
		fadeProc = &startLeaderboard;
	};
};

//start leaderboard
void startLeaderboard(){
	//set state to leaderboard
	state = LEADER_BOARD;
};

//update leaderboard
void updateLeaderboard(){
	//update buttons
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(leaderboardButtons[i].alive == 1){
			updateButton(&leaderboardButtons[i]);
		};
	};
};

//render leaderboard
void renderLeaderboard(){
	//draw menu bg
	drawMenuBG();
	
	//draw background rectangle
	drawRectangle(30, 158, BUFFER_WIDTH - 30, 392, 255, 255, 255);
	drawRectangle(32, 160, BUFFER_WIDTH - 32, 390, 0, 0, 0);
	
	//draw entry text
	char menuText[STR_LENGTH] = "Leaderboard";
	drawString(menuText, (BUFFER_WIDTH - strlen(menuText) * 16) / 2, 440, 16, 255, 255, 255);
	
	//sort scores
	sortSaveScores(&gameSave, 0);
	
	//draw all leaderboard scores
	int i;
	for(i = NUMBER_OF_HIGH_SCORES - 1; i >= 0; i--){
		char line[STR_LENGTH];
		sprintf(line, "%s - %d", gameSave.highScoreNames[i], gameSave.highScoreScores[i]);
		drawString(line, (BUFFER_WIDTH - strlen(line) * 16) / 2, 360 - 20 * i, 16, 255, 255, 255);
	};
	
	//render buttons
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(leaderboardButtons[i].alive == 1){
			renderButton(&leaderboardButtons[i]);
		};
	};
	
	//draw mouse 
	drawMouse();
};