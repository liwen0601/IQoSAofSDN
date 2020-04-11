/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Context for flow analysis
 * History:
   <1> 3/28/2020 , create
************************************************************/
#include "Context.h"

string ClassifyEngine::ClassifyFlow (IpPacket *Pkt)
{
    DWORD CfId;
    
    User *Uctxt = GetUser (User (Pkt->m_SrcIp));
    assert (Uctxt != NULL);

    Flow *Fctxt = Uctxt->GetFlow (Flow(Pkt->m_SrcIp, Pkt->m_DstIp, 
                                  Pkt->m_SrcPort, Pkt->m_DstPort, 
                                  Pkt->m_ProtoType)
                                 );
    assert (Fctxt != NULL);

    CfId = Fctxt->GetCfId ();
    if (CfId != 0)
    {
        return m_CfMng->GetCfName (CfId);
    }

    T_Result *PtnRst = m_CfMng->PatternMach (Pkt->m_Payload, Pkt->m_PayloadLen);

    T_CfSet CfSet;
    for (auto Pit = PtnRst->begin (), Pend = PtnRst->end(); Pit != Pend; Pit++)
    {
        Classifier *RelCf = m_CfMng->GetCfByPid (*Pit);
        assert (RelCf != NULL);

        CfSet.insert (RelCf);
    }

    for (auto Cit = CfSet.begin (), Cend = CfSet.end(); Cit  != Cend; Cit++)
    {
        Classifier *Cf = *Cit;
        CfCtxt *Ctx = Fctxt->GetCfCtext (Cf);

        bool IsFin = Cf->Match (PtnRst, &Ctx->m_CurState);
        if (IsFin)
        {
            Fctxt->SetCfId (Cf->GetId());
            return Cf->GetName ();
        }
    }

    return "None";
}





