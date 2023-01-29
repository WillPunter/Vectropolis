/*
	Vectropolis
	
	Vectropolis is a 3d space shooter where you have to defend a city from waves of invading ships.
	This project was written from scratch (as much as possible) in C, using the Win32 API and XAudio2 for sound.
	You may used this code however you would like. Credit would be appreciated but is not required.

	Programmed by Will Punter in 2023
	
	Compile this program using MinGW using the following commands, or run the build.bat script in the same folder as this file:
	windres res.rc -o resource
	gcc main.c resource -mwindows -lgdi32 -lxaudio2_8 -lole32 -o game
	
	This file includes the windows entry point (WinMain), the window procedure (the event handling function for Win32 apps), and the core gameplay logic (i.e. the main loop)
*/

//include main header
//the main header includes all of the headers, enumerations, structures, function declarations and global variable declarations
#include "main.h"

//include other files
#include "loadResources.c"
#include "mathematics.c"
#include "light.c"
#include "render.c"
#include "player.c"
#include "terrain.c"
#include "bullet.c"
#include "enemy.c"
#include "enemyBullet.c"
#include "debris.c"
#include "particle.c"
#include "button.c"
#include "game.c"
#include "menu.c"
#include "nextWave.c"
#include "gameOver.c"
#include "leaderboard.c"
#include "settings.c"
#include "controlChoice.c"
#include "highScore.c"
#include "sound.c"

//window procedure
LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam){
	//check message type
	switch(message){
		//destroy
		case WM_DESTROY:{
			PostQuitMessage(0);
			running = 0;
			return 0;
		};
		
		//resize
		case WM_SIZE:{
			//get client rectangle
			RECT clientRect;
			GetClientRect(window.handle, &clientRect);
			window.width = clientRect.right - clientRect.left;
			window.height = clientRect.bottom - clientRect.top;
			
			//calculate scale
			window.scale = 1;
			
			while(BUFFER_WIDTH * window.scale <= window.width && BUFFER_HEIGHT * window.scale <= window.height){
				window.scale += 1;
			};
			
			//window scale
			window.scale -= 1;
			
			return 0;
		};
		
		//key press
		case WM_KEYDOWN:{
			keys[wParam].isDown = 1;
			
			//check state
			if(state == NEW_HIGH_SCORE){
				//move indicator left or right
				if(wParam == VK_LEFT){
					if(nameStrIndex > 0){
						nameStrIndex -= 1;
					};
				} else if(wParam == VK_RIGHT){
					if(nameStrIndex < strlen(nameStr)){
						nameStrIndex += 1;
					};
				};
			};
			break;
		};
		
		//key release
		case WM_KEYUP:{
			keys[wParam].isDown = 0;
			break;
		};
		
		//left mouse down
		case WM_LBUTTONDOWN:{
			keys[VK_LBUTTON].isDown = 1;
			break;
		};
		
		//left mouse up
		case WM_LBUTTONUP:{
			keys[VK_LBUTTON].isDown = 0;
			break;
		};
		
		//right mouse down
		case WM_RBUTTONDOWN:{
			keys[VK_RBUTTON].isDown = 1;
			break;
		};
		
		//right mouse up
		case WM_RBUTTONUP:{
			keys[VK_RBUTTON].isDown = 0;
			break;
		};
		
		//text entry
		case WM_CHAR:{
			//check state
			if(state == NEW_HIGH_SCORE){
				//check letter
				if(wParam == VK_BACK){
					//get length
					int length = strlen(nameStr);
				
					//remove letter before nameStrIndex
					if(nameStrIndex > 0){
						//get rid of deleted character
						nameStr[nameStrIndex - 1] = 0;
						
						//shift all letters backwards
						int i;
					
						for(i = nameStrIndex; i < length; i++){
							nameStr[i - 1] = nameStr[i];
						};
					
						//add null terminator
						nameStr[i - 1] = 0;
						
						//move back a space
						nameStrIndex -= 1;
					};
				} else if(strlen(nameStr) < 16){
					//insert character at nameStrIndex
					//add to name string
					int i;
					
					//shift all letters from nameStrIndex forwards to make room for
					int length = strlen(nameStr);
					for(i = length - 1; i >= nameStrIndex; i--){
						nameStr[i + 1] = nameStr[i];
					};
					
					//add null terminator to the end
					nameStr[length + 1] = 0;
					
					//insert character into correct position
					nameStr[nameStrIndex] = wParam;
					nameStrIndex += 1;
				};
			};
			
			break;
		};
		
		//set focus
		case WM_SETFOCUS:{
			//set window to focused
			window.isFocused = 1;
			
			//hide cursor
			ShowCursor(FALSE);
			break;
		};
		
		//lose focus
		case WM_KILLFOCUS:{
			//set window to not focused
			window.isFocused = 0;
			
			//show cursor
			ShowCursor(TRUE);
			break;
		};
	};
	
	//default window procedure
	return DefWindowProc(windowHandle, message, wParam, lParam);
};

//entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmd, int nCmdShow){
	//seed random
	srand(time(NULL));
	
	//set window class metadata
	window.windowClass.cbSize = sizeof(WNDCLASSEX);
	window.windowClass.lpszClassName = "WINDOW_CLASS";
	window.windowClass.lpfnWndProc = &windowProcedure;
	window.windowClass.style = CS_HREDRAW | CS_VREDRAW;
	window.windowClass.hInstance = hInstance;
	window.windowClass.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(50, 50, 50));
	window.windowClass.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
	window.windowClass.hIconSm = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);
	
	//register window class
	RegisterClassEx(&window.windowClass);
	
	//set frame size
	RECT frameSize = {0, 0, BUFFER_WIDTH, BUFFER_HEIGHT};
	AdjustWindowRectEx(&frameSize, WS_OVERLAPPEDWINDOW, 0, WS_EX_OVERLAPPEDWINDOW);
	
	//create window
	window.handle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW, //extended style
		window.windowClass.lpszClassName, //window class name
		GAME_TITLE, //title text
		WS_OVERLAPPEDWINDOW, //default style
		CW_USEDEFAULT, CW_USEDEFAULT, //x and y
		frameSize.right - frameSize.left, frameSize.bottom - frameSize.top, //width and height
		0, //no parent
		0, //no menu
		hInstance, //application handle
		0 //no LPARAM
	);
	
	//check if window created
	if(window.handle){
		//show window
		ShowWindow(window.handle, nCmdShow);
		
		//set bitmap info metadata
		window.bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		window.bitmapInfo.bmiHeader.biWidth = BUFFER_WIDTH;
		window.bitmapInfo.bmiHeader.biHeight = BUFFER_HEIGHT;
		window.bitmapInfo.bmiHeader.biBitCount = sizeof(Pixel) * 8;
		window.bitmapInfo.bmiHeader.biCompression = BI_RGB;
		window.bitmapInfo.bmiHeader.biPlanes = 1;
		
		//allocate memory for render buffer
		window.renderBuffer = (Pixel *) malloc(BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(Pixel));
		
		//set device context
		window.deviceContext = GetDC(window.handle);
		
		//set transparent colour
		window.transparentRed = 163;
		window.transparentGreen = 73;
		window.transparentBlue = 164;
		
		//start game
		startGame();
	};
	
	//return 0
	return 0;
};

//handle events
void handleEvents(){
	//message
	MSG message;
	
	while(PeekMessage(&message, window.handle, 0, 0, PM_REMOVE)){
		//translate message
		TranslateMessage(&message);
		
		//dispatch message
		DispatchMessage(&message);
	};
	
	//get screen size
	RECT windowRect;
	GetClientRect(window.handle, &windowRect);
	GetWindowRect(window.handle, &window.frameRect);
		
	//check step
	if(state == GAME && !pause){
		//check that window is focused
		if(window.isFocused && controlType == MOUSE){
			//set the cursor to the centre of the screen
			SetCursorPos((window.frameRect.left + window.frameRect.right) / 2, (window.frameRect.top + window.frameRect.bottom) / 2);
		};
	} else {
		//get mouse coordinates relative to client
		POINT mouseCoords;
		GetCursorPos(&mouseCoords);
		ScreenToClient(window.handle, &mouseCoords);
	
		//flip to adjust for reversed direction of game coordinates
		mouseCoords.y = -mouseCoords.y;
		mouseCoords.x -= (windowRect.right - BUFFER_WIDTH) / 2;
		mouseCoords.y += (windowRect.bottom - BUFFER_HEIGHT) / 2;
		mouseCoords.y += BUFFER_HEIGHT;
		mouseX = mouseCoords.x;
		mouseY = mouseCoords.y;
	};
};

//update
void update(){
	//check state
	switch(state){
		//menu
		case MENU:{
			updateMenu();
			break;
		};
		
		//game update
		case GAME:{
			updateGame();
			break;
		};
		
		//update next wave
		case NEXT_WAVE:{
			updateNextWave();
			break;
		};
		
		//update game over
		case GAME_OVER:{
			updateGameOver();
			break;
		};
		
		//new high score
		case NEW_HIGH_SCORE:{
			updateNewHighScore();
			break;
		};
		
		//leaderboard
		case LEADER_BOARD:{
			updateLeaderboard();
			break;
		};
		
		//settings
		case SETTINGS:{
			updateSettings();
			break;
		};
		
		//control choice
		case CONTROL_CHOICE:{
			updateControlChoice();
			break;
		};
	};
	
	//fade
	if(fadeState == FADE_OUT){
		fade -= gameClock.deltaTime * fadeRate;
		
		if(fade <= 0){
			fade = 0;
			fadeState = FADE_IN;
			
			if(fadeProc){
				fadeProc();
			};
		};
	} else if(fadeState == FADE_IN){
		fade += gameClock.deltaTime * fadeRate;
		
		if(fade >= 1){
			fade = 1;
			fadeState = NONE;
		};
	};
};

//render
void render(){
	//clear screen
	memset(window.renderBuffer, 0, BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(Pixel));
	
	//check state
	switch(state){
		//menu
		case MENU:{
			renderMenu();
			break;
		};
		
		//game
		case GAME:{
			renderGame();
			break;
		};
		
		//next wave
		case NEXT_WAVE:{
			renderNextWave();
			break;
		};
		
		//game over
		case GAME_OVER:{
			renderGameOver();
			break;
		};
		
		//new high score
		case NEW_HIGH_SCORE:{
			renderNewHighScore();
			break;
		};
		
		//leaderboard
		case LEADER_BOARD:{
			renderLeaderboard();
			break;
		};
		
		//settings
		case SETTINGS:{
			renderSettings();
			break;
		};
		
		//control choice
		case CONTROL_CHOICE:{
			renderControlChoice();
			break;
		};
	};
	
	//check if fade != 1
	if(fade < 1){
		//fade out
		int i;
	
		for(i = 0; i < BUFFER_WIDTH * BUFFER_HEIGHT; i++){
			window.renderBuffer[i].red *= fade;
			window.renderBuffer[i].green *= fade;
			window.renderBuffer[i].blue *= fade;
		};
	};
	
	//send to window buffer
	StretchDIBits(window.deviceContext, (window.width - window.scale * BUFFER_WIDTH) / 2, (window.height - window.scale * BUFFER_HEIGHT) / 2, BUFFER_WIDTH * window.scale, BUFFER_HEIGHT * window.scale, 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT, window.renderBuffer, &window.bitmapInfo, BI_RGB, SRCCOPY);
};

//main loop
void mainLoop(){
	//iterate while running
	while(running){
		//get frame start time
		QueryPerformanceCounter(&gameClock.start);
		
		//handle events
		handleEvents();
	
		//update
		update();
	
		//render
		render();
		
		//get change in mouse coordinates
		POINT mouseCoords;
		GetCursorPos(&mouseCoords);
		deltaX = mouseCoords.x - ((window.frameRect.right + window.frameRect.left) / 2);
		deltaY = mouseCoords.y - ((window.frameRect.bottom + window.frameRect.top) / 2);
		
		//get key times
		int i;
		
		for(i = 0; i < NUMBER_OF_KEYS; i++){
			if(keys[i].isDown){
				keys[i].timePressed += gameClock.deltaTime;
			} else {
				keys[i].timePressed = 0;
			};
		};
		
		//get performance frequency
		QueryPerformanceFrequency(&gameClock.clockFrequency);
		
		//get frame end time
		QueryPerformanceCounter(&gameClock.end);
		
		//get delta time
		gameClock.deltaTime = (double) (gameClock.end.QuadPart - gameClock.start.QuadPart) / gameClock.clockFrequency.QuadPart;
	};
};