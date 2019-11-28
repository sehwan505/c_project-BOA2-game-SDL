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


bool init() //초기화 함수
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
		gDuck[i].Player_VEL = 4;
		gDuck[i].mBox.w = gDuck[i].Player_WIDTH;
		gDuck[i].mBox.h = gDuck[i].Player_HEIGHT;
		gDuck[i].mVelX = 0;
		gDuck[i].mVelY = 0;
		gDuck[i].mBox.x = 0;
		gDuck[i].mBox.y = 0;
	}

	//타이머 구조체 맴버 초기화
	timer.mStartTicks = 0;
	timer.mPauseTicks = 0;
	timer.mPaused = false;
	timer.mStarted = false;

	return Sflag;

}
SDL_Surface* loadSurface(char* path)  //이미지 로드 함수(비트맵 이미지)
{
	//경로의 이미지 로드
	SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	if (loadedSurface == NULL) {
		printf("이미지 로드 실패! : %s\n", SDL_GetError());
	}
	return loadedSurface;
}
SDL_Texture* loadTexture(char* path)  //이미지 로드&텍스쳐 변환 함수(PNG)
{
	SDL_Texture* newTexture = NULL;

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

		SDL_FreeSurface(loadedSurface); //자원 반납
	}
	return newTexture;

}
bool loadMedia(_LTile* tiles[]) //각종 이미지 로드
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

	// 폰트 불러옴
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
		if (!loadFromRenderedText(&gTextTexture[1], gRenderer, gFont, "시간이 얼마 남지 않아 오리의 모습이 흉폭해집니다!", textColor))
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
void close() //자원 반납 & 라이브러리 종료
{
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	SDL_DestroyTexture(gMinimap);
	gMinimap = NULL;

	SDL_DestroyTexture(gLetterbox);
	gLetterbox = NULL;

	for (enum KeyPressSurfaces i = KEY_PRESS_SURFACE_DEFAULT; i < KEY_PRESS_SURFACE_TOTAL + 2; i++)
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
bool loadFromFile(_LTexture * LT, SDL_Renderer * Renderer, char* path) //PNG로드해서 텍스쳐 생성(이미지 크기도 저장) -> 텍스쳐 구조체로 반환
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
void lfree(_LTexture * LT) //텍스쳐 구조체에 접근해서 널로 초기화
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
void render(_LTexture * LT, SDL_Renderer * Renderer, int x, int y) //랜더링
{
	SDL_Rect renderQuad = { x,y,LT->mWidth,LT->mHeight };
	SDL_RenderCopy(Renderer, LT->mTexture, NULL, &renderQuad);
}
bool loadFromRenderedText(_LTexture * LT, SDL_Renderer * Renderer, TTF_Font * Font, char* stringText, SDL_Color textColor)
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