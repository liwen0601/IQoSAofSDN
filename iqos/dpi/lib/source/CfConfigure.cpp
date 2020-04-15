/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2020
 * Describe: Classifier configure
 * History:
   <1> 4/05/2020 , create
************************************************************/
#include "Classifier.h"
#include "Packet.h"

VOID CfManage::Dump ()
{
    FILE *F = fopen (CF_SUPPORT_FILE, "w");
    if (F == NULL)
    {
        return;
    }

    fprintf (F, "%-32s %-8s\r\n", "ProtocolName", "ProtocolID");
    for (auto it = m_CfId2Cf.begin(); it != m_CfId2Cf.end(); it++)
    {
        fprintf (F, "%-32s %-8u\r\n", it->second->GetName ().c_str(), it->first);
    }

    fclose (F);
    return;
}


VOID CfManage::Init ()
{
    //#######################################################
    // Google
    //#######################################################
    {
    Classifier *Google = NewClassifier ("Google", IPV4_VERSION, 443);

    /* add State */
    State *State0 = Google->NewState (false);
    State *State1 = Google->NewState (true);
    
    DWORD Pid0 = NewPattern ("www.google.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Google);
    }

    //#######################################################
    // Zoom
    //#######################################################
    {
    Classifier *Zoom = NewClassifier ("Zoom", IPV4_VERSION, 443);

    /* add State */
    State *State0 = Zoom->NewState (false);
    State *State1 = Zoom->NewState (true);
    
    DWORD Pid0 = NewPattern ("contactservice.zoom.us");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Zoom);
    }


    //#######################################################
    // office365
    //#######################################################
    {
    Classifier *Office365 = NewClassifier ("Office365", LV4_TCP, 443);

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
    Classifier *QQ = NewClassifier ("QQ", LV4_TCP, 80);

    /* add State */
    State *State0 = QQ->NewState (false);
    State *State1 = QQ->NewState (true);
    
    DWORD Pid0 = NewPattern ("Host: www.qq.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, QQ);
    }

    //#######################################################
    // Facebook
    //#######################################################
    {
    Classifier *Facebook = NewClassifier ("Facebook", LV4_TCP, 443);

    /* add State */
    State *State0 = Facebook->NewState (false);
    State *State1 = Facebook->NewState (true);
    
    DWORD Pid0 = NewPattern ("www.facebook.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Facebook);
    }

    //#######################################################
    // Whatsapp
    //#######################################################
    {
    Classifier *Whatsapp = NewClassifier ("Whatsapp", LV4_TCP, 443);

    /* add State */
    State *State0 = Whatsapp->NewState (false);
    State *State1 = Whatsapp->NewState (true);
    
    DWORD Pid0 = NewPattern ("www.whatsapp.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Whatsapp);
    }

    //#######################################################
    // Twitter
    //#######################################################
    {
    Classifier *Twitter = NewClassifier ("Twitter", LV4_TCP, 443);

    /* add State */
    State *State0 = Twitter->NewState (false);
    State *State1 = Twitter->NewState (true);
    
    DWORD Pid0 = NewPattern ("abs.twimg.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Twitter);
    }

    //#######################################################
    // OneNote
    //#######################################################
    {
    Classifier *OneNote = NewClassifier ("OneNote", LV4_TCP, 443);

    /* add State */
    State *State0 = OneNote->NewState (false);
    State *State1 = OneNote->NewState (false);
    State *State2 = OneNote->NewState (true);
    
    DWORD Pid0 = NewPattern ("docs.live.net");
    State0->AddNextState (Pid0, State1->GetId ());

    DWORD Pid1 = NewPattern ("Microsoft Corporation");
    State1->AddNextState (Pid1, State2->GetId ());

    MapPatternCf (Pid0, OneNote);
    MapPatternCf (Pid1, OneNote);
    }

    //#######################################################
    // Dropbox
    //#######################################################
    {
    Classifier *Dropbox = NewClassifier ("Dropbox", LV4_TCP);

    /* add State */
    State *State0 = Dropbox->NewState (false);
    State *State1 = Dropbox->NewState (true);
    
    DWORD Pid0 = NewPattern (".dropboxstatic.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Dropbox);
    }

    //#######################################################
    // Overleaf
    //#######################################################
    {
    Classifier *Overleaf = NewClassifier ("Overleaf", LV4_TCP);

    /* add State */
    State *State0 = Overleaf->NewState (false);
    State *State1 = Overleaf->NewState (true);
    
    DWORD Pid0 = NewPattern ("www.overleaf.com");
    State0->AddNextState (Pid0, State1->GetId ());

    MapPatternCf (Pid0, Overleaf);
    }



    //#######################################################
    // END
    //#######################################################
    Dump ();
} 


