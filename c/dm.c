
#include <stdio.h>
#include <string.h>

#include "dm.h"



int degitFlag = FALSE;
int g_rules_count;

char digitsbuf[MAX_SUBRULES_NUM] ={0};
int digits_num = 0;
dmap_rules_t *normalRules = NULL;
dmap_rules_t *specialRules = NULL;
dmap_rules_t *currentRules = NULL;

digitmap_t *normalDigitMap = NULL;
digitmap_t *specialDigitMap = NULL;

int normalDMapNum = 0;
int specialDMapNum = 0;


char dmapetocDtmf(int dtmf){
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

uint32 voipatoi(char ch)
{
	uint32 num;

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

int voipDigitMapGetSize()
{
    return g_rules_count;
}

void setCurrentDM(dmap_rules_t * rules)
{
    currentRules = rules;
}

dmap_rules_t * getCurrentDM()
{
    return currentRules;
}

int dmRulesPretreatment(dmap_rules_t *rules)
{
	int range = 0;
    int rules_count = 0;
    char *param[MAX_RULES_NUM] = {0};
	int a,b;
	int i, j, k;
	char *p = NULL;
	char *pos = NULL;
	char *pos_after = NULL;

	for (i = 0; i < voipDigitMapGetSize(); i++)
	{
		rules[i].valid = 1;
		p = rules[i].rulestr;
		printf("subrule-i:%s\n", p);
        
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
                continue;
//				break;
			}else if (*p == '.')
			{
				rules[i].subrules[j].required = 0;
				rules[i].subrules[j].token = rules[i].subrules[j-1].token;
				continue;
			}else{
				rules[i].valid = 0;
				printf("rule[%d]is invalid\n", i);
				break;
			}
			rules[i].subrules[j].rulesLen = j+1;
			// printf("sub-rules-len:%d\n", j);
		}
		rules[i].subrules_num = j;
		printf("subrulesLen:%d\n", rules[i].subrules_num);
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

char *dmDigitMapGetElem(char *dm, uint32 num)
{
    char *param[MAX_RULES_NUM] = {0};
    
    getTokens(dm, "|", param);
    
    return param[num];
}

int dmDigitPositionIsMultiple(subrules_t *patIt)
{
    return ((patIt->required) ? 0 : 1);
}

int dmDigitPositionMatchEvent(subrules_t *patIt, char elem)
{
    return (((patIt->token)&(DIGIT(voipatoi(elem)))) ? 1 : 0);
}
    
int dmDigitStringMatchSection(subrules_t *patIt, subrules_t *patEnd, char *strIt, char *strEnd, uint32 *timerToUse)
{
    int m0=0,m1=0;
    
    if(strIt == strEnd)
        if(patIt == patEnd)
            return DIGITSTRING_MATCHED;

    if(dmDigitPositionIsMultiple(patIt) && ((patIt + 1) == patEnd))
        return DIGITSTRING_MATCHEDANDWAITING;

    if(patIt == patEnd)
        return DIGITSTRING_UNMATCHED;

    if(patIt->timer == 1){
        *timerToUse = patIt->timer;
    }

    if(dmDigitPositionIsMultiple(patIt)){
       m0 = dmDigitStringMatchSection(patIt + 1, patEnd, strIt, strEnd, timerToUse);
       if(m0 == DIGITSTRING_MATCHED)
        return DIGITSTRING_MATCHED;

       m1 = dmDigitPositionMatchEvent(patIt, *strIt) ? \
            dmDigitStringMatchSection(patIt, patEnd, strIt + 1, strEnd, timerToUse) : \
            DIGITSTRING_UNMATCHED;
       return m0>m1 ? m0 : m1;
    }

    if(dmDigitPositionMatchEvent(patIt, *strIt)){
        return dmDigitStringMatchSection(patIt + 1, patEnd, strIt + 1, strEnd,timerToUse);
    }

    return DIGITSTRING_UNMATCHED;
}

int dmDigitStringMatch(subrules_t *subrules, char *dialString, uint32 *mode)
{
    int subrulesLen = 0;
    int strLen = 0;
    
    subrulesLen = subrules->rulesLen;
    strLen = strlen(dialString);
    
    return dmDigitStringMatchSection(subrules, &subrules[subrulesLen-1], dialString, &dialString[strLen-1], mode);
}

int dmDigitMapMatch(char *digits, int len, dmap_rules_t *rules, uint32 *timerDuration)
{
	int i, j, k=0;
	int currvalid = 0;
	int required_flag = 0;
	int ret = 0;
    int numMatched = 0;
    int numWaiting = 0;
    int numPossible = 0;
    int matchType = 0;
    int mode = 0;
    int timerToUse;
    
	for (i = 0; i < voipDigitMapGetSize(); i++)
	{
		if (rules[i].valid == 0){
			continue;
		}
        switch(dmDigitStringMatch(rules[i].subrules, digits, &mode))
        {
            case DIGITSTRING_MATCHED:
                numMatched++;
                break;
                
            case DIGITSTRING_MATCHEDANDWAITING:
                numWaiting++;
                if(mode != DIGITPOSITION_NOCHANGE)
                    timerToUse = mode;
                break;
                
            case DIGITSTRING_MATCHPOSSIBLE:
                numPossible++;
                if(mode != DIGITPOSITION_NOCHANGE)
                    timerToUse = mode;
                break;
                
            case DIGITSTRING_UNMATCHED:
                rules[i].valid = 1;
                //nothing to do
                break;
        }
		printf("invalid_ruiles[%d]:%d\n", i, currentValidRulesCount(rules));
		printf("================rules[%d] end=[%d]========\n", i, ret);
	}

    switch (numMatched)
    {
        case 0:
            matchType = numWaiting ? DIGITMAP_FULLMATCH: \
                        numPossible ? DIGITMAP_PARTIALMATCH: \
                        DIGITMAP_NOMATCH;
            break;
        case 1:
            matchType = (numWaiting || numPossible) ? DIGITMAP_FULLMATCH : DIGITMAP_UNAMBIGUOUSMATCH;
            break;
        default:
            matchType = DIGITMAP_FULLMATCH;
            
    }

    switch (matchType)
    {
        case DIGITMAP_PARTIALMATCH :
            *timerDuration = (timerToUse ==DIGITPOSITION_LONGTIMER) ? \
                               normalDigitMap->longTimer : normalDigitMap->shortTimer;
            break;
        case DIGITMAP_FULLMATCH :
            
            break;
        case DIGITMAP_NOMATCH :
        case DIGITMAP_UNAMBIGUOUSMATCH :
            *timerDuration = 0;
            break;
            
    }
	return matchType;
}

int voipMatchDigitMap(char *digits,uint32 *timerDuration)
{
    int i,j;
    int len = 0;

    len = strlen(digits);
    printf("=====================[%s]\n", digits);
    for(i=0;i<normalDMapNum;i++)
    {
        printf("Start Normal DigitMap[%d]...\n", i);
        dmDigitMapMatch(digits, len, normalRules, timerDuration);
    }

    for(j=0;j<specialDMapNum;j++)
    {
        printf("Start Special DigitMap[%d]...\n", j);
        dmDigitMapMatch(digits, len, specialRules, timerDuration);
    }
    
}

void voipRulesInit(char *dm, dmap_rules_t **rule)
{
    int rules_count = 0;
    char *param[MAX_RULES_NUM] = {0};
    dmap_rules_t *ruletmp = NULL;
    int i;

    rules_count = getTokens(dm, "|", param);
    ruletmp = (dmap_rules_t *)pj_pool_calloc(app_config.pool , rules_count , sizeof(dmap_rules_t));
    for(i=0;i<rules_count;i++)
    {
        strcpy(ruletmp[i].rulestr,param[i]);
    }
    *rule = ruletmp;
    g_rules_count = rules_count;
    
    return ;
}

void voipDigitMapInit()
{
    char matchMode[4] = {0};
    char normalEnable[4] = {0};
    char specialEnable[4] = {0};
    digitmap_t * ndm = NULL;
    digitmap_t * sdm = NULL;

    printf("DigitMapInit...\n");
    if(0==tcapi_get("VoIPDigitMap_Entry","DigitMapMatchMode",normalEnable)&& (strcmp(normalEnable,"1") == 0))
    {
        ndm = (digitmap_t *)pj_pool_calloc(app_config.pool , 1 , sizeof(digitmap_t));
        if(ndm == NULL){
            printf("accloc memory failed\n");
            return ;
        }
        tcapi_get("VoIPDigitMap_Entry","DigitMapMatchMode",matchMode);
        ndm->matchMode = atoi(matchMode);

        tcapi_get("VoIPDigitMap_Entry","DigitMap1",ndm->digitMap);
        if(strcmp(ndm->digitMap, "") == 0){
            strcpy(ndm->digitMap,DEFAULT_DM);
        }
//        printf("----->dm:%s\n", ndm->digitMap);
        ndm->dmType = DM_NORMAL;
        voipRulesInit(ndm->digitMap, &normalRules);
        normalDigitMap = ndm;
        normalDMapNum++;
        dmRulesPretreatment(normalRules);
    }
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

		ret = voipDigitMapMatch(digitsbuf);
	}

	return 0;
}