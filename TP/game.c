#include <SDL.h>                  //SDL lib
#include <SDL_image.h>            //SDL_image lib
#include <SDL_ttf.h>              //SDL ttf lib
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include "game.h"

void T_handleEvent(_LTexture* CT, _LTexture* LT, SDL_Event* e,int time) //텍스쳐 핸들링
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
void reverse_T_handleEvent(_LTexture* CT, _LTexture* LT, SDL_Event* e, int time) //텍스쳐 핸들링(오리)
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
bool setTiles(_LTile *tiles) //맵파일 읽어서 타일셋팅
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
				printf("\n set tileType : 00 ");
			}
			else if (!strcmp(map[i], "01"))
			{
				tileType = 1;
				printf("\n set tileType : 01 ");
			}
			else if (!strcmp(map[i], "02"))
			{
				tileType = 2;
				printf("\n set tileType : 02 ");
			}
			else if (!strcmp(map[i], "03"))
			{
				tileType = 3;
				printf("\n set tileType : 03 ");
			}
			else if (!strcmp(map[i], "04"))
			{
				tileType = 4;
				printf("\n set tileType : 04 ");
			}
			else if (!strcmp(map[i], "05"))
			{
				tileType = 5;
				printf("\n set tileType : 05 ");
			}
			else if (!strcmp(map[i], "06"))
			{
				tileType = 6;
				printf("\n set tileType : 06 ");
			}
			else if (!strcmp(map[i], "07"))
			{
				tileType = 7;
				printf("\n set tileType : 07 ");
			}
			else if (!strcmp(map[i], "08"))
			{
				tileType = 8;
				printf("\n set tileType : 08 ");
			}
			else if (!strcmp(map[i], "09"))
			{
				tileType = 9;
				printf("\n set tileType : 09 ");
			}
			else if (!strcmp(map[i], "10"))
			{
				tileType = 10;
				printf("\n set tileType : 10 ");
			}
			else if (!strcmp(map[i], "11"))
			{
				tileType = 11;
				printf("\n set tileType : 11 ");
			}
			else if (!strcmp(map[i], "12"))
			{
				tileType = 12;
				printf("\n set tileType : 12 ");
			}
			else if (!strcmp(map[i], "13"))
			{
				tileType = 13;
				printf("\n set tileType : 13 ");
			}
			else if (!strcmp(map[i], "14"))
			{
				tileType = 14;
				printf("\n set tileType : 14 ");
			}
			else if (!strcmp(map[i], "15"))
			{
				tileType = 15;
				printf("\n set tileType : 15 ");
			}
			else if (!strcmp(map[i], "16"))
			{
				tileType = 16;
				printf("\n set tileType : 16 ");
			}
			else if (!strcmp(map[i], "17"))
			{
				tileType = 17;
				printf("\n set tileType : 17 ");
			}
			else if (!strcmp(map[i], "18"))
			{
				tileType = 18;
				printf("\n set tileType : 18 ");
			}
			else if (!strcmp(map[i], "19"))
			{
				tileType = 19;
				printf("\n set tileType : 19 ");
			}
			else if (!strcmp(map[i], "20"))
			{
				tileType = 20;
				printf("\n set tileType : 20 ");
			}
			else if (!strcmp(map[i], "21"))
			{
				tileType = 21;
				printf("\n set tileType : 21 ");
			}
			else if (!strcmp(map[i], "22"))
			{
				tileType = 22;
				printf("\n set tileType : 22 ");
			}

			(tiles + i)->mBox.x = x;
			(tiles + i)->mBox.y = y;
			(tiles + i)->mBox.w = TILE_WIDTH;
			(tiles + i)->mBox.h = TILE_HEIGHT;
			(tiles + i)->mType = tileType;

			printf(" tiles[%d] = %d\n", i, (tiles + i)->mType);
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
bool touchesWall(SDL_Rect box, _LTile* tiles) //벽타일과 충돌하는지 검사
{
	for (int i = 0; i < TOTAL_TILES; i++)
	{
		if (((tiles + i)->mType >= 6) && ((tiles + i)->mType <= 22))
		{
			if (checkCollision(box, (tiles + i)->mBox))
			{
				return true;
			}
		}
		
	}
	return false;
}
