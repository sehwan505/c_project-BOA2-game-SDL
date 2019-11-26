#include <SDL.h>                  //SDL lib
#include <SDL_image.h>            //SDL_image lib
#include <SDL_ttf.h>              //SDL ttf lib
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include "game.h"
#include "han2unicode.h"
#include "timer.h"


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
		gWindow = SDL_CreateWindow("BoA2 Production", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

	gPlayer.Player_HEIGHT = 78;
	gPlayer.Player_WIDTH = 60;
	gPlayer.Player_VEL = 3;

	//기본 플레이어 위치 설정
	gPlayer.mBox.x = 0;
	gPlayer.mBox.y = 0;
	gPlayer.mBox.w = gPlayer.Player_WIDTH;
	gPlayer.mBox.h = gPlayer.Player_HEIGHT;
	gPlayer.mVelX = 0;
	gPlayer.mVelY = 0;

	//오리 변수 초기화
	for (int i = 0; i < 5; i++)
	{
		gDuck[i].Player_HEIGHT = 75;
		gDuck[i].Player_WIDTH = 75;
		gDuck[i].Player_VEL = 2;
		gDuck[i].mBox.w = gDuck[i].Player_WIDTH;
		gDuck[i].mBox.h = gDuck[i].Player_HEIGHT;
		gDuck[i].mVelX = 0;
		gDuck[i].mVelY = 0;
		gDuck[i].mBox.x = 0;
		gDuck[i].mBox.y = 0;
	}

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
bool loadMedia(_LTile *tiles[])
{
	bool Sflag = true; //success flag

	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT], gRenderer, "images/player_front.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_UP], gRenderer, "images/player_back.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_UP].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(up) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_DOWN], gRenderer, "images/player_front.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_DOWN].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(down) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_LEFT], gRenderer, "images/player_left.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_LEFT].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(left) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_RIGHT], gRenderer, "images/player_right.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_RIGHT].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(right) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[KEY_PRESS_SURFACE_O], gRenderer, "images/duck_insane.png");
	if (gMainplayerTexture[KEY_PRESS_SURFACE_O].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(insane) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[6], gRenderer, "images/player_left02.png");
	if (gMainplayerTexture[6].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(left02) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gMainplayerTexture[7], gRenderer, "images/player_right02.png");
	if (gMainplayerTexture[7].mTexture == NULL)
	{
		printf("메인플레이어 디폴트 텍스쳐(right02) 로드 실패!\n");
		Sflag = false;
	}


	loadFromFile(&gDuckTexture[0], gRenderer, "images/duck_front.png");
	if (gDuckTexture[0].mTexture == NULL)
	{
		printf("오리(디폴트) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture[1], gRenderer, "images/duck_back.png");
	if (gDuckTexture[1].mTexture == NULL)
	{
		printf("오리(뒤) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture[2], gRenderer, "images/duck_front.png");
	if (gDuckTexture[2].mTexture == NULL)
	{
		printf("오리(앞) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture[3], gRenderer, "images/duck_left.png");
	if (gDuckTexture[3].mTexture == NULL)
	{
		printf("오리(left) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture[4], gRenderer, "images/duck_right.png");
	if (gDuckTexture[4].mTexture == NULL)
	{
		printf("오리(right) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture[5], gRenderer, "images/duck_insane.png");
	if (gDuckTexture[5].mTexture == NULL)
	{
		printf("오리(insane) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture[6], gRenderer, "images/duck_left_02.png");
	if (gDuckTexture[6].mTexture == NULL)
	{
		printf("오리(left02) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture[7], gRenderer, "images/duck_right_02.png");
	if (gDuckTexture[7].mTexture == NULL)
	{
		printf("오리(right02) 로드 실패!\n");
		Sflag = false;
	}
	loadFromFile(&gSightLimiter, gRenderer, "images/SightLimiter.png");
	if (gSightLimiter.mTexture == NULL)
	{
		printf("사이트리미터 로드 실패!\n");
		Sflag = false;
	}
	gLetterbox = loadTexture("images/Lbox.png");
	if (gLetterbox == NULL)
	{
		printf("텍스쳐이미지(gLetterbox) 로드 실패!\n");
		Sflag = false;
	}

	gMinimap = loadTexture("images/minimap.png");
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

		if (!loadFromRenderedText(&gTextTexture[0], gRenderer, gFont, "오리를 피해 살아남으세요!", textColor))
		{
			printf("텍스쳐 랜더 실패!");
			Sflag = false;
		}
		if (!loadFromRenderedText(&gTimeText, gRenderer, gFont, "Time :", textColor))
		{
			printf("타이머를 랜더할 수 없습니다! \n");
		}
		if (!loadFromRenderedText(&gLeaderBoard[0], gRenderer, gFont, "1위 : ", textColor))
		{
			printf("리더보드 랜더 실패!");
			Sflag = false;
		}
		if (!loadFromRenderedText(&gLeaderBoard[1], gRenderer, gFont, "2위 : ", textColor))
		{
			printf("리더보드 랜더 실패!");
			Sflag = false;
		}
		if (!loadFromRenderedText(&gLeaderBoard[2], gRenderer, gFont, "3위 : ", textColor))
		{
			printf("리더보드 랜더 실패!");
			Sflag = false;
		}
		if (!loadFromRenderedText(&gLeaderBoard[3], gRenderer, gFont, "4위 : ", textColor))
		{
			printf("리더보드 랜더 실패!");
			Sflag = false;
		}
		if (!loadFromRenderedText(&gLeaderBoard[4], gRenderer, gFont, "5위 : ", textColor))
		{
			printf("리더보드 랜더 실패!");
			Sflag = false;
		}

		if (!loadFromRenderedText(&gStartText, gRenderer, gFont, "Enter키를 눌러 게임을 시작하세요", textColor))
		{
			printf("스타트텍스트 랜더 실패!");
			Sflag = false;
		}
		

		textColor.g = 0;
		textColor.b = 0;
		if (!loadFromRenderedText(&gTextTexture[1], gRenderer, gFont, "시간이 얼마 남지 않아 오리의 이동속도가 증가합니다!", textColor))
		{
			printf("텍스쳐 랜더 실패!");
			Sflag = false;
		}
		

	}
	//타일 텍스쳐 오픈
	loadFromFile(&gTileTexture[0], gRenderer, "images/tiles/00.png");
	loadFromFile(&gTileTexture[1], gRenderer, "images/tiles/01.png");
	loadFromFile(&gTileTexture[2], gRenderer, "images/tiles/02.png");
	loadFromFile(&gTileTexture[3], gRenderer, "images/tiles/03.png");
	loadFromFile(&gTileTexture[4], gRenderer, "images/tiles/04.png");
	loadFromFile(&gTileTexture[5], gRenderer, "images/tiles/05.png");
	loadFromFile(&gTileTexture[6], gRenderer, "images/tiles/06.png");
	loadFromFile(&gTileTexture[7], gRenderer, "images/tiles/07.png");
	loadFromFile(&gTileTexture[8], gRenderer, "images/tiles/08.png");
	loadFromFile(&gTileTexture[9], gRenderer, "images/tiles/09.png");
	loadFromFile(&gTileTexture[10], gRenderer, "images/tiles/10.png");
	loadFromFile(&gTileTexture[11], gRenderer, "images/tiles/11.png");
	loadFromFile(&gTileTexture[12], gRenderer, "images/tiles/12.png");
	loadFromFile(&gTileTexture[13], gRenderer, "images/tiles/13.png");
	loadFromFile(&gTileTexture[14], gRenderer, "images/tiles/14.png");
	loadFromFile(&gTileTexture[15], gRenderer, "images/tiles/15.png");
	loadFromFile(&gTileTexture[16], gRenderer, "images/tiles/16.png");
	loadFromFile(&gTileTexture[17], gRenderer, "images/tiles/17.png");
	loadFromFile(&gTileTexture[18], gRenderer, "images/tiles/18.png");
	loadFromFile(&gTileTexture[19], gRenderer, "images/tiles/19.png");
	loadFromFile(&gTileTexture[20], gRenderer, "images/tiles/20.png");
	loadFromFile(&gTileTexture[21], gRenderer, "images/tiles/21.png");
	loadFromFile(&gTileTexture[22], gRenderer, "images/tiles/22.png");
	for (int i = 0; i < 23; i++) {
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

	SDL_DestroyTexture(gMinimap);
	gMinimap = NULL;

	SDL_DestroyTexture(gLetterbox);
	gLetterbox = NULL;

	for (enum KeyPressSurfaces i = KEY_PRESS_SURFACE_DEFAULT; i < KEY_PRESS_SURFACE_TOTAL+2; i++)
		lfree(&gMainplayerTexture[i]);
	for (enum KeyPressSurfaces i = KEY_PRESS_SURFACE_DEFAULT; i < KEY_PRESS_SURFACE_TOTAL; i++)
		lfree(&gMainplayerTexture[i]);
	for (int i = 0; i < 12; i++)
		lfree(&gTileTexture[i]);
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
void reverse_V_handleEvent(_LPlayer* LP, SDL_Event* e) //키입력에 따른 좌표조절
{
	if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY += LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX += LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX -= LP->Player_VEL; break;
		}
	}
	else if (e->type == SDL_KEYUP && e->key.repeat == 0)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY += LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX -= LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX += LP->Player_VEL; break;
		}
	}
}
void move(_LPlayer* LP, _LTile *tiles) //플레이어 무브 & 충돌처리
{
	LP->mBox.x += LP->mVelX;


	if ((LP->mBox.x < 0) || (LP->mBox.x + LP->Player_WIDTH > LEVEL_WIDTH) || touchesWall(LP->mBox, tiles))
	{
		LP->mBox.x -= LP->mVelX;
	}

	LP->mBox.y += LP->mVelY;

	if ((LP->mBox.y < 0) || (LP->mBox.y + LP->Player_HEIGHT > LEVEL_HEIGHT) || touchesWall(LP->mBox, tiles))
	{
		LP->mBox.y -= LP->mVelY;
	}
}
void setCamera(_LPlayer* LP, SDL_Rect* camera)
{
	camera->x = (getPosX(LP) + LP->Player_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera->y = (getPosY(LP) + LP->Player_HEIGHT / 2) - (SCREEN_HEIGHT*2/3) / 2;

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
void T_handleEvent(_LTexture* CT, _LTexture* LT, SDL_Event* e,int time)
{
	if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
			CT->mTexture = (LT + KEY_PRESS_SURFACE_UP)->mTexture;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_UP)->mWidth;
			CT->mHeight = (LT + KEY_PRESS_SURFACE_UP)->mHeight;
			break;
		case SDLK_DOWN:
			CT->mHeight = (LT + KEY_PRESS_SURFACE_DOWN)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_DOWN)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_DOWN)->mTexture;
			break;
		case SDLK_LEFT:
			if (time % 2 == 0)
			{
				CT->mHeight = (LT + KEY_PRESS_SURFACE_LEFT)->mHeight;
				CT->mTexture = (LT + KEY_PRESS_SURFACE_LEFT)->mTexture;
				CT->mWidth = (LT + KEY_PRESS_SURFACE_LEFT)->mWidth;
			}
			else
			{
				CT->mHeight = (LT + 6)->mHeight;
				CT->mTexture = (LT + 6)->mTexture;
				CT->mWidth = (LT + 6)->mWidth;
			}
			break;
		case SDLK_RIGHT:
			if (time % 2 == 0)
			{
				CT->mHeight = (LT + KEY_PRESS_SURFACE_RIGHT)->mHeight;
				CT->mTexture = (LT + KEY_PRESS_SURFACE_RIGHT)->mTexture;
				CT->mWidth = (LT + KEY_PRESS_SURFACE_RIGHT)->mWidth;
			}
			else
			{
				CT->mHeight = (LT + 7)->mHeight;
				CT->mTexture = (LT + 7)->mTexture;
				CT->mWidth = (LT + 7)->mWidth;
			}
			break;
		case SDLK_o:
			CT->mHeight = (LT + KEY_PRESS_SURFACE_O)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_O)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_O)->mTexture;
			break;
		default:
			CT->mHeight = (LT + KEY_PRESS_SURFACE_DEFAULT)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_DEFAULT)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_DEFAULT)->mTexture;
			break;
		}
	}
}
void reverse_T_handleEvent(_LTexture* CT, _LTexture* LT, SDL_Event* e, int time)
{
	if (e->type == SDL_KEYDOWN)
	{

		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
			if ((150 - time / 1000) <= 140)
			{
				CT->mTexture = (LT + KEY_PRESS_SURFACE_O)->mTexture;
				CT->mWidth = (LT + KEY_PRESS_SURFACE_O)->mWidth;
				CT->mHeight = (LT + KEY_PRESS_SURFACE_O)->mHeight;
			}
			else
			{
				CT->mTexture = (LT + KEY_PRESS_SURFACE_DOWN)->mTexture;
				CT->mWidth = (LT + KEY_PRESS_SURFACE_DOWN)->mWidth;
				CT->mHeight = (LT + KEY_PRESS_SURFACE_DOWN)->mHeight;
			}
			break;
		case SDLK_DOWN:
			CT->mHeight = (LT + KEY_PRESS_SURFACE_UP)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_UP)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_UP)->mTexture;
			break;
		case SDLK_LEFT:
			if ((time/350) % 2 == 0)
			{
				CT->mHeight = (LT + KEY_PRESS_SURFACE_RIGHT)->mHeight;
				CT->mTexture = (LT + KEY_PRESS_SURFACE_RIGHT)->mTexture;
				CT->mWidth = (LT + KEY_PRESS_SURFACE_RIGHT)->mWidth;
			}
			else
			{
				CT->mHeight = (LT + 7)->mHeight;
				CT->mTexture = (LT + 7)->mTexture;
				CT->mWidth = (LT + 7)->mWidth;
			}
			break;
		case SDLK_RIGHT:
			if ((time/350) % 2 == 0)
			{
				CT->mHeight = (LT + KEY_PRESS_SURFACE_LEFT)->mHeight;
				CT->mTexture = (LT + KEY_PRESS_SURFACE_LEFT)->mTexture;
				CT->mWidth = (LT + KEY_PRESS_SURFACE_LEFT)->mWidth;
			}
			else
			{
				CT->mHeight = (LT + 6)->mHeight;
				CT->mTexture = (LT + 6)->mTexture;
				CT->mWidth = (LT + 6)->mWidth;
			}
			break;
		case SDLK_o:
			CT->mHeight = (LT + KEY_PRESS_SURFACE_O)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_O)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_O)->mTexture;
			break;
		default:
			CT->mHeight = (LT + KEY_PRESS_SURFACE_DEFAULT)->mHeight;
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
	bool loaded = true;
	int x = 0, y = 0, i = 0;

	char tempbuf[2500];
	char* map[2500] = { 0, };
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
		for (int i = 0; i < 767; i++)
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
			else if (!strcmp(map[i], "12"))
			{
				tileType = 12;
				printf("\n set tileType : 12");
			}
			else if (!strcmp(map[i], "13"))
			{
				tileType = 13;
				printf("\n set tileType : 13");
			}
			else if (!strcmp(map[i], "14"))
			{
				tileType = 14;
				printf("\n set tileType : 14");
			}
			else if (!strcmp(map[i], "15"))
			{
				tileType = 15;
				printf("\n set tileType : 15");
			}
			else if (!strcmp(map[i], "16"))
			{
				tileType = 16;
				printf("\n set tileType : 16");
			}
			else if (!strcmp(map[i], "17"))
			{
				tileType = 17;
				printf("\n set tileType : 17");
			}
			else if (!strcmp(map[i], "18"))
			{
				tileType = 18;
				printf("\n set tileType : 18");
			}
			else if (!strcmp(map[i], "19"))
			{
				tileType = 19;
				printf("\n set tileType : 19");
			}
			else if (!strcmp(map[i], "20"))
			{
				tileType = 20;
				printf("\n set tileType : 20");
			}
			else if (!strcmp(map[i], "21"))
			{
				tileType = 21;
				printf("\n set tileType : 21");
			}
			else if (!strcmp(map[i], "22"))
			{
				tileType = 22;
				printf("\n set tileType : 22");
			}

			printf("\nready to set tiles");
			(tiles + i)->mBox.x = x;
			(tiles + i)->mBox.y = y;
			(tiles + i)->mBox.w = TILE_WIDTH;
			(tiles + i)->mBox.h = TILE_HEIGHT;
			(tiles + i)->mType = tileType;

			printf("tiles[%d] = %d\n", i, (tiles + i)->mType);
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
	for (int i = 0; i < TOTAL_TILES; i++)
	{
		if (((tiles + i)->mType >= 5) && ((tiles + i)->mType <= 22))
		{
			if (checkCollision(box, (tiles + i)->mBox))
			{
				return true;
			}
		}
		
	}
	return false;
}