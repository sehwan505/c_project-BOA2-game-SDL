#include <SDL.h>                  //SDL lib
#include <SDL_image.h>            //SDL_image lib
#include <SDL_ttf.h>              //SDL ttf lib
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include "game.h"

/*
game.c
게임에서 필요한 기본적인 함수들
-텍스처 이벤트
-충돌처리
-타일셋팅
-벽과의 충돌처리
*/

void T_handleEvent(_LTexture* CT, _LTexture* LT, SDL_Event* e,int time) //텍스쳐 핸들링
{
	if (e->type == SDL_KEYDOWN) //키다운시 
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP://윗쪽 화살표입력시 텍스쳐 지정
			CT->mTexture = (LT + KEY_PRESS_SURFACE_UP)->mTexture; 
			CT->mWidth = (LT + KEY_PRESS_SURFACE_UP)->mWidth;
			CT->mHeight = (LT + KEY_PRESS_SURFACE_UP)->mHeight;
			break;
		case SDLK_DOWN://아래쪽 화살표입력시 텍스쳐 지정
			CT->mHeight = (LT + KEY_PRESS_SURFACE_DOWN)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_DOWN)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_DOWN)->mTexture;
			break;
		case SDLK_LEFT://왼쪽 화살표입력시 텍스쳐 지정
			if (time % 2 == 0) //애니메이션 효과를 위해 입력받은 time이 2의 배수일경우와 아닌경우로 나눠서 지정
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
		case SDLK_RIGHT: //오른쪽 화살표입력시 텍스쳐 지정
			if (time % 2 == 0) //애니메이션 효과를 위해 입력받은 time이 2의 배수일경우와 아닌경우로 나눠서 지정
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
		default: //혹시나 다른키 입력시 정면 모습으로 초기화 해줌
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
		case SDLK_UP://윗쪽 화살표입력시 아래 모습으로 텍스쳐 지정
			if ((60 - (SDL_GetTicks() / 1000 - time))<=30) //30초 남았을 때 부터 흉폭해져야 하므로 시간을 검사해준다.
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
		case SDLK_DOWN: //아래쪽 화살표입력시 위쪽 모습으로 텍스쳐 지정
			CT->mHeight = (LT + KEY_PRESS_SURFACE_UP)->mHeight;
			CT->mWidth = (LT + KEY_PRESS_SURFACE_UP)->mWidth;
			CT->mTexture = (LT + KEY_PRESS_SURFACE_UP)->mTexture;
			break;
		case SDLK_LEFT: //왼쪽 화살표입력시 오른쪽 모습으로 텍스쳐 지정
			if (time % 2 == 0) //애니메이션 효과를 위해 입력받은 time이 2의 배수일경우와 아닌경우로 나눠서 지정
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
		case SDLK_RIGHT: //오른쪽 화살표입력시 오른쪽 모습으로 텍스쳐 지정
			if (time % 2 == 0) //애니메이션 효과를 위해 입력받은 time이 2의 배수일경우와 아닌경우로 나눠서 지정
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

		default: //혹시나 다른키 입력시 정면 모습으로 초기화 해줌
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
	//충돌판정 사각형 변수, 각각 A사각형의 상,하,좌,우 / B사각형의 상,하,좌,우 변수를 생성하여 충돌하는지 검사함

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
	} //충돌할 시 false 리턴

	return true;
}
bool setTiles(_LTile *tiles) //맵파일 읽어서 타일셋팅
{
	bool loaded = true; //정상적으로 셋팅이 끝났는지 검사할 bool 대수 변수
	int x = 0, y = 0, i = 0; //x좌표,y좌표, 타일셋 핸들링용 변수

	char tempbuf[2500];
	char* map[2500] = { 0, };
	FILE *fp = fopen("level.map", "r"); //level.map 오픈해서

	fgets(tempbuf, sizeof(tempbuf), fp); //tempbuf에 저장 

	char *ptr = strtok(tempbuf, " "); //공백문자 기준으로 잘라서

	while (ptr != NULL)
	{
		map[i] = ptr;
		i++;
		ptr = strtok(NULL, " ");
	} //map 배열에 넣어준다.

	if (fp == NULL) //파일 포인터 예외 처리
	{
		printf("맵파일 로드 실패!\n");
		loaded = false;
	}
	else
	{
		for (int i = 0; i < TOTAL_TILES-1; i++)
		{
			int tileType = -1; //타일 타입 지정용 변수
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
			// map배열에서 각각의 원소와 문자열을 비교하여 무슨 타일인지 지정

			(tiles + i)->mBox.x = x;
			(tiles + i)->mBox.y = y;
			(tiles + i)->mBox.w = TILE_WIDTH;
			(tiles + i)->mBox.h = TILE_HEIGHT;
			(tiles + i)->mType = tileType;
			//타일 배열에 타일의 좌표,크기,타입 지정해서 저장

			printf(" tiles[%d] = %d\n", i, (tiles + i)->mType);

			x += TILE_WIDTH;
			if (x >= LEVEL_WIDTH)
			{
				x = 0;
				y += TILE_HEIGHT;
			}
			//타일 좌표 계산
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
		if (((tiles + i)->mType >= 6) && ((tiles + i)->mType <= 22)) // 벽 타일에 대해서(6번~22번 타일)
		{
			if (checkCollision(box, (tiles + i)->mBox)) 
			{
				return true; //충돌시 true 리턴
			}
		}
	}
	return false;
}
