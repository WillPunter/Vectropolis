/*
	main.h
	
	This is the main header file for Vectropolis. It contains all of the headers, enumerations, structures, function declarations and global variale declarations.
	
	This project uses a unity build system, so all of the files are included directly into main.c
*/

//include header files
#include "resource.h"

#include <Windows.h>
#include <xaudio2.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <direct.h>

//define constants
#define BUFFER_WIDTH 640
#define BUFFER_HEIGHT 480

#define GAME_TITLE "Vectropolis"

#define NUMBER_OF_LIGHTS 64
#define NUMBER_OF_ENTITIES 64
#define NUMBER_OF_BUTTONS 16
#define NUMBER_OF_KEYS 256
#define NUMBER_OF_STARS 1024
#define NUMBER_OF_PARTICLES 1024
#define NUMBER_OF_SFX_SRC_VOICES 16
#define NUMBER_OF_HIGH_SCORES 10
#define MAX_LIVES 10

#define PLAYER_START_X 0
#define PLAYER_START_Y 100
#define PLAYER_START_Z 0

#define STR_LENGTH 128
#define FONT_SIZE 8

#define TRANSPARENT_RED 163
#define TRANSPARENT_GREEN 73
#define TRANSPARENT_BLUE 164

#define PI 3.141592

//enumeration
enum AXES {
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS
};

//light types
enum LIGHT_TYPES {
	POINT_LIGHT = 0,
	DIRECTIONAL_LIGHT,
	AMBIENT_LIGHT
};

//terrain types
enum TERRAIN_TYPES {
	FLOOR = 0,
	BUILDING
};

//enemy types
enum ENEMY_TYPES {
	BOMBER = 0,
	CHASER,
	HEAVY_BOMBER
};

//game states
enum STATES {
	MENU = 0,
	GAME,
	NEXT_WAVE,
	GAME_OVER,
	NEW_HIGH_SCORE,
	LEADER_BOARD,
	SETTINGS,
	CONTROL_CHOICE
};

//entity states
enum ENTITY_STATES {
	ALIVE =0,
	DEAD
};

//fade states
enum FADE_STATES {
	NONE = 0,
	FADE_OUT,
	FADE_IN
};

//control states
enum CONTROL_STATES {
	KEYBOARD = 0,
	MOUSE
};

//pixel structure
typedef struct Pixel {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} Pixel;

//window structure
typedef struct Window {
	WNDCLASSEX windowClass;
	HWND handle;
	HDC deviceContext;
	BITMAPINFO bitmapInfo;
	RECT frameRect;
	Pixel * renderBuffer;
	int width;
	int height;
	int scale;
	int isFocused;
	uint8_t transparentRed;
	uint8_t transparentGreen;
	uint8_t transparentBlue;
	double fov;
	double screenDistance;
	double depthBuffer[BUFFER_WIDTH][BUFFER_HEIGHT];
} Window;

//game clock structure
typedef struct GameClock {
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER clockFrequency;
	double deltaTime;
} GameClock;

//bitmap structure
typedef struct Bitmap {
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	Pixel * pixels;
} Bitmap;

//sprite
typedef struct Sprite {
	Bitmap * bitmap;
	int srcX1;
	int srcX2;
	int srcY1;
	int srcY2;
	int originX;
	int originY;
	double xScale;
	double yScale;
	double angle;
	int solidColour;
	Pixel colour;
} Sprite;

//vector4f
typedef struct Vector4f {
	double x;
	double y;
	double z;
	double w;
} Vector4f;

//matrix 4x4f
typedef struct Matrix4x4f {
	double data[4][4];
} Matrix4x4f;

//triangle
typedef struct Triangle {
	Vector4f vertices[3];
	double intensities[3];
} Triangle;

//mesh
typedef struct Mesh {
	int numberOfVertices;
	int numberOfTriangles;
	Vector4f * vertices;
	Triangle * triangles;
} Mesh;

//model
typedef struct Model {
	Mesh * mesh;
	Vector4f scale;
	Vector4f rotation;
	Pixel colour;
} Model;

//camera
typedef struct Camera {
	Vector4f position;
	Vector4f rotation;
	Vector4f scale;
} Camera;

//light
typedef struct Light {
	Vector4f position;
	Vector4f direction;
	double intensity;
	int type;
	int alive;
} Light;

//entity
typedef struct Entity {
	int alive;
	Vector4f position;
	Vector4f rotation;
	Vector4f velocity;
	Vector4f size;
	double reloadTimer;
	double reloadTime;
	int hp;
	int maxHp;
	int type;
	double speed;
	double angularSpeed;
	double range;
	double altitude;
	struct Entity * target;
	Vector4f targetOffset;
	int state;
	double deathTimer;
	void (*updateProc)(struct Entity *);
	Model model;
} Entity;

//particle
typedef struct Particle {
	int alive;
	Vector4f position;
	Vector4f velocity;
	double timeToLive;
	Pixel colour;
} Particle;

//button
typedef struct Button {
	char text[STR_LENGTH];
	double x;
	double y;
	double width;
	double height;
	int fontSize;
	Pixel bgColour;
	Pixel fgColour;
	Pixel fgColourHighlight;
	int hovering;
	int alive;
	void (*onClick)();
} Button;

//key press
typedef struct KeyPress {
	int isDown;
	double timePressed;
} KeyPress;

//sound structure
typedef struct Sound {
	WAVEFORMATEX waveFormat;
	XAUDIO2_BUFFER buffer;
} Sound;

//sound handler
typedef struct SoundHandler {
	IXAudio2 * xaudio2;
	IXAudio2MasteringVoice * masteringVoice;
	IXAudio2SourceVoice * musicSrcVoice;
	IXAudio2SourceVoice * sfxSrcVoices[NUMBER_OF_SFX_SRC_VOICES];
	Sound * currentMusic;
	int srcVoiceSelecter;
} SoundHandler;

//save
typedef struct Save {
	char highScoreNames[NUMBER_OF_HIGH_SCORES][STR_LENGTH];
	int highScoreScores[NUMBER_OF_HIGH_SCORES];
	double musicVolume;
	double soundVolume;
	double xSensitivity;
	double ySensitivity;
} Save;

//declare functions
LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
void fadeStartNewGame();
void fadeNextWave();
void fadeGameOver();
void fadeMenu();
void fadeLeaderboard();
void fadeSettings();
void fadeControlChoice();
void fadeHighScore();
void fadeSubmitHighScore();
void unpause();
void setKeyboardControl();
void setMouseControl();
void generateStars();
void generateBuildings();
void generateLights();
void initialiseMenuScene();
void startGame();
void startNewArcadeGame();
void endArcadeGame();
void startWave(int n);
void startNextWaveMenu();
void startNextWave();
void upgradeDamage();
void upgradeBattery();
void upgradeLife();
void upgradeBuilding();
void upgradeAllBuildings();
void startMenu();
void startHighScore();
void submitHighScore();
void startLeaderboard();
void startSettings();
void startControlChoice();
void endGame();
void handleEvents();
void updateMenu();
void updateGame();
void updateNextWave();
void updateGameOver();
void updateNewHighScore();
void updateLeaderboard();
void updateSettings();
void updateControlChoice();
void update();
void renderMenu();
void renderGame();
void renderNextWave();
void renderGameOver();
void renderNewHighScore();
void renderLeaderboard();
void renderSettings();
void renderControlChoice();
void render();
void mainLoop();

//loading functions
void loadMeshFromFile(char filePath[], Mesh * mesh);
void loadMeshFromResource(int resourceID, Mesh * mesh);
void loadBitmapFromFile(char filePath[], Bitmap * bitmap);
void loadBitmapFromResource(int resourceID, Bitmap * bitmap);
void loadSoundFromFile(char filePath[], Sound * sound);
void loadSoundFromResource(int resourceID, Sound * sound);

//light functions
void initialiseLight(double x, double y, double z, double dirX, double dirY, double dirZ, double intensity, int type);

//player functions
void initialisePlayer(Entity * player, double x, double y, double z);
void updatePlayer(Entity * player);

//terrain functions
void initialiseTerrain(double x, double y, double z, double width, double height, double length, int type);

//update terrain
void updateTerrain(Entity * terrain);

//bullet functions
void initialiseBullet(double x, double y, double z, double xAngle, double yAngle, double zAngle);

//update bullet
void updateBullet(Entity * bullet);

//enemy functions
void initialiseEnemy(double x, double y, double z, int type);

//rotate towards target
void rotateTowardsTarget(double * angle, double targetAngle, double angularSpeed);

//update enemy
void updateBomber(Entity * enemy);

//update chaser
void updateChaser(Entity * enemy);

//enemy bullet functions
void initialiseEnemyBullet(double x, double y, double z, double xAngle, double yAngle, double zAngle, double width, double height, double length, double speed);

//update enemy bullet
void updateEnemyBullet(Entity * enemy);

//debris functions
void initialiseDebris(double x, double y, double z, uint8_t red, uint8_t green, uint8_t blue);

//update debris
void updateDebris(Entity * debris);

//particle functions
void initialiseParticle(double timeToLive, double x, double y, double z, double vx, double vy, double vz, uint8_t red, uint8_t green, uint8_t blue);

//update particle
void updateParticle(Particle * particle);

//render particle
void renderParticle(Vector4f position, Pixel colour, Matrix4x4f cameraTransform);

//initialise button
void initialiseButton(Button buttons[NUMBER_OF_BUTTONS], char text[STR_LENGTH], double x, double y, double width, double height, int fontSize, void (*onClick)(), uint8_t bgRed, uint8_t bgGreen, uint8_t bgBlue, uint8_t fgRed1, uint8_t fgGreen1, uint8_t fgBlue1, uint8_t fgRed2, uint8_t fgGreen2, uint8_t fgBlue2);

//update button
void updateButton(Button * button);

//render button
void renderButton(Button * button);

//mathematical functions
double vectorMagnitude(Vector4f v);
Vector4f normaliseVector(Vector4f v);
Vector4f addVectors(Vector4f a, Vector4f b);
Vector4f subtractVectors(Vector4f a, Vector4f b);
Vector4f scalarProduct(Vector4f a, double k);
double dotProduct(Vector4f a, Vector4f b);
Vector4f crossProduct(Vector4f a, Vector4f b);
Matrix4x4f addMatrices(Matrix4x4f a, Matrix4x4f b);
Matrix4x4f subtractMatrices(Matrix4x4f a, Matrix4x4f b);
Matrix4x4f matrixScalarProduct(Matrix4x4f m, double k);
Vector4f matrixVectorProduct(Matrix4x4f m, Vector4f v);
Matrix4x4f matrixProduct(Matrix4x4f a, Matrix4x4f b);
Matrix4x4f createIdentityMatrix();
Matrix4x4f createEnlargementMatrix(Vector4f scale);
Matrix4x4f createXRotationMatrix(double angle);
Matrix4x4f createYRotationMatrix(double angle);
Matrix4x4f createZRotationMatrix(double angle);
Matrix4x4f createRotationMatrix(Vector4f rotation);
Matrix4x4f createCameraRotationMatrix(Vector4f rotation);
Matrix4x4f createTransformationMatrix(Vector4f enlargement, Vector4f rotation, Vector4f position);
Triangle transformTriangle(Triangle triangle, Matrix4x4f transform);
double get2dLineIntersection(double s1x, double s1y, double d1x, double d1y, double s2x, double s2y, double d2x, double d2y, int * doesIntersect);
double getLinePlaneIntersection(Vector4f s, Vector4f d, Vector4f r, Vector4f n);
Vector4f directionToAngle(Vector4f d);
int checkBasicEntityCollision(Entity * e1, Entity * e2, double tolerance);
int checkAdvancedEntityCollision(Entity * e1, Entity * e2, double tolerance);
Entity * checkTerrainCollision(Entity * e);
double standardiseAngle(double a);

//render functions
void resetDepthBuffer();
void drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
void drawLine(int x1, int y1, int x2, int y2, uint8_t red, uint8_t green, uint8_t blue);
void drawWireframeTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t red, uint8_t green, uint8_t blue);
void drawInterpolatedRow(int x1, int x2, int y, double i1, double i2, double depth1, double depth2, uint8_t red, uint8_t green, uint8_t blue);
void drawShadedTriangle(int x1, int y1, int x2, int y2, int x3, int y3, double i1, double i2, double i3, double d1, double d2, double d3, uint8_t red, uint8_t green, uint8_t blue);
void drawRectangle(int x1, int y1, int x2, int y2, uint8_t red, uint8_t green, uint8_t blue);
void drawBitmap(Bitmap * bitmap, int x, int y);
void drawSprite(Sprite * sprite, int x, int y);
void drawCharacter(char c, int x, int y, int size, uint8_t red, uint8_t green, uint8_t blue);
void drawString(char str[STR_LENGTH], int x, int y, int size, uint8_t red, uint8_t green, uint8_t blue);
void drawModel(Model * model, Vector4f position, Matrix4x4f cameraTransform, Vector4f cameraPosition);
void cullBackFaces(Triangle t, Pixel colour);
int clipPlane(Triangle t, Vector4f p0, Vector4f n, int greater, int compareAxis, Triangle * t1, Triangle * t2);
void clipScreenPlane(Triangle t, Pixel colour);
void clipLeftPlane(Triangle t, Pixel colour);
void clipRightPlane(Triangle t, Pixel colour);
void clipTopPlane(Triangle t, Pixel colour);
void clipBottomPlane(Triangle t, Pixel colour);
void projectTriangle(Triangle t, Pixel colour);
double lightVertex(Vector4f v, Vector4f normal);
void shadeAndDrawTriangle(Triangle projectedTriangle, Triangle original, Pixel colour);
void drawMenuBG();
void drawMouse();

//sound functions
HRESULT initialiseSoundEngine();
void playSound(Sound * sound);
void stopSound();
void setSoundVolume();
void soundVolumeUp();
void soundVolumeDown();
void playMusic(Sound * sound);
void stopMusic();
void setMusicVolume();
void musicVolumeUp();
void musicVolumeDown();

//miscellaneous settings functions
void xSensitivityUp();
void xSensitivityDown();
void ySensitivityUp();
void ySensitivityDown();

//save functions
void writeSave(Save * save);
int loadSave(Save * save);
void sortSaveScores(Save * save, int ascending);
void addScore(Save * save, char name[STR_LENGTH], int score);

//create global variables
int running = 1;
Window window = {0};
GameClock gameClock;
Vector4f model_position = {0};
Camera camera = {0};
Light lights[NUMBER_OF_LIGHTS];
Light transformedLights[NUMBER_OF_LIGHTS];
Entity playerInstance;
Entity terrain[NUMBER_OF_ENTITIES];
Entity bullets[NUMBER_OF_ENTITIES];
Entity enemies[NUMBER_OF_ENTITIES];
Entity enemyBullets[NUMBER_OF_ENTITIES];
Entity debris[NUMBER_OF_ENTITIES];
Button menuButtons[NUMBER_OF_BUTTONS];
Button nextWaveButtons[NUMBER_OF_BUTTONS];
Button gameOverButtons[NUMBER_OF_BUTTONS];
Button newHighScoreButtons[NUMBER_OF_BUTTONS];
Button leaderboardButtons[NUMBER_OF_BUTTONS];
Button settingsButtons[NUMBER_OF_BUTTONS];
Button controlChoiceButtons[NUMBER_OF_BUTTONS];
Button pauseButtons[NUMBER_OF_BUTTONS];
KeyPress keys[NUMBER_OF_KEYS];
Particle particles[NUMBER_OF_PARTICLES];
Vector4f stars[NUMBER_OF_STARS];
Pixel starColours[NUMBER_OF_STARS];
SoundHandler soundHandler = {0};
double mouseX;
double mouseY;
double deltaX;
double deltaY;
int state;
int wave;
int numberOfEnemies;
int numberOfBuildings;
int lives;
Save gameSave = {0};
int score;
char nameStr[STR_LENGTH];
int nameStrIndex = 0;
double cursorBlinkTimer = 1;
int upgradePoints = 0;
int damage;
int battery;
double currentBattery;
double fade = 1;
int fadeState = 0;
void (*fadeProc)();
double fadeRate = 4;
double endWaveTimer = 3;
int musicOn = 1;
int soundOn = 1;
double controlType = KEYBOARD;
Pixel menuStarColours[NUMBER_OF_STARS];
Camera menuCamera;
char gameOverFeedbackText[STR_LENGTH];
int pause = 0;
double instructionTextTimer = 3;
double instructionTextYOffset = 0;

//declare bitmaps
Bitmap bmp_font;
Bitmap bmp_life;
Bitmap bmp_cursor;
Sprite spr_player;

//declare mesh structures
Mesh mesh_cube;
Mesh mesh_spaceship;
Mesh mesh_enemy;
Mesh mesh_chaser;
Mesh mesh_heavyBomber;
Mesh mesh_debris;

//declare sound structures
Sound sound_shoot;
Sound sound_explosion;
Sound sound_buildingDestroy;
Sound sound_enemyShoot;
Sound sound_bossExplosion;
Sound sound_select;
Sound music_menu;
Sound music_game;