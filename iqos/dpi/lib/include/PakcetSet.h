/***********************************************************
 * Author: Wen Li
 * Date  : 4/13/2020
 * Describe: packet set
 * History:
   <1> 3/28/2020 , create
************************************************************/

#ifndef _PACKETSET_H_
#define _PACKETSET_H_
#include <BasicType.h>
#include <Log.h>
#include <Packet.h>

using namespace std;

typedef vector<IpPacket*> T_Packets;

class PacketSet
{
private:
    T_Packets m_PacketSet;
    pthread_mutex_t m_Mutex;

public:
    PacketSet ()
    {
        pthread_mutex_init(&m_Mutex, NULL);
    }

    inline VOID Push (IpPacket* Pkt)
    {
        pthread_mutex_lock(&m_Mutex);

        m_PacketSet.push_back (Pkt);

        pthread_mutex_unlock(&m_Mutex);
    }

    inline IpPacket* Pop ()
    {
        pthread_mutex_lock(&m_Mutex);

        IpPacket* Pkt = m_PacketSet.back ();
        m_PacketSet.pop_back ();

        pthread_mutex_unlock(&m_Mutex);

        return Pkt;
    }

    inline bool IsEmpty ()
    {
        return m_PacketSet.empty();
    }
    
};


#endif 
