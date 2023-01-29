/*
	button.c
	
	This file provides code for initialising and updating buttons.
*/

//initialise button
void initialiseButton(Button buttons[NUMBER_OF_BUTTONS], char text[STR_LENGTH], double x, double y, double width, double height, int fontSize, void (*onClick)(), uint8_t bgRed, uint8_t bgGreen, uint8_t bgBlue, uint8_t fgRed1, uint8_t fgGreen1, uint8_t fgBlue1, uint8_t fgRed2, uint8_t fgGreen2, uint8_t fgBlue2){
	//search for vacant slot
	int i;
	
	for(i = 0; i < NUMBER_OF_BUTTONS; i++){
		if(buttons[i].alive == 0){
			//fill vacant slot
			buttons[i].alive = 1;
			strcpy(buttons[i].text, text);
			buttons[i].x = x;
			buttons[i].y = y;
			buttons[i].width = width;
			buttons[i].height = height;
			buttons[i].fontSize = fontSize;
			buttons[i].onClick = onClick;
			buttons[i].bgColour.red = bgRed;
			buttons[i].bgColour.green = bgGreen;
			buttons[i].bgColour.blue = bgBlue;
			buttons[i].fgColour.red = fgRed1;
			buttons[i].fgColour.green = fgGreen1;
			buttons[i].fgColour.blue = fgBlue1;
			buttons[i].fgColourHighlight.red = fgRed2;
			buttons[i].fgColourHighlight.green = fgGreen2;
			buttons[i].fgColourHighlight.blue = fgBlue2;
			break;
		};
	};
};
//update button
void updateButton(Button * button){
	//check if mouse is hovering over button
	if(mouseX > button->x - button->width / 2 && mouseX < button->x + button->width / 2 && mouseY > button->y - button->height / 2 && mouseY < button->y + button->height / 2){
		button->hovering = 1;
		
		if(keys[VK_LBUTTON].isDown && keys[VK_LBUTTON].timePressed == 0){
			//play sound
			playSound(&sound_select);
			
			//execute click function
			button->onClick();
		};
	} else {
		button->hovering = 0;
	};
};

//render button
void renderButton(Button * button){
	//check if hovering
	Pixel textColour;
		
	if(button->hovering){
		textColour = button->fgColourHighlight;
	} else {
		textColour = button->fgColour;
	};
	
	//draw outline
	drawRectangle(button->x - button->width / 2 - 2, button->y - button->height / 2 - 2, button->x + button->width / 2 + 2, button->y + button->height / 2 + 2, textColour.red, textColour.green, textColour.blue);
	
	//draw background
	drawRectangle(button->x - button->width / 2, button->y - button->height / 2, button->x + button->width / 2, button->y + button->height / 2, button->bgColour.red, button->bgColour.green, button->bgColour.blue);
	
	//draw text
	int textWidth = strlen(button->text) * button->fontSize;
	drawString(button->text, button->x - textWidth / 2, button->y - button->fontSize / 2, button->fontSize, textColour.red, textColour.green, textColour.blue);
};