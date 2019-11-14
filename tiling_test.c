/*
2019-11-07
Written by Tolerblanc(HyunJun KIM)
SDL Pratice

#Used external library
-SDL
-SDL_Image
-SDL_ttf
-han2unicode

#To do
-partition compile
-dynamic string
-tiling
-particle engine
-basic game setting
-minimap

-resize*
-random monster spawn*
-random exit portal*
-items*
*/
#include <SDL.h>                  //SDL lib
#include <SDL_image.h>            //SDL_image lib
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include <string.h>

const int LEVEL_WIDTH = 2560;  //레벨층 가로
const int LEVEL_HEIGHT = 1480; //레벨층 세로

const int SCREEN_WIDTH = 1280; //스크린 가로
const int SCREEN_HEIGHT = 720; //스크린 세로

const int SCREEN_FPS = 60;     //스크린 프레임
const int SCREEN_TICK_PER_FRAME = 17; //틱당 프레임

//타일 상수
const int TILE_WIDTH = 80;//타일 가로크기
const int TILE_HEIGHT = 80;//타일 세로크기
const int TOTAL_TILES = 763;
const int TOTAL_TILE_SPRITES = 12;

enum KeyPressSurfaces //키입력 변수를 담는 열거형
{
	KEY_PRESS_SURFACE_DEFAULT, //=0
	KEY_PRESS_SURFACE_UP,      //=1
	KEY_PRESS_SURFACE_DOWN,    //=2
	KEY_PRESS_SURFACE_LEFT,    //=3
	KEY_PRESS_SURFACE_RIGHT,   //=4
	KEY_PRESS_SURFACE_O,       //=5
	KEY_PRESS_SURFACE_TOTAL,   //=6
};

typedef struct _LTexture       //텍스쳐 구조체
{
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;

}_LTexture;

typedef struct _LPlayer //플레이어 구조체
{
	int Player_WIDTH;
	int Player_HEIGHT;
	int Player_VEL;

	SDL_Rect mBox;
	int mVelX, mVelY;
}_LPlayer;

typedef struct _LTimer //타이머 구조체
{
	Uint32 mStartTicks;
	Uint32 mPauseTicks;
	bool mPaused;
	bool mStarted;
}_LTimer;

typedef struct _LTile
{
	SDL_Rect mBox;
	int mType;
}_LTile;

//텍스쳐 등 구현할 구조체들 정의
struct _LTexture gMainplayerTexture[KEY_PRESS_SURFACE_TOTAL];
struct _LTexture gDuckTexture;
struct _LTexture gTextTexture[2]; //택스트도 구조체 배열을 통하여 미리 집어넣어놓고, 이벤트에 따라서 꺼내어 랜더링 할 수 있다.
struct _LTexture gTileTexture[12];
struct _LTexture gCurrentTile;
struct _LTexture gCurrentSurface; //현재 표시되는 서피스
struct _LPlayer gPlayer;


//함수 정의
bool init(); //초기화
bool loadMedia(_LTile* tiles[]); //미디어 로딩
void close(); // SDL 라이브러리 닫기
bool loadFromFile(struct _LTexture* LT, SDL_Renderer* Renderer, char* path);  //텍스쳐 불러오기
void lfree(struct _LTexture* LT); //닫기
void render(_LTexture* LT, SDL_Renderer* Renderer, int x, int y); //지정된 좌표에 랜더링
void V_handleEvent(_LPlayer* LP, SDL_Event*e);  //이벤트에 따른 속도조절
void T_handleEvent(_LTexture* CT,_LTexture* LT, SDL_Event* e); //이벤트에 따른 텍스쳐 조절
bool setTiles(_LTile *tiles);
bool touchesWall(SDL_Rect box, _LTile* tiles);

//character
void move(_LPlayer* LP, _LTile *tiles);
bool checkCollision(SDL_Rect a, SDL_Rect b); //충돌판정
int getPosX(_LPlayer* LP);
int getPosY(_LPlayer* LP);
void setCamera(_LPlayer* LP, SDL_Rect* camera);

SDL_Texture* loadTexture(char* path); //위와 같은 기능 ( 서피스보다 성능 좋음)
SDL_Window* gWindow = NULL; //랜더링 윈도우 포인터
SDL_Surface* gScreenSurface = NULL; //스크린서피스 포인터
SDL_Renderer* gRenderer = NULL;     //랜더러 포인터
SDL_Texture* gTexture = NULL;       //기본 텍스쳐 포인터

int main()
{
	//SDL 모듈 초기화
	if (!init())
	{
		printf("초기화 실패!\n");
	}
	else
	{
		_LTile tileSet[12000] = { 0, };
		//미디어 로드
		if (!loadMedia(tileSet))
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//메인루프 플래그
			bool quit = false;

			//Event handler
			SDL_Event e;

			SDL_Rect Camera;
			Camera.x = 0;
			Camera.y = 0;
			Camera.w = SCREEN_WIDTH;
			Camera.h = SCREEN_HEIGHT;

			//기본 키 설정 서피스
			gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mTexture;
			gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mHeight;
			gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mWidth;

			//메인루프 
			while (!quit)
			{
				//이벤트 처리
				while (SDL_PollEvent(&e) != 0)
				{
					//종료 호출시 종료
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					
					V_handleEvent(&gPlayer, &e); //키다운에 따른 이동 이벤트
					T_handleEvent(&gCurrentSurface, &gMainplayerTexture, &e);  
				}


				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); //랜더러 색을 검은색으로 설정
				SDL_RenderClear(gRenderer);
				//흰색화면 랜더러 설정 후 랜더러 클리어

				move(&gPlayer,&tileSet);
				setCamera(&gPlayer, &Camera);
				
				for (int i = 0;i < TOTAL_TILES;++i)
				{
					if (checkCollision(Camera, (tileSet+i)->mBox))
					{
						render(&gTileTexture[(tileSet + i)->mType], gRenderer, (tileSet + i)->mBox.x - Camera.x, (tileSet + i)->mBox.y - Camera.y);
					}
				}
				
				//!!!랜더링 순서 중요함!!! 
				//render(&gTileTexture, gRenderer, 0, 0);
				render(&gCurrentSurface, gRenderer, gPlayer.mBox.x - Camera.x, gPlayer.mBox.y - Camera.y); //플레이어 무브

				SDL_RenderPresent(gRenderer);  //Update

			}
		}

	}

	close();

	return 0;
}
bool init()
{
	bool Sflag = true; //success flag

	//SDL 초기화
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL을 초기화 할 수 없습니다. : %s", SDL_GetError());
		Sflag = false;
	}
	else
	{
		//윈도우 생성
		gWindow = SDL_CreateWindow("FPS Limit & Scrolling", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("윈도우를 생성할 수 없습니다. : %s", SDL_GetError());
			Sflag = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED); //랜더러 생성
			if (gRenderer == NULL)
			{
				printf("랜더러 생성 실패! : %s", SDL_GetError());
				Sflag = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); //랜더러를 흰색화면으로 그리도록 세팅


				int imgFlags = IMG_INIT_PNG; //PNG 로딩 플래그
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL이미지 모듈을 초기화 할 수 없습니다! : %s", IMG_GetError());
					Sflag = false;
				}
				else
				{
					gScreenSurface = SDL_GetWindowSurface(gWindow);
				}


			}
		}
	}
	//플레이어&타이머 변수 초기화

	gPlayer.Player_HEIGHT = 40;
	gPlayer.Player_WIDTH = 40;
	gPlayer.Player_VEL = 1;

	//기본 플레이어 위치 설정
	gPlayer.mBox.x = 0;
	gPlayer.mBox.y = 0;

	gPlayer.mVelX = 0;
	gPlayer.mVelY = 0;
	gPlayer.mBox.w = gPlayer.Player_WIDTH;
	gPlayer.mBox.h = gPlayer.Player_HEIGHT;

	return Sflag;
}
SDL_Texture* loadTexture(char* path)
{
	SDL_Texture* newTexture = NULL;

	//SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("이미지 로드 실패! : %s\n", IMG_GetError()); //IMG모듈 사용시 SDL_GetError 대신 IMG_GetError사용
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("텍스쳐 생성 실패! : %s\n", SDL_GetError());
		}

		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;

}
bool loadMedia(_LTile *tiles[])
{
	bool Sflag = true; //success flag

	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT], gRenderer, "duck1.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_UP], gRenderer, "up.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_UP].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(up) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_DOWN], gRenderer, "duck1.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_DOWN].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(down) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_LEFT], gRenderer, "left.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_LEFT].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(left) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_RIGHT], gRenderer, "right.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_RIGHT].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(right) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_O], gRenderer, "insaneduck.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_O].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(insane) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture, gRenderer, "duck.png");
	if (gDuckTexture.mTexture == NULL)
	{
		printf("오리 로드 실패!\n");
		Sflag = false;
	}

	loadFromFile(&gTileTexture[0], gRenderer, "tile00.png");
	loadFromFile(&gTileTexture[1], gRenderer, "tile01.png");
	loadFromFile(&gTileTexture[2], gRenderer, "tile02.png");
	loadFromFile(&gTileTexture[3], gRenderer, "tile03.png");
	loadFromFile(&gTileTexture[4], gRenderer, "tile04.png");
	loadFromFile(&gTileTexture[5], gRenderer, "tile05.png");
	loadFromFile(&gTileTexture[6], gRenderer, "tile06.png");
	loadFromFile(&gTileTexture[7], gRenderer, "tile07.png");
	loadFromFile(&gTileTexture[8], gRenderer, "tile08.png");
	loadFromFile(&gTileTexture[9], gRenderer, "tile09.png");
	loadFromFile(&gTileTexture[10], gRenderer, "tile10.png");
	loadFromFile(&gTileTexture[11], gRenderer, "tile11.png");
	for (int i = 0;i < 12;i++) {
		if (gTileTexture[i].mTexture == NULL)
		{
			printf("타일 텍스쳐(%d) 로드 실패! \n", i);
			Sflag = false;
			break;
		}
	}
	if (!setTiles(tiles))
	{
		printf("타일셋 설정 실패!\n");
		Sflag = false;
	}

	//위와 같이 원하는 이미지를 포인터에 박은후 랜더러에 각각 매치시켜서 업데이트 
	return Sflag;
}
void close()
{
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
	for(enum KeyPressSurfaces i =KEY_PRESS_SURFACE_DEFAULT;i<KEY_PRESS_SURFACE_TOTAL;i++)
		lfree(&gMainplayerTexture[i]);
	for (int i = 0;i < 12;i++)
		lfree(&gTileTexture[i]);
	//서피스 초기화
	lfree(&gCurrentSurface);
	
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//sdl 라이브러리 닫기
	SDL_Quit();
	IMG_Quit();

}
bool loadFromFile(_LTexture* LT, SDL_Renderer* Renderer, char* path)
{
	lfree(LT);

	SDL_Texture* newTexture = NULL;  //텍스쳐 포인터

	SDL_Surface* loadedSurface = IMG_Load(path);

	if (loadedSurface == NULL)
	{
		printf("이미지 로드 실패! : %s", IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
		newTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("텍스쳐 생성 실패! : %s", SDL_GetError());
		}
		else
		{
			LT->mWidth = loadedSurface->w;
			LT->mHeight = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface);
	}

	LT->mTexture = newTexture;
	return LT->mTexture != NULL;
}
void lfree(_LTexture* LT) //텍스쳐 구조체에 접근해서 널로 초기화
{
	if (LT != NULL)
	{
		if (LT->mTexture != NULL)
		{
			SDL_DestroyTexture(LT->mTexture);
			LT->mTexture = NULL;
			LT->mWidth = 0;
			LT->mHeight = 0;
		}
	}
}
void render(_LTexture* LT, SDL_Renderer* Renderer, int x, int y) //랜더링
{
	SDL_Rect renderQuad = { x,y,LT->mWidth,LT->mHeight };
	SDL_RenderCopy(Renderer, LT->mTexture, NULL, &renderQuad);
}
void V_handleEvent(_LPlayer* LP, SDL_Event* e) //키입력에 따른 좌표조절
{
	if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY += LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX -= LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX += LP->Player_VEL; break;
		}
	}
	else if (e->type == SDL_KEYUP && e->key.repeat == 0)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY += LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX += LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX -= LP->Player_VEL; break;
		}
	}
}

void move(_LPlayer* LP,_LTile* tiles) //플레이어 무브 & 충돌처리
{
	LP->mBox.x += LP->mVelX;
	

	if ((LP->mBox.x < 0) || (LP->mBox.x + LP->Player_WIDTH > LEVEL_WIDTH) || touchesWall(LP->mBox,tiles))
	{
		LP->mBox.x -= LP->mVelX;
	}

	LP->mBox.y += LP->mVelY;

	if ((LP->mBox.y < 0) || (LP->mBox.y + LP->Player_HEIGHT> LEVEL_HEIGHT) || touchesWall(LP->mBox,tiles))
	{
		LP->mBox.y -= LP->mVelY;
	}
}
void setCamera(_LPlayer* LP, SDL_Rect* camera)
{
	camera->x = (getPosX(LP) + LP->Player_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera->y = (getPosY(LP) + LP->Player_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	if (camera->x < 0)
	{
		camera->x = 0;
	}
	if (camera->y < 0)
	{
		camera->y = 0;
	}
	if (camera->x > LEVEL_WIDTH - camera->w)
	{
		camera->x = LEVEL_WIDTH - camera->w;
	}
	if (camera->y > LEVEL_HEIGHT - camera->h)
	{
		camera->y = LEVEL_HEIGHT - camera->h;
	}
}
void T_handleEvent(_LTexture* CT, _LTexture* LT, SDL_Event* e) 
{
	if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
			CT->mTexture = (LT+KEY_PRESS_SURFACE_UP)->mTexture;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_UP)->mWidth;
			CT->mHeight = (LT + KEY_PRESS_SURFACE_UP)->mHeight;
			break;
		case SDLK_DOWN:
			CT->mHeight = (LT+KEY_PRESS_SURFACE_DOWN)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_DOWN)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_DOWN)->mTexture;
			break;
		case SDLK_LEFT:
			CT->mHeight = (LT+KEY_PRESS_SURFACE_LEFT)->mHeight;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_LEFT)->mTexture;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_LEFT)->mWidth;
			break;
		case SDLK_RIGHT:
			CT->mHeight = (LT+KEY_PRESS_SURFACE_RIGHT)->mHeight;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_RIGHT)->mTexture;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_RIGHT)->mWidth;
			break;
		case SDLK_o:
			CT->mHeight = (LT+KEY_PRESS_SURFACE_O)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_O)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_O)->mTexture;
			break;
		default:
			CT->mHeight = (LT+KEY_PRESS_SURFACE_DEFAULT)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_DEFAULT)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_DEFAULT)->mTexture;
			break;
		}
	}
}
bool checkCollision(SDL_Rect a, SDL_Rect b) //충돌판정 함수(충돌-> return false)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int botA, botB;
	//충돌판정 사각형 변수

	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	botA = a.y + a.h;

	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	botB = b.y + b.h;

	if (botA <= topB)
	{
		return false;
	}
	if (topA >= botB)
	{
		return false;
	}
	if (rightA <= leftB)
	{
		return false;
	}
	if (leftA >= rightB)
	{
		return false;
	}

	return true;
}
int getPosX(_LPlayer* LP)
{
	return LP->mBox.x;
}
int getPosY(_LPlayer* LP)
{
	return LP->mBox.y;
}

bool setTiles(_LTile *tiles)
{
	printf("setTiles");
	bool loaded = true;
	int x = 0, y = 0, i = 0;

	char tempbuf[2304];
	char* map[2304] = { 0, };
	FILE *fp = fopen("level.map", "r");

	fgets(tempbuf, sizeof(tempbuf), fp);

	char *ptr = strtok(tempbuf, " ");

	while (ptr != NULL)
	{
		map[i] = ptr;
		i++;
		ptr = strtok(NULL, " ");
	}
	
	if (fp == NULL)
	{
		printf("맵파일 로드 실패!\n");
		loaded = false;
	}
	else
	{
		printf("else");
		i = 0;
		for(int i =0;i<763;i++)
		{

			int tileType = -1;
			if (!strcmp(map[i], "00"))
			{
				tileType = 0;
				printf("\n set tileType : 00");
			}
			else if (!strcmp(map[i], "01"))
			{
				tileType = 1;
				printf("\n set tileType : 01");
			}
			else if (!strcmp(map[i], "02"))
			{
				tileType = 2;
				printf("\n set tileType : 02");
			}
			else if (!strcmp(map[i], "03"))
			{
				tileType = 3;
				printf("\n set tileType : 03");
			}
			else if (!strcmp(map[i], "04"))
			{
				tileType = 4;
				printf("\n set tileType : 04");
			}
			else if (!strcmp(map[i], "05"))
			{
				tileType = 5;
				printf("\n set tileType : 05");
			}
			else if (!strcmp(map[i], "06"))
			{
				tileType = 6;
				printf("\n set tileType : 06");
			}
			else if (!strcmp(map[i], "07"))
			{
				tileType = 7;
				printf("\n set tileType : 07");
			}
			else if (!strcmp(map[i], "08"))
			{
				tileType = 8;
				printf("\n set tileType : 08");
			}
			else if (!strcmp(map[i], "09"))
			{
				tileType = 9;
				printf("\n set tileType : 09");
			}
			else if (!strcmp(map[i], "10"))
			{
				tileType = 10;
				printf("\n set tileType : 10");
			}
			else if (!strcmp(map[i], "11"))
			{
				tileType = 11;
				printf("\n set tileType : 11");
			}

			printf("\nready to set tiles");
			(tiles + i)->mBox.x = x;
			(tiles + i)->mBox.y = y;
			(tiles + i)->mBox.w = TILE_WIDTH;
			(tiles + i)->mBox.h = TILE_HEIGHT;
			(tiles + i)->mType = tileType;

			printf("tiles[%d] = %d\n", i, (tiles+i)->mType);
			x += TILE_WIDTH;

			if (x >= LEVEL_WIDTH)
			{
				x = 0;
				y += TILE_HEIGHT;
			}
			
		}
	}
	printf("\n Finish setting tiles successfully");
	fclose(fp);
	return loaded;
}

bool touchesWall(SDL_Rect box, _LTile* tiles)
{
	for (int i = 0;i < TOTAL_TILES; i++)
	{
		if (((tiles+i)->mType>=3) && ((tiles+i)->mType<=11))
		{
			if (checkCollision(box, (tiles+i)->mBox))
			{
				return true;
			}
		}
	}
	return false;
}

