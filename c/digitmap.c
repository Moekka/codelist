/***************************************************************************
 *
 *  Module:         digitmap.c
 *
 *  Description:    check digit is invalid according to the digitmap
 *
 *  Author:         mar
 *
 *  Copyright 2001, Lucent Technologies, All rights reserved
 *
 *
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include "digitmap.h"

int degitFlag = FALSE;


#define DIGITDEBUG

#ifdef DIGITDEBUG
#define digitDbg(x) \
if(degitFlag) \
printf x
#else
#define digitDbg(x)
#endif

/*========================
处理后面有多个.的情况
=========================*/
int ifPerfectMatch(int seqIndex,DigMap_t *pDigMap,digitInfo_t *digitInfo)
{

  int current, flag = 0;

  current = digitInfo->currentIndex;

  while (1)
  {

    current = current + 1;
    if (pDigMap->digStr[seqIndex].digStrEl[current].bdot != TRUE)
      break;

    if (pDigMap->digStr[seqIndex].ucCntDigStrEl == current + 1)
    {

      flag = 1;
      break;

    }

  }

  if (flag == 1)
    return 1;
  else
    return 0;

}

/*===================================
查找下一个符合
seqIndex表示第几个digitmap
===================================*/
int findNextMacth(int seqIndex,char chIndex,char ch,DigMap_t *pDigMap,digitInfo_t *digitInfo)
{

  int current;
  DigPos_t *pDigPos;
  int flag = 0;

  if (pDigMap == NULL || digitInfo == NULL)
  {

    printf("findNextMacth:pdigmap is null!");
    return 0;

  }

  if (chIndex > 32)
  {

    printf("findNextMacth:failure0!");
    return 0;

  }

  if (seqIndex > MAX_DIGIT_STR)
  {

    printf("findNextMacth:failure1!");
    return 0;

  }

  current = digitInfo->currentIndex;


  while (1)
  {

    current = current + 1;

    if ( digitInfo->bIndex[chIndex][current] == 1)
      continue;

    pDigPos = &pDigMap->digStr[seqIndex].digStrEl[current].digPos;

    if (rgValidateDigPos(ch, pDigPos) == FAILURE)
    {

      if (pDigMap->digStr[seqIndex].digStrEl[current].bdot == TRUE)
      {

        if (pDigMap->digStr[seqIndex].ucCntDigStrEl == current + 1)
        {

          flag = 1;
          break;

        }
        current = current + 1;

      }
      else
      {

        flag = 0;
        break;

      }

    }
    else
    {

      if (digitInfo->bIndex[chIndex][current] == 0)
      {

        digitInfo->preIndex[chIndex][digitInfo->nextLen[chIndex]] = current;
        digitInfo->bIndex[chIndex][current] = 1;
        digitInfo->nextLen[chIndex]++;

      }

      if ( pDigMap->digStr[seqIndex].ucCntDigStrEl == current + 1)
      {

        flag = 1;
        break;

      }

    }

  }

  if (flag == 1)
    return 1;
  else
    return 0;


}

/*=========================
初始化dspinfo
==========================*/

int initDigitInfo(digitInfo_t *digitInfo)
{

  int i, j, k;

  digitInfo->currentIndex = 0;

  for (j = 0; j < 32; j++)
  {

    digitInfo->nextLen[j] = 0;
    for (k = 0; k < MAX_DIGIT_STR_ELEMENT; k++)
    {

      digitInfo->preIndex[j][k] = 0;
      digitInfo->bIndex[j][k] = 0;

    }

  }


  return 0;

}

/*==================================================
处理字符串是否和拨号规则匹配

phoneNo:拨号串

pDigMap:拨号规则

retryflag标志位，表示是否重新执行while循环
===================================================*/
int   processDigits(unsigned char *phoneNo, DigMap_t *pDigMap)
{

  unsigned char aucDigStr[MAX_DIGIT_STR];
  UCHAR  ucCnt = 0;
  int ii, num = 0;
  char ch;
  DigPos_t *pDigPos;
  digitInfo_t digitInfo;

  int len = 0;
  int retryflag;
  int maxStr;

  digitDbg(("processDigits::in\n\n"));

  if (phoneNo == NULL)
  {

    printf("phoneno is null!");
    return 0;

  }

  if (pDigMap == NULL)
  {

    printf("processDigits:digmap is null!");
    return 0;

  }

  memset(aucDigStr, 1, MAX_DIGIT_STR);

  initDigitInfo(&digitInfo);

  /*only  one digitmap or muti digitmap*/
  if (pDigMap->ucChoice == 0 || pDigMap->ucChoice == 1)
  {

    if (pDigMap->ucChoice == 0)
      maxStr = 1;
    else
      maxStr = pDigMap->ucCntDigStrs;

    /*查找每个规则，看看是否匹配*/
    for (ucCnt = 0; ucCnt < maxStr; ucCnt++)
    {

      digitDbg(("processDigits::test1::digitmap[%d]\n", ucCnt));
      ii = 0;
      for (;;)
      {
        /*根据拨号规则，拨号串是否匹配*/

        if (strlen(phoneNo) <= ii)
          break;

        digitDbg(("processDigits::test2::digit[%d]\n", ii));

        retryflag = 0;

        /*对于一个字符来说，看看有几个地方可以匹配*/
        while (1)
        {

          ch = phoneNo[ii];

          digitDbg(("processDigits::test3\n"));

          /*查找拨号规则的当前需匹配的位置*/
          if (ii == 0)
          {

            if (retryflag == 0)
              digitInfo.currentIndex = 0;
            else {

              if (pDigMap->digStr[ucCnt].digStrEl[digitInfo.currentIndex].bdot == TRUE)
              {

                digitInfo.currentIndex = digitInfo.currentIndex + 1;
                retryflag  = 0;
                digitDbg(("processDigits::test4\n"));

              }
              else
                break;

            }


          }
          else
          {


            if (0 < digitInfo.nextLen[ii - 1])
            {

              digitDbg(("processDigits::test5::choose next index\n"));

              digitInfo.currentIndex = digitInfo.preIndex[ii - 1][digitInfo.nextLen[ii - 1] - 1];

              if (pDigMap->digStr[ucCnt].digStrEl[digitInfo.currentIndex].bdot != TRUE)
                digitInfo.currentIndex = digitInfo.currentIndex + 1;

              digitInfo.nextLen[ii - 1] = digitInfo.nextLen[ii - 1] - 1;;

            }
            else
            {

              digitDbg(("processDigits::test6::next index =null::but dot\n"));

              if (pDigMap->digStr[ucCnt].digStrEl[digitInfo.currentIndex].bdot == TRUE)
              {

                if (retryflag == 1)
                {

                  digitInfo.currentIndex = digitInfo.currentIndex + 1;
                  retryflag  = 0;

                }
                else
                  break;

              }
              else
                break;

            }


          }


          pDigPos = &pDigMap->digStr[ucCnt].digStrEl[digitInfo.currentIndex].digPos;

          /*根据拨号规则判断字符是否合法*/

          if (rgValidateDigPos(ch, pDigPos) == FAILURE)
          {

            if (pDigMap->digStr[ucCnt].digStrEl[digitInfo.currentIndex].bdot == TRUE)
            {

              digitDbg(("processDigits::test7::invalid but have dot\n"));

              if (pDigMap->digStr[ucCnt].ucCntDigStrEl == digitInfo.currentIndex + 1)
                return PERFECT_MATCH;
              retryflag = 1;

            }
            else
            {

              if (ii > 0)

              {

                if (digitInfo.nextLen[ii - 1] < 1)
                {

                  aucDigStr[ucCnt] = 0;
                  digitDbg(("processDigits::test7-1:invalid ::find next index\n"));
                  break;

                }

              }

              /*第一个号码所先不符合第ucCnt的规则*/
              if (ii == 0)
              {

                aucDigStr[ucCnt] = 0;
                break;

              }

            }

          }
          else
          {

            digitDbg(("processDigits::test8::valid number\n"));

            /*判断是否完全匹配*/
            if (pDigMap->digStr[ucCnt].ucCntDigStrEl == digitInfo.currentIndex + 1)
              return PERFECT_MATCH;

            /*查看是否还有符合的*/
            if (pDigMap->digStr[ucCnt].digStrEl[digitInfo.currentIndex].bdot == TRUE)
            {

              ch = phoneNo[ii];
              if (findNextMacth(ucCnt, ii, ch, pDigMap, &digitInfo) == 1)
              {

                digitDbg(("processDigits::test9::perfect match\n"));
                return PERFECT_MATCH;

              }
              digitInfo.preIndex[ii][digitInfo.nextLen[ii]] = digitInfo.currentIndex;
              digitInfo.bIndex[ii][digitInfo.currentIndex] = 1;
              digitInfo.nextLen[ii] = digitInfo.nextLen[ii] + 1;

            }
            else
            {

              if (ifPerfectMatch(ucCnt, pDigMap, &digitInfo) == 1)
              {

                digitDbg(("processDigits::test10::perfect match\n"));
                return PERFECT_MATCH;

              }

              digitInfo.preIndex[ii][digitInfo.nextLen[ii]] = digitInfo.currentIndex;
              digitInfo.bIndex[ii][digitInfo.currentIndex] = 1;
              digitInfo.nextLen[ii] = digitInfo.nextLen[ii] + 1;

            }


          }

          if (ii == 0 && retryflag == 0)
            break;


        }

        if (aucDigStr[ucCnt] == 0)
          break;

        ii++;

      }

    }

    /*判断是否不匹配，或者部分匹配*/
    for (ucCnt = 0; ucCnt < maxStr; ucCnt++)
    {

      if (aucDigStr[ucCnt] == 1)
        break;

    }

    if (ucCnt == maxStr)
    {

      digitDbg(("processDigits::test11::not match\n"));
      return NOT_MATCH;

    }
    else
    {

      digitDbg(("processDigits::test12::partial match\n"));
      return PARTIAL_MATCH;

    }


  }
  else
  {
    /* if digit is not requested */

    len = strlen(phoneNo);

    /*如果没有digitmap，只要#或者是超时，
    我们就认为是完全匹配*/

    if (phoneNo[len - 1] == 'T' || phoneNo[len - 1] == '#')
      return PERFECT_MATCH;
    else
      return PARTIAL_MATCH;


  }


}


int  rgValidateDigPos(unsigned char ucDigit, DigPos_t *pDigPos)
{

  switch (pDigPos->ucChoice)
  {

  case 0:
    /* Check in the digit map letter */

    if (ucDigit != pDigPos->MapLtrs.ucDigMapLtr)
    {

      digitDbg((" rgValidateDigPos::test1\n"));
      return FAILURE;

    }
    else
    {

      digitDbg((" rgValidateDigPos::test1--1:::valid no\n"));

    }
    break;

  case 1:

    if (pDigPos->MapLtrs.DMRange.ucChoice == 0)
    {

      if (ucDigit > 57 || ucDigit < 48)
      {

        digitDbg((" rgValidateDigPos::test2\n"));
        return FAILURE;

      }
      else
      {

        digitDbg((" rgValidateDigPos::test2--2:::x::valid no\n"));

      }

    }
    else
    {

      /* If the range of digits specified */
      if (pDigPos->MapLtrs.DMRange.digLtr.ucChoice == 0)
      {

        if ((atoi(pDigPos->MapLtrs.DMRange.digLtr.aucLowDMLtr) < atoi(ucDigit) )
            || (atoi(pDigPos->MapLtrs.DMRange.digLtr.aucUpDMLtr) >
                atoi(ucDigit) ))
          return FAILURE;

      }

    }
    break;

  }

}

/*调试开关*/
int dmDbg(int flag)
{

  if (flag == 1)
    degitFlag = TRUE;
  else
    degitFlag = FALSE;
  return 0;

}
