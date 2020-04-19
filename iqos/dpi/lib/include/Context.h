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

    DWORD m_PacketNum;
    DWORD m_SduNum;

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
        m_PacketNum = 0;
        m_SduNum = 0;
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

    inline VOID ClearCfCtx ()
    {
        m_Cf2Text.clear();
    }

    typedef struct 
    {
        bool operator()(Flow *L, Flow *R) 
        {
            if (L->m_SrcIp != R->m_SrcIp)
            {
                return (L->m_SrcIp < R->m_SrcIp);
            }
            else if (L->m_DstIp != R->m_DstIp)
            {
                return (L->m_DstIp < R->m_DstIp);
            }
            else if (L->m_SrcPort != R->m_SrcPort)
            {
                return (L->m_SrcPort < R->m_SrcPort);
            }
            else if (L->m_DstPort != R->m_DstPort)
            {
                return (L->m_DstPort < R->m_DstPort);
            }
            else
            {
                return (L->m_ProtoType < R->m_ProtoType);
            }
        }
    } EqualFlow; 
};

typedef set<Flow*, typename Flow::EqualFlow> T_FlowSet;

class User
{
public:
    DWORD m_Ipaddr;
    
private:
    Flow *m_Fkey;
    T_FlowSet m_FlowSet;
    pthread_mutex_t m_Mutex;

public:
    User (DWORD IpAddr)
    {
        m_Ipaddr = IpAddr;
        m_Fkey   = new Flow (0, 0, 0, 0, 0);
        pthread_mutex_init(&m_Mutex, NULL);
    }

    ~User ()
    {
        if (m_Fkey)
        {
            delete m_Fkey;
        }
    }

    inline DWORD GetIpAddr ()
    {
        return m_Ipaddr;
    }

    inline Flow* AddFlow (Flow *Fkey)
    {
        Flow *NewF = new Flow (Fkey->m_SrcIp, Fkey->m_DstIp,
                               Fkey->m_SrcPort, Fkey->m_DstPort, Fkey->m_ProtoType);
        assert (NewF != NULL);
        
        auto It = m_FlowSet.insert (NewF);
        if (It.second == false)
        {
            return NULL;
        }

        //DebugLog ("Add flow-Pro: %d Src: %u-%u, Dst: %u-%u\r\n", F.m_ProtoType, F.m_SrcIp, F.m_SrcPort, F.m_DstIp, F.m_DstPort);
        return NewF;        
    }

    inline Flow* GetFlow (DWORD SrcIp, DWORD DstIp, WORD SrcPort, WORD DstPort, DWORD ProtoType)
    {
        Flow* Fctx;
        
        pthread_mutex_lock(&m_Mutex);
        
        m_Fkey->m_SrcIp = SrcIp;
        m_Fkey->m_DstIp = DstIp;
        m_Fkey->m_SrcPort = SrcPort;
        m_Fkey->m_DstPort = DstPort;
        m_Fkey->m_ProtoType = ProtoType;
        
        auto It = m_FlowSet.find (m_Fkey);
        if (It != m_FlowSet.end())
        {
            Fctx = (Flow*)(*It);
        }
        else
        {
            DebugLog ("Add a new flow:%u %u-%u %u-%u\r\n", \
                      m_Fkey->m_ProtoType, m_Fkey->m_SrcIp, m_Fkey->m_SrcPort, m_Fkey->m_DstIp, m_Fkey->m_DstPort);
            Fctx = AddFlow (m_Fkey);
        }
   
        pthread_mutex_unlock(&m_Mutex);

        return Fctx;
    }

    inline DWORD GetFlowNum ()
    {
        return (DWORD)m_FlowSet.size ();        
    }

    inline T_FlowSet::iterator begin ()
    {
        return m_FlowSet.begin();
    }

    inline T_FlowSet::iterator end ()
    {
        return m_FlowSet.end();
    }

    typedef struct 
    {
        bool operator()(User *L, User *R) 
        {
            if (L->m_Ipaddr != R->m_Ipaddr)
            {
                return (L->m_Ipaddr < R->m_Ipaddr);
            }
            
            return 0;
        }
    } EqualUser;
};


typedef set<User*, typename User::EqualUser> T_UsetSet;

class ClassifyEngine
{
private:
    User* m_Ukey;
    T_UsetSet m_UserSet;
    CfManage *m_CfMng;
    PacketSet *m_PacketSet;

    pthread_mutex_t m_Mutex;

    ULONG m_PacketNum;
    ULONG m_Traffic;

private:
    inline User *AddUser(User* Ukey)
    {
        User* NewU = new User (Ukey->m_Ipaddr);
        assert (NewU != NULL);
            
        auto It = m_UserSet.insert (NewU);
        if (It.second == false)
        {
            return NULL;
        }

        return NewU;    
    }
    
    inline User* GetUser (DWORD SrcIp)
    {
        User *Utx;

        pthread_mutex_lock(&m_Mutex);
        m_Ukey->m_Ipaddr = SrcIp;
        
        auto It = m_UserSet.find (m_Ukey);
        if (It != m_UserSet.end())
        {
            Utx = (User *)(*It);
        }
        else
        {
            DebugLog ("Add a new user: %u\r\n", m_Ukey->m_Ipaddr);
            Utx = AddUser (m_Ukey);
        }
        pthread_mutex_unlock(&m_Mutex);

        return Utx;        
    }

    inline DWORD GetUserCount ()
    {
        return m_UserSet.size();
    }

    DWORD Classify (IpPacket *Pkt);

public:
    ClassifyEngine (PacketSet *PtSet)
    {
        m_CfMng = new CfManage ();
        assert (m_CfMng != NULL);

        m_PacketSet = PtSet;

        m_Ukey = new User (0);

        pthread_mutex_init(&m_Mutex, NULL);
    }

    ~ClassifyEngine ()
    {
        if (m_CfMng != NULL)
        {
            delete m_CfMng;
        }

        if (m_Ukey != NULL)
        {
            delete m_Ukey;
        }
        
    }

    VOID Analysis ();

    inline DWORD QueueSize ()
    {
        return m_PacketSet->Size ();
    }

    inline T_UsetSet::iterator begin ()
    {
        return m_UserSet.begin();
    }

    inline T_UsetSet::iterator end ()
    {
        return m_UserSet.end();
    }

    inline Flow* QueryFlow (IpPacket *Pkt)
    {
        DWORD CfId;
        
        User *Uctxt = GetUser (Pkt->m_SrcIp);
        assert (Uctxt != NULL);

        Flow *Fctxt = Uctxt->GetFlow (Pkt->m_SrcIp, Pkt->m_DstIp, 
                                      Pkt->m_SrcPort, Pkt->m_DstPort, 
                                      Pkt->m_ProtoType);
        assert (Fctxt != NULL);

        return Fctxt;
    }

    inline ULONG GetPacketNum ()
    {
        return m_PacketNum;
    }

    inline ULONG GetTraffic ()
    {
        return m_Traffic;
    }

    inline ULONG GetFlowNum ()
    {
        ULONG FlowNum = 0;
        
        for (auto Uit = m_UserSet.begin(), Uend = m_UserSet.end(); Uit != Uend; Uit++)
        {
            User *U = *Uit;
            FlowNum += U->GetFlowNum ();
        }

        return FlowNum;
    }

    inline VOID UpdateStatistic (IpPacket *Pkt)
    {
        m_Traffic += Pkt->m_PktLen;
        m_PacketNum++;
    }
    
};


#endif 
