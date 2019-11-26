#pragma once

enum KeyPressSurfaces //키입력 변수를 담는 열거형
{
	KEY_PRESS_SURFACE_DEFAULT, //=0
	KEY_PRESS_SURFACE_UP,      //=1
	KEY_PRESS_SURFACE_DOWN,    //=2
	KEY_PRESS_SURFACE_LEFT,    //=3
	KEY_PRESS_SURFACE_RIGHT,   //=4
	KEY_PRESS_SURFACE_O,       //=5
	KEY_PRESS_SURFACE_TOTAL,   //=6
};

typedef struct _LTexture       //텍스쳐 구조체
{
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;

}_LTexture;

typedef struct _LPlayer //플레이어 구조체
{
	int Player_WIDTH;
	int Player_HEIGHT;
	int Player_VEL;

	SDL_Rect mBox;
	int mVelX, mVelY;
}_LPlayer;

typedef struct _LTile
{
	SDL_Rect mBox;
	int mType;
}_LTile;

extern SDL_Renderer* gRenderer;     //랜더러 포인터
extern SDL_Surface* loadSurface(char* path); //경로에 있는 서피스 로드 함수
extern SDL_Texture* loadTexture(char* path); //위와 같은 기능 ( 서피스보다 성능 좋음)
extern SDL_Window* gWindow; //랜더링 윈도우 포인터
extern SDL_Surface* gScreenSurface; //스크린서피스 포인터
extern SDL_Texture* gTexture;       //기본 텍스쳐 포인터
extern SDL_Texture* gLetterbox ;     //왼쪽아래 메시지박스 포인터
extern SDL_Texture* gMinimap;       //오른쪽 아래 미니맵 포인터
extern TTF_Font* gFont;  //기본 폰트 포인터

extern const int LEVEL_WIDTH ;  //레벨층 가로
extern const int LEVEL_HEIGHT; //레벨층 세로

extern const int SCREEN_WIDTH ; //스크린 가로
extern const int SCREEN_HEIGHT ; //스크린 세로

extern const int SCREEN_FPS ;     //스크린 프레임
extern const int SCREEN_TICK_PER_FRAME ; //틱당 프레임

//타일 상수
extern const int TILE_WIDTH;//타일 가로크기
extern const int TILE_HEIGHT;//타일 세로크기
extern const int TOTAL_TILES;
extern const int TOTAL_TILE_SPRITES;

//텍스쳐 등 구현할 구조체들 정의

extern struct _LTexture gMainplayerTexture[KEY_PRESS_SURFACE_TOTAL+2]; //메인캐릭터 텍스쳐
extern struct _LTexture gDuckTexture[KEY_PRESS_SURFACE_TOTAL+2];
extern struct _LTexture gCurrentDuck;
extern struct _LTexture gTimeText;
extern struct _LTexture gCurrentTime;
extern struct _LTexture gSightLimiter;   //시야 가리기
extern struct _LTexture gTextTexture[2]; //택스트도 구조체 배열을 통하여 미리 집어넣어놓고, 이벤트에 따라서 꺼내어 랜더링 할 수 있다.
extern struct _LTexture gCurrentSurface; //현재 표시되는 서피스
extern struct _LTexture gCurrentText;    //현재 표시되는 텍스트
extern struct _LTexture gTileTexture[23];//타일셋 텍스처
extern struct _LTexture gStartPage;
extern struct _LTexture gLeaderBoard[5];
extern struct _LPlayer gPlayer;
extern struct _LPlayer gDuck[5];
extern struct _LTimer timer;


//함수 정의
bool init(); //초기화
bool loadMedia(_LTile*); //미디어 로딩
void close(); // SDL 라이브러리 닫기
bool loadFromFile(_LTexture*, SDL_Renderer*, char*);  //텍스쳐 불러오기
void lfree(_LTexture*); //닫기
void render(_LTexture* , SDL_Renderer*, int , int ); //지정된 좌표에 랜더링
bool loadFromRenderedText(_LTexture*, SDL_Renderer* , TTF_Font*, char* , SDL_Color); //텍스트 불러오기
void V_handleEvent(_LPlayer*, SDL_Event*);  //이벤트에 따른 속도조절
void reverse_V_handleEvent(_LPlayer*, SDL_Event*);  //이벤트에 따른 속도조절
void T_handleEvent(_LTexture* , _LTexture* , SDL_Event*,int ); //이벤트에 따른 텍스쳐 조절
void reverse_T_handleEvent(_LTexture*, _LTexture*, SDL_Event*, int); //이벤트에 따른 텍스쳐 조절
bool setTiles(_LTile *);
bool touchesWall(SDL_Rect , _LTile* );

//character
void move(_LPlayer* , _LTile *);
bool checkCollision(SDL_Rect , SDL_Rect ); //충돌판정
int getPosX(_LPlayer* );
int getPosY(_LPlayer* );
void setCamera(_LPlayer* , SDL_Rect* );