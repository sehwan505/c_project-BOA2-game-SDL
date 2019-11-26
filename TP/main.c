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
#include "game.h"
#include "timer.h"


SDL_Renderer* gRenderer = NULL;     //랜더러 포인터
SDL_Surface* loadSurface(char* path); //경로에 있는 서피스 로드 함수
SDL_Texture* loadTexture(char* path); //위와 같은 기능 ( 서피스보다 성능 좋음)
SDL_Window* gWindow = NULL; //랜더링 윈도우 포인터
SDL_Surface* gScreenSurface = NULL; //스크린서피스 포인터
SDL_Texture* gTexture = NULL;       //기본 텍스쳐 포인터
SDL_Texture* gLetterbox = NULL;     //왼쪽아래 메시지박스 포인터
SDL_Texture* gMinimap = NULL;       //오른쪽 아래 미니맵 포인터
TTF_Font* gFont = NULL;  //기본 폰트 포인터

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

//텍스쳐 등 구현할 구조체들 정의
struct _LTexture gMainplayerTexture[KEY_PRESS_SURFACE_TOTAL+2]; //메인캐릭터 텍스쳐
struct _LTexture gDuckTexture[KEY_PRESS_SURFACE_TOTAL+2];
struct _LTexture gCurrentDuck;
struct _LTexture gTimeText;
struct _LTexture gCurrentTime;
struct _LTexture gSightLimiter;   //시야 가리기
struct _LTexture gTextTexture[2]; //택스트도 구조체 배열을 통하여 미리 집어넣어놓고, 이벤트에 따라서 꺼내어 랜더링 할 수 있다.
struct _LTexture gCurrentSurface; //현재 표시되는 서피스
struct _LTexture gCurrentText;    //현재 표시되는 텍스트
struct _LTexture gTileTexture[23];//타일셋 텍스처
struct _LTexture gLeaderBoard[5];
struct _LTexture gScore[5];
struct _LTexture gStartText;
struct _LPlayer gPlayer;
struct _LPlayer gDuck[5];
struct _LTimer timer;


int main()
{
	srand(time(NULL));
	//SDL 모듈 초기화
	if (!init())
	{
		printf("초기화 실패!\n");
	}
	else
	{
		char time[150];
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
			bool game_end = false;

			//이벤트 핸들러
			SDL_Event e;

			//카메라 구조체 선언
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

			//기본 오리 위치 설정
			for (int i = 0; i < 5; i++)
			{
				do {
					
					int ranX = rand() % LEVEL_WIDTH;
					int ranY = rand() % LEVEL_HEIGHT;
					gDuck[i].mBox.x = ranX;
					gDuck[i].mBox.y = ranY;
					
				} while (touchesWall(gDuck[i].mBox, tileSet));
			}

			
			
			//fps제어용 & 게임타이머용 타이머 구조체 선언
			struct _LTimer fpsTimer;
			struct _LTimer capTimer;
			int  countedFrames = 0;
			timer_start(&fpsTimer);
			int Stime = SDL_GetTicks() / 1000;
			
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
					else if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_RETURN:
							game_end = false;
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
					for (int i = 0; i < 5; i++)
						reverse_V_handleEvent(&gDuck[i], &e);
					T_handleEvent(&gCurrentSurface, &gMainplayerTexture, &e, SDL_GetTicks() / 350);  //키다운에 따른 텍스쳐 변경
					reverse_T_handleEvent(&gCurrentDuck, &gDuckTexture, &e, SDL_GetTicks());
				}

				float avgFPS = countedFrames / (getTicks(&fpsTimer) / (float)1000);
				if (avgFPS > 2000000)
				{
					avgFPS = 0;
				}

				if (game_end)
				{
					SDL_RenderClear(gRenderer);
					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); //랜더러 색을 검은색으로 설정

					gPlayer.mBox.x = 0;
					gPlayer.mBox.y = 0;//플레이어 위치 초기화

					render(&gDuckTexture[5], gRenderer, 80, 80);

					for (int i = 0;i < 5;i++)
					{
						render(&gLeaderBoard[i], gRenderer, 240, 75 * (i+1));
					}//리더보드 글자 생성

					for (int i = 0;i < 5;i++)
					{
						render(&gLeaderBoard[i], gRenderer, 300, 75 * (i + 1));
					}//리더보드 스코어 생성

					


					for (int i = 0; i < 5; i++)
					{
						do {

							int ranX = rand() % LEVEL_WIDTH;
							int ranY = rand() % LEVEL_HEIGHT;
							gDuck[i].mBox.x = ranX;
							gDuck[i].mBox.y = ranY;

						} while (touchesWall(gDuck[i].mBox, tileSet));
					}//오리위치 재설정


					timer_stop(&capTimer);

					render(&gStartText, gRenderer, 250, 420);
					SDL_RenderPresent(gRenderer);
					Stime = SDL_GetTicks() / 1000;

				}
				else
				{
					
					//시간 표시를 위한 변수 설정
					SDL_Color timeColor = { 255,255,255 };
					int Ctime = 150 - (SDL_GetTicks() / 1000 - Stime);
					sprintf(time, "%d", Ctime);

					if (!loadFromRenderedText(&gCurrentTime, gRenderer, gFont, time, timeColor))
					{
						printf("메인 타이머를 랜더할 수 없습니다! \n");
					}
					if (Ctime <= 0) //남은 시간이 0이하일때 종료
					{
						printf("타임오버!\n");
						game_end = true;
					}


					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); //랜더러 색을 검은색으로 설정
					SDL_RenderClear(gRenderer);
					//검은화면 랜더러 설정 후 랜더러 클리어

					//캐릭터 이동 & 카메라 셋팅
					move(&gPlayer, &tileSet);
					for (int i = 0; i < 5; i++)
					{
						move(&gDuck[i], &tileSet);
						if (checkCollision(gPlayer.mBox, (gDuck + i)->mBox))
						{
							game_end = true;
						}//오리랑 플레이어 충돌시 종료
					}
					setCamera(&gPlayer, &Camera);


					if (checkCollision(gPlayer.mBox, tileSet[2].mBox))
					{
						game_end = true;
					}


					SDL_Rect botLeftViewport; //로그박스 뷰포트
					botLeftViewport.x = 0;
					botLeftViewport.y = SCREEN_HEIGHT * 2 / 3;
					botLeftViewport.w = SCREEN_WIDTH * 2 / 3;
					botLeftViewport.h = SCREEN_HEIGHT / 3;
					SDL_RenderSetViewport(gRenderer, &botLeftViewport);

					SDL_RenderCopy(gRenderer, gLetterbox, NULL, NULL);

					//랜더(타이머, 텍스트)
					render(&gTimeText, gRenderer, 30, 30);
					render(&gCurrentTime, gRenderer, 130, 30);
					if (Ctime <= 140)
					{
						gCurrentText.mTexture = gTextTexture[1].mTexture;
						gCurrentText.mHeight = gTextTexture[1].mHeight;
						gCurrentText.mWidth = gTextTexture[1].mWidth;
						for (int i = 0; i < 5; i++)
							gDuck[i].Player_VEL = 4;
						render(&gCurrentText, gRenderer, 30, 60);
					}
					else
					{
						render(&gCurrentText, gRenderer, 30, 60);
					}


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


					for (int i = 0; i < TOTAL_TILES; ++i)
					{
						if (checkCollision(Camera, (tileSet + i)->mBox))
						{
							render(&gTileTexture[(tileSet + i)->mType], gRenderer, (tileSet + i)->mBox.x - Camera.x, (tileSet + i)->mBox.y - Camera.y);
						}
					}

					//!!!랜더링 순서 중요함!!! 
					for (int i = 0; i < 5; i++)
						render(&gCurrentDuck, gRenderer, gDuck[i].mBox.x - Camera.x, gDuck[i].mBox.y - Camera.y);
					render(&gSightLimiter, gRenderer, gPlayer.mBox.x - 1310 - Camera.x, gPlayer.mBox.y - 730 - Camera.y);  //사이트 리미터(플레이어랑 같이 움직임) 플레이어 기본위치를 빼주어야 정확히 가운데에 위치
					render(&gCurrentSurface, gRenderer, gPlayer.mBox.x - Camera.x, gPlayer.mBox.y - Camera.y); //플레이어 무브

					SDL_RenderPresent(gRenderer);  //Update
					++countedFrames;

					

				}

				int frameTicks = getTicks(&capTimer);
				if (frameTicks < SCREEN_TICK_PER_FRAME)
				{
					SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
				}//초당 60프레임 이상 랜더 방지

			}

		}

	}

	close();

	return 0;
}


void fileRead(char* buffer)
{
	char tempbuf[12000];
	FILE *fp = fopen("level.map", "r");

	fgets(tempbuf, sizeof(tempbuf), fp);

	printf("%s\n", tempbuf);
	fclose(fp);
	strcpy(buffer, tempbuf);

}
int refToken(char* buf[], char *inp[])
{
	int i = 0;
	char *ptr = strtok(buf, " ");


	while (ptr != NULL)
	{
		inp[i] = ptr;
		i++;
		ptr = strtok(NULL, " ");
	}


	for (int i = 0; i < 12000; i++)
	{
		if (inp[i] != NULL)
			printf("%s\n", inp[i]);
	}
	printf("\n\n\n");

	if (!strcmp("02", inp[2]))
	{
		int temp = 2;
		printf("%d\n", temp);
	}


	return i;
}
void selectionSort(int *pArr, int num)
{
	for (int i = 0; i < num; i++)
	{
		int n = i;
		for (int j = (i + 1); j < num; j++)
		{
			if (*(pArr + j) < *(pArr + n))
				n = j;
		}
		SWAP((pArr + i), (pArr + n));
	}
}

void SWAP(int *pa, int *pb)
{
	int temp;
	temp = *pa;
	*pa = *pb;
	*pb = temp;
}