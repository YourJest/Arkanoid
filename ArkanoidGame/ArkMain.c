#include "ArkMain.h"

bool init()
{
	//Initialization flag
	bool success = true;
	//memset(frametimes, 0, sizeof(frametimes));
	framecount = 0;
	framespersecond = 0;
	frametimelast = SDL_GetTicks();

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
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
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
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
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	gBackground = loadTexture("Images/Background.png");
	if (gBackground == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	gPaddle = loadTexture("Images/Paddle.png");
	if (gPaddle == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	gBall = loadTexture("Images/Ball.png");
	if (gBall == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	gBrick = loadTexture("Images/Brick.png");
	if (gBrick == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
     loadBricks("Map/bricks.dat");

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
	SDL_DestroyTexture(gBrick);
	gBrick = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
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

void renderBrick(int x, int y)
 {
	 bricksRect[iter].x = x;
	 bricksRect[iter].y = y;
	 bricksRect[iter].w = BRICK_WIDTH;
	 bricksRect[iter].h = BRICK_HEIGHT;
	 SDL_RenderCopy(gRenderer, gBrick, NULL, &bricksRect[iter]);
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

void loadBricks(char* path)
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

void drawBricks()
{
	int x, y, i ;
	iter = 0;
	for (y = 0; y<MAX_MAP_Y; y++)
	{
		for (x = 0; x < MAX_MAP_X; x++)
		{
			if (bricks.tile[y][x] != 0)
			{
				if (state[iter] == 0)
				{
					renderBrick(x * BRICK_WIDTH, y * BRICK_HEIGHT);
				}else 
				{
					bricksRect[iter].x = NULL;
					bricksRect[iter].y = NULL;
					bricksRect[iter].w = NULL;
					bricksRect[iter].h = NULL;
				}
				iter++;
			}
		}
	}
}

void movePaddle(float shift, float delta)
{
	float length = (float)sqrt(shift * shift);
	shift = PADDLE_SPEED * (shift / length);

	if (paddle.padPosX + PADDLE_WIDTH > SCREEN_WIDTH)
	{
		paddle.padPosX = (int)SCREEN_WIDTH - PADDLE_WIDTH;
	}
	else if (paddle.padPosX < 0)
	{
		paddle.padPosX = 0;
	}
	else
	{
		paddle.padPosX += shift * delta;
	}
	renderPaddle();
}

void changeDirection(float dirx, float diry)
{
	float length = sqrt(dirx * dirx + diry * diry);
	ball.dirX = BALL_SPEED * (dirx / length);
	ball.dirY = BALL_SPEED * (diry / length);
}

float GetReflection(float hitx) {
	// Make sure the hitx variable is within the width of the paddle
	if (hitx < 0) {
		hitx = 0;
	}
	else if (hitx > PADDLE_WIDTH) {
		hitx = PADDLE_WIDTH;
	}

	// Everything to the left of the center of the paddle is reflected to the left
	// while everything right of the center is reflected to the right
	hitx -= PADDLE_WIDTH/ 2.0f;

	// Scale the reflection, making it fall in the range -2.0f to 2.0f
	return 2.0f * (hitx / (PADDLE_WIDTH / 2.0f));
}

void moveBall(float delta)
{
	if (!WallCollision(ballRect))
	{
		if (ball.dirY < 0.0f)
		{
			if (ball.dirX > 0.0f)
			{
				changeDirection(-1.0f, -1.0f);
				ball.ballPosX += ball.dirX * delta - 2.0f;
				ball.ballPosY += ball.dirY * delta;
				
				if (ball.ballPosY < 0)
				{
					changeDirection(1.0f, 1.0f);
					ball.ballPosX += ball.dirX * delta;
					ball.ballPosY += ball.dirY * delta + 2.0f;
				}
			}
			else if (ball.dirX < 0.0f)
			{
				changeDirection(1.0f, -1.0f);
				ball.ballPosX += ball.dirX * delta + 2.0f;
				ball.ballPosY += ball.dirY * delta;
				
				if (ball.ballPosY < 0)
				{
					changeDirection(-1.0f, 1.0f);
					ball.ballPosX += ball.dirX * delta;
					ball.ballPosY += ball.dirY * delta + 2.0f;
				}
			}
		}
		else if (ball.dirY > 0.0f)
		{
			if (ball.dirX > 0.0f)
			{
				changeDirection(-1.0f, 1.0f);
				ball.ballPosX += ball.dirX * delta - 2.0f;
				ball.ballPosY += ball.dirY * delta;
			}
			else if(ball.dirX < 0.0f)
			{
				changeDirection(1.0f, 1.0f);
				ball.ballPosX += ball.dirX * delta + 2.0f;
				ball.ballPosY += ball.dirY * delta;
			}

		} 
	}
	else if (!PadCollision())
	{
		float ballcenterx = ball.ballPosX + BALL_DIAM / 2.0f;

		ball.ballPosY = paddle.padPosY - BALL_DIAM;
		changeDirection(GetReflection(ballcenterx - paddle.padPosX), -1.0f);

		ball.ballPosX += ball.dirX * delta;
		ball.ballPosY += ball.dirY * delta;
	}
	else if (!BrickCollision())
	{
		if (ball.dirY < 0.0f)
		{
			if (ball.dirX > 0.0f)
			{
				changeDirection(-1.0f, 1.0f);
				ball.ballPosX += ball.dirX * delta;
				ball.ballPosY += ball.dirY * delta;
			}
			else if (ball.dirX < 0.0f)
			{
				changeDirection(1.0f, 1.0f);
				ball.ballPosX += ball.dirX * delta;
				ball.ballPosY += ball.dirY * delta;
			}
		}
		else if (ball.dirY > 0.0f)
		{
			if (ball.dirX > 0.0f)
			{
				changeDirection(-1.0f, -1.0f);
				ball.ballPosX += ball.dirX * delta;
				ball.ballPosY += ball.dirY * delta;
			}
			else if (ball.dirX < 0.0f)
			{
				changeDirection(1.0f, -1.0f);
				ball.ballPosX += ball.dirX * delta;
				ball.ballPosY += ball.dirY * delta;
			}
		}
	}
	else
	{
		ball.ballPosX += ball.dirX * delta;
		ball.ballPosY += ball.dirY * delta;
	}
}

bool PadCollision()
{
		if (SDL_HasIntersection(&ballRect, &padRect))
		{
			return false;
		}
		return true;
}
bool WallCollision(SDL_Rect a)
{
	//The sides of the rectangles
	int leftA;
	int rightA;
	int topA;
	int bottomA;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	if (bottomA >= SCREEN_HEIGHT)
	{
		ball.ballPosX = paddle.padPosX + PADDLE_WIDTH / 2 - BALL_DIAM / 2;
		ball.ballPosY = paddle.padPosY - BALL_DIAM - 1.0f;
		paddlestick = true;
	}

	if (topA <= 0)
	{
		return false;
	}

	if (rightA >= SCREEN_WIDTH)
	{
		return false;
	}

	if (leftA <= 0)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}
bool BrickCollision()
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
					state[i] = 1;
					return false;
				}
			    i++;
			}
		}
	}
	return true;
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
			bool quit = false;

			//Event handler
			SDL_Event e;

			paddlestick = true;

			paddle.padPosX = SCREEN_WIDTH / 2 - 64;
			paddle.padPosY = SCREEN_HEIGHT - 32;

			ball.ballPosX = paddle.padPosX + PADDLE_WIDTH / 2 - BALL_DIAM / 2;
			ball.ballPosY = paddle.padPosY - BALL_DIAM - 1.0f;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
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
					paddlestick = false;
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
					ball.ballPosY = paddle.padPosY - BALL_DIAM - 1.0f;
				}
				

				if (ball.ballPosY + BALL_DIAM > SCREEN_HEIGHT)
				{
					paddlestick = true;
				}

				//Clear screen
				SDL_RenderClear(gRenderer);

				//Render texture to screen
				SDL_RenderCopy(gRenderer, gBackground, NULL, NULL);
				renderBall();
				renderPaddle();
				drawBricks();

				//Update screen
				SDL_RenderPresent(gRenderer);

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