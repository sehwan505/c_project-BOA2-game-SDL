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
const int LEVEL_HEIGHT = 1920; //레벨층 세로

const int SCREEN_WIDTH = 1280; //스크린 가로
const int SCREEN_HEIGHT = 720; //스크린 세로

const int SCREEN_FPS = 60;     //스크린 프레임
const int SCREEN_TICK_PER_FRAME = 17; //틱당 프레임

//타일 상수
const int TILE_WIDTH = 80;          //타일 가로크기
const int TILE_HEIGHT = 80;         //타일 세로크기
const int TOTAL_TILES = 768;        //총 타일 갯수 (32x24)
const int TOTAL_TILE_SPRITES = 23;  //총 타일 스프라이트 갯수

//텍스쳐 등 구현할 구조체들 정의
struct _LTexture gMainplayerTexture[KEY_PRESS_SURFACE_TOTAL+2]; //메인캐릭터 텍스쳐
struct _LTexture gCurrentSurface; //현재 표시되는 서피스
struct _LTexture gDuckTexture[KEY_PRESS_SURFACE_TOTAL+2];  //오리 텍스처
struct _LTexture gCurrentDuck;

struct _LTexture gTimeText;
struct _LTexture gCurrentTime;
struct _LTexture gSightLimiter;    //시야 가리기
struct _LTexture gTextTexture[2];  //택스트도 구조체 배열을 통하여 미리 집어넣어놓고, 이벤트에 따라서 꺼내어 랜더링 할 수 있다
struct _LTexture gCurrentText;     //현재 표시되는 텍스트

struct _LTexture gTileTexture[23]; //타일셋 텍스처
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
			bool quit = false;     //SDL WINDOW 탈출 플래그
			bool game_end = false; //메인 게임 탈출 플래그

			//스코어 변수
			int score = 0;

			//이벤트 핸들러
			SDL_Event e;

			//카메라 구조체 선언
			SDL_Rect Camera;
			Camera.x = 0;
			Camera.y = 0;
			Camera.w = SCREEN_WIDTH;
			Camera.h = SCREEN_HEIGHT * 2 / 3;  //윗쪽 뷰포트가 세로의 2/3 지점이기 때문에 카메라 높이를 세로의 2/3으로 설정

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
				} while (touchesWall(gDuck[i].mBox, tileSet));//랜덤 생성된 위치가 벽일수도 있으니 예외처리
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
					else if (e.type == SDL_KEYDOWN) //키다운 이벤트 처리
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_RETURN:
							game_end = false;
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
						reverse_V_handleEvent(&gDuck[i], &e); //키다운에 따른 오리 이동 이벤트
					T_handleEvent(&gCurrentSurface, &gMainplayerTexture, &e, SDL_GetTicks() / 250);  //키다운에 따른 텍스쳐 변경
					reverse_T_handleEvent(&gCurrentDuck, &gDuckTexture, &e, SDL_GetTicks());         //오리는 애니메이션 외에 일정 시간 이하일시 모습을 바꿔야 하기 때문에 시간을 ms단위로 넣음
				}

				//프레임 제한용 변수
				float avgFPS = countedFrames / (getTicks(&fpsTimer) / (float)1000);
				if (avgFPS > 2000000)
				{
					avgFPS = 0;
				}

				//리더보드 루프
				if (game_end)
				{
					SDL_RenderClear(gRenderer);
					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); //랜더러 색을 검은색으로 설정

					score = 0; //스코어 변수 초기화

					gPlayer.mBox.x = 0;
					gPlayer.mBox.y = 0;//플레이어 위치 초기화

					render(&gDuckTexture[5], gRenderer, 80, 80);

					for (int i = 0; i < 5; i++)
					{
						render(&gLeaderBoard[i], gRenderer, 240, 75 * (i + 1));
					}//리더보드 글자 생성

					for (int i = 0; i < 5; i++)
					{
						render(&gLeaderBoard[i], gRenderer, 300, 75 * (i + 1));
					}//리더보드 스코어 생성


					for (int i = 0; i < 5; i++)
					{
						do {
							int ranX = rand() % LEVEL_WIDTH;
							int ranY = rand() % LEVEL_HEIGHT;
							gDuck[i].mBox.x = ranX;
							gDuck[i].mBox.y = ranY; //오리위치 랜덤 생성
						} while (touchesWall(gDuck[i].mBox, tileSet)); //랜덤 생성된 위치가 벽일수도 있으니 예외처리

					}//오리위치&속도 재설정

					render(&gStartText, gRenderer, 250, 420);
					SDL_RenderPresent(gRenderer);

					//스코어 계산&남은시간 계산을 위해 게임을 시작하는 순간의 시간을 저장해둠
					Stime = SDL_GetTicks() / 1000;
				}
				else
				{

					//시간 표시를 위한 변수 설정
					SDL_Color timeColor = { 255,255,255 };
					int Ctime = 60 - (SDL_GetTicks() / 1000 - Stime);
					sprintf(time, "%d", Ctime);

					score = (SDL_GetTicks() - Stime * 1000); //시간(ms)가 곧 스코어가 됨

					if (!loadFromRenderedText(&gCurrentTime, gRenderer, gFont, time, timeColor))
					{
						printf("메인 타이머를 랜더할 수 없습니다! \n");
					}
					if (Ctime <= 0) //남은 시간이 0이하일때 패배
					{
						printf("타임오버!\n");
						score /= 2; //진거니까 스코어 타노스
						printf("\nScore : %d", score);
						game_end = true;
					}

					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); //랜더러 색을 검은색으로 설정
					SDL_RenderClear(gRenderer);
					//검은화면 랜더러 설정 후 랜더러 클리어

					//캐릭터 이동 & 카메라 셋팅
					move(&gPlayer, &tileSet); //타일셋과 플레이어의 충돌처리
					for (int i = 0; i < 5; i++)
					{
						move(&gDuck[i], &tileSet); //타일셋과 오리의 충돌처리
						if (checkCollision(gPlayer.mBox, (gDuck + i)->mBox)) //오리랑 플레이어 충돌시 패배
						{
							score /= 2; //진거니까 스코어 타노스
							printf("\nScore : %d", score);
							game_end = true;
						}
					}
					setCamera(&gPlayer, &Camera); //카메라 셋팅(보여지는 부분 설정)


					if (checkCollision(gPlayer.mBox, tileSet[733].mBox)) //탈출구 타일과 충돌시 승리
					{
						printf("\nScore : %d", score);
						game_end = true;
					}


					SDL_Rect botLeftViewport; //로그박스 뷰포트(왼쪽 아래)
					botLeftViewport.x = 0;
					botLeftViewport.y = SCREEN_HEIGHT * 2 / 3;
					botLeftViewport.w = SCREEN_WIDTH * 2 / 3;
					botLeftViewport.h = SCREEN_HEIGHT / 3;
					SDL_RenderSetViewport(gRenderer, &botLeftViewport);

					SDL_RenderCopy(gRenderer, gLetterbox, NULL, NULL);

					//랜더(타이머, 텍스트)
					render(&gTimeText, gRenderer, 30, 30);
					render(&gCurrentTime, gRenderer, 130, 30);
					if (Ctime <= 30) //남은시간이 일정 이하일때 
					{
						gCurrentText.mTexture = gTextTexture[1].mTexture;
						gCurrentText.mHeight = gTextTexture[1].mHeight;
						gCurrentText.mWidth = gTextTexture[1].mWidth;//텍스트를 바꿔줌
					}
					render(&gCurrentText, gRenderer, 30, 60);

					SDL_Rect botRightViewport; //미니맵 뷰포트 (구현중)
					botRightViewport.x = SCREEN_WIDTH * 2 / 3;
					botRightViewport.y = SCREEN_HEIGHT * 2 / 3;
					botRightViewport.w = SCREEN_WIDTH / 3;
					botRightViewport.h = SCREEN_HEIGHT / 3;
					SDL_RenderSetViewport(gRenderer, &botRightViewport);

					SDL_RenderCopy(gRenderer, gMinimap, NULL, NULL);
					//벽.png 길.png 두개만 만들고 타일넘버에 따라서 구분뒤에 1/3크기로 랜더링 하면 될듯 차피 tileSet 배열 있으니까


					SDL_Rect topViewport; //메인 게임 뷰포트
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
						} //카메라 안쪽에 들어오는 타일들만 랜더링함
					}

					//!!!랜더링 순서 중요함!!! 
					for (int i = 0; i < 5; i++)
						render(&gCurrentDuck, gRenderer, gDuck[i].mBox.x - Camera.x, gDuck[i].mBox.y - Camera.y); //오리 무브
					render(&gCurrentSurface, gRenderer, gPlayer.mBox.x - Camera.x, gPlayer.mBox.y - Camera.y); //플레이어 무브
					render(&gSightLimiter, gRenderer, gPlayer.mBox.x - 1350 - Camera.x, gPlayer.mBox.y - 670 - Camera.y);  //사이트 리미터(플레이어랑 같이 움직임) 플레이어 기본위치를 빼주어야 정확히 가운데에 위치
					

					SDL_RenderPresent(gRenderer);  //랜더링 된걸 모두 업데이트 
					++countedFrames; //프레임 제어용
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



