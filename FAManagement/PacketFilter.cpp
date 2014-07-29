#pragma once
#include "stdafx.h"
#include "PacketFilter.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment( lib, "res/lib/WinDivert.lib")


FILE *fr;
HANDLE handle, console, hThread;
UINT i;
INT16 priority = 0;
char packet[MAXBUF];
UINT packet_len;
DIVERT_ADDRESS recv_addr, send_addr;
PDIVERT_IPHDR ip_header;
PDIVERT_IPV6HDR ipv6_header;
PDIVERT_ICMPHDR icmp_header;
PDIVERT_ICMPV6HDR icmpv6_header;
PDIVERT_TCPHDR tcp_header;
PDIVERT_UDPHDR udp_header;
UINT payload_len;


TCPPACKET reset0;
PTCPPACKET reset = &reset0;
UINT8 dnr0[sizeof(ICMPPACKET) + 0x0F*sizeof(UINT32) + 8 + 1];
PICMPPACKET dnr = (PICMPPACKET)dnr0;

TCPV6PACKET resetv6_0;
PTCPV6PACKET resetv6 = &resetv6_0;
UINT8 dnrv6_0[sizeof(ICMPV6PACKET) + sizeof(DIVERT_IPV6HDR) + sizeof(DIVERT_TCPHDR)];
PICMPV6PACKET dnrv6 = (PICMPV6PACKET)dnrv6_0;


PacketFilter::PacketFilter(void)
{
	PacketIpTcpInit(reset);
	PacketIpIcmpInit(dnr);
	PacketIpv6TcpInit(resetv6);
	PacketIpv6Icmpv6Init(dnrv6);
}


PacketFilter::~PacketFilter(void)
{
}


void PacketFilter::StartThread(void)
{
	Stopbit=1;
	m_ThreadHandle = (HANDLE)_beginthreadex(   
		0, 
		0, 
		(unsigned int (__stdcall *)(void *))Update, 
		(LPVOID)this,  
		0, 
		//(unsigned int *)m_ThreadID );    
		&m_ThreadID ); 
}

void PacketFilter::StopThread(void)
{
	Stopbit=0;
}

UINT WINAPI PacketFilter::Update(LPVOID parameter)
{
	fr = fopen("dump.txt", "w");

	handle = DivertOpen("outbound and tcp.DstPort != 3306", (DIVERT_LAYER)0, 0, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_INVALID_PARAMETER)
		{
			fprintf(stderr, "error: filter syntax error\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "error: failed to open Divert device (%d)\n",
			GetLastError());
		exit(EXIT_FAILURE);
	}

	
	PacketFilter* pThread = ( PacketFilter* )parameter;  
	pThread->m_cLife=LIFE_LIVE;  
    pThread->Run();


	DivertClose(handle);
	fclose(fr);
	return 0;
}


void PacketFilter::Run()
{
	while (TRUE)
	{
		if(Stopbit==0)
			break;


		// Read a matching packet.
		if (!DivertRecv(handle, packet, sizeof(packet), &recv_addr,
			&packet_len))
		{
			fprintf(stderr, "warning: failed to read packet\n");
			continue;
		}


		fprintf(fr,"원본 패킷의 길이는 %d", packet_len);
		fprintf(fr,"\n↓↓↓↓↓↓↓↓↓↓↓원본패킷↓↓↓↓↓↓↓↓↓↓↓");
		for (i = 0; i < packet_len; i++)
		{
			if (i % 20 == 0)
			{
				fprintf(fr,"\n");
			}
			fprintf(fr,"%.2X ", (UINT8)packet[i]);
		}
		fprintf(fr,"\n↑↑↑↑↑↑↑↑↑↑↑원본패킷↑↑↑↑↑↑↑↑↑↑↑\n\n\n");



		// Print info about the matching packet.
		DivertHelperParse(packet, packet_len, &ip_header, &ipv6_header,
			&icmp_header, &icmpv6_header, &tcp_header, &udp_header, NULL,
			&payload_len);
		if (ip_header == NULL && ipv6_header == NULL)
		{
			continue;
		}

		// Dump packet info: 
		SetConsoleTextAttribute(console, FOREGROUND_RED);
		fputs("BLOCK ", stdout);
		SetConsoleTextAttribute(console,
			FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		if (ip_header != NULL)
		{
			UINT8 *src_addr = (UINT8 *)&ip_header->SrcAddr;
			UINT8 *dst_addr = (UINT8 *)&ip_header->DstAddr;
			printf("\nIPv4 [Version=%u HdrLength=%u TOS=%u Length=%u Id=0x%.4X "
				"Reserved=%u DF=%u MF=%u FragOff=%u TTL=%u Protocol=%u "
				"Checksum=0x%.4X SrcAddr=%u.%u.%u.%u DstAddr=%u.%u.%u.%u]\n",
				ip_header->Version, ip_header->HdrLength,
				ntohs(ip_header->TOS), ntohs(ip_header->Length),
				ntohs(ip_header->Id), DIVERT_IPHDR_GET_RESERVED(ip_header),
				DIVERT_IPHDR_GET_DF(ip_header), DIVERT_IPHDR_GET_MF(ip_header),
				ntohs(DIVERT_IPHDR_GET_FRAGOFF(ip_header)), ip_header->TTL,
				ip_header->Protocol, ntohs(ip_header->Checksum),
				src_addr[0], src_addr[1], src_addr[2], src_addr[3],
				dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3]);
		}
		if (ipv6_header != NULL)
		{
			UINT16 *src_addr = (UINT16 *)&ipv6_header->SrcAddr;
			UINT16 *dst_addr = (UINT16 *)&ipv6_header->DstAddr;
			fputs("ipv6.SrcAddr=", stdout);
			for (i = 0; i < 8; i++)
			{
				printf("%x%c", ntohs(src_addr[i]), (i == 7? ' ': ':'));
			} 
			fputs(" ipv6.DstAddr=", stdout);
			for (i = 0; i < 8; i++)
			{
				printf("%x%c", ntohs(dst_addr[i]), (i == 7? ' ': ':'));
			}
			putchar(' ');
		}
		if (icmp_header != NULL)
		{
			printf("icmp.Type=%u icmp.Code=%u ",
				icmp_header->Type, icmp_header->Code);
			// Simply drop ICMP
		}
		if (icmpv6_header != NULL)
		{
			printf("icmpv6.Type=%u icmpv6.Code=%u ",
				icmpv6_header->Type, icmpv6_header->Code);
			// Simply drop ICMPv6
		}




		if (tcp_header != NULL)
		{
			//printf("tcp.SrcPort=%u tcp.DstPort=%u tcp.Flags=",
			//             ntohs(tcp_header->SrcPort), ntohs(tcp_header->DstPort));
			//         if (tcp_header->Fin)
			//         {
			//             fputs("[FIN]", stdout);
			//         }
			//         if (tcp_header->Rst)
			//         {
			//             fputs("[RST]", stdout);
			//         }
			//         if (tcp_header->Urg)
			//         {
			//             fputs("[URG]", stdout);
			//         }
			//         if (tcp_header->Syn)
			//         {
			//             fputs("[SYN]", stdout);
			//         }
			//         if (tcp_header->Psh)
			//         {
			//             fputs("[PSH]", stdout);
			//         }
			//         if (tcp_header->Ack)
			//         {
			//             fputs("[ACK]", stdout);
			//         }
			//         putchar(' ');













			//if (0)
			if (ip_header != NULL)
			{
				UINT8 *src_addr;
				UINT8 *dst_addr;
				UINT8 *test;
				UINT8 *userid;


				//reset->ip.SrcAddr = ip_header->DstAddr;
				//reset->ip.DstAddr = ip_header->SrcAddr;
				//reset->tcp.SrcPort = tcp_header->DstPort;
				//reset->tcp.DstPort = tcp_header->SrcPort;
				//reset->tcp.SeqNum = 
				//    (tcp_header->Ack? tcp_header->AckNum: 0);
				//reset->tcp.AckNum =
				//    (tcp_header->Syn?
				//        htonl(ntohl(tcp_header->SeqNum) + 1):
				//        htonl(ntohl(tcp_header->SeqNum) + payload_len));
				
				
				//reset이 복사해온 패킷
				memset(reset, 0, packet_len+20);
				memcpy(reset, packet, packet_len);

				//reset->ip.Checksum = 10;
				reset->ip.DstAddr = ip_header->DstAddr;
				//reset->ip.FragOff0 = ip_header->FragOff0;
				//reset->ip.HdrLength = ip_header->HdrLength;
				//reset->ip.HdrLength = 7;
				//reset->ip.Id = ip_header->Id;
				//reset->ip.Length = ip_header->Length;
				//reset->ip.Protocol = ip_header->Protocol;
				//reset->ip.SrcAddr = ip_header->SrcAddr;
				//reset->ip.TOS = ip_header->TOS;
				reset->ip.TOS = 168;
				//reset->ip.TTL = ip_header->TTL;
				//reset->ip.Version = ip_header->Version;


				//reset->tcp.Ack = tcp_header->Ack;
				//reset->tcp.AckNum = tcp_header->AckNum;
				//reset->tcp.Checksum = tcp_header->Checksum;
				//reset->tcp.DstPort = tcp_header->DstPort;
				//reset->tcp.Fin = tcp_header->Fin;
				//reset->tcp.HdrLength = tcp_header->HdrLength;
				//reset->tcp.Psh = tcp_header->Psh;
				//reset->tcp.Reserved1 = tcp_header->Reserved1;
				//reset->tcp.Reserved2 = tcp_header->Reserved2;
				//reset->tcp.Rst = tcp_header->Rst;
				//reset->tcp.SeqNum = tcp_header->SeqNum;
				//reset->tcp.SrcPort = tcp_header->SrcPort;
				//reset->tcp.Syn = tcp_header->Syn;
				//reset->tcp.Urg = tcp_header->Urg;
				//reset->tcp.UrgPtr = tcp_header->UrgPtr;
				//reset->tcp.Window = tcp_header->Window;

				//DivertHelperCalcChecksums((PVOID)reset, sizeof(TCPPACKET), 0);






				src_addr = (UINT8 *)&reset->ip.SrcAddr;
				dst_addr = (UINT8 *)&reset->ip.DstAddr;


				printf("\n\nIPv4 [Version=%u HdrLength=%u TOS=%u Length=%u Id=0x%.4X "
					"Reserved=%u DF=%u MF=%u FragOff=%u TTL=%u Protocol=%u "
					"Checksum=0x%.4X SrcAddr=%u.%u.%u.%u DstAddr=%u.%u.%u.%u]\n",
					reset->ip.Version, 
					reset->ip.HdrLength,
					ntohs(reset->ip.TOS), 
					ntohs(reset->ip.Length),
					ntohs(reset->ip.Id), 
					DIVERT_IPHDR_GET_RESERVED(&reset->ip),
					DIVERT_IPHDR_GET_DF(&reset->ip), 
					DIVERT_IPHDR_GET_MF(&reset->ip),
					ntohs(DIVERT_IPHDR_GET_FRAGOFF(&reset->ip)), 
					reset->ip.TTL,
					reset->ip.Protocol, 
					ntohs(reset->ip.Checksum),
					src_addr[0], src_addr[1], src_addr[2], src_addr[3],
					dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3]);





				memcpy(&send_addr, &recv_addr, sizeof(recv_addr));
				send_addr.Direction = recv_addr.Direction;
				send_addr.IfIdx = recv_addr.IfIdx;
				send_addr.SubIfIdx = recv_addr.SubIfIdx;


				//reset[2]=A;

				test = (UINT8 *)reset;



				fprintf(fr,"복사 후 패킷의 길이는 %d", packet_len);
				fprintf(fr,"\n↓↓↓↓↓↓↓↓↓↓↓복사패킷↓↓↓↓↓↓↓↓↓↓↓");
				for (i = 0; i < packet_len; i++)
				{
					if (i % 20 == 0)
					{
						fprintf(fr,"\n");
					}
					fprintf(fr,"%.2X ", test[i]);
				}
				//패킷 오류 수정
				//fprintf(fr,"\n↑↑↑↑↑↑↑↑↑↑↑복사 후 패킷↑↑↑↑↑↑↑↑↑↑↑\n\n\n");




				if (!DivertSend(handle, reset, packet_len,
					&send_addr, NULL))
				{
					fprintf(stderr, "warning: failed to send TCP reset (%d)\n",
						GetLastError());
				} else {
					printf("\nipv4 헤더 본론 인젝션\n");
				}
				//if (!DivertSend(handle, packet, packet_len, &recv_addr, NULL))
				//{
				//	fprintf(stderr, "warning: failed to reinject packet (%d)\n",
				//		GetLastError());
				//}
			}else {

				if (!DivertSend(handle, packet, packet_len, &recv_addr, NULL))
				{
					fprintf(stderr, "warning: failed to reinject packet (%d)\n",
						GetLastError());
				}
				printf("\nipv4 헤더 기타로 포워딩\n");

			}

			//    if (ipv6_header != NULL)
			//    {
			//        memcpy(resetv6->ipv6.SrcAddr, ipv6_header->DstAddr,
			//            sizeof(resetv6->ipv6.SrcAddr));
			//        memcpy(resetv6->ipv6.DstAddr, ipv6_header->SrcAddr,
			//            sizeof(resetv6->ipv6.DstAddr));
			//        resetv6->tcp.SrcPort = tcp_header->DstPort;
			//        resetv6->tcp.DstPort = tcp_header->SrcPort;
			//        resetv6->tcp.SeqNum =
			//            (tcp_header->Ack? tcp_header->AckNum: 0);
			//        resetv6->tcp.AckNum =
			//            (tcp_header->Syn?
			//                htonl(ntohl(tcp_header->SeqNum) + 1):
			//                htonl(ntohl(tcp_header->SeqNum) + payload_len));

			//        DivertHelperCalcChecksums((PVOID)resetv6, sizeof(TCPV6PACKET),
			//            0);
			//        
			//        memcpy(&send_addr, &recv_addr, sizeof(send_addr));
			//        send_addr.Direction = !recv_addr.Direction;
			//        if (!DivertSend(handle, (PVOID)resetv6, sizeof(TCPV6PACKET),
			//                &send_addr, NULL))
			//        {
			//            fprintf(stderr, "warning: failed to send TCP (IPV6) "
			//                "reset (%d)\n", GetLastError());
			//        }
			//    }
			//}
			//if (udp_header != NULL)
			//{
			//    printf("udp.SrcPort=%u udp.DstPort=%u ",
			//        ntohs(udp_header->SrcPort), ntohs(udp_header->DstPort));
			//
			//    if (ip_header != NULL)
			//    {
			//        // NOTE: For some ICMP error messages, WFP does not seem to
			//        //       support INBOUND injection.  As a work-around, we
			//        //       always inject OUTBOUND.
			//        UINT icmp_length = ip_header->HdrLength*sizeof(UINT32) + 8;
			//        memcpy(dnr->data, ip_header, icmp_length);
			//        icmp_length += sizeof(ICMPPACKET);
			//        dnr->ip.Length = htons((UINT16)icmp_length);
			//        dnr->ip.SrcAddr = ip_header->DstAddr;
			//        dnr->ip.DstAddr = ip_header->SrcAddr;
			//        
			//        DivertHelperCalcChecksums((PVOID)dnr, icmp_length, 0);
			//        
			//        memcpy(&send_addr, &recv_addr, sizeof(send_addr));
			//        send_addr.Direction = DIVERT_DIRECTION_OUTBOUND;
			//        if (!DivertSend(handle, (PVOID)dnr, icmp_length, &send_addr,
			//            NULL))
			//        {
			//            fprintf(stderr, "warning: failed to send ICMP message "
			//                "(%d)\n", GetLastError());
			//        }
			//    }
			//
			//    if (ipv6_header != NULL)
			//    {
			//        UINT icmpv6_length = sizeof(DIVERT_IPV6HDR) +
			//            sizeof(DIVERT_TCPHDR);
			//        memcpy(dnrv6->data, ipv6_header, icmpv6_length);
			//        icmpv6_length += sizeof(ICMPV6PACKET);
			//        memcpy(dnrv6->ipv6.SrcAddr, ipv6_header->DstAddr,
			//            sizeof(dnrv6->ipv6.SrcAddr));
			//        memcpy(dnrv6->ipv6.DstAddr, ipv6_header->SrcAddr,
			//            sizeof(dnrv6->ipv6.DstAddr));
			//        
			//        DivertHelperCalcChecksums((PVOID)dnrv6, icmpv6_length, 0);

			//        memcpy(&send_addr, &recv_addr, sizeof(send_addr));
			//        send_addr.Direction = DIVERT_DIRECTION_OUTBOUND;
			//        if (!DivertSend(handle, (PVOID)dnrv6, icmpv6_length,
			//                &send_addr, NULL))
			//        {
			//            fprintf(stderr, "warning: failed to send ICMPv6 message "
			//                "(%d)\n", GetLastError());
			//        }
			//    }
		} else {
			if (!DivertSend(handle, packet, packet_len, &recv_addr, NULL))
			{
				fprintf(stderr, "warning: failed to reinject packet (%d)\n",
					GetLastError());
			} else {
				printf("\n전체 기타로 포워딩!!!!!!\n");
			}

		}





		putchar('\n');
	}
}




void PacketFilter::PacketIpInit(PDIVERT_IPHDR packet)
{
	memset(packet, 0, sizeof(DIVERT_IPHDR));
	packet->Version = 4;
	packet->HdrLength = sizeof(DIVERT_IPHDR) / sizeof(UINT32);
	packet->Id = ntohs(0xDEAD);
	packet->TTL = 64;
}


void PacketFilter::PacketIpTcpInit(PTCPPACKET packet)
{
	memset(packet, 0, sizeof(TCPPACKET));
	PacketIpInit(&packet->ip);
	packet->ip.Length = htons(sizeof(TCPPACKET));
	packet->ip.Protocol = IPPROTO_TCP;
	packet->tcp.HdrLength = sizeof(DIVERT_TCPHDR) / sizeof(UINT32);
}


void PacketFilter::PacketIpIcmpInit(PICMPPACKET packet)
{
	memset(packet, 0, sizeof(ICMPPACKET));
	PacketIpInit(&packet->ip);
	packet->ip.Protocol = IPPROTO_ICMP;
}


void PacketFilter::PacketIpv6Init(PDIVERT_IPV6HDR packet)
{
	memset(packet, 0, sizeof(DIVERT_IPV6HDR));
	packet->Version = 6;
	packet->HopLimit = 64;
}


void PacketFilter::PacketIpv6TcpInit(PTCPV6PACKET packet)
{
	memset(packet, 0, sizeof(TCPV6PACKET));
	PacketIpv6Init(&packet->ipv6);
	packet->ipv6.Length = htons(sizeof(DIVERT_TCPHDR));
	packet->ipv6.NextHdr = IPPROTO_TCP;
	packet->tcp.HdrLength = sizeof(DIVERT_TCPHDR) / sizeof(UINT32);
}


void PacketFilter::PacketIpv6Icmpv6Init(PICMPV6PACKET packet)
{
	memset(packet, 0, sizeof(ICMPV6PACKET));
	PacketIpv6Init(&packet->ipv6);
	packet->ipv6.NextHdr = IPPROTO_ICMPV6;
}










//
//
//#include "stdafx.h"
//#include "divert_device.h"
//#include "PacketFilter.h"
//#include <winsock2.h>
//#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#pragma comment( lib, "res/lib/WinDivert.lib")
//
//
//
///*
// * Entry.
// */
//int __cdecl mainf()
//{
//	int argc;
//	char** argv;
//	FILE *fr;
//    HANDLE handle, console;
//    UINT i;
//    INT16 priority = 0;
//    char packet[MAXBUF];
//    UINT packet_len;
//    DIVERT_ADDRESS recv_addr, send_addr;
//    PDIVERT_IPHDR ip_header;
//    PDIVERT_IPV6HDR ipv6_header;
//    PDIVERT_ICMPHDR icmp_header;
//    PDIVERT_ICMPV6HDR icmpv6_header;
//    PDIVERT_TCPHDR tcp_header;
//    PDIVERT_UDPHDR udp_header;
//    UINT payload_len;
//    
//    
//	TCPPACKET reset0;
//    PTCPPACKET reset = &reset0;
//    UINT8 dnr0[sizeof(ICMPPACKET) + 0x0F*sizeof(UINT32) + 8 + 1];
//    PICMPPACKET dnr = (PICMPPACKET)dnr0;
//
//    TCPV6PACKET resetv6_0;
//    PTCPV6PACKET resetv6 = &resetv6_0;
//    UINT8 dnrv6_0[sizeof(ICMPV6PACKET) + sizeof(DIVERT_IPV6HDR) +
//        sizeof(DIVERT_TCPHDR)];
//    PICMPV6PACKET dnrv6 = (PICMPV6PACKET)dnrv6_0;
//	//fr = fopen("dump.txt", "w");
//    // Check arguments.
//	argc=2;
//	argv[1]="outbound";
//	//argv[1]="ip.DstAddr=173.194.72.94";
//	//argv[1]="ip.SrcAddr=172.16.101.219";
//	//argv[1]="ip.SrcAddr=192.168.0.19";
//    switch (argc)
//    {
//        case 2:
//            break;
//        case 3:
//            priority = (INT16)atoi(argv[2]);
//            break;
//        default:
//            fprintf(stderr, "usage: %s divert-filter [priority]\n",
//                argv[0]);
//            fprintf(stderr, "examples:\n");
//            fprintf(stderr, "\t%s true\n", argv[0]);
//            fprintf(stderr, "\t%s \"outbound and tcp.DstPort == 80\" 1000\n",
//                argv[0]);
//            fprintf(stderr, "\t%s \"inbound and tcp.Syn\" -4000\n", argv[0]);
//            exit(EXIT_FAILURE);
//    }
//
//    // Initialize all packets.
//    PacketIpTcpInit(reset);
//    //reset->tcp.Rst = 1;
//    //reset->tcp.Ack = 1;
//    PacketIpIcmpInit(dnr);
//    //dnr->icmp.Type = 3;         // Destination not reachable.
//    //dnr->icmp.Code = 3;         // Port not reachable.
//    PacketIpv6TcpInit(resetv6);
//    //resetv6->tcp.Rst = 1;
//    //resetv6->tcp.Ack = 1;
//    PacketIpv6Icmpv6Init(dnrv6);
//    //dnrv6->ipv6.Length = htons(sizeof(DIVERT_ICMPV6HDR) + 4 +
//    //    sizeof(DIVERT_IPV6HDR) + sizeof(DIVERT_TCPHDR));
//    //dnrv6->icmpv6.Type = 1;     // Destination not reachable.
//    //dnrv6->icmpv6.Code = 4;     // Port not reachable.
//
//    // Get console for pretty colors.
//    console = GetStdHandle(STD_OUTPUT_HANDLE);
//
//    // Divert traffic matching the filter:
//    handle = DivertOpen("outbound", (DIVERT_LAYER)0, 0, 0);
//    if (handle == INVALID_HANDLE_VALUE)
//    {
//        if (GetLastError() == ERROR_INVALID_PARAMETER)
//        {
//            fprintf(stderr, "error: filter syntax error\n");
//            exit(EXIT_FAILURE);
//        }
//        fprintf(stderr, "error: failed to open Divert device (%d)\n",
//            GetLastError());
//        exit(EXIT_FAILURE);
//    }
//
//    // Main loop:
//    while (TRUE)
//    {
//        // Read a matching packet.
//        if (!DivertRecv(handle, packet, sizeof(packet), &recv_addr,
//                &packet_len))
//        {
//            fprintf(stderr, "warning: failed to read packet\n");
//            continue;
//		}
//
//
//		fprintf(fr,"원본 패킷의 길이는 %d", packet_len);
//		fprintf(fr,"\n↓↓↓↓↓↓↓↓↓↓↓원본패킷↓↓↓↓↓↓↓↓↓↓↓");
//		for (i = 0; i < packet_len; i++)
//		{
//			if (i % 20 == 0)
//			{
//				fprintf(fr,"\n");
//			}
//			fprintf(fr,"%.2X ", (UINT8)packet[i]);
//		}
//		fprintf(fr,"\n↑↑↑↑↑↑↑↑↑↑↑원본패킷↑↑↑↑↑↑↑↑↑↑↑\n\n\n");
//
//
//
//        // Print info about the matching packet.
//        DivertHelperParse(packet, packet_len, &ip_header, &ipv6_header,
//            &icmp_header, &icmpv6_header, &tcp_header, &udp_header, NULL,
//            &payload_len);
//        if (ip_header == NULL && ipv6_header == NULL)
//        {
//            continue;
//        }
//
//        // Dump packet info: 
//        SetConsoleTextAttribute(console, FOREGROUND_RED);
//        fputs("BLOCK ", stdout);
//        SetConsoleTextAttribute(console,
//            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//        if (ip_header != NULL)
//        {
//            UINT8 *src_addr = (UINT8 *)&ip_header->SrcAddr;
//            UINT8 *dst_addr = (UINT8 *)&ip_header->DstAddr;
//			printf("\nIPv4 [Version=%u HdrLength=%u TOS=%u Length=%u Id=0x%.4X "
//                "Reserved=%u DF=%u MF=%u FragOff=%u TTL=%u Protocol=%u "
//                "Checksum=0x%.4X SrcAddr=%u.%u.%u.%u DstAddr=%u.%u.%u.%u]\n",
//                ip_header->Version, ip_header->HdrLength,
//                ntohs(ip_header->TOS), ntohs(ip_header->Length),
//                ntohs(ip_header->Id), DIVERT_IPHDR_GET_RESERVED(ip_header),
//                DIVERT_IPHDR_GET_DF(ip_header), DIVERT_IPHDR_GET_MF(ip_header),
//                ntohs(DIVERT_IPHDR_GET_FRAGOFF(ip_header)), ip_header->TTL,
//                ip_header->Protocol, ntohs(ip_header->Checksum),
//                src_addr[0], src_addr[1], src_addr[2], src_addr[3],
//                dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3]);
//        }
//        if (ipv6_header != NULL)
//        {
//            UINT16 *src_addr = (UINT16 *)&ipv6_header->SrcAddr;
//            UINT16 *dst_addr = (UINT16 *)&ipv6_header->DstAddr;
//            fputs("ipv6.SrcAddr=", stdout);
//            for (i = 0; i < 8; i++)
//            {
//                printf("%x%c", ntohs(src_addr[i]), (i == 7? ' ': ':'));
//            } 
//            fputs(" ipv6.DstAddr=", stdout);
//            for (i = 0; i < 8; i++)
//            {
//                printf("%x%c", ntohs(dst_addr[i]), (i == 7? ' ': ':'));
//            }
//            putchar(' ');
//        }
//        if (icmp_header != NULL)
//        {
//            printf("icmp.Type=%u icmp.Code=%u ",
//                icmp_header->Type, icmp_header->Code);
//            // Simply drop ICMP
//        }
//        if (icmpv6_header != NULL)
//        {
//            printf("icmpv6.Type=%u icmpv6.Code=%u ",
//                icmpv6_header->Type, icmpv6_header->Code);
//            // Simply drop ICMPv6
//		}
//
//
//
//
//		if (tcp_header != NULL)
//		{
//			//printf("tcp.SrcPort=%u tcp.DstPort=%u tcp.Flags=",
//   //             ntohs(tcp_header->SrcPort), ntohs(tcp_header->DstPort));
//   //         if (tcp_header->Fin)
//   //         {
//   //             fputs("[FIN]", stdout);
//   //         }
//   //         if (tcp_header->Rst)
//   //         {
//   //             fputs("[RST]", stdout);
//   //         }
//   //         if (tcp_header->Urg)
//   //         {
//   //             fputs("[URG]", stdout);
//   //         }
//   //         if (tcp_header->Syn)
//   //         {
//   //             fputs("[SYN]", stdout);
//   //         }
//   //         if (tcp_header->Psh)
//   //         {
//   //             fputs("[PSH]", stdout);
//   //         }
//   //         if (tcp_header->Ack)
//   //         {
//   //             fputs("[ACK]", stdout);
//   //         }
//   //         putchar(' ');
//
//
//
//
//
//
//
//
//
//
//
//
//
//            //if (0)
//			if (ip_header != NULL)
//            {
//				UINT8 *src_addr;
//				UINT8 *dst_addr;
//				UINT8 *test;
//
//				
//                //reset->ip.SrcAddr = ip_header->DstAddr;
//                //reset->ip.DstAddr = ip_header->SrcAddr;
//                //reset->tcp.SrcPort = tcp_header->DstPort;
//                //reset->tcp.DstPort = tcp_header->SrcPort;
//                //reset->tcp.SeqNum = 
//                //    (tcp_header->Ack? tcp_header->AckNum: 0);
//                //reset->tcp.AckNum =
//                //    (tcp_header->Syn?
//                //        htonl(ntohl(tcp_header->SeqNum) + 1):
//                //        htonl(ntohl(tcp_header->SeqNum) + payload_len));
//
//				memset(reset, 0, packet_len);
//				memcpy(reset, packet, packet_len);
//
//				//reset->ip.Checksum = 10;
//				//reset->ip.DstAddr = ip_header->DstAddr;
//				//reset->ip.FragOff0 = ip_header->FragOff0;
//				//reset->ip.HdrLength = ip_header->HdrLength;
//				//reset->ip.HdrLength = 7;
//				//reset->ip.Id = ip_header->Id;
//				//reset->ip.Length = ip_header->Length;
//				//reset->ip.Protocol = ip_header->Protocol;
//				//reset->ip.SrcAddr = ip_header->SrcAddr;
//				//reset->ip.TOS = ip_header->TOS;
//				reset->ip.TOS = 170;
//				//reset->ip.TTL = ip_header->TTL;
//				//reset->ip.Version = ip_header->Version;
//
//
//				//reset->tcp.Ack = tcp_header->Ack;
//				//reset->tcp.AckNum = tcp_header->AckNum;
//				//reset->tcp.Checksum = tcp_header->Checksum;
//				//reset->tcp.DstPort = tcp_header->DstPort;
//				//reset->tcp.Fin = tcp_header->Fin;
//				//reset->tcp.HdrLength = tcp_header->HdrLength;
//				//reset->tcp.Psh = tcp_header->Psh;
//				//reset->tcp.Reserved1 = tcp_header->Reserved1;
//				//reset->tcp.Reserved2 = tcp_header->Reserved2;
//				//reset->tcp.Rst = tcp_header->Rst;
//				//reset->tcp.SeqNum = tcp_header->SeqNum;
//				//reset->tcp.SrcPort = tcp_header->SrcPort;
//				//reset->tcp.Syn = tcp_header->Syn;
//				//reset->tcp.Urg = tcp_header->Urg;
//				//reset->tcp.UrgPtr = tcp_header->UrgPtr;
//				//reset->tcp.Window = tcp_header->Window;
//
//				//DivertHelperCalcChecksums((PVOID)reset, sizeof(TCPPACKET), 0);
//				
//				
//				
//				
//
//
//				src_addr = (UINT8 *)&reset->ip.SrcAddr;
//				dst_addr = (UINT8 *)&reset->ip.DstAddr;
//
//
//				printf("\n\nIPv4 [Version=%u HdrLength=%u TOS=%u Length=%u Id=0x%.4X "
//					"Reserved=%u DF=%u MF=%u FragOff=%u TTL=%u Protocol=%u "
//					"Checksum=0x%.4X SrcAddr=%u.%u.%u.%u DstAddr=%u.%u.%u.%u]\n",
//					reset->ip.Version, 
//					reset->ip.HdrLength,
//					ntohs(reset->ip.TOS), 
//					ntohs(reset->ip.Length),
//					ntohs(reset->ip.Id), 
//					DIVERT_IPHDR_GET_RESERVED(&reset->ip),
//					DIVERT_IPHDR_GET_DF(&reset->ip), 
//					DIVERT_IPHDR_GET_MF(&reset->ip),
//					ntohs(DIVERT_IPHDR_GET_FRAGOFF(&reset->ip)), 
//					reset->ip.TTL,
//					reset->ip.Protocol, 
//					ntohs(reset->ip.Checksum),
//					src_addr[0], src_addr[1], src_addr[2], src_addr[3],
//					dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3]);
//
//
//
//				
//
//                memcpy(&send_addr, &recv_addr, sizeof(recv_addr));
//                send_addr.Direction = recv_addr.Direction;
//				send_addr.IfIdx = recv_addr.IfIdx;
//				send_addr.SubIfIdx = recv_addr.SubIfIdx;
//
//				
//				test = (UINT8 *)reset;
//
//
//
//				fprintf(fr,"복사 후 패킷의 길이는 %d", packet_len);
//				fprintf(fr,"\n↓↓↓↓↓↓↓↓↓↓↓복사패킷↓↓↓↓↓↓↓↓↓↓↓");
//				for (i = 0; i < packet_len; i++)
//				{
//					if (i % 20 == 0)
//					{
//						fprintf(fr,"\n");
//					}
//					fprintf(fr,"%.2X ", test[i]);
//				}
//
//				fprintf(fr,"\n↑↑↑↑↑↑↑↑↑↑↑복사 후 패킷↑↑↑↑↑↑↑↑↑↑↑\n\n\n");
//
//
//
//
//				if (!DivertSend(handle, reset, packet_len,
//                        &send_addr, NULL))
//                {
//                    fprintf(stderr, "warning: failed to send TCP reset (%d)\n",
//                        GetLastError());
//                } else {
//					printf("\nipv4 헤더 본론 인젝션\n");
//				}
//				//if (!DivertSend(handle, packet, packet_len, &recv_addr, NULL))
//				//{
//				//	fprintf(stderr, "warning: failed to reinject packet (%d)\n",
//				//		GetLastError());
//				//}
//            }else {
//
//				if (!DivertSend(handle, packet, packet_len, &recv_addr, NULL))
//				{
//					fprintf(stderr, "warning: failed to reinject packet (%d)\n",
//						GetLastError());
//				}
//				printf("\nipv4 헤더 기타로 포워딩\n");
//
//			}
//
//        //    if (ipv6_header != NULL)
//        //    {
//        //        memcpy(resetv6->ipv6.SrcAddr, ipv6_header->DstAddr,
//        //            sizeof(resetv6->ipv6.SrcAddr));
//        //        memcpy(resetv6->ipv6.DstAddr, ipv6_header->SrcAddr,
//        //            sizeof(resetv6->ipv6.DstAddr));
//        //        resetv6->tcp.SrcPort = tcp_header->DstPort;
//        //        resetv6->tcp.DstPort = tcp_header->SrcPort;
//        //        resetv6->tcp.SeqNum =
//        //            (tcp_header->Ack? tcp_header->AckNum: 0);
//        //        resetv6->tcp.AckNum =
//        //            (tcp_header->Syn?
//        //                htonl(ntohl(tcp_header->SeqNum) + 1):
//        //                htonl(ntohl(tcp_header->SeqNum) + payload_len));
//
//        //        DivertHelperCalcChecksums((PVOID)resetv6, sizeof(TCPV6PACKET),
//        //            0);
//        //        
//        //        memcpy(&send_addr, &recv_addr, sizeof(send_addr));
//        //        send_addr.Direction = !recv_addr.Direction;
//        //        if (!DivertSend(handle, (PVOID)resetv6, sizeof(TCPV6PACKET),
//        //                &send_addr, NULL))
//        //        {
//        //            fprintf(stderr, "warning: failed to send TCP (IPV6) "
//        //                "reset (%d)\n", GetLastError());
//        //        }
//        //    }
//        //}
//        //if (udp_header != NULL)
//        //{
//        //    printf("udp.SrcPort=%u udp.DstPort=%u ",
//        //        ntohs(udp_header->SrcPort), ntohs(udp_header->DstPort));
//        //
//        //    if (ip_header != NULL)
//        //    {
//        //        // NOTE: For some ICMP error messages, WFP does not seem to
//        //        //       support INBOUND injection.  As a work-around, we
//        //        //       always inject OUTBOUND.
//        //        UINT icmp_length = ip_header->HdrLength*sizeof(UINT32) + 8;
//        //        memcpy(dnr->data, ip_header, icmp_length);
//        //        icmp_length += sizeof(ICMPPACKET);
//        //        dnr->ip.Length = htons((UINT16)icmp_length);
//        //        dnr->ip.SrcAddr = ip_header->DstAddr;
//        //        dnr->ip.DstAddr = ip_header->SrcAddr;
//        //        
//        //        DivertHelperCalcChecksums((PVOID)dnr, icmp_length, 0);
//        //        
//        //        memcpy(&send_addr, &recv_addr, sizeof(send_addr));
//        //        send_addr.Direction = DIVERT_DIRECTION_OUTBOUND;
//        //        if (!DivertSend(handle, (PVOID)dnr, icmp_length, &send_addr,
//        //            NULL))
//        //        {
//        //            fprintf(stderr, "warning: failed to send ICMP message "
//        //                "(%d)\n", GetLastError());
//        //        }
//        //    }
//        //
//        //    if (ipv6_header != NULL)
//        //    {
//        //        UINT icmpv6_length = sizeof(DIVERT_IPV6HDR) +
//        //            sizeof(DIVERT_TCPHDR);
//        //        memcpy(dnrv6->data, ipv6_header, icmpv6_length);
//        //        icmpv6_length += sizeof(ICMPV6PACKET);
//        //        memcpy(dnrv6->ipv6.SrcAddr, ipv6_header->DstAddr,
//        //            sizeof(dnrv6->ipv6.SrcAddr));
//        //        memcpy(dnrv6->ipv6.DstAddr, ipv6_header->SrcAddr,
//        //            sizeof(dnrv6->ipv6.DstAddr));
//        //        
//        //        DivertHelperCalcChecksums((PVOID)dnrv6, icmpv6_length, 0);
//
//        //        memcpy(&send_addr, &recv_addr, sizeof(send_addr));
//        //        send_addr.Direction = DIVERT_DIRECTION_OUTBOUND;
//        //        if (!DivertSend(handle, (PVOID)dnrv6, icmpv6_length,
//        //                &send_addr, NULL))
//        //        {
//        //            fprintf(stderr, "warning: failed to send ICMPv6 message "
//        //                "(%d)\n", GetLastError());
//        //        }
//        //    }
//        } else {
//			if (!DivertSend(handle, packet, packet_len, &recv_addr, NULL))
//			{
//				fprintf(stderr, "warning: failed to reinject packet (%d)\n",
//					GetLastError());
//			} else {
//				printf("\n전체 기타로 포워딩!!!!!!\n");
//			}
//			
//		}
//
//
//
//
//
//		putchar('\n');
//    }
//
//
//	fclose(fr);
//}
//
///*
// * Initialize a PACKET.
// */
//static void PacketIpInit(PDIVERT_IPHDR packet)
//{
//    memset(packet, 0, sizeof(DIVERT_IPHDR));
//    packet->Version = 4;
//    packet->HdrLength = sizeof(DIVERT_IPHDR) / sizeof(UINT32);
//    packet->Id = ntohs(0xDEAD);
//    packet->TTL = 64;
//}
//
///*
// * Initialize a TCPPACKET.
// */
//static void PacketIpTcpInit(PTCPPACKET packet)
//{
//    memset(packet, 0, sizeof(TCPPACKET));
//    PacketIpInit(&packet->ip);
//    packet->ip.Length = htons(sizeof(TCPPACKET));
//    packet->ip.Protocol = IPPROTO_TCP;
//    packet->tcp.HdrLength = sizeof(DIVERT_TCPHDR) / sizeof(UINT32);
//}
//
///*
// * Initialize an ICMPPACKET.
// */
//static void PacketIpIcmpInit(PICMPPACKET packet)
//{
//    memset(packet, 0, sizeof(ICMPPACKET));
//    PacketIpInit(&packet->ip);
//    packet->ip.Protocol = IPPROTO_ICMP;
//}
//
///*
// * Initialize a PACKETV6.
// */
//static void PacketIpv6Init(PDIVERT_IPV6HDR packet)
//{
//    memset(packet, 0, sizeof(DIVERT_IPV6HDR));
//    packet->Version = 6;
//    packet->HopLimit = 64;
//}
//
///*
// * Initialize a TCPV6PACKET.
// */
//static void PacketIpv6TcpInit(PTCPV6PACKET packet)
//{
//    memset(packet, 0, sizeof(TCPV6PACKET));
//    PacketIpv6Init(&packet->ipv6);
//    packet->ipv6.Length = htons(sizeof(DIVERT_TCPHDR));
//    packet->ipv6.NextHdr = IPPROTO_TCP;
//    packet->tcp.HdrLength = sizeof(DIVERT_TCPHDR) / sizeof(UINT32);
//}
//
///*
// * Initialize an ICMP PACKET.
// */
//static void PacketIpv6Icmpv6Init(PICMPV6PACKET packet)
//{
//    memset(packet, 0, sizeof(ICMPV6PACKET));
//    PacketIpv6Init(&packet->ipv6);
//    packet->ipv6.NextHdr = IPPROTO_ICMPV6;
//}