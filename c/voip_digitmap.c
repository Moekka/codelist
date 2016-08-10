#include <stdio.h>
#include <string.h>

typedef unsigned int UINT32;
typedef unsigned char BOOL;
#define MAX_SUBRULES 32
#define MAX_RULES    32
#define MAX_TOKEN    MAX_RULES
#define MAX_CMD_LEN  MAX_SUBRULES
#define DIGIT(a)      (1<<(a))
#define MAX_NUMBER  11
#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))
// #define DIGIT_POUND    10 //#
// #define DIGIT_STAR     11 //*
#define DIGIT_ADD     17 //+
// #define DIGIT_A      13
// #define DIGIT_B      14
// #define DIGIT_C      15
// #define DIGIT_D      16

int g_rules_count;
typedef enum {
	TONE_DTMF_1 = 1,
	TONE_DTMF_2,
	TONE_DTMF_3,
	TONE_DTMF_4,
	TONE_DTMF_5,
	TONE_DTMF_6,
	TONE_DTMF_7,
	TONE_DTMF_8,
	TONE_DTMF_9,
	TONE_DTMF_0,
	TONE_DTMF_STAR,
	TONE_DTMF_POUND,
	TONE_DTMF_A,
	TONE_DTMF_B,
	TONE_DTMF_C,
	TONE_DTMF_D,
	TONE_DIAL,
	TONE_RINGBACK,	
	TONE_BUSY,
	TONE_REORDER,
	TONE_SIT,
	TONE_CUSTOM_1,
	TONE_CUSTOM_2,
	TONE_CUSTOM_3,
	TONE_CUSTOM_4,
	TONE_CNG,
	TONE_CED,
	TONE_ANS,
	TONE_ANSAM,
	TONE_V21PREAMBLE,
	TONE_FAX_CM,
	TONE_FAX_TIMEOUT,
	TONE_VBD_FAX_END,
	TONE_ANSBAR,
	TONE_INVALID
} toneCode_e;

typedef struct DMAP_RULE
{
	struct DMAP_SUBRULE
	{
		UINT32 token;
		BOOL   required;
		BOOL   timer;
	}subrules[MAX_SUBRULES];
	char    rulestr[MAX_SUBRULES];
	UINT32 subrules_num;
	int     matchState;
	int   pri;
	BOOL valid;
	BOOL timer;
}dmap_rules_t;

typedef struct _match
{
	UINT32 match_mode;
	char   dec[32];
}match_mode_t;

dmap_rules_t rules[MAX_RULES] = {0};
match_mode_t match[] = {
	-1, "NOT_MATCH",
	1 , "MATCH",
	2 , "PART_MATCH"
};

char * findMatchMode(UINT32 mode)
{
	int i;

	for(i=0;i<ARRAY_SIZE(match);i++)
	{
		if(match[i].match_mode == mode){
			return match[i].dec;
		}
	}
	return NULL;
}

int getTokens(const char *inStr, const char* delim, char *pArgv[]) {

	int numOfToken = 0;
	static char tmp[MAX_CMD_LEN] = {0};
	char *t;

	strcpy(tmp, inStr);
	t = strtok(tmp, delim);
	
	while(t && numOfToken < MAX_TOKEN) 	{
		pArgv[numOfToken++] = t;
		t = strtok(NULL, delim);
	}

	return numOfToken;
}

char etocDtmf(int dtmf){
	char ch='\0';
	
	if((dtmf >= TONE_DTMF_1) && (dtmf <= TONE_DTMF_9)) {
		ch='1' + dtmf-1;//,d1....d9
	} else if(dtmf ==TONE_DTMF_0) {
		ch= '0';//*:ds
	} else if(dtmf == TONE_DTMF_STAR) {
		ch='*';//#:do
	} else if(dtmf == TONE_DTMF_POUND){
		ch='#';
	}else if(dtmf >=TONE_DTMF_A && dtmf<=TONE_DTMF_D){
		ch='A' + (dtmf - 13);//a~d
	}

	return ch;

}

UINT32 voipatoi(char ch)
{
	UINT32 num;

	if ('0' <= ch && ch <= '9'){
		return (ch - '0');
	}else{
		switch(ch){
		case '#':
		return TONE_DTMF_POUND;
		case '*':
		return TONE_DTMF_STAR;
		case '+':
		return DIGIT_ADD;
		case 'A':
		return TONE_DTMF_A;
		case 'B':
		return TONE_DTMF_B;
		case 'C':
		return TONE_DTMF_C;
		case 'D':
		return TONE_DTMF_D;
		default:
		return -1;
	}
	}
	
}

int rulesPretreat(char * rulesStr)
{
	char *param[MAX_RULES];
	int rules_count;
	int range = 0;
	int a,b;
	int i, j, k;
	char *p = NULL;
	char *pos = NULL;
	char *pos_after = NULL;

	rules_count = getTokens(rulesStr, "|", param);
	g_rules_count = rules_count;
	for (i = 0; i < rules_count; i++)
	{
		rules[i].valid = 1;
		p = param[i];
		printf("subrule-i:%s\n", p);
		strcpy(rules[i].rulestr, p);
		for(j=0; *p; p++,j++)
		{
			printf("->[j:%d]:%c\n", j, *p);
			if (('0' <= *p && *p <= '9') || ('A' <= *p && *p <= 'D')\
				|| (*p == '+') || (*p == '#') || (*p == '*'))
			{
				printf("digit:%d\n", voipatoi(*p));
				rules[i].subrules[j].token |= DIGIT(voipatoi(*p));
				rules[i].subrules[j].required = 1;
				rules[i].subrules[j].timer = 0;
				printf("token:%d\n", rules[i].subrules[j].token);
				continue;
			}else if(*p == 'X' || *p == 'x')
			{
				rules[i].subrules[j].token = 0x3FF;
				rules[i].subrules[j].required = 1;
				rules[i].subrules[j].timer = 0;
				printf("token:%d\n", rules[i].subrules[j].token);
				continue;
			}else if ((*p == '['))
			{
				if ((pos_after=strchr(p,']')) != NULL){
					for (p= ++p; p<pos_after; p++)
					{
						printf("pos:%c\n", *p);
						if (*p == 'T'){
							rules[i].subrules[j-1].timer = 1;
							rules[i].timer = 1;
							continue;
						}else if(*p == '-'){
							a = voipatoi(*(p-1));
							b = voipatoi(*(p+1));
							range = b - a;
							printf("range:%d-%d-%d\n", range, a, b);
							for (k=a+1; k<b; k++)
							{
								// printf("k:%d\n", k);
								rules[i].subrules[j].token |= DIGIT(k);
								// printf("range-tok:%d\n", rules[i].subrules[j].token);
							}
							rules[i].subrules[j].required = 1;
							rules[i].subrules[j].timer = 0;
							continue;
						}else{
							printf("range-ch:%d\n", DIGIT(voipatoi(*p)));
							rules[i].subrules[j].token |= DIGIT(voipatoi(*p));
							rules[i].subrules[j].required = 1;
							rules[i].subrules[j].timer = 0;
						}
					}
				}else{
					rules[i].valid = 0;
					printf("rule[%d]is invalid\n", i);
					break;
				}
			}else if (*p == 'T')
			{
				rules[i].subrules[j-1].timer = 1;
				rules[i].timer = 1;
				break;
			}else if (*p == '.')
			{
				rules[i].subrules[j].required = 0;
				rules[i].subrules[j].token = rules[i].subrules[j-1].token;
				printf("token:%d\n", rules[i].subrules[j].token);
				continue;
			}else{
				rules[i].valid = 0;
				printf("rule[%d]is invalid\n", i);
				break;
			}
			
			// printf("sub-rules-len:%d\n", j);
		}
		rules[i].subrules_num = j;
		printf("subrules_num:%d\n", rules[i].subrules_num);
	}

	return rules_count;
}

int currentValidRulesCount(dmap_rules_t *rules)
{
	int i;
	int count = 0;

	for (i = 0; i < g_rules_count; i++)
	{
		if (rules[i].valid == 1){
			count++;
		}
	}
	return count;
}

char digitsbuf[MAX_SUBRULES] ={0};
int digits_num = 0;

int voipDigitMapMatch(char *digits)
{
	int i, j, k=0;
	int currvalid = 0;
	int required_flag = 0;
	int ret = 0;

	for (i = 0; i < g_rules_count; i++)
	{
		if (rules[i].valid == 0){
			continue;
		}
		printf("================rules[%d][%s] begin=========\n", i, rules[i].rulestr);
		for(j=0, k=0; k<digits_num;)
		{
			// printf("subrule_num[%d]:%d\n", i, rules[i].subrules_num);
			printf("match-i:%d-j:%d-k:%d-token: %d-digit:%d\n", i,j,k,rules[i].subrules[j].token,
				voipatoi(digits[k]));
			printf("token&digit:%d\n", (rules[i].subrules[j].token)&(DIGIT(voipatoi(digits[k]))));

			if(((rules[i].subrules[j].token)&(DIGIT(voipatoi(digits[k])))))
			{

				if(rules[i].subrules[j+1].required == 0){
					printf("xxxx[%d][%d]\n", j, (rules[i].subrules_num-1));
					if((j+1) == (rules[i].subrules_num-1) && (rules[i].subrules[j+1].timer != 1)){
						ret = 1;
					}
					
				}

				if(rules[i].subrules[j].required == 0){
					if(rules[i].subrules[j].timer == 1){
						//start timer
						printf("start timer\n");
					}
					k++;
					continue;
				}

				if(rules[i].subrules[j].timer == 1){
					ret = 3;
					//start timer
					break;
				}
				j++;
				k++;
				if(digits_num > 10){
				ret = 1;
				break;
			}
				continue;
			}else
			{
				if(j == rules[i].subrules_num){
					printf("===match out of the ruiles->match error===\n");
					rules[i].valid = 0;
					ret = -1;
					break;//next rules
				}

				if(rules[i].subrules[j].required == 0){
					printf("0....[%d][%d]\n",j,k);
					if(j == (rules[i].subrules_num-1)){
						rules[i].valid = 0;
						ret = -1;
						break;//next rules
					}else{
						j++;
						continue;
					}
				}
				printf("===match in the ruiles->match error===\n");
				rules[i].valid = 0;
				ret = -1;
				break;//next rules

			}

		}

		if(ret == 0){
			if(j >= rules[i].subrules_num){
				ret = 1;
			}else{
				ret = 2;
			}
		}
		printf("invalid_ruiles[%d]:%d\n", i, currentValidRulesCount(rules));
		printf("================rules[%d] end=[%s]========\n\n", i, (findMatchMode(ret) == NULL)?"NULL":findMatchMode(ret));
	}

	return ret;
}

int main(int argc, char const *argv[])
{
	// char * rulesStr = "[*#][0-9][0-9*].#|**xx|*#xx#|##|*57*|*4[01]*00|#9|10xxx.|11[02479]|11[13568]x.|12[026789].|121xx|12[3-5]x.|168xxxxx|17[29]x.|19xx.|20[01]|400[16-9]xxxxxx|600x.|800xxxxxxx|9xxxx.|70*xxxx.|71*|72*xxxxxxx|16[0-79]xxxx|9669xxxxxxx.|[2-8]xxxxxx|1[34578]xxxxxxxxx|01[34578]xxxxxxxxx|010xxxxxxxx.|02xxxxxxxxx.|xxx|xxxx|0[3-9]xxxxxxxxx.";
	// char * rules = "[*#][0-9][0-9*].#|**xx|*#xx#|##|*57*|*4[01]*00|#9|10xxx.";
	char * rulesStr = "x.";//400[16-9]xxxxxx|[2-8]xxxxxx|xxx
	int dtmf;
	int ret = 2;
	char lastRecv[32]={0};
	char tmp = '\0';

	rulesPretreat(rulesStr);
	while(1)
	{
		printf("input dtmf:");
		scanf("%d", &dtmf);
		memcpy(lastRecv,digitsbuf,32);
		tmp = etocDtmf(dtmf);
		sprintf(digitsbuf,"%s%c",lastRecv,tmp);
		memset(lastRecv, 0, sizeof(lastRecv));
		printf("digitsbuf:%s\n", digitsbuf);
		// sprintf(digitsbuf, "%c", etocDtmf(dtmf));

		digits_num++;
		ret = voipDigitMapMatch(digitsbuf);
		// switch(ret)
		// {
		// 	case 1:
		// 	printf("MATCH\n");
		// 	break;
		// 	case -1:
		// 	printf("NOT MATCH\n");
		// 	break;
		// 	case 2:
		// 	printf("PART MATCH\n");
		// 	break;
		// 	default:
		// 	printf("Error\n");
		// 	break;
		// }
	}

	return 0;
}