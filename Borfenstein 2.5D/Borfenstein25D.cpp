#include "stdafx.h"
#include <math.h>
#include <iostream>
#include <string>
#include <SDL.h>
using namespace std;

//constant declarations
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int textureSize = 64;
const Uint8 map[12][12] = {
	{1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,1,1,0,1},
	{1,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1}
};

//sdl variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

//structures

typedef struct vector2d {
	int x;
	int y;
};

struct player {
	vector2d position;
	int height;
	int distanceToPlane;
	double angle;
	float speed;
}player1;

//functions
bool init() {
	bool success = true;

	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else {
		//window init
		window = SDL_CreateWindow("Borfenstein 2.5D", 50, 50, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (window == NULL) {
			cout << "WINDOW FAILED TO INITIALIZE" << SDL_GetError() << endl;
			success = false;
		}
	}
	return success;
}

bool initTexture() {
	bool success = true;

	//initialize the renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
		cout << "RENDERER FAILED TO INITIALIZE: " << SDL_GetError() << endl;
		success = false;
	}

	//initalise the texture
	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,//Represents each pixel as 4 bytes, with one byte for each colour and one for alpha. That is to say, each colour can have 255 shades
		SDL_TEXTUREACCESS_STATIC,//writes numeric values directly to the texture every iteration
		SCREEN_WIDTH,
		SCREEN_WIDTH
	);

	if (texture == NULL) {
		cout << "TEXTURE FAILED TO INITIALIZE" << SDL_GetError() << endl;
		success = false;
	}

	return success;

}

void quit() {
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
		printf("Destroyed Renderer\n");
	}
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
		printf("Destroyed Texture\n");
	}
	if (window != NULL) {
		SDL_DestroyWindow(window);
		printf("Destroyed Window\n");
	}
	SDL_Quit();
}

void drawLine(Uint8 red, Uint8 green, Uint8 blue, vector2d start, vector2d end) {
	SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
}

/******************************************************************************MAIN*********************************************************************************/
/*******************************************************************************************************************************************************************/
int main(int argc, char* argv[]) {

	//initialising window and SDL
	if (!init()) {
		return 1;
	}

	//initialize renderer and texture
	if (!initTexture()) {
		quit();
		return 2;
	}

	//initialize the player
	player1.position = {2*64 + 10,2*64 + 10};
	player1.height = 32;
	player1.angle = M_PI/2;
	player1.speed = 1;
	player1.distanceToPlane = 693;

	bool quitLoop = false;
	SDL_Event event;

	//main loop
	while (!quitLoop) {

		int startFrameTime = SDL_GetTicks();

		double castArc = player1.angle;
		castArc += M_PI / 6;//starts the cast arc at +30 degrees relative to the player's viewline

		if (castArc < 0) {
			castArc = 2 * M_PI + castArc;
		}
		if (castArc > 2 * M_PI) {
			castArc -= 2 * M_PI;
		}

		//raycasting loop
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			char rayYDir;//1 is up, 0 is down
			char rayXDir;//1 is right, 0 is left
			vector2d relPos = { player1.position.x % 64, player1.position.y % 64 };
			double rayAngle = castArc - (i*((M_PI/3)/SCREEN_WIDTH));

			//these represent constant distances between gridlines
			int Ya;
			int Yb;
			int Xa;
			int Xb;

			//these are flags for whether to check the horizontal or vertical intersections
			bool checkVert = true;
			bool checkHorz = true;

			//determining which quadrant the ray is in
			if (rayAngle >= 0 && rayAngle < M_PI / 2) {
				rayYDir = 1;
				rayXDir = 1;
			}
			else if (rayAngle >= M_PI/2 && rayAngle < M_PI) {
				rayYDir = 1;
				rayXDir = 0;
			}
			else if (rayAngle >= M_PI && rayAngle < 3*(M_PI)/2) {
				rayYDir = 0;
				rayXDir = 0;
			}
			else if (rayAngle >= 3*(M_PI)/2 && rayAngle < 0) {
				rayYDir = 0;
				rayXDir = 1;
			}

			vector2d HorzIntersection;
			vector2d VertIntersection;
			
			//finding the nearest wallintersects
			if (rayYDir == 1) {
				//cout << "ray is pointing up!" << endl;
				HorzIntersection.y = (player1.position.y / 64 * 64 -1);
				Ya = -64;
			}
			else {
				//cout << "ray is pointing down!" << endl;
				HorzIntersection.y = player1.position.y / 64 * 64 + 64;
				Ya = 64;
			}
			if (rayXDir == 1) {
				//cout << "ray is pointing right!" << endl;
				VertIntersection.x = (player1.position.x / 64) * 64 + 64;
				Xb = 64;
			}
			else {
				//cout << "ray is pointing left!" << endl;
				VertIntersection.x = (player1.position.x / 64) * 64 + -1;
				Xb = -64;
			}
			HorzIntersection.x = abs(HorzIntersection.y - player1.position.y) / tan(rayAngle) + player1.position.x;
			VertIntersection.y = player1.position.y + (player1.position.x - VertIntersection.x)*tan(rayAngle) + 64;

			if (VertIntersection.y < 0) {
				checkVert = false;
			}
			if (HorzIntersection.x < 0) {
				checkHorz = false;
			}

			Yb = 64 / tan(rayAngle);
			Xa = 64 * tan(rayAngle);
			/*
			cout << "iteration " << i << endl;
			cout << "player position is: " << player1.position.x << " " << player1.position.y << endl << endl;
			cout << "player relative position is " << relPos.x << " " << relPos.y << endl;
			cout << "player grid location is: " << player1.position.x/64 << " " << player1.position.y/64 << endl << endl;
			cout << "intersection is: " << VertIntersection.x << " " << VertIntersection.y << endl << endl;
			cout << "angle is: " << rayAngle << " radians" << endl;
			*/

			bool looping = true;
			//bool xySwitch = 1;
			
			//int iterations = 0;

			//find the horizontal wall intercept
			if (checkHorz) {
				do {
					if (map[HorzIntersection.x / 64][HorzIntersection.y / 64] > 0) {
						//cout << "WALL HORZ HIT at: " << HorzIntersection.x / 64 << " " << HorzIntersection.y / 64 << endl << endl;
						looping = false;
						break;
					}
					else {
						HorzIntersection.x += Yb;
						HorzIntersection.y += Ya;
					}

				} while (looping);
			}

			//find the vertical wall intercept
			looping = true;
			if (checkVert) {
				do {
					if (checkVert && map[VertIntersection.x / 64][VertIntersection.y / 64] > 0) {
						//cout << "WALL VERT HIT at: " << VertIntersection.x / 64 << " " << VertIntersection.y / 64 << endl << endl;
						looping = false;
						break;
					}
					else {
						VertIntersection.x += Xb;
						VertIntersection.y += Xa;
					}
				} while (looping);
			}
			//Now, let's calculate the oblique distance to each point!
			int horzDistance = (int)sqrt(pow((player1.position.x - HorzIntersection.x),2) + pow((player1.position.y - HorzIntersection.y), 2));
			int vertDistance = (int)sqrt(pow((player1.position.x - VertIntersection.x), 2) + pow((player1.position.y - VertIntersection.y), 2));

			//cout << "horizontal oblique distance is: " << horzDistance << endl;
			//cout << "vertical oblique distance is: " << vertDistance << endl;

			//Which wall should we render? We render the wall with the closest oblique distance of course!
			bool horzOrVert = NULL;

			if (horzDistance <= vertDistance) {
				horzOrVert = 0;
			}
			else {
				horzOrVert = 1;
			}

			int IncompleteWallDistance;
			if (horzOrVert == 0) {
				IncompleteWallDistance = horzDistance;
			}
			else if (horzOrVert == 1) {
				IncompleteWallDistance = vertDistance;
			}

			//getting the correct wall distance
			double angleBeta = (rayAngle - player1.angle);
			int wallDistance = IncompleteWallDistance * cos(angleBeta);
			//cout << "wall distance is " << wallDistance << endl;
			//cout << "angle beta is " << angleBeta << endl;
			//cout << "cosine of angle beta is " << cos(angleBeta) << endl;

			int lineHeight = int((64.0 / wallDistance)*player1.distanceToPlane);
			//cout << "line height is: " << lineHeight << endl;
			
			int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
			
			if (drawStart < 0) {
				drawStart = 0;
			}
			int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
			
			if (drawEnd >= SCREEN_HEIGHT) {
				drawEnd = SCREEN_HEIGHT - 1;
			}

			Uint8 green = (horzOrVert == 1) ? 0xFF / 2 : 0xFF;

			drawLine(0, green, 0, { i, drawStart }, { i, drawEnd });

		}//end of raycasting

		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		//controls
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_A]) {
			cout << "turning widdershins! Angle is: " << player1.angle << endl;
			player1.angle += M_PI / 24;
		}
		if (state[SDL_SCANCODE_D]) {
			cout << "turning clockwise! Angle is: " << player1.angle << endl;
			player1.angle -= M_PI / 24;
		}

		while (SDL_PollEvent(&event)) {	//checks to see if the event queue has any more events, stops if it doesn't
										//also, it changes the event variable to be the next event in the queue

			if (event.type == SDL_QUIT) {
				quitLoop = true;
			}
		}

		//set framerate
		int endFrameTime = SDL_GetTicks();
		int deltaFrameTime = endFrameTime - startFrameTime;
		double frameRate = 1 / (deltaFrameTime / 1000.0);

		if (frameRate > 30) {
			SDL_Delay(frameRate - 30);
			frameRate -= (frameRate - 30);
		}
		//cout << frameRate << endl;
	}

	quit();

	return 0;
}