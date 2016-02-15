#pragma once
#ifndef ARKMAIN_H
#define FRAME_VALUES 10
#define MAX_MAP_X 10
#define MAX_MAP_Y 15
#define _CRT_SECURE_NO_WARNINGS

#include "SDL\SDL.h"
#include "SDL\SDL_image.h"
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int PADDLE_WIDTH = 128;
const int PADDLE_HEIGHT = 32;

const int BRICK_WIDTH = 80;
const int BRICK_HEIGHT = 40;

const float BALL_SPEED = 600;
const float PADDLE_SPEED = 600;
const int BALL_DIAM = 24;

int state[MAX_MAP_Y*MAX_MAP_X] = { 0 };
int iter, brickSide, wallSide;
int paddlestick;

Uint32 frametimes[FRAME_VALUES];
Uint32 frametimelast;
Uint32 framecount;

float framespersecond;

float delta;

struct Paddle { float padPosX, padPosY; };
struct Paddle paddle;

struct Ball { float ballPosX, ballPosY; float dirX, dirY; };
struct Ball ball;

struct Bricks { int tile[MAX_MAP_Y][MAX_MAP_X]; int state[MAX_MAP_Y*MAX_MAP_X]; };
struct Bricks bricks = {0};

int init();
int loadMedia();
void loadBricks(char* path);
void close();

void renderBrick(SDL_Rect* brick, int x, int y);
void renderBall();
void renderPaddle();

void moveBall(float delta);
void movePaddle(float shift, float delta);

void changeDirection(float dirx, float diry);

int PadCollision();
int WallCollision();
int BrickCollision();

//Loads individual image as texture

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

SDL_Texture* loadTexture(char* path);
SDL_Texture* gBackground = NULL;
SDL_Texture* gPaddle = NULL;
SDL_Texture* gBall = NULL;
SDL_Texture* gBrick = NULL;

SDL_Rect padRect;
SDL_Rect ballRect;
SDL_Rect bricksRect[MAX_MAP_Y*MAX_MAP_X];

#endif