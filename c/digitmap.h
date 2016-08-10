/***************************************************************************
 *
 *  Module:         digitmap.h
 *
 *  Description:    check digit is invalid according to the digitmap
 *
 *  Author:         mar
 *
 *  Copyright 2001, Lucent Technologies, All rights reserved
 *
 *
 ***************************************************************************/

#define PERFECT_MATCH  1
#define PARTIAL_MATCH 2
#define NOT_MATCH  3
#define MAX_DIGIT_STR_ELEMENT 32 
#define DM_LETTER_RANGE 32
#define MAX_DIGIT_STR 32
#define FALSE 0
#define TRUE 1
#define FAILURE -1


typedef unsigned char UCHAR; 
typedef char BOOL;

typedef struct digitInfo_s{
 
int currentIndex;
unsigned long nextLen[32];
int bIndex[32][MAX_DIGIT_STR_ELEMENT];  
int  preIndex[32][MAX_DIGIT_STR_ELEMENT]; 
 
}digitInfo_t; 
 
typedef struct
{
 
  UCHAR     ucChoice;  /*indicates whether seq.og DigitMap Letters or 
                         Range of DIGITS is present */
  UCHAR     aucLowDMLtr[DM_LETTER_RANGE]; //LowerLimit of DMLetter Range
  UCHAR     aucUpDMLtr[DM_LETTER_RANGE];  //UpperLimit of DMLetter Range
  UCHAR     ucCnt;     //Count of DigitMap Letters;
 
}DigLtr_t;
 
 
typedef struct
{
 
  UCHAR    ucChoice;  /*indicates whether all Digits are specified('x') or only 
                      a subset of them are specified in the DigitMapRange */
  DigLtr_t digLtr;
 
}DMRange_t;
 
typedef union
{
 
  UCHAR     ucDigMapLtr;
  DMRange_t DMRange;
 
}MapLtrs_u;
 
typedef struct
{
 
  UCHAR     ucChoice;  //indicates which of DigMapLetter,DigMapRange is present
  MapLtrs_u MapLtrs;
 
}DigPos_t;
 
typedef struct
{
  
  BOOL     bdot;  //Indicates whether '.' is part of StrElement
  DigPos_t digPos;
 
}DigStrEl_t;
 
typedef struct
{
 
  UCHAR      ucCntDigStrEl;   //Count of DigStringElements;
  DigStrEl_t digStrEl[MAX_DIGIT_STR_ELEMENT];        //Digit String Element
 
}DigStr_t; 
 
typedef struct
{
 
  UCHAR    ucChoice;       /*indicates which of DigitString or 
                              List of DigitStrings is present*/
  UCHAR    ucCntDigStrs;   //Number of DigitStrings
  DigStr_t digStr[MAX_DIGIT_STR];       //Digit String
 
}DigMap_t;
 
/*函数声明*/
 
int  rgValidateDigPos(unsigned char ucDigit,DigPos_t *pDigPos);