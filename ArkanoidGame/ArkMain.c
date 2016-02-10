#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>

#define FRAME_VALUES 10

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int PADDLE_WIDTH = 128;
const int PADDLE_HEIGHT = 32;

const float BALL_SPEED = 550;
const float PADDLE_SPEED = 550;

Uint32 frametimes[FRAME_VALUES];
Uint32 frametimelast;
Uint32 framecount;

float framespersecond;

float delta;

const int FRAMES_LIMIT = 500;

const int BALL_DIAM = 24;

struct Paddle {	float padPosX, padPosY; };

struct Ball { float ballPosX, ballPosY; float dirX, dirY; };

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

bool paddlestick;

//unsigned int lasttick, fpstick, fps, framecount;
//Frees media and shuts down SDL
void close();

void moveBall(struct Ball *, struct Paddle*, float delta);

void movePaddle(struct Paddle *, int shift, float delta);

void render(int* w, int* h, SDL_Texture* gTexture, SDL_Rect* rect, struct Ball* ball, struct Paddle* paddle);

void changeDirection(struct Ball* _ball, float dirx, float diry);

bool PadCollision(struct Ball*, struct Paddle*);
bool WallCollision(SDL_Rect a);

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

SDL_Rect padRect;

SDL_Rect ballRect;

bool init()
{
	//Initialization flag
	_Bool success = true;
	memset(frametimes, 0, sizeof(frametimes));
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
	_Bool success = true;

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

void render(int* w, int* h, SDL_Texture* gTexture, SDL_Rect* rect, struct Ball* ball, struct Paddle* paddle)
 {
	if (paddle == NULL)
	{
  		rect->x = (int)(ball->ballPosX + 0.5f);
		rect->y = (int)(ball->ballPosY + 0.5f);
		rect->w = *w;
		rect->h = *h;
		SDL_RenderCopy(gRenderer, gTexture, NULL, rect);
	}
	else
	{
		rect->x = (int)(paddle->padPosX + 0.5f);
		rect->y = (int)(paddle->padPosY);
		rect->w = *w;
		rect->h = *h;
		SDL_RenderCopy(gRenderer, gTexture, NULL, rect);
	}
}

void movePaddle(struct Paddle* _paddle, int shift, float delta)
{
	float length = sqrt(shift * shift);
	shift = PADDLE_SPEED * (shift / length);

	if (_paddle->padPosX + PADDLE_WIDTH > SCREEN_WIDTH)
	{
		_paddle->padPosX = SCREEN_WIDTH - PADDLE_WIDTH;
	}
	else if (_paddle->padPosX < 0)
	{
		_paddle->padPosX = 0;
	}
	else
	{
		_paddle->padPosX += shift * delta;
	}
	render(&PADDLE_WIDTH, &PADDLE_HEIGHT, gPaddle, &padRect, NULL, _paddle);
}

void changeDirection(struct Ball* _ball, float dirx, float diry)
{
	float length = sqrt(dirx * dirx + diry * diry);
	_ball->dirX = BALL_SPEED * (dirx / length);
	_ball->dirY = BALL_SPEED * (diry / length);
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

void moveBall(struct Ball* _ball, struct Paddle* _paddle, float delta)
{
	if (!WallCollision(ballRect))
	{
		if (_ball->dirY <= 0.0f)
		{
			if (_ball->dirX >= 0.0f)
			{
				changeDirection(_ball, -1.0f, -1.0f);
				_ball->ballPosX += _ball->dirX * delta;
				_ball->ballPosY += _ball->dirY * delta;
				
				if (_ball->ballPosY <= 0)
				{
					changeDirection(_ball, 1.0f, 1.0f);
					_ball->ballPosX += _ball->dirX * delta;
					_ball->ballPosY += _ball->dirY * delta + 1.0f;
				}
			}
			else if (_ball->dirX <= 0.0f)
			{
				changeDirection(_ball, 1.0f, -1.0f);
				_ball->ballPosX += _ball->dirX * delta;
				_ball->ballPosY += _ball->dirY * delta;
				
				if (_ball->ballPosY <= 0)
				{
					changeDirection(_ball, -1.0f, 1.0f);
					_ball->ballPosX += _ball->dirX * delta;
					_ball->ballPosY += _ball->dirY * delta + 1.0f;
				}
			}
		}
		else
		{
			if (_ball->dirX >= 0.0f)
			{
				changeDirection(_ball, -1.0f, 1.0f);
				_ball->ballPosX += _ball->dirX * delta;
				_ball->ballPosY += _ball->dirY * delta;
			}
			else if(_ball->dirX <= 0.0f)
			{
				changeDirection(_ball, 1.0f, 1.0f);
				_ball->ballPosX += _ball->dirX * delta;
				_ball->ballPosY += _ball->dirY * delta;
			}
		}
	}
	else if (!PadCollision(_ball, _paddle))
	{
		float ballcenterx = _ball->ballPosX + BALL_DIAM / 2.0f;

		_ball->ballPosY = _paddle->padPosY - BALL_DIAM;
		changeDirection(_ball, GetReflection(ballcenterx - _paddle->padPosX), -1.0f);

		_ball->ballPosX += _ball->dirX * delta;
		_ball->ballPosY += _ball->dirY * delta;
	}
	else
	{
		_ball->ballPosX += _ball->dirX * delta;
		_ball->ballPosY += _ball->dirY * delta;
	}
	render(&BALL_DIAM, &BALL_DIAM, gBall, &ballRect, _ball, NULL);
}

bool PadCollision(struct Ball* ball, struct Paddle* paddle)
{
	//The sides of the rectangles
		int leftA, leftB;
		int rightA, rightB;
		int topA, topB;
		int bottomA, bottomB;

		//Calculate the sides of rect A
		leftA = ball->ballPosX;
		rightA = ball->ballPosX + BALL_DIAM;
		topA = ball->ballPosY;
		bottomA = ball->ballPosY + BALL_DIAM;

		//Calculate the sides of rect B
		leftB = paddle->padPosX;
		rightB = paddle->padPosX + PADDLE_WIDTH;
		topB = paddle->padPosY;
		bottomB = paddle->padPosY + PADDLE_HEIGHT;

		//If any of the sides from A are outside of B

		//If any of the sides from A are outside of B
		if (bottomA >= topB && ball->ballPosX + BALL_DIAM >= paddle->padPosX && ball->ballPosX <= paddle->padPosX + PADDLE_WIDTH)
		{
			return false;
		}

		//If none of the sides from A are outside B
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
		paddlestick = true;
		return true;
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

void fpsthink() {

	Uint32 frametimesindex;
	Uint32 getticks;
	Uint32 count;
	Uint32 i;

	// frametimesindex is the position in the array. It ranges from 0 to FRAME_VALUES.
	// This value rotates back to 0 after it hits FRAME_VALUES.
	frametimesindex = framecount % FRAME_VALUES;
	
	// store the current time
	getticks = SDL_GetTicks();

	// save the frame time value
	frametimes[frametimesindex] = getticks - frametimelast;
	delta = (getticks - frametimelast) / 1000.0f;
	// save the last frame time for the next fpsthink
	frametimelast = getticks;

	// increment the frame count
	framecount++;

	// Work out the current framerate

	// The code below could be moved into another function if you don't need the value every frame.

	// I've included a test to see if the whole array has been written to or not. This will stop
	// strange values on the first few (FRAME_VALUES) frames.
	if (framecount < FRAME_VALUES) {

		count = framecount;

	}
	else {

		count = FRAME_VALUES;

	}

	// add up all the values and divide to get the average frame time.
	framespersecond = 0;
	for (i = 0; i < count; i++) {

		framespersecond += frametimes[i];

	}

	framespersecond /= count;

	// now to make it an actual frames per second value...
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

			padRect.x = SCREEN_WIDTH / 2 - 64;
			padRect.y = SCREEN_HEIGHT - 32;
			padRect.w = PADDLE_WIDTH;
			padRect.h = PADDLE_HEIGHT;

			ballRect.x = SCREEN_WIDTH / 2 - 12;
			ballRect.y = SCREEN_HEIGHT - 56;
			ballRect.h = BALL_DIAM;
			ballRect.w = BALL_DIAM;

			paddlestick = true;

			struct Paddle paddle;
			paddle.padPosX = SCREEN_WIDTH / 2 - 64;
			paddle.padPosY = SCREEN_HEIGHT - 32;
			
			struct Ball ball;
			ball.ballPosX = SCREEN_WIDTH / 2 - 12;
			ball.ballPosY = SCREEN_HEIGHT - 55;
			ball.dirX = 0;
			ball.dirY =  0;

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

				paddle.padPosX = padRect.x;
				paddle.padPosY = padRect.y;

				ball.ballPosX = ballRect.x;
				ball.ballPosY = ballRect.y;

				const Uint8 *keystates = SDL_GetKeyboardState(NULL);
				if (keystates[SDL_SCANCODE_LEFT])
				{
					int shift = -1.0f;
					movePaddle(&paddle, shift, delta);
				}
				if (keystates[SDL_SCANCODE_RIGHT])
				{
					int shift = 1.0f;
					movePaddle(&paddle, shift, delta);
				}

				if (keystates[SDL_SCANCODE_SPACE])
				{
						paddlestick = false;
				}

				
				if (paddlestick)
				{
					ballRect.x = padRect.x + PADDLE_WIDTH / 2 - BALL_DIAM / 2;
					ballRect.y = padRect.y - BALL_DIAM;
				}
				
				if (!paddlestick)
				{
					moveBall(&ball, &paddle, delta);
				}
				
		
				//Clear screen
				SDL_RenderClear(gRenderer);

				//Render texture to screen
				SDL_RenderCopy(gRenderer, gBackground, NULL, NULL);
				SDL_RenderCopy(gRenderer, gPaddle, NULL, &padRect);
				SDL_RenderCopy(gRenderer, gBall, NULL, &ballRect);
				//Update screen
				SDL_RenderPresent(gRenderer);

				SDL_Delay(4);

				fpsthink();
				printf("%f\n", framespersecond);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}