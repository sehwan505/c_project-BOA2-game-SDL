#pragma once


typedef struct _LTimer //타이머 구조체
{
	Uint32 mStartTicks;
	Uint32 mPauseTicks;
	bool mPaused;
	bool mStarted;
}_LTimer;


void timer_start(_LTimer*);
void timer_stop(_LTimer* );
void timer_pause(_LTimer* );
void timer_unpause(_LTimer*);
Uint32 getTicks(_LTimer*);
bool timer_isStarted(_LTimer*);
bool timer_isPaused(_LTimer*);