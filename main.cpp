#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<cmath>
#include<fstream>
#include<sstream>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
#include <time.h>
}

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		480
#define BACKGROUND_WIDTH	1920/2
#define BACKGROUND_HEIGHT	1080/2
#define PLANE_WH			50
#define GRADE_WH			50
#define PLANE_HEALTH		3
#define BULLET_WH			10
#define CIRCLE_ENEMY_W		400
#define CIRCLE_ENEMY_H		200
#define NONSTATIC_ENEMY_W   400
#define NONSTATIC_ENEMY_H	200
#define MOVING_ENEMY_W		400
#define MOVING_ENEMY_H		200
#define SHOTS_COUNT			20
#define CIRCLE_DENSITY		20
#define CIRCLE_ENEMY_HEALTH	100
#define NONSTATIC_ENEMY_HEALTH 100
#define MOVING_ENEMY_HEALTH 100

double dmod(double x, double y) {
	return x - (int)(x / y) * y;
}
void camera_center(int planePosX, int planePosY, SDL_Rect* camera) {
	camera->x = planePosX - SCREEN_WIDTH /  2;
	camera->y = planePosY - SCREEN_HEIGHT / 2;
	if (camera->x < 0) camera->x = 0;
	if (camera->x > BACKGROUND_WIDTH - camera->w) camera->x = BACKGROUND_WIDTH - camera->w;
	if (camera->y < 0) camera->y = 0;
	if (camera->y > BACKGROUND_HEIGHT - camera->h) camera->y = BACKGROUND_HEIGHT - camera->h;
}



void SetPlane(SDL_Rect& planeC, int &planeHealth)
{
	planeC = { BACKGROUND_WIDTH / 2, BACKGROUND_HEIGHT / 2 + 100, BULLET_WH, BULLET_WH };
	planeHealth = 3;
}
void SetStage0(int enemyPosX[], int enemyPosY[])
{
	for (int i = 0; i < 10; i++)
	{
		enemyPosX[i] = i * 100 + 50;
		enemyPosY[i] = 50;
	}
}

void SetStage1(SDL_Rect &circleEnemyC, int &circleEnemyHealth)
{
		circleEnemyC = { BACKGROUND_WIDTH/2-CIRCLE_ENEMY_W/2, 100, CIRCLE_ENEMY_W, CIRCLE_ENEMY_H };
		circleEnemyHealth = CIRCLE_ENEMY_HEALTH;
}

void SetStage2(SDL_Rect &nonstaticEnemyC,SDL_Rect nonstaticEnemyBulletC[], SDL_Rect nonstaticEnemyBulletDest[], int &nonstaticEnemyHealth)
{
	nonstaticEnemyC = { BACKGROUND_WIDTH / 2 - CIRCLE_ENEMY_W / 2, 100, CIRCLE_ENEMY_W, CIRCLE_ENEMY_H };
	nonstaticEnemyHealth = NONSTATIC_ENEMY_HEALTH;
	for (int i = 0; i < SHOTS_COUNT; i++)
	{
		nonstaticEnemyBulletC[i] = { nonstaticEnemyC.x + CIRCLE_ENEMY_W / 2, nonstaticEnemyC.y + CIRCLE_ENEMY_H / 2,BULLET_WH, BULLET_WH };
		nonstaticEnemyBulletDest[i] = { BACKGROUND_WIDTH/2, BACKGROUND_HEIGHT, 0,0 };
	}
}

void SetStage3(SDL_Rect& movingEnemyC, SDL_Rect movingEnemyBulletC[], int& movingEnemyHealth)
{
	movingEnemyC = { BACKGROUND_WIDTH / 2 - CIRCLE_ENEMY_W / 2, 100, MOVING_ENEMY_W, MOVING_ENEMY_H };
	movingEnemyHealth = MOVING_ENEMY_HEALTH;
	for (int i = 0; i < SHOTS_COUNT; i++)
	{
		movingEnemyBulletC[i] = { movingEnemyC.x + MOVING_ENEMY_W / 2, movingEnemyC.y + MOVING_ENEMY_H / 2, BULLET_WH, BULLET_WH };
	}
}
// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x;// -sprite->w / 2;
	dest.y = y;// -sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void DrawNotFilledRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor) {
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
};

void MyDrawingFilled(SDL_Surface* screen, SDL_Rect* platform, SDL_Rect* camera, int kolor1, int kolor2)
{
	SDL_Rect onscreen = { 0,0,0,0 };
	//for (int i = 0; i < platform_count; i++) 
	if (SDL_IntersectRect(camera, platform, &onscreen) == SDL_TRUE) {
		DrawRectangle(screen, onscreen.x - camera->x, onscreen.y - camera->y, onscreen.w, onscreen.h, kolor1, kolor2);
	}
}

void MyDrawingNotFilled(SDL_Surface* screen, SDL_Rect* platform, SDL_Rect* camera, int kolor1)
{
	SDL_Rect onscreen = { 0,0,0,0 };
	//for (int i = 0; i < platform_count; i++) 
	if (SDL_IntersectRect(camera, platform, &onscreen) == SDL_TRUE) {
		DrawNotFilledRectangle(screen, onscreen.x - camera->x, onscreen.y - camera->y, onscreen.w, onscreen.h, kolor1);
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	srand(time(NULL));
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface* eti, * background;

	SDL_Surface* enemy[10]; // do usuniecia

	SDL_Surface *grade, *gradeAnim;
	SDL_Surface *blow, *blowAnim;

	//plane
	SDL_Surface* plane, *planeAnim;
	int planeBulletCount = 0;
	int planeHealth = 3;
	SDL_Rect planeHealthBar;
	SDL_Rect planeBulletC[SHOTS_COUNT];
	for (int i = 0; i < SHOTS_COUNT; i++)
		planeBulletC[i] = { -100,-100, BULLET_WH, BULLET_WH };
	SDL_Rect planeC;
	SDL_Rect planeCenter;

	//circleEnemy
	SDL_Surface* circleEnemy;																		//grafika
	SDL_Rect circleEnemyBulletC[SHOTS_COUNT][CIRCLE_DENSITY];										//pociski
	int circleEnemyBulletRadius[SHOTS_COUNT];														//promien pociskow
	int circleEnemyHealth = CIRCLE_ENEMY_HEALTH;
	SDL_Rect circleEnemyHealthBar;
	for (int i = 0; i < SHOTS_COUNT; i++)
	{
		circleEnemyBulletRadius[i] = 0;
		for (int j = 0; j < CIRCLE_DENSITY; j++)
			circleEnemyBulletC[i][j] = { -100,-100, BULLET_WH, BULLET_WH };
	}													
	SDL_Rect circleEnemyC;																			//hitbox i pozycja

	//nonstaticEnemy
	SDL_Surface* nonstaticEnemy;
	SDL_Rect nonstaticEnemyBulletC[SHOTS_COUNT];
	SDL_Rect nonstaticEnemyBulletDest[SHOTS_COUNT];
	int nonstaticEnemyHealth = NONSTATIC_ENEMY_HEALTH;
	SDL_Rect nonstaticEnemyHealthBar;
	for (int i = 0; i < SHOTS_COUNT; i++)
	{
		nonstaticEnemyBulletC[i] = { -100,-100, BULLET_WH, BULLET_WH };
		nonstaticEnemyBulletDest[i] = { -100,-100, BULLET_WH, BULLET_WH };
	}
	SDL_Rect nonstaticEnemyC;

	//movingEnemy
	SDL_Surface* movingEnemy;
	SDL_Rect movingEnemyBulletC[SHOTS_COUNT];
	SDL_Rect movingEnemySplashC[CIRCLE_DENSITY];
	for (int i = 0; i < CIRCLE_DENSITY; i++)
		movingEnemySplashC[i] = { -100,-100, BULLET_WH, BULLET_WH };
	SDL_Rect splashSpot = {-100,-100, 0 ,0};
	int movingEnemySplashRadius = 0;
	int movingEnemyHealth = MOVING_ENEMY_HEALTH;
	SDL_Rect movingEnemyHealthBar;
	for (int i = 0; i < SHOTS_COUNT; i++)
	{
		nonstaticEnemyBulletC[i] = { -100,-100, BULLET_WH, BULLET_WH };
	}
	SDL_Rect movingEnemyC;



	int enemyBulletPosX[10][10], enemyBulletPosY[10][10]; /// todo do usuniecia
	int nonstaticEnemyBulletPosX[3][10], nonstaticEnemyBulletPosY[3][10];
	int movingEnemyBulletPosX[2][10], movingEnemyBulletPosY[2][10];
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	// okno konsoli nie jest widoczne, je¿eli chcemy zobaczyæ
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieniæ na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);

	eti = SDL_LoadBMP("./eti.bmp");
	if(eti == NULL) {
		printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	background = SDL_LoadBMP("./background.bmp");
	if (background == NULL) {
		printf("SDL_LoadBMP(background.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	plane = SDL_LoadBMP("./plane.bmp");
	if (plane == NULL) {
		printf("SDL_LoadBMP(plane.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	planeAnim = SDL_LoadBMP("./plane.bmp");
	if (planeAnim == NULL) {
		printf("SDL_LoadBMP(plane.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	//enemies
	for (int i = 0; i < 10; i++)
	{
		enemy[i] = SDL_LoadBMP("./enemy.bmp");
		if (enemy[i] == NULL) {
			printf("SDL_LoadBMP(enemy.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};
	}
	
	circleEnemy = SDL_LoadBMP("./circleEnemy.bmp");
	if (circleEnemy == NULL) {
		printf("SDL_LoadBMP(circleEnemy.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	nonstaticEnemy = SDL_LoadBMP("./circleEnemy.bmp");
	if (nonstaticEnemy == NULL) {
		printf("SDL_LoadBMP(circleEnemy.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	movingEnemy = SDL_LoadBMP("./circleEnemy.bmp");
	if (movingEnemy == NULL) {
		printf("SDL_LoadBMP(circleEnemy.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	grade = SDL_LoadBMP("./grade.bmp");
	if (grade == NULL) {
		printf("SDL_LoadBMP(grade.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	gradeAnim = SDL_LoadBMP("./grade.bmp");
	if (gradeAnim == NULL) {
		printf("SDL_LoadBMP(grade.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	blow = SDL_LoadBMP("./blow.bmp");
	if (blow == NULL) {
		printf("SDL_LoadBMP(blow.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	blowAnim = SDL_LoadBMP("./blow.bmp");
	if (blowAnim == NULL) {
		printf("SDL_LoadBMP(blow.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	//bullets
	

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int bialy = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);

	SDL_SetColorKey(plane, SDL_TRUE, bialy);
	SDL_SetColorKey(circleEnemy, SDL_TRUE, bialy);
	SDL_SetColorKey(nonstaticEnemy, SDL_TRUE, bialy);
	SDL_SetColorKey(movingEnemy, SDL_TRUE, bialy);
	SDL_SetColorKey(grade, SDL_TRUE, bialy);
	SDL_SetColorKey(blow, SDL_TRUE, bialy);
	

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	const int planeFrames = 5;
	SDL_Rect planeClips[planeFrames];
	for (int i = 0; i < planeFrames; i++)
		planeClips[i] = { i * PLANE_WH, 0, PLANE_WH, PLANE_WH };
	int planeCurrentAnim = 0;

	const int gradeFrames = 7;
	SDL_Rect gradeClips[gradeFrames];
	for (int i = 0; i < gradeFrames; i++)
			gradeClips[i] = { i * GRADE_WH, 0, GRADE_WH, GRADE_WH };
	int gradeCurrentAnim = 0;

	const int blowFrames = 7;
	SDL_Rect blowClips[blowFrames];
	for (int i = 0; i < blowFrames; i++)
		blowClips[i] = { i * GRADE_WH, 0, GRADE_WH, GRADE_WH };
	int blowCurrentAnim = 0;
	double blowTimer = 0;



	SDL_Rect camera; camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	






	planeC = { BACKGROUND_WIDTH / 2, BACKGROUND_HEIGHT / 2+100, BULLET_WH, BULLET_WH };
	

	


	int velocityX = 0, velocityY = 0;
	int movingVelocityX = 1;
	int bulletVelocityX = 1, bulletVelocityY = 1;

	int change = 1;
	int slowdown = 0;
	bool shoot = false;
	int stage = 2;
	bool circleUltimateAbility = false;
	bool circleUltimateAbilityTimerReset = true;
	double circleUltimateAbilityTimer = 0;
	SDL_Rect circleUltimateArea{ -100,-100,100,100 };

	bool bonusHealth = false;
	bool bonusHealthTimerReset = true;
	double bonusHealthTimer = 0;
	SDL_Rect bonusHealthArea{ -100,-100,100,100 };

	


	bool immunity = false;
	double immunityTimer = 0;
	int points = 100;
	int menu = 1;  // 0 game, 1 - menu, - 2 game over, 3 - leaderboard
	bool save = false;
	std::string inputText = "#";
	int menuIndex = 0;
	bool gradeAnimation = true;
	char gradeLetter = 'F';
	char gradeChecker = 'F';
	bool splash = false;
	//##################################################################################################################################################
	while(!quit) {
		SDL_FillRect(plane, NULL, bialy);
		SDL_BlitSurface(planeAnim, &planeClips[planeCurrentAnim], plane, NULL);
		SDL_FillRect(grade, NULL, bialy);
		SDL_BlitSurface(gradeAnim, &gradeClips[gradeCurrentAnim], grade, NULL);

		



		if (menu==0)
		{
			slowdown++;
			t2 = SDL_GetTicks();
			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna³ od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds
			delta = (t2 - t1) * 0.001;
			t1 = t2;

			worldTime += delta;
			circleUltimateAbilityTimer += delta;
			immunityTimer += delta;
			blowTimer += delta;
			distance += etiSpeed * delta;


			//jesli gracz straci zycie idzie do konca gry
			if (planeHealth <= 0)
				menu = 2; //koniec gry

			//przejscie na nastepny poziom
			switch (stage)
			{
			case 1:
				if (circleEnemyHealth < 0)
				{
					SetStage2(nonstaticEnemyC, nonstaticEnemyBulletC, nonstaticEnemyBulletDest, nonstaticEnemyHealth);
					stage = 2;
				}
				break;
			case 2:
				if (nonstaticEnemyHealth < 0)
				{
					SetStage3(movingEnemyC, movingEnemyBulletC, movingEnemyHealth);
					stage = 3;
				}
			case 3:
				if (movingEnemyHealth < 0)
				{
					menu = 2;
				}
				break;
			}

			//health bary
			planeHealthBar = { planeC.x, planeC.y + PLANE_WH, PLANE_WH - (PLANE_HEALTH - planeHealth) * PLANE_WH / PLANE_HEALTH, 5 };
			
			switch (stage)
			{
			case 1:
				circleEnemyHealthBar = { circleEnemyC.x, circleEnemyC.y, CIRCLE_ENEMY_W - (CIRCLE_ENEMY_HEALTH - circleEnemyHealth) * CIRCLE_ENEMY_W / CIRCLE_ENEMY_HEALTH, 10 };
				break;
			case 2:
				nonstaticEnemyHealthBar = { nonstaticEnemyC.x, nonstaticEnemyC.y, NONSTATIC_ENEMY_W - (NONSTATIC_ENEMY_HEALTH - nonstaticEnemyHealth) * NONSTATIC_ENEMY_W / NONSTATIC_ENEMY_HEALTH, 10 };
				break;
			case 3:
				movingEnemyHealthBar = { movingEnemyC.x, movingEnemyC.y, MOVING_ENEMY_W - (MOVING_ENEMY_HEALTH - movingEnemyHealth) * MOVING_ENEMY_W / MOVING_ENEMY_HEALTH, 10 };
				break;
			}
			

			//przesuwanie samolotu i moving enemy o podane velocity
			if (slowdown % 3 == 0)
			{
				planeC.x += velocityX;
				planeC.y += velocityY;
				planeCurrentAnim = 0;
				if (velocityX >= 1) planeCurrentAnim = 1;
				if (velocityX <= -1) planeCurrentAnim = 2;
				if (velocityY <= -1) planeCurrentAnim = 3;
				if (velocityY >= 1) planeCurrentAnim = 4;

				switch (stage)
				{
				case 3:

					if (movingEnemyC.x > BACKGROUND_WIDTH - MOVING_ENEMY_W || movingEnemyC.x < 0)
						movingVelocityX *= -1;
					movingEnemyC.x += movingVelocityX;
					

					break;
				}

			}

			//przesuwanie srodkowego hitboxu
			planeCenter = { planeC.x + PLANE_WH / 2 - BULLET_WH / 2, planeC.y + PLANE_WH / 2 - BULLET_WH / 2, BULLET_WH, BULLET_WH };


			//immunity przez 3 sek
			if (immunityTimer > 3)
				immunity = false;

			//warunek aby plane nie wylecia³ poza planszê
			if (planeC.x < 0)
				planeC.x = 0;
			if (planeC.x > BACKGROUND_WIDTH - PLANE_WH)
				planeC.x = BACKGROUND_WIDTH - PLANE_WH;
			if (planeC.y < 0)
				planeC.y = 0;
			if (planeC.y > BACKGROUND_HEIGHT - PLANE_WH)
				planeC.y = BACKGROUND_HEIGHT - PLANE_WH;

			//Poruszanie sie pociskow
			if (shoot) //plane
			{
				planeBulletC[planeBulletCount].y = planeC.y;
				planeBulletC[planeBulletCount].x = planeC.x + PLANE_WH / 2 - BULLET_WH / 2;
				planeBulletCount = (planeBulletCount + 1) % SHOTS_COUNT;
				shoot = false;
			}
			if (slowdown % 10 == 0) //opponents
			{
				//plane
				for (int i = 0; i < SHOTS_COUNT; i++)
					planeBulletC[i].y -= bulletVelocityY * 10;

				//circleEnemy
				switch (stage)
				{
				case 1:
					for (int i = 0; i < SHOTS_COUNT; i++)
					{
						circleEnemyBulletRadius[i] += i / 3;
						if (circleEnemyBulletRadius[i] > SCREEN_WIDTH)
							circleEnemyBulletRadius[i] = 0;
						for (int j = 0; j < CIRCLE_DENSITY; j++)
						{
							circleEnemyBulletC[i][j].x = cos(2 * M_PI * j / CIRCLE_DENSITY + i) * circleEnemyBulletRadius[i] + circleEnemyC.x + CIRCLE_ENEMY_W / 2 - BULLET_WH / 2;
							circleEnemyBulletC[i][j].y = sin(2 * M_PI * j / CIRCLE_DENSITY + i) * circleEnemyBulletRadius[i] + circleEnemyC.y + CIRCLE_ENEMY_H / 2 - BULLET_WH / 2;
						}
					}
					break;
				case 2:
					for (int i = 0; i < SHOTS_COUNT; i++)
					{
						if (nonstaticEnemyBulletC[i].x < nonstaticEnemyBulletDest[i].x)
							nonstaticEnemyBulletC[i].x += rand()%2*10;
						else
							nonstaticEnemyBulletC[i].x -= rand() % 2 * 10;

						nonstaticEnemyBulletC[i].y += (rand()%2+1)*5;

						if (nonstaticEnemyBulletC[i].y > SCREEN_HEIGHT+PLANE_WH)
						{
							nonstaticEnemyBulletC[i].y = nonstaticEnemyC.y + nonstaticEnemyC.h / 2;
							nonstaticEnemyBulletC[i].x = nonstaticEnemyC.x + nonstaticEnemyC.w / 2;
							if (dmod(worldTime, 3) < 0.1)
							{
								nonstaticEnemyBulletC[i].w = BULLET_WH * 2;
								nonstaticEnemyBulletC[i].h = BULLET_WH * 2;
							}
							else
							{
								nonstaticEnemyBulletC[i].w = BULLET_WH;
								nonstaticEnemyBulletC[i].h = BULLET_WH;
							}
							nonstaticEnemyBulletDest[i] = { planeC.x, planeC.y, 0, 0 };
						}

					}
					break;
				case 3:
					for (int i = 0; i < SHOTS_COUNT; i++)
					{

						movingEnemyBulletC[i].y += (i+1)/2+1;

						if (movingEnemyBulletC[i].y > SCREEN_HEIGHT + PLANE_WH)
						{
							if (dmod(worldTime, 3) < 0.1)
							{
								splash = true;
								splashSpot = { movingEnemyBulletC[i].x , movingEnemyBulletC[i].y,0 ,0 };
							}
							movingEnemyBulletC[i].y = movingEnemyC.y + movingEnemyC.h / 2;
							movingEnemyBulletC[i].x = movingEnemyC.x + movingEnemyC.w / 2;
							
						}

					}
					if (splash)
					{
						for (int j = 0; j < CIRCLE_DENSITY; j++)
						{
							movingEnemySplashRadius++;
							movingEnemySplashC[j].x = cos(2 * M_PI * j / CIRCLE_DENSITY ) * movingEnemySplashRadius + splashSpot.x;
							movingEnemySplashC[j].y = sin(2 * M_PI * j / CIRCLE_DENSITY ) * movingEnemySplashRadius + splashSpot.y;
							if (movingEnemySplashRadius > 200)
							{
								movingEnemySplashRadius = 0;
								for(int k=0; k<CIRCLE_DENSITY; k++)
									movingEnemySplashC[k] = { -100, -100, BULLET_WH, BULLET_WH };
								splash = false;
								break;
							}
						}
					}
					break;
				}
				
			}
			if (dmod(worldTime, 5) < 0.1)
				circleUltimateAbility = true; //umiejetnosc specjalna

			if (dmod(worldTime, 3) < 0.1)
				bonusHealth = true; //bonusowe zycie

			//sprawdzanie kolizji
			if (!immunity) //dla samolotu
			{
				switch (stage)
				{
				case 1:
					if (SDL_HasIntersection(&planeCenter, &circleEnemyC))
					{
						immunity = true;
						immunityTimer = 0;
						planeHealth -= 1;
						points /= 4;
					}
					for (int i = 0; i < SHOTS_COUNT; i++)
						for (int j = 0; j < CIRCLE_DENSITY; j++)
							if (SDL_HasIntersection(&planeCenter, &circleEnemyBulletC[i][j]))
							{
								immunity = true;
								immunityTimer = 0;
								planeHealth -= 1;
								points /= 4;
							}
					for (int i = 0; i < SHOTS_COUNT; i++) //dla circleEnemy
						if (SDL_HasIntersection(&circleEnemyC, &planeBulletC[i]))
						{
							planeBulletC[i] = { -100,-100, BULLET_WH, BULLET_WH };
							circleEnemyHealth -= 1;
							points = points * 1.05 + 1;
						}
					break;
				case 2:
					if (SDL_HasIntersection(&planeCenter, &nonstaticEnemyC))
					{
						immunity = true;
						immunityTimer = 0;
						planeHealth -= 1;
						points /= 4;
					}
					for (int i = 0; i < SHOTS_COUNT; i++)
						if (SDL_HasIntersection(&planeCenter, &nonstaticEnemyBulletC[i]))
						{
							immunity = true;
							immunityTimer = 0;
							planeHealth -= 1;
							points /= 4;
						}
					for (int i = 0; i < SHOTS_COUNT; i++) //dla nonstaticEnemy
						if (SDL_HasIntersection(&nonstaticEnemyC, &planeBulletC[i]))
						{
							planeBulletC[i] = { -100,-100, BULLET_WH, BULLET_WH };
							nonstaticEnemyHealth -= 1;
							points = points * 1.05 + 1;
						}
					break;
				case 3:
					if (SDL_HasIntersection(&planeCenter, &movingEnemyC))
					{
						immunity = true;
						immunityTimer = 0;
						planeHealth -= 1;
						points /= 4;
					}
					for (int i = 0; i < SHOTS_COUNT; i++)
						if (SDL_HasIntersection(&planeCenter, &movingEnemyBulletC[i]))
						{
							immunity = true;
							immunityTimer = 0;
							planeHealth -= 1;
							points /= 4;
						}
					for (int i = 0; i < SHOTS_COUNT; i++) //dla nonstaticEnemy
						if (SDL_HasIntersection(&movingEnemyC, &planeBulletC[i]))
						{
							planeBulletC[i] = { -100,-100, BULLET_WH, BULLET_WH };
							movingEnemyHealth -= 1;
							points = points * 1.05 + 1;
						}
					break;
				}	
			}

			



			//RYSOWANIE

			SDL_FillRect(screen, NULL, czarny);
			camera_center(planeC.x, planeC.y, &camera);
			DrawSurface(screen, background, -camera.x, -camera.y);

			if (circleUltimateAbility)
			{
				if (circleUltimateAbilityTimerReset)
				{
					circleUltimateAbilityTimer = 0;
					circleUltimateAbilityTimerReset = false;
					if (rand() % 2)
						circleUltimateArea = { (rand() % SCREEN_WIDTH) - 100,(rand() % SCREEN_HEIGHT) - 100,100,100 };
					else
						circleUltimateArea = { (rand() % SCREEN_WIDTH) - 100,(rand() % SCREEN_HEIGHT) - 100,200,100 };
				}

				MyDrawingNotFilled(screen, &circleUltimateArea, &camera, czerwony);
				if (circleUltimateAbilityTimer > 3.0)
				{
					MyDrawingFilled(screen, &circleUltimateArea, &camera, czerwony, czerwony);
					if (SDL_HasIntersection(&planeCenter, &circleUltimateArea)) DrawRectangle(screen, 100, 100, 100, 100, czerwony, czerwony);
					circleUltimateAbility = false;
					circleUltimateAbilityTimerReset = true;
				}

			}

			if (bonusHealth)
			{
				if (bonusHealthTimerReset)
				{
					bonusHealthTimer = 0;
					bonusHealthTimerReset = false;
						bonusHealthArea = { (rand() % SCREEN_WIDTH) - 100,(rand() % SCREEN_HEIGHT) - 100,10,10 };
				}

				MyDrawingFilled(screen, &bonusHealthArea, &camera, czerwony, zielony);
				if (SDL_HasIntersection(&planeCenter, &bonusHealthArea))
				{
					if(planeHealth < 3)
						planeHealth += 1;
					bonusHealth = false;
					bonusHealthTimerReset = true;
				}

			}
			//samolot
			DrawSurface(screen, plane, planeC.x - camera.x, planeC.y - camera.y);
			DrawRectangle(screen, planeCenter.x - camera.x, planeCenter.y - camera.y, BULLET_WH, BULLET_WH, niebieski, zielony);
			MyDrawingFilled(screen, &planeHealthBar, &camera, czerwony, czerwony);

			//rysowanie enemy
			switch (stage)
			{
			case 1:
				DrawSurface(screen, circleEnemy, circleEnemyC.x - camera.x, circleEnemyC.y - camera.y);
				MyDrawingFilled(screen, &circleEnemyHealthBar, &camera, czerwony, czerwony);
				break;
			case 2:
				DrawSurface(screen, nonstaticEnemy, nonstaticEnemyC.x - camera.x, nonstaticEnemyC.y - camera.y);
				MyDrawingFilled(screen, &nonstaticEnemyHealthBar, &camera, czerwony, czerwony);
				break;
			case 3:
				DrawSurface(screen, movingEnemy, movingEnemyC.x - camera.x, movingEnemyC.y - camera.y);
				MyDrawingFilled(screen, &movingEnemyHealthBar, &camera, czerwony, czerwony);
				break;
			}
			


			//rysowanie pociskow
			for (int i = 0; i < SHOTS_COUNT; i++)
				MyDrawingFilled(screen, &planeBulletC[i], &camera, czerwony, niebieski);


			switch (stage)
			{
			case 1:
				for (int i = 0; i < SHOTS_COUNT; i++)
					for (int j = 0; j < CIRCLE_DENSITY; j++)
						MyDrawingFilled(screen, &circleEnemyBulletC[i][j], &camera, niebieski, czerwony);
				break;
			case 2:
				for(int i =0; i<SHOTS_COUNT; i++)
					MyDrawingFilled(screen, &nonstaticEnemyBulletC[i], &camera, niebieski, czerwony);
				break;
			case 3:
				for (int i = 0; i < SHOTS_COUNT; i++)
					MyDrawingFilled(screen, &movingEnemyBulletC[i], &camera, niebieski, czerwony);
				for(int i =0; i<CIRCLE_DENSITY; i++)
					MyDrawingFilled(screen, &movingEnemySplashC[i], &camera, niebieski, czerwony);
				break;
			}
			

			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};
			save = false;
			inputText = "#";
		}
		else if(menu == 1) // menu glowne
		{
			SDL_FillRect(screen, NULL, czarny);
			DrawRectangle(screen, 4, SCREEN_HEIGHT/3, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			sprintf(text, "Rozpocznij custom poziom: Wcisnij 0");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT/3+15, text, charset);
			DrawRectangle(screen, 4, SCREEN_HEIGHT / 3+40, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			sprintf(text, "Rozpocznij pierwszy poziom: Wcisnij 1");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 55, text, charset);
			DrawRectangle(screen, 4, SCREEN_HEIGHT / 3 + 80, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			sprintf(text, "Rozpocznij drugi poziom: Wcisnij 2");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 95, text, charset);
			DrawRectangle(screen, 4, SCREEN_HEIGHT / 3 + 120, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			sprintf(text, "Rozpocznij trzeci poziom: Wcisnij 3");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 135, text, charset);
			DrawRectangle(screen, 4, SCREEN_HEIGHT / 3 + 160, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			sprintf(text, "Leaderboard: /");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 175, text, charset);


		}
		else if (menu == 2) //koniec gry
		{
			DrawRectangle(screen, 4, SCREEN_HEIGHT / 3, SCREEN_WIDTH - 8, 36, czerwony, czerwony);
			sprintf(text, "KONIEC GRY");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 15, text, charset);
			DrawRectangle(screen, 4, SCREEN_HEIGHT / 3+40, SCREEN_WIDTH - 8, 36, czerwony, czerwony);
			sprintf(text, "WROC DO MENU: ;     SPROBUJ JESZCZE RAZ: WYBIERZ 1, 2 lub 3");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 15 + 40, text, charset);
			DrawRectangle(screen, 4, SCREEN_HEIGHT / 3 + 80, SCREEN_WIDTH - 8, 36, czerwony, czerwony);
			sprintf(text, "ZAPISZ WYNIK: '     USERNAME: %s", inputText.c_str());
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 15 + 80, text, charset);
			SDL_StartTextInput();
			if (save)
			{
				SDL_StopTextInput();
				std::fstream file;
				file.open("scores.txt", std::ios::app|std::ios::ate);
				file << points << " " <<inputText <<  "\n";
				file.close();
				save = false;
				menu = 1;
			}
		}
		else if (menu == 3) //leaderboard
		{
			SDL_FillRect(screen, NULL, czarny);
			std::fstream file;
			file.open("scores.txt", std::ios::in);
			int loop = 0;
			int pointsFromFile[100];
			char username[100][128];
			for (int i = 0; i < 100; i++)
			{
				pointsFromFile[i] = 0;
				strcpy(username[i], "#");
			}

			while (file >> pointsFromFile[loop] >> username[loop])
				loop++;

			for(int i =0; i<99; i++)
				for (int j = 0; j < 99 - i; j++)
					if (pointsFromFile[j] <= pointsFromFile[j + 1])
					{
						std::swap(pointsFromFile[j], pointsFromFile[j + 1]);
						std::swap(username[j], username[j + 1]);
					}

			for (int i = 10*menuIndex; i < (menuIndex*10+10) && i < loop; i++)
			{
				sprintf(text, "%i points, %s", pointsFromFile[i], username[i]);
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 50 + 40 *(i%10) , text, charset);
			}
				
			sprintf(text, "%i page", menuIndex+1);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT-15, text, charset);
			
			file.close();
		}

		// tekst informacyjny / info text
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		sprintf(text, "Bullet hell, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);

		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
		sprintf(text, "Esc - wyjscie, strzalki - poruszanie sie, punkty: %i", points);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

		if (points <= 5000) { gradeCurrentAnim = 0; gradeLetter = 'F';}
		if (points > 5000)  { gradeCurrentAnim = 1; gradeLetter = 'E';}
		if (points > 10000) { gradeCurrentAnim = 2; gradeLetter = 'D';}
		if (points > 15000) { gradeCurrentAnim = 3; gradeLetter = 'C';}
		if (points > 25000) { gradeCurrentAnim = 4; gradeLetter = 'B';}
		if (points > 35000) { gradeCurrentAnim = 5; gradeLetter = 'A';}
		if (points > 50000) { gradeCurrentAnim = 6; gradeLetter = 'S';}

		if (gradeLetter!=gradeChecker)
		{
			SDL_FillRect(blow, NULL, bialy);
			SDL_BlitSurface(blowAnim, &blowClips[blowCurrentAnim], blow, NULL);
			DrawSurface(screen, blow, 0, SCREEN_HEIGHT - PLANE_WH);
			if (dmod(worldTime, 0.5)<0.01)
				blowCurrentAnim++;
			if (blowCurrentAnim > blowFrames)
			{
				blowCurrentAnim = 0;
				gradeAnimation = false;
				gradeChecker = gradeLetter;
			}
		}

		DrawRectangle(screen, 0, SCREEN_HEIGHT-12, 12,12, czerwony, niebieski);
		DrawString(screen, 0, SCREEN_HEIGHT-10, text, charset);
		DrawSurface(screen, grade, 0, SCREEN_HEIGHT - GRADE_WH);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_UP && event.key.repeat == 0) velocityY += -1;
					else if (event.key.keysym.sym == SDLK_DOWN && event.key.repeat == 0) velocityY += 1;
					else if (event.key.keysym.sym == SDLK_LEFT && event.key.repeat == 0) velocityX += -1;
					else if (event.key.keysym.sym == SDLK_RIGHT && event.key.repeat == 0) velocityX += 1;
					else if (event.key.keysym.sym == SDLK_BACKSLASH) shoot = true;
					else if (event.key.keysym.sym == SDLK_SEMICOLON) menu = 1;
					else if (event.key.keysym.sym == SDLK_QUOTE) save = true;
					else if (event.key.keysym.sym == SDLK_SLASH) menu = 3;
					else if (event.key.keysym.sym == SDLK_PERIOD && event.key.repeat == 0) { menuIndex += 1; if (menuIndex > 9) menuIndex = 9; }
					else if (event.key.keysym.sym == SDLK_COMMA && event.key.repeat == 0) { menuIndex -= 1; if (menuIndex < 0) menuIndex = 0; }
					else if (event.key.keysym.sym == SDLK_1)
					{
						points = 100;
						menu = 0;
						SetStage1(circleEnemyC, circleEnemyHealth);
						SetPlane(planeC, planeHealth);
						stage = 1;
					}
					else if (event.key.keysym.sym == SDLK_2)
					{
						points = 100;
						menu = 0;
						SetStage2(nonstaticEnemyC, nonstaticEnemyBulletC, nonstaticEnemyBulletDest, nonstaticEnemyHealth);
						SetPlane(planeC, planeHealth);
						stage = 2;
					}
					else if (event.key.keysym.sym == SDLK_3)
					{
						points = 100;
						menu = 0;
						SetStage3(movingEnemyC, movingEnemyBulletC, movingEnemyHealth);
						SetPlane(planeC, planeHealth);
						stage = 3;
					}
					break;
				case SDL_KEYUP:
					etiSpeed = 1.0;
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_LEFT && event.key.repeat == 0) velocityX += 1;
					else if (event.key.keysym.sym == SDLK_RIGHT && event.key.repeat == 0) velocityX += -1;
					else if (event.key.keysym.sym == SDLK_UP && event.key.repeat == 0) velocityY += 1;
					else if (event.key.keysym.sym == SDLK_DOWN && event.key.repeat == 0) velocityY += -1;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_TEXTINPUT:
					inputText += event.text.text;
					break;
				};
			};
		frames++;
		};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
