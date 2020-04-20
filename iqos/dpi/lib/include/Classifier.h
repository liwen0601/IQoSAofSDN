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
#include <Packet.h>
#include <Wm.h>

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

    inline bool IsFin ()
    {
        return m_IsFin;
    }

    inline T_IdMap::iterator begin ()
    {
        return m_Pid2Sid.begin();
    }

    inline T_IdMap::iterator end ()
    {
        return m_Pid2Sid.end();
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

    inline State* GetState (DWORD Sid)
    {
        auto It = m_StateMap.find (Sid);
        if (It == m_StateMap.end())
        {
            return NULL;
        }

        return It->second;
    }

    inline string GetName ()
    {
        return m_Name;
    }

    inline DWORD GetId ()
    {
        return m_Id;
    }

    bool Match (T_Result* Prst, DWORD* StateNo, DWORD DstPort);
};


typedef map<DWORD, Classifier*> T_Id2Classifier;


class CfManage
{
private:
    T_Id2Classifier m_CfId2Cf;
    T_Pid2Pattern m_Id2Pattern;

    T_Id2Classifier m_Ptn2Cf;
    WmMatch *m_Wm;

private:
    VOID Dump ();
    
public:
    CfManage ()
    {
        Init ();
        
        m_Wm = new WmMatch (&m_Id2Pattern);
        assert (m_Wm != NULL);
    }

    ~CfManage ()
    {
    }


    inline Classifier* NewClassifier (string Name, DWORD Lv4Type, DWORD Port=0)
    {
        DWORD CfId = m_CfId2Cf.size() + CF_ID_BEGIN;
        
        Classifier* Cf = new Classifier (Name, CfId, Lv4Type, Port);
        assert (Cf != NULL);

        m_CfId2Cf[CfId] = Cf;

        return Cf;
    }

    inline Classifier* NewClassifier (DWORD Cfid, string Name, DWORD Lv4Type, DWORD Port=0)
    {
        DWORD CfId = Cfid;
        
        Classifier* Cf = new Classifier (Name, CfId, Lv4Type, Port);
        assert (Cf != NULL);

        m_CfId2Cf[CfId] = Cf;

        return Cf;
    }

    inline DWORD NewPattern (string Pattern)
    {
        DWORD Pid = m_Id2Pattern.size()+1;

        m_Id2Pattern[Pid] = Pattern;

        return Pid;
    }

    inline VOID MapPatternCf (DWORD Pid, Classifier* Cf)
    {
        m_Ptn2Cf[Pid] = Cf;
        return;
    }

    inline Classifier* GetCfByPid (DWORD Pid)
    {
        auto It = m_Ptn2Cf.find (Pid);
        assert (It != m_Ptn2Cf.end());

        return It->second;
    }

    
    inline T_Result* PatternMach (BYTE* Data, DWORD Length)
    {
        T_Result *Rst = m_Wm->Search (Data, Length);
    
        return Rst;
    }

    inline Classifier* GetCf (DWORD CfId)
    {
        auto It = m_CfId2Cf.find (CfId);
        assert (It != m_CfId2Cf.end());

        return It->second;
    }

    inline string GetCfName (DWORD CfId)
    {
        Classifier *Cf = GetCf (CfId);

        return Cf->GetName ();
    }
    
    VOID Init ();
    
};



#endif 