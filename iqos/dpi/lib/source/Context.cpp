/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Context for flow analysis
 * History:
   <1> 3/28/2020 , create
************************************************************/


#include "Context.h"

T_UsetSet *UserCtl::m_UserSet = NULL;

User *UserCtl::Query(ULONG SrcMac, WORD EthType)
{
    User Q (SrcMac, EthType);
    T_UsetSet::iterator iter = m_UserSet->find (&Q);
    if (iter != m_UserSet->end())
    {
        return *iter;
    }

    return NULL;
}


User *UserCtl::Add(ULONG SrcMac, WORD EthType)
{
    User *A = new User(SrcMac, EthType);
    assert (A != NULL);

    if (m_UserSet->insert(A).second)
    {
        return A;
    }

    delete A;
    return NULL;
    
}

VOID UserCtl::Delete(ULONG SrcMac, WORD EthType)
{
    User Q (SrcMac, EthType);

    m_UserSet->erase(&Q);

    return;
}




