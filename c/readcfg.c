#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct tone
{
	char f1[10];
	char f2[10];
	char f3[10];
	char pwr1[10];
	char pwr2[10];
};

#define MAX_TOKEN (20)
#define MAX_CMD_LEN	256
void readcfg(char *path)
{
	FILE *fp;
	char strLine[128];
	struct tone t = {0};


	fp = fopen(path,"r");
	if(fp == NULL)
		return ;
	memset(strLine,0, sizeof(strLine));
	while (fgets(strLine, sizeof(strLine)-1, fp) != NULL) {
		strLine[strlen(strLine) - 1] = '\0';
		if(strstr(strLine,"#") != NULL){
			continue;
		}
		printf("line:%s\n", strLine);
		sscanf(strLine,"%[^,],%[^,],%[^,],%[^,],%s",t.f1,t.f2,t.f3,t.pwr1,t.pwr2);
		printf("%s,%s,%s,%s,[%s]\n",t.f1,t.f2,t.f3,t.pwr1,t.pwr2);

		// printf("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",t.f1,t.f2,t.f3,
		// 	t.pwr1,t.pwr2,t.pwr3,t.mak1,t.brk1,t.mak2,t.brk2,t.mak3,t.brk3);
		// memset(strLine,0, sizeof(strLine));
	}
}

int getTokens(const char *inStr, const char* delim, char *pArgv[]) {
	int numOfToken = 0;
	static char tmp[MAX_CMD_LEN] = {0};
	char *t;

	strcpy(tmp, inStr);
	t = strtok(tmp, delim);
	
	while(t && numOfToken < MAX_TOKEN) 	{
		pArgv[numOfToken++] = t;
		printf("num:%d\n", atoi(t));
		t = strtok(NULL, delim);
	}

	return numOfToken;
}

int main()
{
	const char * str = "2";
	char *param[MAX_TOKEN];
	int i;

	i = getTokens(str,",",param);
	printf("i:%d\n", i);
	return 0;
}