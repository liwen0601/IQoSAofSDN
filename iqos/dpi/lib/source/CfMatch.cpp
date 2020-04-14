/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2020
 * Describe: Classifier match
 * History:
   <1> 4/05/2020 , create
************************************************************/
#include "Log.h"
#include "Classifier.h"


bool Classifier::Match (T_Result* Prst, DWORD* StateNo)
{
    DWORD NxtStateNo = 0;
    
    State *CurState = GetState (*StateNo);
    if (CurState == NULL)
    {
        DebugLog ("State[%d] fetch fail...\r\n", *StateNo);
        return false;
    }

    for (auto It = CurState->begin(), End = CurState->end(); It != End; It++)
    {
        DWORD Pid = It->first;

        if (Prst->find (Pid) != Prst->end())
        {
            NxtStateNo = It->second;
            break;
        }
    }

    if (NxtStateNo == 0)
    {
        return false;
    }

    State *NxtS = GetState (NxtStateNo);
    assert (NxtS != NULL);

    *StateNo = NxtStateNo;

    return NxtS->IsFin ();
}



