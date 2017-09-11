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
				cout << "ray is pointing up!" << endl;
				HorzIntersection.y = (player1.position.y / 64 * 64 -1);
				Ya = -64;
			}
			else {
				cout << "ray is pointing down!" << endl;
				HorzIntersection.y = player1.position.y / 64 * 64 + 64;
				Ya = 64;
			}
			if (rayXDir == 1) {
				cout << "ray is pointing right!" << endl;
				VertIntersection.x = (player1.position.x / 64) * 64 + 64;
				Xb = 64;
			}
			else {
				cout << "ray is pointing left!" << endl;
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

			cout << "iteration " << i << endl;
			cout << "player position is: " << player1.position.x << " " << player1.position.y << endl << endl;
			cout << "player relative position is " << relPos.x << " " << relPos.y << endl;
			cout << "player grid location is: " << player1.position.x/64 << " " << player1.position.y/64 << endl << endl;
			cout << "intersection is: " << VertIntersection.x << " " << VertIntersection.y << endl << endl;
			cout << "angle is: " << rayAngle << " radians" << endl;
			

			bool looping = true;
			bool xySwitch = 1;
			bool horzOrVert = NULL;
			int iterations = 0;

			//find the horizontal wall intercept
			do {
				if (map[HorzIntersection.x / 64][HorzIntersection.y / 64] > 0) {
					cout << "WALL HORZ HIT at: " << HorzIntersection.x / 64 << " " << HorzIntersection.y / 64 << endl << endl;
					looping = false;
					horzOrVert = 0;
					break;
				}
				else {
					HorzIntersection.x += Yb;
					HorzIntersection.y += Ya;
				}

			} while (looping);

			//find the vertical wall intercept
			looping = true;
			
			do {

			} while (looping);

			//initial checks
			if (map[HorzIntersection.x / 64][HorzIntersection.y / 64] > 0) {
				cout << "WALL HORZ HIT at: " << HorzIntersection.x / 64 << " " << HorzIntersection.y / 64 << endl << endl;
				looping = false;
				horzOrVert = 0;
			}
			else if (checkVert && map[VertIntersection.x / 64][VertIntersection.y / 64] > 0) {
				cout << "WALL VERT HIT at: " << VertIntersection.x / 64 << " " << VertIntersection.y / 64 << endl << endl;
				looping = false;
				horzOrVert = 1;
			}

			///*I want to change this. Rather than switching between horizontal and vertical, it should simply calculate wallhits for vert and horz,*///
			///and then calculate the oblique distance to each. Draw the closer one. This will prevent "holes" from forming in the walls

			//begin wallfinding loop
			while (looping && iterations < 100) {
				if (xySwitch == 0 && checkVert) {
					//cout << "added to vert" << endl;
					VertIntersection.x += Xb;
					VertIntersection.y += Xa;

					if (map[VertIntersection.x / 64][VertIntersection.y / 64] > 0) {
						cout << "WALL VERT HIT at: " << VertIntersection.x / 64 << " " << VertIntersection.y / 64 << endl << endl;
						looping = false;
						horzOrVert = 1;
					}

				}
				else if (xySwitch == 1) {
					//cout << "added to horz" << endl;
					HorzIntersection.x += Yb;
					HorzIntersection.y += Ya;

					if (map[HorzIntersection.x / 64][HorzIntersection.y / 64] > 0) {
						cout << "WALL HORZ HIT at: " << HorzIntersection.x / 64 << " " << HorzIntersection.y / 64 << endl << endl;
						looping = false;
						horzOrVert = 0;
					}

				}
				xySwitch = !xySwitch;
			}

			int IncompleteWallDistance;
			if (horzOrVert == 0) {
				IncompleteWallDistance = sqrt(pow(player1.position.x - HorzIntersection.x, 2)+ pow(player1.position.y - HorzIntersection.y, 2));
			}
			else if (horzOrVert == 1) {
				IncompleteWallDistance = sqrt(pow(player1.position.x - VertIntersection.x, 2) + pow(player1.position.y - VertIntersection.y, 2));
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