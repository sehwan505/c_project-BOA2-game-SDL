Z#include <SDL.h> //SDL lib
#include <stdio.h> 

//스크린 사이즈 정의
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/*
written by HyunJun KIM (Tolerblanc)
hihj070914@icloud.com
2019/10/20 11:45
Simple SDL Test
*/

/*
SDL 세팅
http://libsdl.org/download-2.0.php
Development Libraries - Windows:SDL2-devel-2.9.10-VC.zip(Visual C++ 32/64-bit) 다운로드 후
http://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php
요거 보고 세팅
*/

int main(int argc, char args[])
{
	SDL_Window* window = NULL; //랜더링 포인터 

	SDL_Surface* screenSurface = NULL;

	//SDL 초기화
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//윈도우 생성
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//윈도우 서피스 가져옴
			screenSurface = SDL_GetWindowSurface(window);

			//윈도우를 하얀색으로 채움
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

			//서피스 변경사항 업데이트
			SDL_UpdateWindowSurface(window);

			//딜레이
			SDL_Delay(2000);
		}
	}
	//윈도우 닫음
	SDL_DestroyWindow(window);

	//SDL 라이브러리 종료
	SDL_Quit();

	return 0;
}
