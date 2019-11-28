#include <SDL.h>                  //SDL lib
#include <SDL_image.h>            //SDL_image lib
#include <SDL_ttf.h>              //SDL ttf lib
#include <stdio.h>                //basic C lib
#include <stdbool.h>              //use bool fuctions
#include <stdlib.h>               //basic C lib
#include <stdint.h>               //use uint32_t
#include "game.h"

/*
score.c
스코어 파일 입출력관련 함수들
-Input
-Output
-Sort
*/

void fileInput(int inp) { //스코어 파일 불러서 저장
	FILE* fp;
	fp = fopen("score.txt", "a");

	fprintf(fp, " %d", inp);

	fclose(fp);
}
void fileRead(char* buffer) //스코어 파일 읽어오기
{
	char tempbuf[120];
	FILE* fp = fopen("score.txt", "r");

	fgets(tempbuf, sizeof(tempbuf), fp);

	fclose(fp);
	strcpy(buffer, tempbuf);

}
int refToken(char* buf[], char* inp[]) //파일 읽어서 공백문자 단위로 잘라서 다른 배열에 넘기는 함수
{
	int i = 0;
	char* ptr = strtok(buf, " ");

	while (ptr != NULL)
	{
		inp[i] = ptr;
		i++;
		ptr = strtok(NULL, " ");
	}
	return i; //배열의 원소 갯수(몇번 잘랐는지) 반환함
}
void selectionSort(char* pArr[], int num) //선택정렬(스코어 정렬용)
{
	int temp[1024];
	for (int j = 0; j < num; j++) {
		temp[j] = atoi(*(pArr + j));
	}
	for (int i = 0; i < num; i++)
	{
		if (temp[i] == NULL)
			return;
		int n = i;
		for (int j = (i + 1); j < num; j++)
		{
			if (temp[j] > temp[n])
				n = j;
		}
		SWAP((temp + i), (temp + n));
	}
	for (int k = 0; k < num; k++) {
		sprintf(*(pArr + k), "%d", *(temp + k));
	} //int로 만들어서 sort했으므로 텍스트로 랜더링 하기 위해서 다시 string으로 바꾸어준다
}
void SWAP(int* pa, int* pb) //sort용 자리바꾸기 함수
{
	int temp;
	temp = *pa;
	*pa = *pb;
	*pb = temp;
}
