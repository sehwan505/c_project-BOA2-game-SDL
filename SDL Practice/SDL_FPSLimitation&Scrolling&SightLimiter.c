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
#include <SDL_ttf.h>              //SDL ttf lib
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include <string.h>
#include "han2unicode.h"

const int LEVEL_WIDTH = 2560;  //레벨층 가로
const int LEVEL_HEIGHT = 1480; //레벨층 세로

const int SCREEN_WIDTH = 1280; //스크린 가로
const int SCREEN_HEIGHT = 720; //스크린 세로

const int SCREEN_FPS = 60;     //스크린 프레임
const int SCREEN_TICK_PER_FRAME = 17; //틱당 프레임

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

	int mPosX, mPosY;
	int mVelX, mVelY;
	SDL_Rect mCollider; //충돌판정 사각형
}_LPlayer;

typedef struct _LTimer //타이머 구조체
{
	Uint32 mStartTicks;
	Uint32 mPauseTicks;
	bool mPaused;
	bool mStarted;
}_LTimer;

//텍스쳐 등 구현할 구조체들 정의
struct _LTexture gBackgroundTexture;
struct _LTexture gMainplayerTexture[KEY_PRESS_SURFACE_TOTAL];
struct _LTexture gDuckTexture;
struct _LTexture gSightLimiter;   //시야 가리기
struct _LTexture gTextTexture[2]; //택스트도 구조체 배열을 통하여 미리 집어넣어놓고, 이벤트에 따라서 꺼내어 랜더링 할 수 있다.
struct _LTexture gCurrentSurface; //현재 표시되는 서피스
struct _LTexture gCurrentText;    //현재 표시되는 텍스트
struct _LPlayer gPlayer;
struct _LTimer timer;

//함수 정의
bool init(); //초기화
bool loadMedia(); //미디어 로딩
void close(); // SDL 라이브러리 닫기
bool loadFromFile(struct _LTexture* LT, SDL_Renderer* Renderer, char* path);  //텍스쳐 불러오기
void lfree(struct _LTexture* LT); //닫기
void render(_LTexture* LT, SDL_Renderer* Renderer, int x, int y); //지정된 좌표에 랜더링
bool loadFromRenderedText(_LTexture* LT, SDL_Renderer* Renderer, TTF_Font* Font, char* stringText, SDL_Color textColor); //텍스트 불러오기
void V_handleEvent(_LPlayer* LP, SDL_Event*e);  //이벤트에 따른 속도조절
void T_handleEvent(_LTexture* CT,_LTexture* LT, SDL_Event* e); //이벤트에 따른 텍스쳐 조절

//character
void move(_LPlayer* LP, SDL_Rect* wall);
bool checkCollision(SDL_Rect a, SDL_Rect b); //충돌판정
int getPosX(_LPlayer* LP);
int getPosY(_LPlayer* LP);


//Timer
void timer_start(_LTimer* timer);
void timer_stop(_LTimer* timer);
void timer_pause(_LTimer* timer);
void timer_unpause(_LTimer* timer);
Uint32 getTicks(_LTimer* timer);
bool timer_isStarted(_LTimer* timer);
bool timer_isPaused(_LTimer* timer);

//스크린을 구성하는 포인터 정의
SDL_Surface* loadSurface(char* path); //경로에 있는 서피스 로드 함수
SDL_Texture* loadTexture(char* path); //위와 같은 기능 ( 서피스보다 성능 좋음)
SDL_Window* gWindow = NULL; //랜더링 윈도우 포인터
SDL_Surface* gScreenSurface = NULL; //스크린서피스 포인터
SDL_Renderer* gRenderer = NULL;     //랜더러 포인터
SDL_Texture* gTexture = NULL;       //기본 텍스쳐 포인터
SDL_Texture* gLetterbox = NULL;     //왼쪽아래 메시지박스 포인터
SDL_Texture* gMinimap = NULL;       //오른쪽 아래 미니맵 포인터
SDL_Texture* gTimeText = NULL;
TTF_Font* gFont = NULL;  //기본 폰트 포인터


int main()
{
	//SDL 모듈 초기화
	if (!init())
	{
		printf("초기화 실패!\n");
	}
	else
	{
		//미디어 로드
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//메인루프 플래그
			bool quit = false;

			//Event handler
			SDL_Event e;

			//벽 구조체 정의
			SDL_Rect wall; 
			wall.x = 300;
			wall.y = 40;
			wall.w = 40;
			wall.h = 400;

			SDL_Rect Camera;
			Camera.x = 0;
			Camera.y = 0;
			Camera.w = SCREEN_WIDTH;
			Camera.h = SCREEN_HEIGHT;

			//기본 키 설정 서피스
			gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mTexture;
			gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mHeight;
			gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mWidth;

			//기본 텍스트 설정
			gCurrentText.mTexture = gTextTexture[0].mTexture;
			gCurrentText.mHeight = gTextTexture[0].mHeight;
			gCurrentText.mWidth = gTextTexture[0].mWidth;

			

			struct _LTimer fpsTimer;
			struct _LTimer capTimer;
			int  countedFrames = 0;
			timer_start(&fpsTimer);
			//메인루프 
			while (!quit)
			{
				timer_start(&capTimer);
				//이벤트 처리
				while (SDL_PollEvent(&e) != 0)
				{
					//종료 호출시 종료
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type == SDL_KEYDOWN) //키다운 이벤트(추가: 함수로 처리할것)
					{
						switch (e.key.keysym.sym)
						{
							/*
						case SDLK_UP:
							gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_UP].mTexture;
							gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_UP].mWidth;
							gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_UP].mHeight;
							break;
						case SDLK_DOWN:
							gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_DOWN].mHeight;
							gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_DOWN].mWidth;
							gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_DOWN].mTexture;
							break;
						case SDLK_LEFT:
							gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_LEFT].mHeight;
							gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_LEFT].mTexture;
							gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_LEFT].mWidth;
							break;
						case SDLK_RIGHT:
							gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_RIGHT].mHeight;
							gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_RIGHT].mTexture;
							gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_RIGHT].mWidth;
							break;
						case SDLK_o:
							gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_O].mHeight;
							gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_O].mWidth;
							gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_O].mTexture;
							gPlayer.Player_VEL = 20;
							gCurrentText.mTexture = gTextTexture[1].mTexture;
							gCurrentText.mHeight = gTextTexture[1].mHeight;
							gCurrentText.mWidth = gTextTexture[1].mWidth;
							break;
							*/
						case SDLK_o:
							gPlayer.Player_VEL = 20;
							gCurrentText.mTexture = gTextTexture[1].mTexture;
							gCurrentText.mHeight = gTextTexture[1].mHeight;
							gCurrentText.mWidth = gTextTexture[1].mWidth;
							break;
						case SDLK_s: //타이머 멈추기
							if (timer_isStarted(&timer))
							{
								timer_stop(&timer);
							}
							else
							{
								timer_stop(&timer);
							}
							break;
						case SDLK_p: //타이머 일시정지
							if (timer_isPaused(&timer))
							{
								timer_unpause(&timer);
							}
							else
							{
								timer_pause(&timer);
							}
							break;
						default:
							gCurrentText.mTexture = gTextTexture[0].mTexture;
							gCurrentText.mHeight = gTextTexture[0].mHeight;
							gCurrentText.mWidth = gTextTexture[0].mWidth;
							gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mHeight;
							gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mWidth;
							gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mTexture;
						}
					}
						
					V_handleEvent(&gPlayer, &e); //키다운에 따른 이동 이벤트
					T_handleEvent(&gCurrentSurface, &gMainplayerTexture, &e);  //왜 에러나지
				}

				float avgFPS = countedFrames / (getTicks(&fpsTimer) / (float)1000);
				if (avgFPS > 2000000)
				{
					avgFPS = 0;
				}

				//이미지 적용
				//SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);

				//서피스 업데이트
				//SDL_UpdateWindowSurface(gWindow);

				/*
				//시간 표시를 위한 변수 설정 (추가: 실시간으로 1씩증가하게 만들어야함)
				SDL_Color timeColor = { 255,255,255 };
				if (!loadFromRenderedText(&gTimeText, gRenderer, "korean.ttf", "Time :", timeColor))
				{
					printf("타이머를 랜더할 수 없습니다! \n");
				}
				*/

				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); //랜더러 색을 검은색으로 설정
				SDL_RenderClear(gRenderer);
				//흰색화면 랜더러 설정 후 랜더러 클리어
				move(&gPlayer,&wall);

				//카메라 랜더 설정
				Camera.x = (getPosX(&gPlayer) + gPlayer.Player_WIDTH / 2) - SCREEN_WIDTH / 2;
				Camera.y = (getPosY(&gPlayer) + gPlayer.Player_HEIGHT / 2) - SCREEN_HEIGHT / 2;
				
				if (Camera.x < 0)
				{
					Camera.x = 0;
				}
				if (Camera.y < 0)
				{
					Camera.y = 0;
				}
				if (Camera.x > LEVEL_WIDTH - Camera.w)
				{
					Camera.x = LEVEL_WIDTH - Camera.w;
				}
				if (Camera.y > LEVEL_HEIGHT - Camera.h)
				{
					Camera.y = LEVEL_HEIGHT - Camera.h;
				}

				SDL_Rect botLeftViewport;
				botLeftViewport.x = 0;
				botLeftViewport.y = SCREEN_HEIGHT * 2 / 3;
				botLeftViewport.w = SCREEN_WIDTH * 2 / 3;
				botLeftViewport.h = SCREEN_HEIGHT / 3;
				SDL_RenderSetViewport(gRenderer, &botLeftViewport);

				SDL_RenderCopy(gRenderer, gLetterbox, NULL, NULL);
				render(&gCurrentText, gRenderer, 30, 30);



				SDL_Rect botRightViewport;
				botRightViewport.x = SCREEN_WIDTH * 2 / 3;
				botRightViewport.y = SCREEN_HEIGHT * 2 / 3;
				botRightViewport.w = SCREEN_WIDTH / 3;
				botRightViewport.h = SCREEN_HEIGHT / 3;
				SDL_RenderSetViewport(gRenderer, &botRightViewport);

				SDL_RenderCopy(gRenderer, gMinimap, NULL, NULL);



				SDL_Rect topViewport;
				topViewport.x = 0;
				topViewport.y = 0;
				topViewport.w = SCREEN_WIDTH;
				topViewport.h = SCREEN_HEIGHT * 2 / 3;
				SDL_RenderSetViewport(gRenderer, &topViewport); //위에서 설정한 뷰포트로 랜더러 설정

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				
				
				//SDL_RenderCopy(gRenderer, gBackgroundTexture->mTexture, NULL, NULL); //텍스쳐 -> 랜더러 로 카피
				render(&gBackgroundTexture, gRenderer, -Camera.x, -Camera.y);//배경넣기
				//SDL_RenderFillRect(gRenderer, &wall);  //벽만들기

				
				//!!!랜더링 순서 중요함!!! 
				render(&gSightLimiter, gRenderer, gPlayer.mPosX - 1310 - Camera.x, gPlayer.mPosY - 730 - Camera.y);  //사이트 리미터(플레이어랑 같이 움직임) 플레이어 기본위치를 빼주어야 정확히 가운데에 위치
				render(&gCurrentSurface, gRenderer, gPlayer.mPosX - Camera.x, gPlayer.mPosY - Camera.y); //플레이어 무브

				//render(&gTimeText, gRenderer, 1, 1); //타이머
				SDL_RenderPresent(gRenderer);  //Update
				++countedFrames;

				//free(timeText);

				int frameTicks = getTicks(&capTimer);
				if (frameTicks < SCREEN_TICK_PER_FRAME)
				{
					SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
				}

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
				else if (TTF_Init() == -1)
				{
					printf("SDL_TTF 모듈을 초기화 할 수 없습니다! : %s", TTF_GetError());
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
	gPlayer.Player_VEL = 10;
	
	//기본 플레이어 위치 설정
	gPlayer.mPosX = 640;
	gPlayer.mPosY = 240;

	gPlayer.mVelX = 0;
	gPlayer.mVelY = 0;
	gPlayer.mCollider.w = gPlayer.Player_WIDTH;
	gPlayer.mCollider.h = gPlayer.Player_HEIGHT;

	timer.mStartTicks = 0;
	timer.mPauseTicks = 0;
	timer.mPaused = false;
	timer.mStarted = false;

	return Sflag;

}
SDL_Surface* loadSurface(char* path)
{
	//경로의 이미지 로드
	SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	if (loadedSurface == NULL) {
		printf("이미지 로드 실패! : %s\n", SDL_GetError());
	}
	return loadedSurface;
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
bool loadMedia()
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

	loadFromFile(&gSightLimiter, gRenderer, "SightLimiter2.png");
	if (gSightLimiter.mTexture == NULL)
	{
		printf("사이트리미터 로드 실패!\n");
		Sflag = false;
	}

	loadFromFile(&gBackgroundTexture, gRenderer, "BG.png");
	if (gBackgroundTexture.mTexture == NULL)
	{
		printf("백그라운드 텍스쳐 로드 실패!\n");
		Sflag = false;
	}

	gLetterbox = loadTexture("Lbox.png");
	if (gLetterbox == NULL)
	{
		printf("텍스쳐이미지(gLetterbox) 로드 실패!\n");
		Sflag = false;
	}

	gMinimap = loadTexture("minimap.png");
	if (gMinimap == NULL)
	{
		printf("텍스쳐이미지(gMinimap) 로드 실패!\n");
		Sflag = false;
	}

	//위랑 같이 폰트 불러옴
	gFont = TTF_OpenFont("korean.ttf", 28);
	if (gFont == NULL)
	{
		printf("폰트 로드 실패! : %s", TTF_GetError());
		Sflag = false;
	}
	else
	{
		SDL_Color textColor = { 255,255,255 };
		if (!loadFromRenderedText(&gTextTexture[0], gRenderer, gFont, "오리는 꽥꽥", textColor))
		{
			printf("텍스쳐 랜더 실패!");
			Sflag = false;
		}
		textColor.r=255 ;
		if (!loadFromRenderedText(&gTextTexture[1], gRenderer, gFont, "오리가 개빡쳐서 속력이 증가합니다!", textColor))
		{
			printf("텍스쳐 랜더 실패!");
			Sflag = false;
		}
		
	}

	//위와 같이 원하는 이미지를 포인터에 박은후 랜더러에 각각 매치시켜서 업데이트 
	return Sflag;
}
void close()
{
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	SDL_DestroyTexture(gMinimap);
	gMinimap = NULL;

	SDL_DestroyTexture(gLetterbox);
	gLetterbox = NULL;

	lfree(&gBackgroundTexture);
	for(enum KeyPressSurfaces i =KEY_PRESS_SURFACE_DEFAULT;i<KEY_PRESS_SURFACE_TOTAL;i++)
		lfree(&gMainplayerTexture[i]);
	lfree(&gFont);
	//서피스 초기화
	lfree(&gCurrentSurface);
	

	TTF_CloseFont(gFont);
	gFont = NULL;
	//윈도우 닫기
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//sdl 라이브러리 닫기
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
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
bool loadFromRenderedText(_LTexture* LT, SDL_Renderer* Renderer, TTF_Font* Font, char* stringText, SDL_Color textColor)
{
	lfree(LT);
	unsigned short unicode[128]; //유니코드 배열 만들어서
	han2unicode(stringText, unicode); //스트링을 유니코드로 변환
	SDL_Surface* textSurface = TTF_RenderUNICODE_Solid(Font, unicode, textColor); //서피스 포인터에 폰트 랜더함수넣기
	if (textSurface == NULL)
	{
		printf("텍스쳐 서피스 랜더 실패! : %s\n", TTF_GetError());
	}
	else
	{
		LT->mTexture = SDL_CreateTextureFromSurface(Renderer, textSurface);
		if (LT->mTexture == NULL)
		{
			printf("텍스쳐 생성 실패 : %s", SDL_GetError());
		}
		else
		{
			LT->mWidth = textSurface->w;
			LT->mHeight = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	}
	return LT->mTexture != NULL;
}//텍스쳐 로딩
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

void move(_LPlayer* LP,SDL_Rect* wall) //플레이어 무브 & 충돌처리
{
	LP->mPosX += LP->mVelX;
	LP->mCollider.x = LP->mPosX;

	if ((LP->mPosX < 0) || (LP->mPosX + LP->Player_WIDTH > LEVEL_WIDTH) || checkCollision(LP->mCollider,*wall))
	{
		LP->mPosX -= LP->mVelX;
		LP->mCollider.x = LP->mPosX;
	}

	LP->mPosY += LP->mVelY;
	LP->mCollider.y = LP->mPosY;

	if ((LP->mPosY < 0) || (LP->mPosY + LP->Player_HEIGHT> LEVEL_HEIGHT) || checkCollision(LP->mCollider,*wall))
	{
		LP->mPosY -= LP->mVelY;
		LP->mCollider.y = LP->mPosY;
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
	return LP->mPosX;
}
int getPosY(_LPlayer* LP)
{
	return LP->mPosY;
}


void timer_start(_LTimer* timer)
{
	timer->mStarted = true;
	timer->mPaused = false;
	timer->mStartTicks = SDL_GetTicks();
	timer->mPauseTicks = 0;
}
void timer_stop(_LTimer* timer)
{
	timer->mStarted = false;
	timer->mPaused = false;
	timer->mStartTicks = 0;
	timer->mPauseTicks = 0;
}
void timer_pause(_LTimer* timer)
{
	if (timer->mStarted && !(timer->mPaused))
	{
		timer->mPaused = true;
		timer->mPauseTicks = SDL_GetTicks()-(timer->mStartTicks);
		timer->mStartTicks = 0;
	}
}
void timer_unpause(_LTimer* timer)
{
	if (timer->mStarted && timer->mPaused)
	{
		timer->mPaused = false;
		timer->mStartTicks = SDL_GetTicks() - (timer->mPauseTicks);
		timer->mPauseTicks = 0;
		
	}
}
Uint32 getTicks(_LTimer* timer)
{
	Uint32 time = 0;

	if (timer->mStarted)
	{
		if (timer->mPaused)
		{
			time = timer->mPauseTicks;
		}
		else
		{
			time = SDL_GetTicks() - timer->mStartTicks;
		}
	}
	return time;
}
bool timer_isStarted(_LTimer* timer)
{
	return timer->mStarted;
}
bool timer_isPaused(_LTimer* timer)
{
	return timer->mPaused && timer->mStarted;
}