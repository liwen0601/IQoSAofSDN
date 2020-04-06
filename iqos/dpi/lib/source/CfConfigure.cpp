/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2020
 * Describe: Classifier configure
 * History:
   <1> 4/05/2020 , create
************************************************************/
#include "Classifier.h"
#include "Packet.h"


VOID CfEngine::Init ()
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

    Pattern2Classifier (Pid0, Youtube);
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

    Pattern2Classifier (Pid0, Office365);
    }
} 


