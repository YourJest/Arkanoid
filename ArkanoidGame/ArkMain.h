#pragma once
#ifndef ARKMAIN_H
#define FRAME_VALUES 10
#define MAX_MAP_X 10
#define MAX_MAP_Y 15
#define _CRT_SECURE_NO_WARNINGS


#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int PADDLE_WIDTH = 128;
const int PADDLE_HEIGHT = 32;

const int BRICK_WIDTH = 80;
const int BRICK_HEIGHT = 40;

const float BALL_SPEED = 550;
const float PADDLE_SPEED = 550;

int state[MAX_MAP_Y*MAX_MAP_X] = { 0 };
int iter = 0;
Uint32 frametimes[FRAME_VALUES];
Uint32 frametimelast;
Uint32 framecount;

float framespersecond;

float delta;

const int FRAMES_LIMIT = 500;

const int BALL_DIAM = 24;

struct Paddle { float padPosX, padPosY; };

struct Paddle paddle;

struct Ball { float ballPosX, ballPosY; float dirX, dirY; };

struct Ball ball;

struct Bricks { int tile[MAX_MAP_Y][MAX_MAP_X];};

struct Bricks bricks;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

bool paddlestick;

//unsigned int lasttick, fpstick, fps, framecount;
//Frees media and shuts down SDL
void close();

void renderBrick(SDL_Rect brickRect, int x, int y);
void renderBall();
void renderPaddle();

void moveBall(float delta);

void movePaddle(float shift, float delta);

void loadBricks(char* path);

void changeDirection(float dirx, float diry);

bool PadCollision();
bool WallCollision(SDL_Rect a);
bool BrickCollision();
//Loads individual image as texture
SDL_Texture* loadTexture(char* path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gBackground = NULL;

SDL_Texture* gPaddle = NULL;

SDL_Texture* gBall = NULL;

SDL_Texture* gBrick = NULL;

SDL_Rect padRect;

SDL_Rect ballRect;

SDL_Rect bricksRect[MAX_MAP_Y*MAX_MAP_X];

#endif