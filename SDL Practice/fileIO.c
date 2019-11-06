#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

void INPUT();
void fileRead();
int retToken(char* buf[], char* inp);

int main() {
	fileRead();
	return 0;
}
void INPUT() {
	FILE* fp = fopen("test.txt", "wt");

	if (fp == NULL) {
		printf("파일 입출력 오류!");
		exit(0);
	}
	fputs("Hello, world!", fp);

	fclose(fp);

	return;
}
void fileREAD() {
	char buffer[20];  
	int buf_size = MAX * MAX;
	int index, data;

	char* inp;

	FILE* fp;
	if ((fp = fopen("test.txt", "r")) == NULL) {
		printf("읽기 실패!\n");
		exit(0);
	}

	fp = fopen("test.txt", "r");
	inp = malloc(buf_size);
	while (fgets(inp, buf_size, fp)) {
		index = retToken(buffer, inp);
	}

	for (data = 0; data < index; data++) {

		printf("%s \n", buffer[data]);

	}

	fclose(fp);

}
int retToken(char* buf[], char* inp) {

	int i = 0;

	char* ptr = strtok(inp, ";");

	while (ptr != NULL) {
		buf[i] = ptr;
		ptr = strtok(NULL, ";");
		i++;
	}

	return i;

}