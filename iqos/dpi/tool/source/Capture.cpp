/***********************************************************
 * Author: Wen Li
 * Date  : 4/02/2019
 * Describe: packet capture from interface
 * History:
   <1> 4/02/2019 , create
************************************************************/

#include <Log.h>
#include "Capture.h"

#define FILENAME "capture.pcap"

BOOL Capture::IsDeviceOnline ()
{
    char EBuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *DeviceList;

    INT Ret = pcap_findalldevs(&DeviceList, EBuf);
    if (Ret != 0)
    {
        return M_FALSE;
    }

    while (DeviceList != NULL)
    {
        DebugLog ("Get online device: %s", DeviceList->name);
        if (m_Device.compare (DeviceList->name) == 0)
        {
            return M_TRUE;
        }

        DeviceList = DeviceList->next;
    }
    
    return M_FALSE;
}

pcap_t* Capture::InitPcapHandle ()
{
    DWORD LocalNet = 0;
    DWORD NetMask  = 0;
    pcap_t *CapHandle;
    CHAR Error[PCAP_ERRBUF_SIZE];
    struct bpf_program FCode;
    
    CapHandle = pcap_open_live(m_Device.c_str(), m_CapLen, M_TRUE, 1000, Error);
    assert (CapHandle != NULL);

    m_CapLen = pcap_snapshot(CapHandle);
    if(m_CapLen != CAP_LEN) 
    {
        DebugLog ("snaplen changes from %u to %u", CAP_LEN, m_CapLen);
    }
    
    if(pcap_lookupnet(m_Device.c_str(), &LocalNet, &NetMask, Error) < 0)
    {
        LocalNet = NetMask = 0;
        DebugLog("%s\n", Error);
    }
    
    assert (pcap_compile(CapHandle,  &FCode, "", 1, NetMask) >= 0);    
    assert (pcap_setfilter(CapHandle, &FCode) >= 0);

    return CapHandle;
}


VOID Capture::CapturePacket(pcap_handler Analysis)
{
    pcap_dumper_t* Dumper;
    
    if (!IsDeviceOnline ())
    {
        DebugLog ("Device %s is not online.", m_Device.c_str());
        return;
    }
    
    m_CapHandle = InitPcapHandle ();  
    Dumper = pcap_dump_open(m_CapHandle, FILENAME);
    assert (Dumper != NULL);
    
    if(pcap_loop(m_CapHandle, -1, Analysis, (u_char*)Dumper) < 0)
    {
        DebugLog("Error in pcap_loop");
        pcap_close(m_CapHandle);
    }
    
    return;
}


