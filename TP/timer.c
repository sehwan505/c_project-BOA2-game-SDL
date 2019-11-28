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
	timer->mStartTicks = SDL_GetTicks();
	timer->mPauseTicks = 0;
}
void timer_stop(_LTimer* timer)
{
	timer->mStarted = false;
	timer->mPaused = false;
	timer->mStartTicks = 0;
	timer->mPauseTicks = 0;
}
void timer_pause(_LTimer* timer)
{
	if (timer->mStarted && !(timer->mPaused))
	{
		timer->mPaused = true;
		timer->mPauseTicks = SDL_GetTicks() - (timer->mStartTicks);
		timer->mStartTicks = 0;
	}
}
void timer_unpause(_LTimer* timer)
{
	if (timer->mStarted && timer->mPaused)
	{
		timer->mPaused = false;
		timer->mStartTicks = SDL_GetTicks() - (timer->mPauseTicks);
		timer->mPauseTicks = 0;

	}
}
Uint32 getTicks(_LTimer* timer)
{
	Uint32 time = 0;

	if (timer->mStarted)
	{
		if (timer->mPaused)
		{
			time = timer->mPauseTicks;
		}
		else
		{
			time = SDL_GetTicks() - timer->mStartTicks;
		}
	}
	return time;
}
bool timer_isStarted(_LTimer* timer)
{
	return timer->mStarted;
}
bool timer_isPaused(_LTimer* timer)
{
	return timer->mPaused && timer->mStarted;
}