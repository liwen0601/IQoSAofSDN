/***********************************************************
 * Author: Wen Li
 * Date  : 4/13/2020
 * Describe: command line for debug
 * History:
   <1> 4/13/2020 , create
************************************************************/

#ifndef _SHELL_H_
#define _SHELL_H_
#include <BasicType.h>
#include <Log.h>
#include <Context.h>

using namespace std;

class CmdShell;

class Cmd
{
protected:

CmdShell *m_Cs;

public:
    Cmd ()
    {
        m_Cs = NULL;
    }

    Cmd (CmdShell *Cs)
    {
        m_Cs = Cs;
    }

    virtual ~Cmd ()
    {
    }
public:
    inline string IP (DWORD addr)
    {
        struct in_addr Ip;
        
        Ip.s_addr = htonl (addr);
        return string(inet_ntoa(Ip));
    }
    
    virtual VOID Execute (ClassifyEngine *CfEngine) = 0;
};

typedef map<string, Cmd*> T_CmdMap;

class CmdShell
{
private:
    T_CmdMap m_CmdMap;
    ClassifyEngine *m_CfEngine;

private:
    VOID Init ();
    
public:
    CmdShell(ClassifyEngine *CfEngine)
    {
        m_CfEngine  = CfEngine;
        Init ();
    }

    ~CmdShell()
    {
        for (auto It = m_CmdMap.begin(); It != m_CmdMap.end(); It++)
        {
            delete It->second;
        }
    }

    T_CmdMap::iterator begin ()
    {
        return m_CmdMap.begin();
    }

    T_CmdMap::iterator end ()
    {
        return m_CmdMap.end();
    }

    VOID ExeCmd (string Cmd);
};



#endif 
