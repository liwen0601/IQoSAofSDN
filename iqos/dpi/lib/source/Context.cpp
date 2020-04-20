/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Context for flow analysis
 * History:
   <1> 3/28/2020 , create
************************************************************/
#include "Context.h"

bool g_PringSwitch = false;


DWORD ClassifyEngine::Classify (IpPacket *Pkt)
{
    DWORD CfId;

    m_PacketNum++;
    m_Traffic += Pkt->m_PktLen;
    
    Flow *Fctxt = QueryFlow (Pkt);
    assert (Fctxt != NULL);

    Fctxt->m_PacketNum++;
    Fctxt->m_SduNum += (Pkt->m_Payload > 0);

    CfId = Fctxt->GetCfId ();
    if (CfId != 0)
    {
        return CfId;
    }

    if (Fctxt->m_SduNum > DPI_NUM)
    {
        Fctxt->SetCfId (CF_UN_IDENTIFY);
        return CF_UN_IDENTIFY;
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

        bool IsFin = Cf->Match (PtnRst, &Ctx->m_CurState, Fctxt->m_DstPort);
        if (IsFin)
        {
            Fctxt->SetCfId (Cf->GetId());
            Fctxt->ClearCfCtx ();
            return Cf->GetId();
        }
    }

    return 0;
}


VOID ClassifyEngine::Analysis ()
{
    IpPacket *Ip;

    while (!m_PacketSet->IsEmpty ())
    {
        Ip = m_PacketSet->Pop ();

        Classify (Ip);

        delete Ip;
    }
}



VOID ContextTest ()
{
    User U (111);

    Flow *F1 = U.GetFlow (111, 222, 1, 2, 6);
    assert (U.GetFlowNum() == 1);

    Flow *F2 = U.GetFlow (111, 333, 1, 2, 6);
    assert (U.GetFlowNum() == 2);
    
    Flow *F3 = U.GetFlow (111, 444, 1, 2, 6);
    assert (U.GetFlowNum() == 3);

    Flow *F4 = U.GetFlow (111, 222, 1, 2, 6);
    assert (U.GetFlowNum() == 3);

    Flow *F5 = U.GetFlow (111, 333, 1, 2, 6);
    assert (U.GetFlowNum() == 3);
}



