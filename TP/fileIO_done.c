#include <stdio.h>
#include <string.h>

void fileRead(char* buffer);
int refToken(char* buf[], char *inp[]);
void selectionSort(int *pArr, int num);
void SWAP(int *pa, int *pb);

int main()
{
	char buffer[12000] = { 0, };
	char *temp[12000] = { 0, };
	
	fileRead(buffer);

	printf("string index : %d", refToken(buffer, temp));
	
}

void fileRead(char* buffer)
{
	char tempbuf[12000];
	FILE *fp = fopen("level.map", "r");

	fgets(tempbuf, sizeof(tempbuf), fp);

	printf("%s\n", tempbuf);
	fclose(fp);
	strcpy(buffer, tempbuf);

}
int refToken(char* buf[], char *inp[])
{
	int i = 0;
	char *ptr = strtok(buf, " ");


	while (ptr != NULL)
	{
		inp[i] = ptr;
		i++;
		ptr = strtok(NULL, " ");
	}


	for (int i = 0; i < 12000; i++)
	{
		if (inp[i] != NULL)
			printf("%s\n", inp[i]);
	}
	printf("\n\n\n");

	if (!strcmp("02",inp[2]))
	{
		int temp = 2;
		printf("%d\n", temp);
	}


	return i;
}
void selectionSort(int *pArr, int num)
{
	for (int i = 0; i < num; i++)
	{
		int n = i;
		for (int j = (i + 1); j < num; j++)
		{
			if (*(pArr + j) < *(pArr + n))
				n = j;
		}
		SWAP((pArr + i), (pArr + n));
	}
}

void SWAP(int *pa, int *pb)
{
	int temp;
	temp = *pa;
	*pa = *pb;
	*pb = temp;
}