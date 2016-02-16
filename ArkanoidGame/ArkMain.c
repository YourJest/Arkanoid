#define _CRT_SECURE_NO_WARNINGS
#include "ArkMain.h"

int init()
{
	//Initialization flag
	int success = 1;
	
	framecount = 0;
	framespersecond = 0;
	frametimelast = SDL_GetTicks();

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = 0;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("ArkanoiDen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = 0;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = 0;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = 0;
				}
			}
		}
	}

	return success;
}
SDL_Texture* loadTexture(char* path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}
void loadLevel (char* path)
{
	int x, y;
	FILE *fp;

	fp = fopen(path, "rb");
	if (fp == NULL)
	{
		printf("Failed to open map %s\n", path);
	}
	for (y = 0; y<MAX_MAP_Y; y++)
	{
		for (x = 0; x<MAX_MAP_X; x++)
		{
			fscanf_s(fp, "%d", &bricks.tile[y][x]);
		}
	}
	fclose(fp);
}
int loadMedia()
{
	int success = 1;
	int i;
	gBackground = loadTexture("Images/Background.png");
	if (gBackground == NULL)
	{
		printf("Failed to load texture image!\n");
		success = 0;
	}
	gPaddle = loadTexture("Images/Paddle.png");
	if (gPaddle == NULL)
	{
		printf("Failed to load texture image!\n");
		success = 0;
	}
	gBall = loadTexture("Images/Ball.png");
	if (gBall == NULL)
	{
		printf("Failed to load texture image!\n");
		success = 0;
	}
	for (i = 0; i < 4; i++)
	{
		gBricks[i] = loadTexture(bricksPaths[i]);
		if (gBricks[i] == NULL)
		{
			printf("Failed to load texture image!\n");
			success = 0;
		}
	}
	gLive = loadTexture("Images/Ball.png");
	if (gLive == NULL)
	{
		printf("Failed to load texture image!\n");
		success = 0;
	}
	return success;
}
void close()
{
	//Free loaded image
	SDL_DestroyTexture(gBackground);
	gBackground = NULL;
	SDL_DestroyTexture(gPaddle);
	gPaddle = NULL;
	SDL_DestroyTexture(gBall);
	gBall = NULL;
	for (int i = 0; i < 4; i++)
	{
		SDL_DestroyTexture(gBricks[i]);
		gBricks[i] = NULL;
	}

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void renderBall()
{
	ballRect.x = (int)(ball.ballPosX + 0.5f);
	ballRect.y = (int)(ball.ballPosY + 0.5f);
	ballRect.w = BALL_DIAM;
	ballRect.h = BALL_DIAM;
	SDL_RenderCopy(gRenderer, gBall, NULL, &ballRect);
}
void renderPaddle()
{
	padRect.x = (int)(paddle.padPosX + 0.5f);
	padRect.y = (int)(paddle.padPosY + 0.5f);
	padRect.w = PADDLE_WIDTH;
	padRect.h = PADDLE_HEIGHT;
	SDL_RenderCopy(gRenderer, gPaddle, NULL, &padRect);
}
void drawBricks()
{
	int x, y, i;
	i = 0;
	for (y = 0; y<MAX_MAP_Y; y++)
	{
		for (x = 0; x < MAX_MAP_X; x++)
		{
			if (bricks.tile[y][x] != 0)
			{
				if (bricks.state[i] == 0)
				{
					renderBrick(&bricksRect[i], x * BRICK_WIDTH, y * BRICK_HEIGHT, bricks.color[i]);
				}
				else
				{
					bricksRect[i].x = NULL;
					bricksRect[i].y = NULL;
					bricksRect[i].w = NULL;
					bricksRect[i].h = NULL;
				}
				i++;
			}
		}
	}
}
void renderBrick(SDL_Rect* brick, int x, int y, int tilePos)
{
	brick->x = x;
	brick->y = y;
	brick->w = BRICK_WIDTH;
	brick->h = BRICK_HEIGHT;
	SDL_RenderCopy(gRenderer, gBricks[tilePos], NULL, brick);
}
void renderLives(SDL_Rect* live, int x, int y)
{
	live->x = x;
	live->y = y;
	live->w = LIVE_DIAM;
	live->h = LIVE_DIAM;
	SDL_RenderCopy(gRenderer, gLive, NULL, live);
}
void drawLives()
{
	int i;
	for (i = 0; i < livesCounter; i++)
	{
		renderLives(&lives[i], SCREEN_WIDTH - (LIVE_DIAM * (i + 1)), SCREEN_HEIGHT - LIVE_DIAM);
	}
}

void movePaddle(float shift, float delta)
{
	float length = (float)sqrt(shift * shift);
	shift = PADDLE_SPEED * (shift / length);

	if (padRect.x + PADDLE_WIDTH > SCREEN_WIDTH)
	{
		paddle.padPosX = SCREEN_WIDTH - PADDLE_WIDTH;
	}
	else if (padRect.x < 0)
	{
		paddle.padPosX = 0;
	}
	else
	{
		paddle.padPosX += shift * delta;
	}
}

void changeDirection(float dirx, float diry)
{
	float length = sqrt(dirx * dirx + diry * diry);
	ball.dirX = BALL_SPEED * (dirx / length);
	ball.dirY = BALL_SPEED * (diry / length);
}
float GetReflection(float hitx) {
	if (hitx < 0)
		hitx = 0;
	else if (hitx > PADDLE_WIDTH) 
		hitx = PADDLE_WIDTH;

	hitx -= PADDLE_WIDTH/ 2.0f;
	return 2.0f * (hitx / (PADDLE_WIDTH / 2.0f));
}

int checkSide(SDL_Rect* _brick)
{
	int topBall, topBrick;
	int botBall, botBrick;
	int leftBall, leftBrick;
	int rightBall, rightBrick;

	topBall = ballRect.y;
	botBall = ballRect.y + ballRect.h;
	leftBall = ballRect.x;
	rightBall = ballRect.x + ballRect.w;

	topBrick = _brick->y;
	botBrick = _brick->y + _brick->h;
	leftBrick = _brick->x;
	rightBrick = _brick->x + _brick->w;

	if (topBall <= botBrick && leftBall >= leftBrick - ballRect.w + 3 && rightBall <= rightBrick + ballRect.w - 3 && topBall >= topBrick)
		//Top of ball collides with bot of brick
		return 0;
	if (botBall >= topBrick && leftBall >= leftBrick - ballRect.w + 3 && rightBall <= rightBrick + ballRect.w - 3)
		//bot of ball collides with top of brick
		return 1;
	if (leftBall <= rightBrick && rightBall >= rightBrick)
		//left 
		return 2;
	if (rightBall >= leftBrick && leftBall <= leftBrick)
		//right 
		return 3;
}

int PadCollision()
{
		if (SDL_HasIntersection(&ballRect, &padRect))
		{
			return 0;
		}
		return 1;
}
int WallCollision()
{
	int leftA;
	int rightA;
	int topA;
	int bottomA;

	leftA = ballRect.x;
	rightA = ballRect.x + ballRect.w;
	topA = ballRect.y;
	bottomA = ballRect.y + ballRect.h;

	if (bottomA >= SCREEN_HEIGHT)
	{
		ball.ballPosX = paddle.padPosX + PADDLE_WIDTH / 2 - BALL_DIAM / 2;
		ball.ballPosY = paddle.padPosY - BALL_DIAM - 1.0f;
		paddlestick = 1;
		livesCounter -= 1;
	}

	if (topA <= 0)
	{
		wallSide = 0;
		return 0;
	}

	if (rightA >= SCREEN_WIDTH)
	{
		wallSide = 1;
		return 0;
	}
	if (leftA <= 0)
	{
		wallSide = 2;
		return 0;
	}
	return 1;
}
int BrickCollision()
{
	int x, y, i;
	i = 0;

	for (y = 0; y < MAX_MAP_Y; y++)
	{
		for (x = 0; x < MAX_MAP_X; x++)
		{
			if (bricks.tile[y][x] != 0)
			{
				if (SDL_HasIntersection(&bricksRect[i], &ballRect))
				{
					bricks.state[i] = 1;
					brickSide = checkSide(&bricksRect[i]);
					return 0;
				}
				i++;
			}
		}
	}
	return 1;
}

void moveBall(float delta){
	if (!WallCollision())
	{
		//WallSide: 0 = Top, 1 = Right, 2 = Left
		if (wallSide == 0)
		{
			if (ball.dirX > 0)
				changeDirection(abs(ball.dirX), abs(ball.dirY));
			else if (ball.dirX < 0)
				changeDirection(-(abs(ball.dirX)), abs(ball.dirY));
		}
		else if (wallSide == 1)
		{
			if (ball.dirY > 0)
				changeDirection(-(abs(ball.dirX)), abs(ball.dirY));
			else if (ball.dirY < 0)
				changeDirection(-(abs(ball.dirX)), -(abs(ball.dirY)));
		}
		else if (wallSide == 2)
		{
			if (ball.dirY > 0)
				changeDirection(abs(ball.dirX), abs(ball.dirY));
			else if (ball.dirY < 0)
				changeDirection(abs(ball.dirX), -(abs(ball.dirY)));
		}
	}
	else if (!PadCollision())
	{
		float ballcenterx = ball.ballPosX + BALL_DIAM / 2.0f;

		ball.ballPosY = paddle.padPosY - BALL_DIAM;
		changeDirection(GetReflection(ballcenterx - paddle.padPosX), -1.0f);

	}
	else if (!BrickCollision())
	{
		//BrickSide: 0 = Top, 1 = Bottom, 2 = Left, 3 = Right
		if (brickSide == 0)
		{
			if (ball.dirX > 0)
				changeDirection(ball.dirX, abs(ball.dirY));
			else if (ball.dirX < 0)
				changeDirection(-(abs(ball.dirX)), abs(ball.dirY));
		}
		else if (brickSide == 1)
		{
			if (ball.dirX > 0)
				changeDirection(abs(ball.dirX), -(abs(ball.dirY)));
			else if (ball.dirX < 0)
				changeDirection(-(abs(ball.dirX)), -(abs(ball.dirY)));
		}
		else if (brickSide == 2)
		{
			if (ball.dirY > 0)
				changeDirection(abs(ball.dirX), abs(ball.dirY));
			else if (ball.dirY < 0)
				changeDirection(abs(ball.dirX), -(abs(ball.dirY)));
		}
		else if (brickSide == 3)
		{
			if (ball.dirY > 0)
				changeDirection(-(abs(ball.dirX)), abs(ball.dirY));
			else if (ball.dirY < 0)
				changeDirection(-(abs(ball.dirX)), -(abs(ball.dirY)));
		}
	}
	ball.ballPosX += ball.dirX * delta;
	ball.ballPosY += ball.dirY * delta;
}

void resetLevel()
{
	srand((unsigned)time(&t));
	for (int i = 0; i < MAX_MAP_Y*MAX_MAP_X; i++)
		bricks.color[i] = rand() % 4;
	for (int i = 0; i < MAX_MAP_X*MAX_MAP_Y; i++)
		bricks.state[i] = 0;

	paddlestick = 1;
}

void fpsthink() {

	Uint32 frametimesindex;
	Uint32 getticks;
	Uint32 count;
	Uint32 i;

	frametimesindex = framecount % FRAME_VALUES;
	
	getticks = SDL_GetTicks();

	frametimes[frametimesindex] = getticks - frametimelast;
	delta = (getticks - frametimelast) / 1000.0f;

	frametimelast = getticks;

	framecount++;

	if (framecount < FRAME_VALUES) {
		count = framecount;
	}
	else {
		count = FRAME_VALUES;
	}

	framespersecond = 0;
	for (i = 0; i < count; i++) {
		framespersecond += frametimes[i];
	}

	framespersecond /= count;

	framespersecond = 1000.f / framespersecond;
}

int isCompleted()
{
	int isEmpty;
	for (int i = 0; i < MAX_MAP_Y*MAX_MAP_X; i++)
	{
		if (bricksRect[i].x == 0 && bricksRect[i].y == 0 && bricksRect[i].h == 0 && bricksRect[i].w == 0)
			isEmpty = 1;
		else
			return 0;
	}
	return isEmpty;
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			int quit = 0;
			//Event handler
			SDL_Event e;

			paddlestick = 1;

			paddle.padPosX = SCREEN_WIDTH / 2 - 64;
			paddle.padPosY = SCREEN_HEIGHT - 32;

			ball.ballPosX = paddle.padPosX + PADDLE_WIDTH / 2 - BALL_DIAM / 2;
			ball.ballPosY = paddle.padPosY - BALL_DIAM;

			levelCounter = 0;
			loadLevel(lelvelPaths[levelCounter]);
			resetLevel();
			livesCounter = 3;
			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = 1;
					}
				}

				const Uint8 *keystates = SDL_GetKeyboardState(NULL);
				if (keystates[SDL_SCANCODE_LEFT])
				{
					float shift = -1.0f;
					movePaddle(shift, delta);
				}
				if (keystates[SDL_SCANCODE_RIGHT])
				{
					float shift = 1.0f;
					movePaddle(shift, delta);
				}

				if (keystates[SDL_SCANCODE_SPACE])
				{
					if (paddlestick)
					{
					paddlestick = 0;
 					changeDirection(1.0f, -1.0f);
					}
				}
				
				if (!paddlestick)
				{
					moveBall(delta);
				}

				if (paddlestick)
				{
					ball.ballPosX = paddle.padPosX + PADDLE_WIDTH / 2 - BALL_DIAM / 2;
					ball.ballPosY = paddle.padPosY - BALL_DIAM ;
				}

				if (livesCounter == 0)
				{
					livesCounter = 3;
					levelCounter = 0;
					loadLevel(lelvelPaths[levelCounter]);
					resetLevel();
				}

				//Clear screen
				SDL_RenderClear(gRenderer);

				//Render texture to screen
				SDL_RenderCopy(gRenderer, gBackground, NULL, NULL);
				renderBall();
				renderPaddle();
				drawBricks();
				drawLives();

				//Update screen
				SDL_RenderPresent(gRenderer);

 				if (isCompleted() == 1)
				{
					livesCounter ++;
					levelCounter++;
					if (levelCounter <= 4)
						loadLevel(lelvelPaths[levelCounter]);
					else
					{
						levelCounter = 0;
						loadLevel(lelvelPaths[levelCounter]);
					}
					resetLevel();
				}

				SDL_Delay(3);

				fpsthink();
				printf("%f\n", framespersecond);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}