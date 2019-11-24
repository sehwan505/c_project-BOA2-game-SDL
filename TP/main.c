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



SDL_Renderer* gRenderer = NULL;     //������ ������
SDL_Surface* loadSurface(char* path); //��ο� �ִ� ���ǽ� �ε� �Լ�
SDL_Texture* loadTexture(char* path); //���� ���� ��� ( ���ǽ����� ���� ����)
SDL_Window* gWindow = NULL; //������ ������ ������
SDL_Surface* gScreenSurface = NULL; //��ũ�����ǽ� ������
SDL_Texture* gTexture = NULL;       //�⺻ �ؽ��� ������
SDL_Texture* gLetterbox = NULL;     //���ʾƷ� �޽����ڽ� ������
SDL_Texture* gMinimap = NULL;       //������ �Ʒ� �̴ϸ� ������
TTF_Font* gFont = NULL;  //�⺻ ��Ʈ ������

const int LEVEL_WIDTH = 2560;  //������ ����
const int LEVEL_HEIGHT = 1480; //������ ����

const int SCREEN_WIDTH = 1280; //��ũ�� ����
const int SCREEN_HEIGHT = 720; //��ũ�� ����

const int SCREEN_FPS = 60;     //��ũ�� ������
const int SCREEN_TICK_PER_FRAME = 17; //ƽ�� ������

//Ÿ�� ���e3
const int TILE_WIDTH = 80;//Ÿ�� ����ũ��
const int TILE_HEIGHT = 80;//Ÿ�� ����ũ��
const int TOTAL_TILES = 763;
const int TOTAL_TILE_SPRITES = 12;

//�ؽ��� �� ������ ����ü�� ����
struct _LTexture gMainplayerTexture[KEY_PRESS_SURFACE_TOTAL+2]; //����ĳ���� �ؽ���
struct _LTexture gDuckTexture[KEY_PRESS_SURFACE_TOTAL+2];
struct _LTexture gCurrentDuck;
struct _LTexture gTimeText;
struct _LTexture gCurrentTime;
struct _LTexture gSightLimiter;   //�þ� ������
struct _LTexture gTextTexture[2]; //�ý�Ʈ�� ����ü �迭�� ���Ͽ� �̸� ����־����, �̺�Ʈ�� ���� ������ ������ �� �� �ִ�.
struct _LTexture gCurrentSurface; //���� ǥ�õǴ� ���ǽ�
struct _LTexture gCurrentText;    //���� ǥ�õǴ� �ؽ�Ʈ
struct _LTexture gTileTexture[23];//Ÿ�ϼ� �ؽ�ó
struct _LPlayer gPlayer;
struct _LPlayer gDuck[5];
struct _LTimer timer;


int main()
{
	srand(time(NULL));
	//SDL ��� �ʱ�ȭ
	if (!init())
	{
		printf("�ʱ�ȭ ����!\n");
	}
	else
	{
		char time[150];
		_LTile tileSet[12000] = { 0, };
		//�̵�� �ε�
		if (!loadMedia(tileSet))
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//���η��� �÷���
			bool quit = false;

			//�̺�Ʈ �ڵ鷯
			SDL_Event e;

			//ī�޶� ����ü ����
			SDL_Rect Camera;
			Camera.x = 0;
			Camera.y = 0;
			Camera.w = SCREEN_WIDTH;
			Camera.h = SCREEN_HEIGHT;

			//�⺻ Ű ���� ���ǽ�
			gCurrentSurface.mTexture = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mTexture;
			gCurrentSurface.mHeight = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mHeight;
			gCurrentSurface.mWidth = gMainplayerTexture[KEY_PRESS_SURFACE_DEFAULT].mWidth;

			//�⺻ �ؽ�Ʈ ����
			gCurrentText.mTexture = gTextTexture[0].mTexture;
			gCurrentText.mHeight = gTextTexture[0].mHeight;
			gCurrentText.mWidth = gTextTexture[0].mWidth;

			//�⺻ ���� ��ġ ����
			for (int i = 0; i < 5; i++)
			{
				do {
					
					int ranX = rand() % LEVEL_WIDTH;
					int ranY = rand() % LEVEL_HEIGHT;
					gDuck[i].mBox.x = ranX;
					gDuck[i].mBox.y = ranY;
					
				} while (touchesWall(gDuck[i].mBox, tileSet));
			}

			
			
			//fps����� & ����Ÿ�̸ӿ� Ÿ�̸� ����ü ����
			struct _LTimer fpsTimer;
			struct _LTimer capTimer;
			int  countedFrames = 0;
			timer_start(&fpsTimer);

			//���η��� 
			while (!quit)
			{
				
				timer_start(&capTimer);
				//�̺�Ʈ ó��
				while (SDL_PollEvent(&e) != 0)
				{
					//���� ȣ��� ����
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type == SDL_KEYDOWN) 
					{
						switch (e.key.keysym.sym)
						{
							
						case SDLK_o:
							gPlayer.Player_VEL = 20;
							gCurrentText.mTexture = gTextTexture[1].mTexture;
							gCurrentText.mHeight = gTextTexture[1].mHeight;
							gCurrentText.mWidth = gTextTexture[1].mWidth;
							break;
						case SDLK_s: //Ÿ�̸� ���߱�
							if (timer_isStarted(&timer))
							{
								timer_stop(&timer);
							}
							else
							{
								timer_stop(&timer);
							}
							break;
						case SDLK_p: //Ÿ�̸� �Ͻ�����
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
						
					V_handleEvent(&gPlayer, &e); //Ű�ٿ ���� �̵� �̺�Ʈ
					for (int i =0;i<5;i++)
						reverse_V_handleEvent(&gDuck[i], &e);
					T_handleEvent(&gCurrentSurface, &gMainplayerTexture, &e, SDL_GetTicks() / 350);  //Ű�ٿ ���� �ؽ��� ����
					reverse_T_handleEvent(&gCurrentDuck, &gDuckTexture, &e, SDL_GetTicks() / 350);
				}

				float avgFPS = countedFrames / (getTicks(&fpsTimer) / (float)1000);
				if (avgFPS > 2000000)
				{
					avgFPS = 0;
				}

			

				
				//�ð� ǥ�ø� ���� ���� ����
				SDL_Color timeColor = { 255,255,255 };
				int Ctime = 150 - SDL_GetTicks() / 1000;
				sprintf(time, "%d", Ctime);
				
				if (!loadFromRenderedText(&gCurrentTime, gRenderer, gFont,time, timeColor))
				{
					printf("���� Ÿ�̸Ӹ� ������ �� �����ϴ�! \n");
				}
				if (Ctime <= 0) //���� �ð��� 0�����϶� ����
				{
					printf("Ÿ�ӿ���!\n");
					quit = true;
				}
				

				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); //������ ���� ���������� ����
				SDL_RenderClear(gRenderer);
				//����ȭ�� ������ ���� �� ������ Ŭ����

				//ĳ���� �̵� & ī�޶� ����
				move(&gPlayer,&tileSet);
				for (int i = 0; i < 5; i++)
				{
					move(&gDuck[i], &tileSet);
					if (checkCollision(gPlayer.mBox, (gDuck+i)->mBox))
					{
						quit = true;
					}
				}
				setCamera(&gPlayer, &Camera);

				/*
				if (checkCollision(gPlayer.mBox, tileSet[2].mBox))
				{
					quit = true;
					printf("\n�¸�!\n");
				}
				*/
				
				SDL_Rect botLeftViewport; //�α׹ڽ� ����Ʈ
				botLeftViewport.x = 0;
				botLeftViewport.y = SCREEN_HEIGHT * 2 / 3;
				botLeftViewport.w = SCREEN_WIDTH * 2 / 3;
				botLeftViewport.h = SCREEN_HEIGHT / 3;
				SDL_RenderSetViewport(gRenderer, &botLeftViewport);

				SDL_RenderCopy(gRenderer, gLetterbox, NULL, NULL);

				//����(Ÿ�̸�, �ؽ�Ʈ)
				render(&gTimeText, gRenderer, 30, 30);
				render(&gCurrentTime, gRenderer, 130, 30);
				render(&gCurrentText, gRenderer, 30, 60);



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

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				
			
				for (int i = 0; i < TOTAL_TILES; ++i)
				{
					if (checkCollision(Camera, (tileSet + i)->mBox))
					{
						render(&gTileTexture[(tileSet + i)->mType], gRenderer, (tileSet + i)->mBox.x - Camera.x, (tileSet + i)->mBox.y - Camera.y);
					}
				}
				
				//!!!������ ���� �߿���!!! 
				for(int i=0;i<5;i++)
					render(&gCurrentDuck, gRenderer, gDuck[i].mBox.x - Camera.x, gDuck[i].mBox.y - Camera.y);
				//render(&gSightLimiter, gRenderer, gPlayer.mBox.x - 1310 - Camera.x, gPlayer.mBox.y - 730 - Camera.y);  //����Ʈ ������(�÷��̾�� ���� ������) �÷��̾� �⺻��ġ�� ���־�� ��Ȯ�� ����� ��ġ
				render(&gCurrentSurface, gRenderer, gPlayer.mBox.x - Camera.x, gPlayer.mBox.y - Camera.y); //�÷��̾� ����
				
				SDL_RenderPresent(gRenderer);  //Update
				++countedFrames;

				

				int frameTicks = getTicks(&capTimer);
				if (frameTicks < SCREEN_TICK_PER_FRAME)
				{
					SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
				}//�ʴ� 60������ �̻� ���� ����

			}
		}

	}

	close();

	return 0;
}

