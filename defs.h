#include "SDL/SDL.h"	

#define	NO_X	11
#define NO_Y	5

#define ROBOCOUNT 10
#define FIELD_X 16
#define FIELD_Y 12
#define FIELD_WIDTH 50

enum robots {
	EMPTY=0,
	ROBOT,
	HERO,
	TRASH,
	MOVED_ROBOT,
	EXPLOSION,
	HERO_EXPLOSION,
};

SDL_Surface *screen;
SDL_Surface *sprites;
SDL_Surface *robot;
SDL_Surface *hero;
SDL_Surface *trash;

SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };
SDL_Color black = { 0x00, 0x00, 0x00, 0 };
SDL_Color *forecol;
SDL_Color *backcol;	
	
SDL_Rect dstrect, srcrect;

TTF_Font *font;
	
char playfield[FIELD_X][FIELD_Y];
int robotCount, currentLevel, HERO_X, HERO_Y,  updateMovement, HERO_MOVEMENT, safeTeleports, robotsKilled;
	
int createSurfaces(void);	
int drawEverything(void);

