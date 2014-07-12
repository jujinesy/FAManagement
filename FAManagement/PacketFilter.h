#pragma once
#define MAXBUF  0xFFFF
#include "divert_device.h"
/*
 * Pre-fabricated packets.
 */
typedef struct
{
    DIVERT_IPHDR ip;
    DIVERT_TCPHDR tcp;
	UINT8 data[1700]; 
} TCPPACKET, *PTCPPACKET;

typedef struct
{
    DIVERT_IPV6HDR ipv6;
    DIVERT_TCPHDR tcp;
} TCPV6PACKET, *PTCPV6PACKET;

typedef struct
{
    DIVERT_IPHDR ip;
    DIVERT_ICMPHDR icmp;
    UINT8 data[];
} ICMPPACKET, *PICMPPACKET;

typedef struct
{
    DIVERT_IPV6HDR ipv6;
    DIVERT_ICMPV6HDR icmpv6;
    UINT8 data[];
} ICMPV6PACKET, *PICMPV6PACKET;


#pragma once
class PacketFilter
{
public:
	HANDLE m_ThreadHandle;          //스레드 핸들  
    //DWORD m_ThreadID;               //스레드 아이디 
	unsigned int m_ThreadID;               //스레드 아이디 
    enum 
    {  
        LIFE_DEF,  
        LIFE_LIVE,  
        LIFE_TERMINATE,  
    };  
    char m_cLife;  
	int Stopbit;



	PacketFilter(void);
	~PacketFilter(void);

	void PacketFilter::StartThread(void);
	void PacketFilter::StopThread(void);
	static UINT WINAPI PacketFilter::Update(LPVOID parameter); 
	void PacketFilter::Run();
	static void PacketIpInit(PDIVERT_IPHDR packet);
	static void PacketIpTcpInit(PTCPPACKET packet);
	static void PacketIpIcmpInit(PICMPPACKET packet);
	static void PacketIpv6Init(PDIVERT_IPV6HDR packet);
	static void PacketIpv6TcpInit(PTCPV6PACKET packet);
	static void PacketIpv6Icmpv6Init(PICMPV6PACKET packet);
};
