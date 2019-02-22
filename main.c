#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/time.h>
#include <time.h>

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "defs.h"

void insertPersonsToField(void);

/*!*****************************************************************************
	\brief List of different display states (aka gamestates)	

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
enum {
	MENU_STATE=0,
	PLAY_STATE,
	NEXT_LEVEL,
	LEVEL_TEXT,
	END_GAME,
	CREDITS,
	START_MENU,
};

int gamestate = MENU_STATE;

/*!*****************************************************************************
	\brief	List of title states

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
enum {
	GAME_OVER=0,
	TITLE,
	LEVEL,
};

/*!*****************************************************************************
	\brief	Load text image from bitmap 

	\param	rect
		SDL rectangle handler

	\param	image
		Image from a previously set enum

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int getText(SDL_Rect *rect, int image) {

	if(image > LEVEL) {
		return -1;
	}

	rect->x = 0;
	rect->y = image * 40;
	rect->h = 40;
	rect->w = 270;
	return 0;
}


/*!*****************************************************************************
	\brief 	Initialize SDL rectangle dimensions

	\date	7.1.18
	
	\param	rect
		SDL rectangle handler

	\param	x, y, w, h
		Dimensions of the rectangle

	\author	Lari Koskinen
*******************************************************************************/
void initRectangle(SDL_Rect *rect, int x, int y, int w, int h) {
	if(rect != NULL) {
		rect->x = x;
		rect->y = y;
		rect->w = w;
		rect->h = h;
	}
}

/*!*****************************************************************************
	\brief	Draw string on screen	

	\param	x, y
		The position of the text, if 0, then will be centered

	\param	w
		Maximum width of the text, 0 will use text width

	\param	text
		String to be written on screen

	\param	colour
		Color of the font to be written on screen in hex

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int drawTTFText(int x, int y, int w, char *text, unsigned int colour) {
	SDL_Surface *rendText;
	SDL_Rect rect, src;
	SDL_Color color = {(colour & 0xFF0000) >> 16, (colour & 0xFF00) >> 8, (colour & 0xFF)};
	int middleX, middleY;

	if((screen != NULL) && (font != NULL) && (text != NULL)) {
		if((rendText = TTF_RenderText_Solid(font, text, color)) != NULL) {
			initRectangle(&src, 0, 0, ((!w)? rendText->w: w), rendText->h);
			middleX = ((FIELD_X * FIELD_WIDTH) / 2) - (rendText->w / 2);
			middleY = ((FIELD_Y * FIELD_WIDTH) / 2) - (rendText->h / 2);
			initRectangle(&rect, ((!x)? middleX: x), ((!y)? middleY: y), ((!w)? rendText->w: w), rendText->h);
			SDL_BlitSurface(rendText, &src, screen, &rect);
			SDL_FreeSurface(rendText);
			return 0;
		}
	}
	return -1;
}

/*!*****************************************************************************
	\brief	Draw menu text on screen 	

	\param	txt
		Enum-state of the menu text

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int drawText(int txt) {
	char textInfo[1024];

	gamestate = MENU_STATE;
	SDL_FillRect(screen, NULL, 0xFFFFFF);
	switch(txt) {
		case GAME_OVER:
			return drawTTFText(0, 0, 0, "GAME OVER", 0xFF00FF); 
		case TITLE:
			drawTTFText(0, 100, 0, "R.O.B.O.T.S.", 0xFF00FF); 
			drawTTFText(0, 200, 0, "Use numpad to move", 0xFF00FF); 
			drawTTFText(0, 250, 0, "5 to wait", 0xFF00FF); 
			drawTTFText(0, 300, 0, "0 to teleport", 0xFF00FF); 
			drawTTFText(0, 350, 0, "Left corner displays safe teleports", 0xFF00FF); 
			return drawTTFText(0, 450, 0, "Press space to start", 0xFF00FF); 
		case LEVEL:
			sprintf(textInfo, "ENTERING LEVEL %d", currentLevel);
			return drawTTFText(0, 0, 0, textInfo, 0xFF00FF); 
	}
	return -1;
};

/*!*****************************************************************************
	\brief  Free all reserved resources	

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
void quit() {
	SDL_FreeSurface(sprites);
	SDL_FreeSurface(robot);
	SDL_FreeSurface(hero);
	SDL_FreeSurface(trash);
	SDL_FreeSurface(screen);
	SDL_Quit();
	TTF_Quit();
	exit(1);
}

/*!*****************************************************************************
	\brief 	List of robot states

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
enum {
	ROBOT_RIGHT_LURK=0,
	ROBOT_RIGHT_1,
	ROBOT_RIGHT_2,
	ROBOT_MOVE_RIGHT,
	ROBOT_STAND_RIGHT,
	ROBOT_STAND_LEFT,
	ROBOT_MOVE_LEFT,
	ROBOT_LEFT_2,
	ROBOT_LEFT_1,
	ROBOT_LEFT_LURK,
};

/*!*****************************************************************************
	\brief	Load robot image from bitmap

	\param	rect
		SDL rectangle handler

	\param	image
		enum-state of the robots image

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int getRobotImage(SDL_Rect *rect, int image) {

	if(image > ROBOT_LEFT_LURK) {
		return -1;
	}

	rect->x = image * FIELD_WIDTH;
	rect->y = 0;
	rect->h = FIELD_WIDTH;
	rect->w = FIELD_WIDTH;
	return 0;
}

/*!*****************************************************************************
	\brief	List of hero states

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
enum {
	HERO_TELEPORT=0,
	HERO_WAVE_RIGHT,
	HERO_PONDERING,
	HERO_WAVE_DOWN,
	HERO_POINT_LEFT,
	HERO_RIGHT,
	HERO_DOWN,
	HERO_MOVE_RIGHT,
	HERO_MOVE_DOWN_1,
	HERO_MOVE_DOWN_2,
	HERO_MOVE_LEFT,
	HERO_LEFT,
	HERO_WAVE_LEFT,
};

/*!*****************************************************************************
	\brief	Load hero image from bitmap

	\param	rect
		SDL rectangle handler

	\param	image
		enum state of the hero image

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int getHeroImage(SDL_Rect *rect, int image) {
	if(image > HERO_WAVE_LEFT) {
		return -1;
	}
	if(!updateMovement) {
		switch(image) {
			case HERO_MOVE_LEFT:
			case HERO_WAVE_LEFT:
				image = HERO_LEFT;
			break;
			case HERO_MOVE_RIGHT:
			case HERO_WAVE_RIGHT:
				image = HERO_RIGHT;
			break;
			case HERO_MOVE_DOWN_1:
			case HERO_MOVE_DOWN_2:
			case HERO_WAVE_DOWN:
				image = HERO_PONDERING;
			break;
			case HERO_RIGHT:
				image = HERO_WAVE_RIGHT;
			break;
			case HERO_LEFT:
				image = HERO_WAVE_LEFT;
			break;
			case HERO_PONDERING:
				image = HERO_DOWN;
			break;
			case HERO_DOWN:
				image = HERO_WAVE_DOWN;
			break;
			case HERO_TELEPORT:
				image = HERO_DOWN;
			break;
		}
		HERO_MOVEMENT = image;
	}

	rect->x = image * FIELD_WIDTH;
	rect->y = 0;
	rect->h = FIELD_WIDTH;
	rect->w = FIELD_WIDTH;
	return 0;
}

/*!*****************************************************************************
	\brief	Load all game bitmaps to SDL-image surfaces for later use

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int createSurfaces(void) {
	
	// Initialize SDL 
	char basepath[1024], path[1024];
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return(2);
	}
	// Hide cursor
	SDL_ShowCursor(SDL_DISABLE);
	
	// Load image
	if(getcwd(basepath, 1024) != NULL) {
		sprintf(path, "%s/%s", basepath, "evil.bmp");
		if((robot = IMG_Load(path)) == NULL) {
			fprintf(stderr, "Couldn't load image: %s\n", SDL_GetError());
			return -1;
		}
	
		sprintf(path, "%s/%s", basepath, "hero.bmp");
		if((hero = IMG_Load(path)) == NULL) {
			fprintf(stderr, "Couldn't load image: %s\n", SDL_GetError());
			return -1;
		}

		sprintf(path, "%s/%s", basepath, "scrapheap.bmp");
		if((trash = IMG_Load(path)) == NULL) {
			fprintf(stderr, "Couldn't load image: %s\n", SDL_GetError());
			return -1;
		}

		/* Set a 800x600x16 video mode  16x12 */
		if((screen = SDL_SetVideoMode(FIELD_X * FIELD_WIDTH, FIELD_Y * FIELD_WIDTH, 16, SDL_SWSURFACE)) == NULL) {
			fprintf(stderr, "Couldn't set 640x480x16 video mode: %s\n", SDL_GetError());
			return -1;
		}
		robotCount = ROBOCOUNT;
		currentLevel = 0;

		sprintf(path, "%s/%s", basepath, "arial.ttf");
		if(!(font = TTF_OpenFont(path, 40))) {
			fprintf(stderr, "Font load error %s\n", TTF_GetError());
			return -1;
		}
		return 0;
	}
	return -1;
}

/*!*****************************************************************************
	\brief	Handle winning a level and add more robots on the field

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
void nextLevel(void) {
	float calculate = (float)robotCount;
	calculate = calculate * 1.2;
	robotCount = (int)calculate;
	robotCount = (robotCount >= (FIELD_X * FIELD_Y))? robotCount - 8: robotCount;
	safeTeleports+=2;
}

/*!*****************************************************************************
	\brief	Set pieces on the playfield

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int setPlayfield(void) {	
	int i, j;
	if(currentLevel) {
		nextLevel();
	}
	else {
		robotCount = ROBOCOUNT;
	}
	currentLevel++;
	for(i=0;i<FIELD_X;i++) {
		for(j=0;j<FIELD_Y;j++) {
			playfield[i][j] = EMPTY;
		}
	}
	insertPersonsToField();
	if(currentLevel) {
		drawText(LEVEL);
		gamestate = LEVEL_TEXT;
	}

	return 0;
}

/*!*****************************************************************************
	\brief	Reset game playfield	

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int resetPlayfield(void) {
	HERO_MOVEMENT = HERO_PONDERING; 
	currentLevel = 0;
	robotsKilled = 0;
	safeTeleports = 4;
	setPlayfield();
	return 0;
}

/*!*****************************************************************************
	\brief	Display game over

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
void endGame(void) {
	currentLevel = 0;
	resetPlayfield();
	drawText(GAME_OVER);
}

/*!*****************************************************************************
	\brief	Draw robot to the playfield

	\param	x, y
		Position to draw the robot

	\param	dir
		The direction the robot will be facing

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int drawRobot(int x, int y, int dir) {
	SDL_Rect dstrect, srcrect;

	if(hero != NULL) {
		if(getRobotImage(&srcrect, dir)) {
			return -1;
		}
		dstrect.x = x * FIELD_WIDTH;
		dstrect.y = y * FIELD_WIDTH;
		dstrect.w = FIELD_WIDTH;
		dstrect.h = FIELD_WIDTH;
			
		// Blit sprite to surface
		SDL_BlitSurface(robot, &srcrect, screen, &dstrect);
	}
	return 0;
}

/*!*****************************************************************************
	\brief	Draw hunkheap on the playfield

	\param	x, y
		Position of the trash
	
	\param	state
		enum state of the trash

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int drawTrash(int x, int y, int state) {
	SDL_Rect dstrect, srcrect;

	if(trash != NULL) {
		srcrect.x = state * FIELD_WIDTH;
		srcrect.y = 0;
		srcrect.w = FIELD_WIDTH;
		srcrect.h = FIELD_WIDTH;
		dstrect.x = x * FIELD_WIDTH;
		dstrect.y = y * FIELD_WIDTH;
		dstrect.w = FIELD_WIDTH;
		dstrect.h = FIELD_WIDTH;
			
		// Blit sprite to surface
		SDL_BlitSurface(trash, &srcrect, screen, &dstrect);
	}
	return 0;
}

/*!*****************************************************************************
	\brief	Get direction to move the robot from given position towards the hero

	\param	x, y
		Position from where to calculate the position, the robot should face

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int getDirection(int x, int y) {
	int dir = 0;

	if(updateMovement) {
		if(x < HERO_X) {
			dir = ROBOT_MOVE_RIGHT;
		}
		else if(x > HERO_X) {
			dir = ROBOT_MOVE_LEFT;
		}
	}
	else {
		if(x < HERO_X) {
			dir = ROBOT_STAND_RIGHT;
		}
		else if(x > HERO_X) {
			dir = ROBOT_STAND_LEFT;
		}
	}

	return dir;
}

/*!*****************************************************************************
	\brief	Draw the hero

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int drawProgtagonist(void) {
	SDL_Rect dstrect, srcrect;

	if(hero != NULL) {
		if(getHeroImage(&srcrect, HERO_MOVEMENT)) {
			return -1;
		}
		dstrect.x = HERO_X * FIELD_WIDTH;
		dstrect.y = HERO_Y * FIELD_WIDTH;
		dstrect.w = FIELD_WIDTH;
		dstrect.h = FIELD_WIDTH;
			
		// Blit sprite to surface
		SDL_BlitSurface(hero, &srcrect, screen, &dstrect);
	}
	return 0;
}

/*!*****************************************************************************
	\brief	Fill the field with robots

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
void drawRobots(void) {
	int x, y, item, dir;
	char textInfo[2014];
	for(x=0; x<FIELD_X; x++) {
		for(y=0;y<FIELD_Y; y++) {
			if((item = playfield[x][y]) != EMPTY) {
				switch(item) {
					case HERO_EXPLOSION:
						drawTrash(x, y, 2);
						playfield[x][y] = EXPLOSION;
					break;
					case EXPLOSION:
						drawTrash(x, y, 1);
						playfield[x][y] = TRASH;
					break;
					case TRASH:
						drawTrash(x, y, 0);
					break;
					case ROBOT:
						dir = getDirection(x, y);
						drawRobot(x, y, dir);
					break;
					case HERO:
						drawProgtagonist();
					break;
				};
			}
		}
	}
	sprintf(textInfo, "%d", safeTeleports);
	drawTTFText(1, 1, 0, textInfo, 0x0000FF);
	updateMovement = 0;
}

/*!*****************************************************************************
	\brief	Move all robots towards the hero

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int moveRobots(void) {
	int x = HERO_X, y = HERO_Y, move_x, move_y;

	for(;y<FIELD_Y;y++) {
		for(;x<FIELD_X;x++) {
			if(playfield[x][y] == ROBOT) {
				playfield[x][y] = EMPTY;
				move_x = (x < HERO_X)? x+1: (x > HERO_X)? x-1: x;
				move_y = (y < HERO_Y)? y+1: (y > HERO_Y)? y-1: y;
				if(playfield[move_x][move_y] == HERO) {
					playfield[move_x][move_y] = HERO_EXPLOSION; //TRASH;
					gamestate = END_GAME;
					drawEverything();
					return 1;
				}
				else if(playfield[move_x][move_y] != EMPTY) {
					if(playfield[move_x][move_y] == MOVED_ROBOT) {
						robotsKilled+=2;
					}
					else {
						robotsKilled++;
					}
					playfield[move_x][move_y] = EXPLOSION; //TRASH;
				}
				else {
					playfield[move_x][move_y] = MOVED_ROBOT;
				}
			}
		}
		x = 0;
	}

	x = HERO_X;
	y = HERO_Y;
	for(;y>=0;y--) {
		for(;x>=0;x--) {
			if(playfield[x][y] == ROBOT) {
				move_x = (x < HERO_X)? x+1: (x > HERO_X)? x-1: x;
				move_y = (y < HERO_Y)? y+1: (y > HERO_Y)? y-1: y;
				if(playfield[move_x][move_y] == HERO) {
					playfield[move_x][move_y] = HERO_EXPLOSION; //TRASH;
					gamestate = END_GAME;
					drawEverything();
					return 1;
				}
				else if(playfield[move_x][move_y] != EMPTY) {
					if(playfield[move_x][move_y] == MOVED_ROBOT) {
						robotsKilled+=2;
					}
					else {
						robotsKilled++;
					}
					playfield[x][y] = EMPTY;
					playfield[move_x][move_y] = EXPLOSION; //TRASH;
				}
				else
				{
					playfield[x][y] = EMPTY;
					playfield[move_x][move_y] = MOVED_ROBOT;
				}
			}
		}
		x = FIELD_X - 1;
	}
	for(x=0;x<FIELD_X;x++) {
		for(y=0;y<FIELD_Y;y++) {
			if(playfield[x][y] == MOVED_ROBOT) {
				playfield[x][y] = ROBOT;
			}
			else if(playfield[x][y] == ROBOT) {
				printf("What? An unmoved robot?\n");
			}
		}
	}
	updateMovement = 1;
	return 0;
}

/*!*****************************************************************************
	\brief	Get amount of active robots on field

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int getRobotCount(void) {
	int x,y, count=0;
	for(x=0;x<FIELD_X;x++) {
		for(y=0;y<FIELD_Y;y++) {
			if(playfield[x][y] == ROBOT) {
				count++;
			}
		}
	}
	return count;
}

/*!*****************************************************************************
	\brief	Get current time in milliseconds

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
unsigned long long TickCount(void) {
	unsigned long long dw = 0;

	struct timeval tvnow;
	unsigned long long s,us;

	gettimeofday(&tvnow, NULL);
	s= (unsigned long long)tvnow.tv_sec;
	us= (unsigned long long)tvnow.tv_usec;

	dw = (unsigned long long)((s * (unsigned long long)1000) +(us / (unsigned long long)1000));

	return (unsigned long long)dw;
}

/*!*****************************************************************************
	\brief	Create a random value according to TickCount

	\param	max
		Maximum value of the random number to be created

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
long long randomValue(int max) {
	long long item = 0;
	static long long next2 = 1;
	next2 = next2 * 1103515245 + TickCount();
	item = (((unsigned)(next2/65536) % 32768) % (max));
	return ((item < 0)? 0: (item > max)? max: item);
}

/*!*****************************************************************************
	\brief	Insert items into field

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
void insertPersonsToField(void) {
	int i, x, y;

	HERO_X = randomValue(FIELD_X-1);
	HERO_Y = randomValue(FIELD_Y-1);
	x = HERO_X;
	y = HERO_Y;
	playfield[HERO_X][HERO_Y] = HERO;
	for(i=0; i < robotCount; i++) {
		while(playfield[x][y] != EMPTY) {
			x = randomValue(FIELD_X-1);
			y = randomValue(FIELD_Y-1);
		}
		playfield[x][y] = ROBOT;
	}
}

/*!*****************************************************************************
	\brief	Move hero on the field

	\param	keyPressed
		SDL keyboard value handler

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int moveProgtagonist(SDLKey keyPressed) {
	int x = HERO_X, y = HERO_Y;
	switch(keyPressed) {
		case SDLK_KP0:
			while((x == HERO_X) && (y == HERO_Y)) {
				while(playfield[x][y] != EMPTY) {
					x = randomValue(FIELD_X-1);
					y = randomValue(FIELD_Y-1);
				}
			}
			HERO_MOVEMENT = HERO_TELEPORT;
			updateMovement--;
		break;
		case SDLK_KP1:
			x--;
			y++;
			HERO_MOVEMENT = HERO_MOVE_LEFT; 
		break;
		case SDLK_DOWN:
		case SDLK_KP2:
			y++;
			HERO_MOVEMENT = HERO_MOVE_DOWN_1;
		break;
		case SDLK_KP3:
			x++;
			y++;
			HERO_MOVEMENT = HERO_MOVE_RIGHT;
		break;
		case SDLK_LEFT:
		case SDLK_KP4:
			x--;
			HERO_MOVEMENT = HERO_MOVE_LEFT;
		break;
		case SDLK_KP5:
			//wait for a round
			HERO_MOVEMENT = HERO_PONDERING; 
		break;
		case SDLK_RIGHT:
		case SDLK_KP6:
			x++;
			HERO_MOVEMENT = HERO_MOVE_RIGHT;
		break;
		case SDLK_KP7:
			x--;
			y--;
			HERO_MOVEMENT = HERO_MOVE_LEFT;
		break;
		case SDLK_UP:
		case SDLK_KP8:
			y--;
			HERO_MOVEMENT = HERO_MOVE_DOWN_2;
		break;
		case SDLK_KP9:
			x++;
			y--;
			HERO_MOVEMENT = HERO_MOVE_RIGHT;
		break;
		case SDLK_ESCAPE:
			quit();
			return -1;
		break;
		default:
		return -1;
	};
	updateMovement++;
	playfield[HERO_X][HERO_Y] = EMPTY;
	HERO_X = (x < FIELD_X)? (x >= 0)? x: 0: (FIELD_X - 1);
	HERO_Y = (y < FIELD_Y)? (y >= 0)? y: 0: (FIELD_Y - 1);
	if(playfield[HERO_X][HERO_Y] != EMPTY) {		// Hero collision
		playfield[HERO_X][HERO_Y] = HERO_EXPLOSION;
		gamestate = END_GAME;
		drawEverything();
		return 1;
	}
	playfield[HERO_X][HERO_Y] = HERO;
	if(!updateMovement) {
		updateMovement++;
		if(safeTeleports > 0)
		{
			safeTeleports--;
			drawEverything();
			return 1;
		}
		drawEverything();
	}
	return 0;
}

/*!*****************************************************************************
	\brief	Update screen graphics

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int drawEverything(void) {
	SDL_FillRect(screen, NULL, 0xFFFFFF);
	drawProgtagonist();
	drawRobots();
	return 0;
}

/*!*****************************************************************************
	\brief	Initialize SDL surfaces

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int init(void) {
	if(TTF_Init() == -1) {
		printf("Unable to init TTF\n");
		return -1;
	} 

	if(createSurfaces()) {
		printf("Surface load error?\n");
		return -1;
	}

	/* Default is black and white */
	forecol = &white;
	backcol = &black;
	
	/* Clear the background to background color */
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, backcol->r, backcol->g, backcol->b));
	SDL_WM_SetCaption("R.O.B.O.T.S.", 0);

	return 0;
	
}

/*!*****************************************************************************
	\brief	Do the main game functions

	\param	keyPressed
		SDL keyboard input handler

	\param	pressedOnce
		Keyboard repetitive input limiter

	\param	pollTime
		ticktime of the last button press

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
void doGameGraphs(SDLKey *keyPressed, int *pressedOnce, int *pollTime) {
	switch(gamestate) {
		case PLAY_STATE:
			if(*pressedOnce == 1) {
				if(!moveProgtagonist(*keyPressed)) {
					if(moveRobots()) {
						*keyPressed = 0;
						*pollTime = SDL_GetTicks();
						return;
					}
					if(!getRobotCount()) {
						gamestate = NEXT_LEVEL;
					}
					drawEverything();
				}
				*keyPressed = 0;
				*pollTime = SDL_GetTicks();
			} 
			else if((SDL_GetTicks() - *pollTime) >1000) {	
				*pollTime = SDL_GetTicks();
				drawEverything();
			}
		break;
		case MENU_STATE:
			if(*keyPressed == SDLK_SPACE) {
				gamestate = PLAY_STATE;
				drawEverything();
				*pollTime = SDL_GetTicks();
			}
			*keyPressed = 0;
		break;
		case LEVEL_TEXT:
			if((*keyPressed == SDLK_SPACE) || ((SDL_GetTicks() - *pollTime) >1000)) {
				gamestate = PLAY_STATE;
				drawEverything();
				*pollTime = SDL_GetTicks();
			}
			*keyPressed = 0;
		break;
		case NEXT_LEVEL:
			if((SDL_GetTicks() - *pollTime) >1000) {
				setPlayfield();
				*pollTime = SDL_GetTicks();
			}
		break;
		case END_GAME:
			if((SDL_GetTicks() - *pollTime) >1000) {
				endGame();
				*pollTime = SDL_GetTicks();
				gamestate = START_MENU;
			}
		break;
		case CREDITS: // TODO
		break;
		case START_MENU:
			if((SDL_GetTicks() - *pollTime) >1000) {
				drawText(TITLE);
			}
		break;
	}
}

/*!*****************************************************************************
	\brief	The main loop of the game

	\date	7.1.18

	\author	Lari Koskinen
*******************************************************************************/
int main(int argc, char *argv[])
{
	SDL_Event event;
	SDLKey keyPressed = 0;

	int pressedOnce=0;
	int pollTime = 0;
	
	if(init()) {
		return -1;
	}

	pollTime = SDL_GetTicks();
	resetPlayfield();
	drawText(TITLE);
	while (1) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				keyPressed = event.key.keysym.sym;
				if(pressedOnce++) {
					keyPressed = 0;
				}
			}
			else if (event.type == SDL_KEYUP) {
				keyPressed = 0;
				pressedOnce = 0;
			}
		}
		if(keyPressed == SDLK_ESCAPE) {
			quit();
		}
	
		doGameGraphs(&keyPressed, &pressedOnce, &pollTime);	
		
		// Update whole screen
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
		
	return 0;
}
