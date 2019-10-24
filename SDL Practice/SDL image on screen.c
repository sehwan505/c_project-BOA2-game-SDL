#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//SDL 이니셜라이징&윈도우 생성
bool init();
//미디어 로드
bool loadMedia();
//모두 닫기
void close();
//랜더링 윈도우
SDL_Window* gWindow = NULL; //항상 null로 초기화 할것
//sdl surface
SDL_Surface* gScreenSurface = NULL; //항상 null로 초기화 할것
//표시할 이미지
SDL_Surface* gHelloWorld = NULL; //항상 null로 초기화 할것

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("이니셜라이즈 실패!\n");
	}
	else
	{
		if (!loadMedia())
		{
			printf("미디어 로드 실패!\n");
		}
		else
		{
			//메인 루프 플래그
			bool quit = false;

			//이벤트 핸들러
			SDL_Event e;

			//실행중일때
			while (!quit)
			{
				//큐에다가 이벤트 핸들
				while (SDL_PollEvent(&e) != 0)
				{
					//만약 유저가 종료 요청시
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
				//로드된 surface를 surface 위에 blit
				SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
				//스크린 업데이트
				SDL_UpdateWindowSurface(gWindow);
			}

			
			
			
		}
	}
	close();

	return 0;
}

bool init()
{
	//init flag
	bool success = true;

	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL_Init_ERROR: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//윈도우 생성
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("윈도우 생성 에러 : %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//window surface 가져오기
			gScreenSurface = SDL_GetWindowSurface(gWindow); 
		}
	}

	return success;
}

bool loadMedia()
{
	//플래그
	bool success = true;
	//비트맵이미지 불러오기
	gHelloWorld = SDL_LoadBMP("test.bmp");

	if (gHelloWorld == NULL)
	{
		printf("이미지 로드 에러 : %s", SDL_GetError());
		success = false;
	}
	return success;
}

void close()
{
	//서피스 해제
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;
	//윈도우 파괴
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//SDL종료
	SDL_Quit();
}