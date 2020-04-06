/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Context for flow analysis
 * History:
   <1> 3/28/2020 , create
************************************************************/

#ifndef _CONTEXT_H_
#define _CONTEXT_H_
#include <BasicType.h>
#include <Log.h>
#include <Packet.h>


using namespace std;

class User
{
private:

    DWORD m_TimeStamp;

public:
    User (ULONG SrcMac, WORD EthType)
    {

    }

    typedef struct 
    {
        bool operator()(User* L, User* R) 
        {
            return 0;
        }
    } EqualUser;
};

typedef std::set<User*, typename User::EqualUser> T_UsetSet;

class UserCtl
{
private:
    static T_UsetSet *m_UserSet;

public:
    UserCtl ()
    {
        if (m_UserSet == NULL)
        {
            m_UserSet = new T_UsetSet;
            assert (m_UserSet != NULL);
        }
    }

    VOID Release ()
    {
        if (m_UserSet != NULL)
        {
            delete m_UserSet;
            m_UserSet = NULL;
        }
        
    }

    User *Query(ULONG SrcMac, WORD EthType);
    User *Add(ULONG SrcMac, WORD EthType);
    VOID Delete(ULONG SrcMac, WORD EthType);

    inline static T_UsetSet::iterator begin()
    {
        return m_UserSet->begin();
    }

    inline static T_UsetSet::iterator end()
    {
        return m_UserSet->end();
    }

    inline static DWORD GetUserNum ()
    {
        if (m_UserSet == NULL)
        {
            return 0;
        }
        
        return m_UserSet->size();
    }
};


#endif 
