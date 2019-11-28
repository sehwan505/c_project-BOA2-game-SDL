#include <SDL.h>                  //SDL lib
#include <SDL_image.h>            //SDL_image lib
#include <SDL_ttf.h>              //SDL ttf lib
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include "game.h"

/*
character.c
캐릭터와 관련된 함수들
-현재 좌표 반환
-물리법칙 관련(이동, 속도계산, 좌표계산)
-카메라 좌표 계산
*/

int getPosX(_LPlayer* LP) //현재 플레이어의 x좌표 반환
{
	return LP->mBox.x;
}
int getPosY(_LPlayer* LP) //현재 플레이어의 y좌표 반환
{
	return LP->mBox.y;
}
void V_handleEvent(_LPlayer* LP, SDL_Event* e) //키입력에 따른 좌표조절
{
	if (e->type == SDL_KEYDOWN && e->key.repeat == 0) //키를 눌렀을때
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY += LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX -= LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX += LP->Player_VEL; break;
		} //방향에 맞게 속도 조절
	}
	else if (e->type == SDL_KEYUP && e->key.repeat == 0) //키를 땠을때
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY += LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX += LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX -= LP->Player_VEL; break;
		} //방향에 맞게 역연산 해줌
	}
} //꾹 눌러도 계속 가도록 속도 개념을 도입하여 게산한다. 
void reverse_V_handleEvent(_LPlayer* LP, SDL_Event* e) //키입력에 따른 좌표조절(오리 -> 플레이어와 역방향)
{
	if (e->type == SDL_KEYDOWN && e->key.repeat == 0) //키를 눌렀을때
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY += LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX += LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX -= LP->Player_VEL; break;
		} //방향에 맞게 속도 조절
	}
	else if (e->type == SDL_KEYUP && e->key.repeat == 0) //키를 땠을때
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP: LP->mVelY -= LP->Player_VEL; break;
		case SDLK_DOWN: LP->mVelY += LP->Player_VEL; break;
		case SDLK_LEFT: LP->mVelX -= LP->Player_VEL; break;
		case SDLK_RIGHT: LP->mVelX += LP->Player_VEL; break;
		} //방향에 맞게 역연산 해줌
	}
} //꾹 눌러도 계속 가도록 속도 개념을 도입하여 게산한다. 
void move(_LPlayer* LP, _LTile* tiles) //플레이어 무브 & 충돌처리
{
	LP->mBox.x += LP->mVelX; //계산된 속도만큼 x좌표 계산

	if ((LP->mBox.x < 0) || (LP->mBox.x + LP->Player_WIDTH > LEVEL_WIDTH) || touchesWall(LP->mBox, tiles))
	{
		LP->mBox.x -= LP->mVelX;
	} //벽과 충돌하거나 맵밖으로 나가면 다시 빼준다.

	LP->mBox.y += LP->mVelY; //계산된 속도만큼 y좌표 계산

	if ((LP->mBox.y < 0) || (LP->mBox.y + LP->Player_HEIGHT > LEVEL_HEIGHT) || touchesWall(LP->mBox, tiles))
	{
		LP->mBox.y -= LP->mVelY;
	} //벽과 충돌하거나 맵밖으로 나가면 다시 빼준다.
}
void setCamera(_LPlayer* LP, SDL_Rect* camera) //카메라 셋팅
{
	camera->x = (getPosX(LP) + LP->Player_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera->y = (getPosY(LP) + LP->Player_HEIGHT / 2) - (SCREEN_HEIGHT * 2 / 3) / 2;
	//카메라 좌표 초기화

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
	//카메라 좌표가 레벨층 밖으로 나간다면 고정해줌
}