
#define PERFECT_MATCH   1
#define PARTIAL_MATCH   2
#define TIMEOUT_MATCH   3
#define MATCHEDANDWAITING  4
#define NOT_MATCH      5
#define MAX_DIGIT_STR_ELEMENT 32 
#define DM_LETTER_RANGE 3
#define MAX_DIGIT_STR   2
#define DM_FALSE 0
#define DM_TRUE  1
#define DM_FAILURE -1

#define MAX_SUBRULES_NUM 32
#define MAX_RULES_NUM    100
#define DIGIT(a)         (1<<(a))
#define MAX_NUMBER       11
#define DIGIT_ADD        17 //+
#define DEFAULT_DM       "x.T"

#define TIMEOUT_T            //long timer
#define TIMEOUT_S            //start timer
#define TIMEOUT_L            //short timer
#define MAX_DIGITMAP_LEN 4096

typedef unsigned char UCHAR; 
typedef char BOOL;

enum _DIGITSTRMATCH{
    DIGITSTRING_UNMATCHED = -1,
    DIGITSTRING_MATCHED = 1,
    DIGITSTRING_MATCHEDANDWAITING,
    DIGITSTRING_MATCHPOSSIBLE
};

enum _DMMATCH{
    DIGITMAP_NOMATCH = -1,
    DIGITMAP_FULLMATCH = 1,
    DIGITMAP_PARTIALMATCH,
    DIGITMAP_UNAMBIGUOUSMATCH
};

enum _DMTIMER{
    DIGITPOSITION_NOCHANGE,
    DIGITPOSITION_SHORTTIMER,
    DIGITPOSITION_LONGTIMER
};

enum _dmType{
    DM_NORMAL = 1,
    DM_SPECIAL
};

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
    TONE_DTMF_STAR,//11
    TONE_DTMF_POUND,//12
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

typedef struct DMAP_SUBRULE
{
	uint32   token;
	uint32   required;
	uint32   timer;
    uint32   rulesLen;
}subrules_t;

typedef struct DMAP_RULE
{
	subrules_t subrules[MAX_SUBRULES_NUM];
	char    rulestr[MAX_SUBRULES_NUM];
	uint32  subrules_num;
	int     matchState;
	int     pri;
	uint32  valid;
	uint32  timer;
}dmap_rules_t;

typedef struct _DMAP
{ 
    char   digitMap[MAX_DIGITMAP_LEN];
    uint32 matchMode;
    uint32 dialTimeOut;
    uint32 digitTimeOut;
    uint32 startTimer;
    uint32 shortTimer;
    uint32 longTimer;
    uint32 dmType;
}digitmap_t;

void voipDigitMapInit();
int voipMatchDigitMap(char *digits,uint32 *timerDuration);


