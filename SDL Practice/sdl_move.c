#include <SDL.h>                  //SDL lib
#include <SDL_image.h>            //SDL_image lib
#include <SDL_ttf.h>
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include "han2unicode.h"

const int SCREEN_WIDTH = 640; //��ũ�� ����
const int SCREEN_HEIGHT = 480; //��ũ�� ����

enum KeyPressSurfaces //Ű�Է� ����
{
	KEY_PRESS_SURFACE_DEFAULT, //=0
	KEY_PRESS_SURFACE_UP,      //=1
	KEY_PRESS_SURFACE_DOWN,    //=2
	KEY_PRESS_SURFACE_LEFT,    //=3
	KEY_PRESS_SURFACE_RIGHT,   //=4
	KEY_PRESS_SURFACE_TOTAL,   //=5
};

typedef struct _LTexture       //�ؽ��� ����ü
{
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;

}_LTexture;
typedef struct _LPlayer
{
	int Player_WIDTH;
	int Player_HEIGHT;
	int Player_VEL;

	int mPosX, mPosY;
	int mVelX, mVelY;
}_LPlayer;

struct _LTexture gBackgroundTexture;
struct _LTexture gMainplayerTexture;
struct _LTexture gDuckTexture;
struct _LTexture gTextTexture;

struct _LPlayer gPlayer;


bool init(); //�ʱ�ȭ
bool loadMedia(); //�̵�� �ε�
void close(); // SDL ���̺귯�� �ݱ�
bool loadFromFile(struct _LTexture* LT, SDL_Renderer* Renderer, char* path);
void lfree(struct _LTexture* LT);
void render(_LTexture* LT, SDL_Renderer* Renderer, int x, int y); //������ ��ǥ�� ������
bool loadFromRenderedText(_LTexture* LT, SDL_Renderer* Renderer, TTF_Font* Font, char* stringText, SDL_Color textColor);
void handleEvent(_LPlayer* LP, SDL_Event*e);
void move(_LPlayer* LP);

SDL_Surface* loadSurface(char* path); //��ο� �ִ� ���ǽ� �ε� �Լ�
SDL_Texture* loadTexture(char* path); //���� ���� ��� ( ���ǽ����� ���� ����)

SDL_Window* gWindow = NULL; //������ ������ ������
SDL_Surface* gScreenSurface = NULL; //��ũ�����ǽ� ������


SDL_Renderer* gRenderer = NULL;     //������ ������

SDL_Texture* gTexture = NULL;       //�⺻ �ؽ��� ������
SDL_Texture* gLetterbox = NULL;
SDL_Texture* gMinimap = NULL;

SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL]; //Ű�� ���� �̹��� ������

SDL_Surface* gCurrentSurface = NULL; //���� ǥ�õǴ� �̹����� ����Ű�� ������

TTF_Font* gFont = NULL;

int main()
{

	if (!init())
	{
		printf("�ʱ�ȭ ����!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//�⺻ Ű ���� ���ǽ�
			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					handleEvent(&gPlayer, &e);
					/*
					else if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_UP:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
							printf("UP key\n");
							break;
						case SDLK_DOWN:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
							printf("DOWN key\n");
							break;
						case SDLK_LEFT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
							printf("LEFT key\n");
							break;
						case SDLK_RIGHT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
							printf("RIGHT key\n");
							break;
						default:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
							break;
						}
					}
					*/
				}

				//�̹��� ����
				//SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);

				//���ǽ� ������Ʈ
				//SDL_UpdateWindowSurface(gWindow);

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				move(&gPlayer);

				//���ȭ�� ������ �� ������ Ŭ����

				SDL_Rect botLeftViewport;
				botLeftViewport.x = 0;
				botLeftViewport.y = SCREEN_HEIGHT * 2 / 3;
				botLeftViewport.w = SCREEN_WIDTH * 2 / 3;
				botLeftViewport.h = SCREEN_HEIGHT / 3;
				SDL_RenderSetViewport(gRenderer, &botLeftViewport);

				SDL_RenderCopy(gRenderer, gLetterbox, NULL, NULL);
				render(&gTextTexture, gRenderer, 12, 12);



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
				SDL_RenderSetViewport(gRenderer, &topViewport); //������ ������ ����Ʈ�� ������ ����

				//SDL_RenderCopy(gRenderer, gBackgroundTexture->mTexture, NULL, NULL); //�ؽ��� -> ������ �� ī��
				render(&gBackgroundTexture, gRenderer, 0, 0);
				render(&gMainplayerTexture, gRenderer, 24, 19);
				
				render(&gDuckTexture, gRenderer, gPlayer.mPosX, gPlayer.mPosY);

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

	//SDL �ʱ�ȭ
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL�� �ʱ�ȭ �� �� �����ϴ�. : %s", SDL_GetError());
		Sflag = false;
	}
	else
	{
		//������ ����
		gWindow = SDL_CreateWindow("Sprite Render", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("�����츦 ������ �� �����ϴ�. : %s", SDL_GetError());
			Sflag = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED); //������ ����
			if (gRenderer == NULL)
			{
				printf("������ ���� ����! : %s", SDL_GetError());
				Sflag = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); //�������� ���ȭ������ �׸����� ����


				int imgFlags = IMG_INIT_PNG; //PNG �ε� �÷���
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL�̹��� ����� �ʱ�ȭ �� �� �����ϴ�! : %s", IMG_GetError());
					Sflag = false;
				}
				else if (TTF_Init() == -1)
				{
					printf("SDL_TTF ����� �ʱ�ȭ �� �� �����ϴ�! : %s", TTF_GetError());
					Sflag = false;
				}
				else
				{
					gScreenSurface = SDL_GetWindowSurface(gWindow);
				}


			}
		}
	}
	//�ƴ� �� �Լ� �ٱ����� �ʱ�ȭ �ȵ�? �׷��� ���⼭ �Ԥ���
	gPlayer.Player_HEIGHT = 20;
	gPlayer.Player_WIDTH = 20;
	gPlayer.Player_VEL = 1;
	gPlayer.mPosX = 0;
	gPlayer.mPosY = 0;
	gPlayer.mVelX = 0;
	gPlayer.mVelY = 0;

	return Sflag;

}

SDL_Surface* loadSurface(char* path)
{
	//����� �̹��� �ε�
	SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	if (loadedSurface == NULL) {
		printf("�̹��� �ε� ����! : %s\n", SDL_GetError());
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
		printf("�̹��� �ε� ����! : %s\n", IMG_GetError()); //IMG��� ���� SDL_GetError ��� IMG_GetError���
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("�ؽ��� ���� ����! : %s\n", SDL_GetError());
		}

		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;

}

bool loadMedia()
{
	bool Sflag = true; //success flag

	/* ����Ű�� ���� ���ǽ� �ε�
	//����Ʈ
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("example.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("����Ʈ �̹��� �ε� ����!\n");
		Sflag = false;
	}
	//��
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("up.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
	{
		printf("�� �̹��� �ε� ����!\n");
		Sflag = false;
	}
	//�ٿ�
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("down.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
	{
		printf("�Ʒ� �̹��� �ε� ����!\n");
		Sflag = false;
	}
	//����
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("left.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
	{
		printf("���� �̹��� �ε� ����!\n");
		Sflag = false;
	}
	//������
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("right.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
	{
		printf("������ �̹��� �ε� ����!\n");
		Sflag = false;
	}
	*/
	/*
	gTexture = loadTexture("example.png");
	if (gTexture == NULL)
	{
		printf("�ؽ����̹��� �ε� ����!\n");
		Sflag = false;
	}
	*/
	loadFromFile(&gBackgroundTexture, gRenderer, "BG.png");
	if (gBackgroundTexture.mTexture == NULL)
	{
		printf("��׶��� �ؽ��� �ε� ����!\n");
		Sflag = false;
	}

	loadFromFile(&gMainplayerTexture, gRenderer, "player.png");
	if (gMainplayerTexture.mTexture == NULL)
	{
		printf("�����÷��̾� �ؽ��� �ε� ����!\n");
		Sflag = false;
	}
	loadFromFile(&gDuckTexture, gRenderer, "duck.png");
	if (gDuckTexture.mTexture == NULL)
	{
		printf("���� �ε� ����!\n");
		Sflag = false;
	}

	gLetterbox = loadTexture("Lbox.png");
	if (gLetterbox == NULL)
	{
		printf("�ؽ����̹���(gLetterbox) �ε� ����!\n");
		Sflag = false;
	}

	gMinimap = loadTexture("minimap.png");
	if (gMinimap == NULL)
	{
		printf("�ؽ����̹���(gMinimap) �ε� ����!\n");
		Sflag = false;
	}

	//���� ���� ��Ʈ �ҷ���
	gFont = TTF_OpenFont("korean.ttf", 28);
	if (gFont == NULL)
	{
		printf("��Ʈ �ε� ����! : %s", TTF_GetError());
		Sflag = false;
	}
	else
	{
		SDL_Color textColor = { 255,0,0 };
		if (!loadFromRenderedText(&gTextTexture, gRenderer, gFont, "������ �в�", textColor))
		{
			printf("�ؽ��� ���� ����!");
			Sflag = false;
		}
	}

	//���� ���� ���ϴ� �̹����� �����Ϳ� ������ �������� ���� ��ġ���Ѽ� ������Ʈ 
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
	lfree(&gMainplayerTexture);
	lfree(&gFont);
	//���ǽ� �ʱ�ȭ
	SDL_FreeSurface(gCurrentSurface);
	gCurrentSurface = NULL;


	TTF_CloseFont(gFont);
	gFont = NULL;
	//������ �ݱ�
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//sdl ���̺귯�� �ݱ�
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}

bool loadFromFile(_LTexture* LT, SDL_Renderer* Renderer, char* path)
{

	lfree(LT);

	SDL_Texture* newTexture = NULL;  //�ؽ��� ������

	SDL_Surface* loadedSurface = IMG_Load(path);

	if (loadedSurface == NULL)
	{
		printf("�̹��� �ε� ����! : %s", IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
		newTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("�ؽ��� ���� ����! : %s", SDL_GetError());
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

void lfree(_LTexture* LT) //�ؽ��� ����ü�� �����ؼ� �η� �ʱ�ȭ
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

void render(_LTexture* LT, SDL_Renderer* Renderer, int x, int y) //������
{
	SDL_Rect renderQuad = { x,y,LT->mWidth,LT->mHeight };
	SDL_RenderCopy(Renderer, LT->mTexture, NULL, &renderQuad);
}

bool loadFromRenderedText(_LTexture* LT, SDL_Renderer* Renderer, TTF_Font* Font, char* stringText, SDL_Color textColor)
{
	lfree(LT);
	unsigned short unicode[128]; //�����ڵ� �迭 ����
	han2unicode(stringText, unicode); //��Ʈ���� �����ڵ�� ��ȯ
	SDL_Surface* textSurface = TTF_RenderUNICODE_Solid(Font, unicode, textColor); //���ǽ� �����Ϳ� ��Ʈ �����Լ��ֱ�
	if (textSurface == NULL)
	{
		printf("�ؽ��� ���ǽ� ���� ����! : %s\n", TTF_GetError());
	}
	else
	{
		LT->mTexture = SDL_CreateTextureFromSurface(Renderer, textSurface);
		if (LT->mTexture == NULL)
		{
			printf("�ؽ��� ���� ���� : %s", SDL_GetError());
		}
		else
		{
			LT->mWidth = textSurface->w;
			LT->mHeight = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	}
	return LT->mTexture != NULL;
}//�ؽ��� �ε�

void handleEvent(_LPlayer* LP, SDL_Event* e)
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

void move(_LPlayer* LP)
{
	LP->mPosX += LP->mVelX;

	if ((LP->mPosX < 0) || (LP->mPosX + LP->Player_WIDTH > SCREEN_WIDTH))
	{
		LP->mPosX -= LP->mVelX;
	}

	LP->mPosY += LP->mVelY;

	if ((LP->mPosY < 0) || (LP->mPosY + LP->Player_HEIGHT> SCREEN_HEIGHT*2/3))
	{
		LP->mPosY -= LP->mVelY;
	}
}