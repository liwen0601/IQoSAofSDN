/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2019
 * Describe: classifier
 * History:
   <1> 4/05/2019 , create
************************************************************/
#ifndef _CLASSIFIER_H_
#define _CLASSIFIER_H_
#include <BasicType.h>

using namespace std;

typedef map<DWORD, DWORD> T_IdMap;

class State
{
private:
    DWORD m_Sid;
    bool m_IsFin;
    T_IdMap m_Pid2Sid;
    
public:
    State (DWORD Sid, bool IsFin)
    {
        m_Sid = Sid;
        m_IsFin = IsFin;
    }

    ~State ()
    {
    }

    inline VOID AddNextState (DWORD Pattern, DWORD NxtSid)
    {
        m_Pid2Sid[Pattern] = NxtSid;
    }

    inline DWORD GetId ()
    {
        return m_Sid;
    }
};


typedef map<DWORD, State*> T_StateMap;

class Classifier
{
private:
    string m_Name;
    DWORD m_Id;
    DWORD m_Lv4Type;
    DWORD m_Port;
    
    DWORD m_CurState;
    
    T_StateMap m_StateMap;
    

private:
    VOID Init ();

public:
    Classifier (string Name, DWORD Id, DWORD Lv4Type, DWORD Port)
    {
        m_Name = Name;
        m_Id   = Id;
        m_Lv4Type = Lv4Type;
        m_Port = Port;
    }

    ~Classifier ()
    {
        for (auto it = m_StateMap.begin(); it != m_StateMap.end(); it++)
        {
            delete (it->second);
        }
        m_StateMap.clear ();
    }

    inline State* NewState (bool IsFin)
    {
        DWORD Sid = m_StateMap.size();
        
        State* S = new State (Sid, IsFin);
        assert (S != NULL);

        m_StateMap[Sid] = S;

        return S;
    }
};


typedef map<DWORD, Classifier*> T_Id2Classifier;
typedef map<DWORD, string> T_Id2Pattern;


class CfManage
{
private:
    T_Id2Classifier m_CfId2Cf;
    T_Id2Pattern m_Id2Pattern;

    T_Id2Classifier m_Ptn2Cf;
    
public:
    CfManage ()
    {
    }

    ~CfManage ()
    {
    }

    inline Classifier* NewClassifier (string Name, DWORD Lv4Type, DWORD Port=0)
    {
        DWORD CfId = m_CfId2Cf.size();
        
        Classifier* Cf = new Classifier (Name, CfId, Lv4Type, Port);
        assert (Cf != NULL);

        m_CfId2Cf[CfId] = Cf;

        return Cf;
    }

    inline DWORD NewPattern (string Pattern)
    {
        DWORD Pid = m_Id2Pattern.size();

        m_Id2Pattern[Pid] = Pattern;

        return Pid;
    }

    inline VOID Pattern2Classifier (DWORD Pid, Classifier* Cf)
    {
        m_Ptn2Cf[Pid] = Cf;
        return;
    }
    
    VOID Init ();
    
};



#endif 