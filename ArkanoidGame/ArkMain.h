#pragma once
#ifndef ARKMAIN_H
#define ARKMAIN_H

#include "SDL\SDL.h"
#include "SDL\SDL_image.h"
#include "Const.h"
#include <stdio.h>


int brickSide, wallSide, livesCounter, levelCounter;

int paddlestick;

time_t t;
Uint32 frametimes[FRAME_VALUES];
Uint32 frametimelast;
Uint32 framecount;

float framespersecond;

float delta;

int init();
int loadMedia();
void loadLevel(char* path);
void close();

void renderBrick(SDL_Rect* brick, int x, int y, int tilePos);
void renderBall();
void renderPaddle();
void renderLives(SDL_Rect* live,int x, int y);
void drawLives();

void moveBall(float delta);
void movePaddle(float shift, float delta);

void resetLevel();

void changeDirection(float dirx, float diry);

int PadCollision();
int WallCollision();
int BrickCollision();

char *bricksPaths[] =
{
	"Images/CyanBrick.png",
	"Images/GreenBrick.png",
	"Images/YellowBrick.png",
	"Images/Purplebrick.png"
};

char *lelvelPaths[] =
{
	"Map/1.dat",
	"Map/2.dat",
	"Map/3.dat",
	"Map/4.dat",
	"Map/5.dat",
};

struct Paddle { float padPosX, padPosY; };
struct Paddle paddle;

struct Ball { float ballPosX, ballPosY; float dirX, dirY; };
struct Ball ball;

struct Bricks { int tile[MAX_MAP_Y][MAX_MAP_X]; int state[MAX_MAP_Y*MAX_MAP_X]; int color[MAX_MAP_Y*MAX_MAP_X]; };
struct Bricks bricks;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

SDL_Texture* loadTexture(char* path);
SDL_Texture* gBackground = NULL;
SDL_Texture* gWin = NULL;
SDL_Texture* gPaddle = NULL;
SDL_Texture* gBall = NULL;
SDL_Texture* gBricks[4] = { 0 };
SDL_Texture* gLive = NULL;

SDL_Rect padRect;
SDL_Rect ballRect;
SDL_Rect bricksRect[MAX_MAP_Y*MAX_MAP_X];
SDL_Rect lives[9];

#endif