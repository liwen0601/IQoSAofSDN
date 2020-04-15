/***********************************************************
 * Author: Wen Li
 * Date  : 4/13/2020
 * Describe: conmand line for debug
 * History:
   <1> 4/13/2020 , create
************************************************************/
#include "Log.h"
#include "Shell.h"

class FlowPrint:public Cmd
{
private:
    map<DWORD, string> m_Id2Cf;

    inline VOID LoadCfFile ()
    {
        FILE *F = fopen (CF_SUPPORT_FILE, "r");
        if (F == NULL)
        {
            return;
        }

        char CfName[64];
        DWORD CfId;

        while (!feof (F))
        {
            CfId = 0;
            memset (CfName, 0, sizeof(CfName));
            
            fscanf (F, "%s %u", CfName, &CfId);
            if (CfName[0] != 0 && CfId >= CF_ID_BEGIN)
            {
                m_Id2Cf[CfId] = string (CfName);
            }
        }

        fclose (F);
    }

    inline char* GetCfName (DWORD Id)
    {
        if (Id == 0)
        {
            return (char*)" ";
        }
        
        auto it = m_Id2Cf.find (Id);
        if (it == m_Id2Cf.end())
        {
            return (char*)" ";
        }

        return (char*)it->second.c_str();
    }

public:
    FlowPrint ()
    {
        LoadCfFile ();
    }
    
    VOID Execute (ClassifyEngine *CfEngine)
    {
        DWORD Id = 0;
        
        printf ("%-4s %-16s %-16s %-10s %-10s %-6s %-6s %-16s\r\n",
                "ID", "Source", "Destination", "SrcPort", "DstPort", "Proto", "CF-Id", "CF-Name");
        
        for (auto it = CfEngine->begin (); it != CfEngine->end(); it++)
        {
            User *U = (User *)(&(*it));

            for (auto fit = U->begin (); fit != U->end (); fit++)
            {
                Flow *F = (Flow *)(&(*fit));
                printf ("%-4u %-16s %-16s %-10u %-10u %-6u %-6u %-16s\r\n",
                        Id,
                        IP(F->m_SrcIp).c_str(),
                        IP(F->m_DstIp).c_str(),
                        F->m_SrcPort, F->m_DstPort,
                        F->m_ProtoType, F->GetCfId (), GetCfName(F->GetCfId ()));
                Id++;
            }
        }
    }
};

class UserPrint:public Cmd
{
    VOID Execute (ClassifyEngine *CfEngine)
    {
        DWORD Id = 0;
        printf ("%-4s %-16s\r\n", "ID", "IP");
        
        for (auto it = CfEngine->begin (); it != CfEngine->end(); it++)
        {
            User *U = (User *)(&(*it));
            printf ("%-4u %-16s\r\n", Id, IP(U->m_Ipaddr).c_str());
            Id++;
        }
    }
};

class IpTable:public Cmd
{
    VOID Execute (ClassifyEngine *CfEngine)
    {
        FILE *F = fopen (USER_IP_PROPERTITY, "r");
        if (F == NULL)
        {
            printf ("NULL\r\n");
        }
        else
        {
            DWORD Id = 0;
            char  IPaddr[128];
            
            printf ("%-4s %-16s\r\n", "ID", "IP");
            while (!feof(F))
            {
                memset (IPaddr, 0, sizeof(IPaddr));
                fgets (IPaddr, sizeof (IPaddr), F);
                if (strlen (IPaddr) < 4)
                {
                    continue;
                }
                
                printf ("%-4u %-16s", Id, IPaddr);
                Id++;
            }

            printf ("\r\n");
            fclose (F);
        }
    }
};

class Help:public Cmd
{
public:
    Help (CmdShell *cs):Cmd(cs)
    {
    }
    
    VOID Execute (ClassifyEngine *CfEngine)
    {
        for (auto it = m_Cs->begin (); it != m_Cs->end (); it++)
        {
            printf ("%s ", it->first.c_str());
        }

        printf ("\r\n");
    }
};


class Exit:public Cmd
{
    VOID Execute (ClassifyEngine *CfEngine)
    {
        exit(0);
    }
};


VOID CmdShell::Init () 
{
    m_CmdMap["flow"] = new FlowPrint();
    m_CmdMap["user"] = new UserPrint();
    m_CmdMap["exit"] = new Exit();
    m_CmdMap["iptable"] = new IpTable();
    m_CmdMap["?"] = new Help(this);
    
	return;
}


VOID CmdShell::ExeCmd (string Comand)
{
    auto It = m_CmdMap.find (Comand);
    if (It == m_CmdMap.end())
    {
        cout<<Comand<<" not found, enter \"?\" for help.."<<endl;
        return;
    }

    Cmd* C = It->second;
    C->Execute (m_CfEngine);

    return;
}
