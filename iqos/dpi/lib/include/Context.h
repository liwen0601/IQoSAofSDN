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
#include <Classifier.h>
#include <PakcetSet.h>


using namespace std;


class CfCtxt
{
public:
    DWORD m_CurState;
};

typedef map<Classifier*, CfCtxt> T_Cf2Ctext;
typedef set<Classifier*> T_CfSet;


class Flow
{
public:
    DWORD m_SrcIp;
    DWORD m_DstIp;
    WORD  m_SrcPort;
    WORD  m_DstPort;
    DWORD m_ProtoType;

private:
    T_Cf2Ctext m_Cf2Text;
    DWORD m_CfId;

public:
    Flow (DWORD SrcIp, DWORD DstIp, WORD SrcPort, WORD DstPort, DWORD ProtoType)
    {
        m_SrcIp = SrcIp;
        m_DstIp = DstIp;
        m_SrcPort = SrcPort;
        m_DstPort = DstPort;
        m_ProtoType = ProtoType;

        m_CfId = 0;
    }

    ~Flow ()
    {
    }

    inline CfCtxt* GetCfCtext (Classifier *Cf)
    {
        auto It = m_Cf2Text.find (Cf);
        if (It == m_Cf2Text.end())
        {
            m_Cf2Text[Cf] = CfCtxt ();            
        }
        
        return &(m_Cf2Text[Cf]);    
    }

    inline T_Cf2Ctext::iterator CfBegin ()
    {
        return m_Cf2Text.begin();
    }

    inline T_Cf2Ctext::iterator CfEnd ()
    {
        return m_Cf2Text.end();
    }

    inline DWORD GetCfId ()
    {
        return m_CfId;
    }

    inline VOID SetCfId (DWORD Cfid)
    {
        m_CfId = Cfid;
    }

    typedef struct 
    {
        bool operator()(Flow L, Flow R) 
        {
            if (L.m_SrcIp != R.m_SrcIp)
            {
                return (L.m_SrcIp < R.m_SrcIp);
            }
            else if (L.m_DstIp != R.m_DstIp)
            {
                return (L.m_DstIp < R.m_DstIp);
            }
            else if (L.m_SrcPort != R.m_SrcPort)
            {
                return (L.m_SrcPort < R.m_SrcPort);
            }
            else if (L.m_DstPort != R.m_DstPort)
            {
                return (L.m_DstPort < R.m_DstPort);
            }
            else
            {
                return (L.m_ProtoType < R.m_ProtoType);
            }
        }
    } EqualFlow; 
};

typedef set<Flow, typename Flow::EqualFlow> T_FlowSet;

class User
{
public:
    DWORD m_Ipaddr;
    
private:
    T_FlowSet m_FlowSet;
    DWORD m_TimeStamp;

public:
    User (DWORD IpAddr)
    {
        m_Ipaddr = IpAddr;
    }

    inline DWORD GetIpAddr ()
    {
        return m_Ipaddr;
    }

    inline Flow* AddFlow (Flow F)
    {
        auto It = m_FlowSet.insert (F);
        if (It.second == false)
        {
            return NULL;
        }

        //DebugLog ("Add flow-Pro: %d Src: %u-%u, Dst: %u-%u\r\n", F.m_ProtoType, F.m_SrcIp, F.m_SrcPort, F.m_DstIp, F.m_DstPort);
        return (Flow*)(&(*It.first));        
    }

    inline Flow* GetFlow (Flow F)
    {
        auto It = m_FlowSet.find (F);
        if (It != m_FlowSet.end())
        {
            return (Flow*)(&(*It));
        }

        return AddFlow (F);        
    }

    inline T_FlowSet* GetFlowSet ()
    {
        return &m_FlowSet;        
    }

    typedef struct 
    {
        bool operator()(User L, User R) 
        {
            if (L.m_Ipaddr != R.m_Ipaddr)
            {
                return (L.m_Ipaddr < R.m_Ipaddr);
            }
            
            return 0;
        }
    } EqualUser;
};


typedef set<User, typename User::EqualUser> T_UsetSet;

class ClassifyEngine
{
private:
    T_UsetSet m_UserSet;
    CfManage *m_CfMng;
    PacketSet *m_PacketSet;

private:
    inline User *AddUser(User U)
    {
        auto It = m_UserSet.insert (U);
        if (It.second == false)
        {
            return NULL;
        }

        return (User *)(&(*It.first));    
    }
    
    inline User* GetUser (User U)
    {
        auto It = m_UserSet.find (U);
        if (It != m_UserSet.end())
        {
            return (User *)(&(*It));
        }

        return AddUser (U);        
    }

    inline DWORD GetUserCount ()
    {
        return m_UserSet.size();
    }

    inline Flow* QueryFlow (IpPacket *Pkt)
    {
        DWORD CfId;
        
        User *Uctxt = GetUser (User (Pkt->m_SrcIp));
        assert (Uctxt != NULL);

        Flow *Fctxt = Uctxt->GetFlow (Flow(Pkt->m_SrcIp, Pkt->m_DstIp, 
                                      Pkt->m_SrcPort, Pkt->m_DstPort, 
                                      Pkt->m_ProtoType)
                                     );
        assert (Fctxt != NULL);

        return Fctxt;
    }

    inline DWORD GetFlowCount ()
    {
        DWORD FlowCount = 0;
        for (auto ItU = m_UserSet.begin(); ItU != m_UserSet.end(); ItU++)
        {
            User *U = (User *)&(*ItU);
            T_FlowSet *F = U->GetFlowSet();

            FlowCount += F->size();
        }

        return FlowCount;
    }

    DWORD Classify (IpPacket *Pkt);

public:
    ClassifyEngine (PacketSet *PtSet)
    {
        m_CfMng = new CfManage ();
        assert (m_CfMng != NULL);

        m_PacketSet = PtSet;
    }

    ~ClassifyEngine ()
    {
        if (m_CfMng != NULL)
        {
            delete m_CfMng;
        }
        
    }

    DWORD Query (IpPacket *Pkt);
    VOID Analysis ();
    
};


#endif 
