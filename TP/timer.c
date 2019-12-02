#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include <SDL.h>  
#include "timer.h"

/*
timer.c
fps 제어용 타이머 관련 함수
-타이머 시작
-타이머 종료
-타이머 일시정지
-타이머 일시정지 해제
-현재시각
*/

void timer_start(_LTimer* timer)
{
	timer->mStarted = true;
	timer->mPaused = false;
	timer->mStartTicks = SDL_GetTicks();//SDL라이브러리가 초기화 된 후 부터의 시간(ms) 리턴
	timer->mPauseTicks = 0;
}//타이머를 시작시킴
void timer_stop(_LTimer* timer)
{
	timer->mStarted = false;
	timer->mPaused = false;
	timer->mStartTicks = 0;
	timer->mPauseTicks = 0;
}//타이머를 모두 초기화함
void timer_pause(_LTimer* timer)
{
	if (timer->mStarted && !(timer->mPaused)) //타이머가 시작됐거나 멈춤상태가 아니라면
	{
		timer->mPaused = true; //멈춤상태로 만들고
		timer->mPauseTicks = SDL_GetTicks() - (timer->mStartTicks); //일시정지된 시각을 저장
		timer->mStartTicks = 0;
	}
}
void timer_unpause(_LTimer* timer) 
{
	if (timer->mStarted && timer->mPaused) //타이머가 시작됐거나 멈춤상태라면
	{
		timer->mPaused = false; 
		timer->mStartTicks = SDL_GetTicks() - (timer->mPauseTicks); //일시정지된 시각을 다시 불러옴
		timer->mPauseTicks = 0;

	}
}
Uint32 getTicks(_LTimer* timer)
{
	Uint32 time = 0;

	if (timer->mStarted) //타이머가 시작된 상태라면
	{
		if (timer->mPaused) //타이머가 멈췄다면
		{
			time = timer->mPauseTicks; //멈춘 시각을 리턴
		}
		else
		{
			time = SDL_GetTicks() - timer->mStartTicks; //타이머가 멈춘상태가 아니라면 타이머가 가진 시각을 리턴
		}
	}
	return time;
} //SDL 기본함수인 SDL_GetTicks를 응용하여 타이머 구조체에 맞도록 설계
bool timer_isStarted(_LTimer* timer)
{
	return timer->mStarted; //시작여부 리턴
}
bool timer_isPaused(_LTimer* timer)
{
	return timer->mPaused && timer->mStarted; //멈춤여부 리턴
}