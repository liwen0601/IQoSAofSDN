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
    pthread_mutex_t m_Mutex;

public:
    User (DWORD IpAddr)
    {
        m_Ipaddr = IpAddr;
        pthread_mutex_init(&m_Mutex, NULL);
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
        Flow* Fctx;
        
        pthread_mutex_lock(&m_Mutex);
        auto It = m_FlowSet.find (F);
        if (It != m_FlowSet.end())
        {
            Fctx = (Flow*)(&(*It));
        }
        else
        {
            DebugLog ("Add a new flow:%u %u-%u %u-%u\r\n", \
                      F.m_ProtoType, F.m_SrcIp, F.m_SrcPort, F.m_DstIp, F.m_DstPort);
            Fctx = AddFlow (F);
        }

        Fctx->m_PacketNum++;
   
        pthread_mutex_unlock(&m_Mutex);

        return Fctx;
    }

    inline T_FlowSet* GetFlowSet ()
    {
        return &m_FlowSet;        
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

    pthread_mutex_t m_Mutex;

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
        User *Utx;

        pthread_mutex_lock(&m_Mutex);
        auto It = m_UserSet.find (U);
        if (It != m_UserSet.end())
        {
            Utx = (User *)(&(*It));
        }
        else
        {
            DebugLog ("Add a new user: %u\r\n", U.m_Ipaddr);
            Utx = AddUser (U);
        }
        pthread_mutex_unlock(&m_Mutex);

        return Utx;        
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


    DWORD Classify (IpPacket *Pkt);

public:
    ClassifyEngine (PacketSet *PtSet)
    {
        m_CfMng = new CfManage ();
        assert (m_CfMng != NULL);

        m_PacketSet = PtSet;

        pthread_mutex_init(&m_Mutex, NULL);
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
    
};


#endif 
