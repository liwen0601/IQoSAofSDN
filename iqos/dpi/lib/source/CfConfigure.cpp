/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2020
 * Describe: Classifier configure
 * History:
   <1> 4/05/2020 , create
************************************************************/
#include "Classifier.h"
#include "Packet.h"


VOID CfManage::Init ()
{
    //#######################################################
    // configure classifier Youtube
    //#######################################################
    {
    Classifier *Youtube = NewClassifier ("Youtube", IPV4_VERSION, 443);

    /* add State */
    State *State0 = Youtube->NewState (false);
    State *State1 = Youtube->NewState (true);
    
    DWORD Pid0 = NewPattern ("www.gstatic.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Youtube);
    }


    //#######################################################
    // office365
    //#######################################################
    {
    Classifier *Office365 = NewClassifier ("Office365", IPV4_VERSION, 443);

    /* add State */
    State *State0 = Office365->NewState (false);
    State *State1 = Office365->NewState (true);
    
    DWORD Pid0 = NewPattern ("outlookmobile-office365-tas.msedge.net");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Office365);
    }

    //#######################################################
    // QQ
    //#######################################################
    {
    Classifier *QQ = NewClassifier ("QQ", IPV4_VERSION, 80);

    /* add State */
    State *State0 = QQ->NewState (false);
    State *State1 = QQ->NewState (true);
    
    DWORD Pid0 = NewPattern ("Host: www.qq.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, QQ);
    }

    //#######################################################
    // BNJP
    //#######################################################
    {
    Classifier *BJNP = NewClassifier ("BJNP", IPV4_VERSION, 0);

    /* add State */
    State *State0 = BJNP->NewState (false);
    State *State1 = BJNP->NewState (true);
    
    DWORD Pid0 = NewPattern ("BJNP");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, BJNP);
    }
} 


