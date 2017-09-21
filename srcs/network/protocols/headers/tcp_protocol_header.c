/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcp_protocol_header.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/19 00:10:30 by jguyet            #+#    #+#             */
/*   Updated: 2017/09/19 00:10:31 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

//                                           TCP HEADER
///////////////////////////////////////////////////////////////////////////////////////////////////
//00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31//
//               Source Port                     |               Destination Port                //
//                                         Sequence number                                       //
//                                      Acknowledgment Number                                    //
//Data Offset|reserved|  ECN   |   Control Bits  |                    Window                     //
//                Checksum                       |                 Urgent Pointer                //
//                                      Options and padding :::                                  //
//                                            Data :::                                           //
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
** ----------------------------------------------
** Source Port. 16 bits.
** ----------------------------------------------
** Destination Port. 16 bits.
** ----------------------------------------------
** Sequence Number. 32 bits.
** The sequence number of the first data byte in this segment. If the SYN bit is set,<br>
** the sequence number is the initial sequence number and the first data byte is initial<br>
** sequence number + 1.
** ----------------------------------------------
** Acknowledgment Number. 32 bits.
** If the ACK bit is set, this field contains the value of the next sequence number the<br>
** sender of the segment is expecting to receive. Once a connection is established this is always sent.
** ----------------------------------------------
** Data Offset. 4 bits.
** The number of 32-bit words in the TCP header. This indicates where the data begins.<br>
** The length of the TCP header is always a multiple of 32 bits.
** ----------------------------------------------
** reserved. 3 bits.
** Must be cleared to zero.
** ----------------------------------------------
** ECN, Explicit Congestion Notification. 3 bits.
** Added in RFC 3168.
**
** 00|01|02
** N |C |E
** N, NS, Nonce Sum. 1 bit.
** Added in RFC 3540. This is an optional field added to ECN intended to protect<br>
** against accidental or malicious concealment of marked packets from the TCP sender.
**
** C, CWR. 1 bit.
** E, ECE, ECN-Echo. 1 bit.
** ----------------------------------------------
** Control Bits. 6 bits.
** 
** 00	01	02	03	04	05
** U	A	P	R	S	F
** U, URG. 1 bit.
** Urgent pointer valid flag.
** 
** A, ACK. 1 bit.
** Acknowledgment number valid flag.
** 
** P, PSH. 1 bit.
** Push flag.
** 
** R, RST. 1 bit.
** Reset connection flag.
** 
** S, SYN. 1 bit.
** Synchronize sequence numbers flag.
** 
** F, FIN. 1 bit.
** End of data flag.
** ----------------------------------------------
** Window. 16 bits, unsigned.
** The number of data bytes beginning with the one indicated in the acknowledgment field<br>
** which the sender of this segment is willing to accept.
** ----------------------------------------------
** Checksum. 16 bits.
** This is computed as the 16-bit one's complement of the one's complement sum<br>
** of a pseudo header of information from the IP header, the TCP header, and the data,<br>
** padded as needed with zero bytes at the end to make a multiple of two bytes.<br>
** The pseudo header contains the following fields:
** ------------------------------------------------------------------------------------------------
** 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31-
**                                        Source IP address                                       -
**                                      Destination IP address                                    -
**           0            |       IP Protocol     |                  Total length                 -
** ------------------------------------------------------------------------------------------------
** ----------------------------------------------
** Urgent Pointer. 16 bits, unsigned.
** If the URG bit is set, this field points to the sequence number of the last byte<br>
** in a sequence of urgent data.
** ----------------------------------------------
**
** Options. 0 to 40 bytes.
** Options occupy space at the end of the TCP header. All options are included in the checksum.<br>
** An option may begin on any byte boundary. The TCP header must be padded with zeros<br>
** to make the header length a multiple of 32 bits.
** |Kind | Length | Description                      | References        |
** |0    | 1      | End of option list.              | RFC 793           |
** |1    | 1      | No operation.                    | RFC 793           |
** |2    | 4      | MSS, Maximum Segement Size.      | RFC 793           |
** |3    | 3      | WSOPT, Window scale factor.      | RFC 1323          |
** |4    | 2      | SACK permitted.                  | RFC 2018          |
** |5    | Var.   | SACK.                            | RFC 2018, RFC 2883|
** |6    | 6      | Echo. (obsolete)                 | RFC 1072          |
** |7    | 6      | Echo reply (obsolete)            | RFC 1072          |
** |8    | 10     | TSOPT, Timestamp.                | RFC 1323          |
** |9    | 2      | Partial Order Connection perm... | RFC 1693          |
** |10   | 3      | Partial Order service profile.   | RFC 1693          |
** |11   | 6      | CC, Connection Count.            | RFC 1644          |
** |12   | 6      | CC.NEW                           | RFC 1644          |
** |13   | 6      | CC.ECHO                          | RFC 1644          |
** |14   | 3      | Alternate checksum request.      | RFC 1146          |
** |15   | Var.   | Alternate checksum data.         | RFC 1146          |
** |16   |        | Skeeter                          |                   |
** |17   |        | Bubba.                           |                   |
** |18   | 3      | Trailer Checksum Option          |                   |
** |19   | 18     | MD5 signature.                   | RFC 2385          |
** |20   |        | SCPS Capabilities.               |                   |
** |21   |        | Selective Negative Acknowledge...|                   |
** |22   |        | Record Boundaries.               |                   |
** |23   |        | Corruption experienced.          |                   |
** |24   |        | SNAP.                            |                   |
** |25   |        |                                  |                   |
** |26   |        | TCP Compression Filter.          |                   |
** |27   | 8      | Quick-Start Response.            | RFC 4782          |
** |28   | 4      | User Timeout.                    | RFC 5482          |
** |29   |        | TCP-AO, TCP Authentification opt | RFC 5925          |
** |30   |        | MPTCP                            |                   |
** |31   |        |                                  |                   |
** | -   |        |                                  |                   |
** |252  |        |                                  |                   |
** |253  |        | RFC3692-style Experiment 1.      | RFC 4727          |
** |254  |        | RFC3692-style Experiment 2.      | RFC 4727          |
** |255  |        |                                  |                   |
**
** TCP State machine:
**
** CLOSE-WAIT	Waits for a connection termination request from the remote host.
** CLOSED	    Represents no connection state at all.
** CLOSING	    Waits for a connection termination request acknowledgment from the remote host.
** ESTABLISHED	Represents an open connection, data received can be delivered to the user. The normal state for the data transfer phase of the connection.
** FIN-WAIT-1	Waits for a connection termination request from the remote host or an acknowledgment of the connection termination request previously sent.
** FIN-WAIT-2	Waits for a connection termination request from the remote host.
** LAST-ACK	    Waits for an acknowledgment of the connection termination request previously sent to the remote host (which includes an acknowledgment of its connection termination request).
** LISTEN	    Waits for a connection request from any remote TCP and port.
** SYN-RECEIVED	Waits for a confirming connection request acknowledgment after having both received and sent a connection request.
** SYN-SENT	    Waits for a matching connection request after having sent a connection request.
** TIME-WAIT	Waits for enough time to pass to be sure the remote host received the acknowledgment of its connection termination request.
*/
void		prepare_tcp_header(t_message *message, t_trace *trace)
{
	message->tcp_header.source = htons(trace->pid);
	message->tcp_header.dest = htons(trace->port + trace->sequence);
	message->tcp_header.seq = htons(trace->sequence);
	message->tcp_header.ack_seq = htonl(1);
	message->tcp_header.doff = 5;//tcp header size
	message->tcp_header.fin = 0;
	message->tcp_header.syn = 1;
	message->tcp_header.rst = 0;
	message->tcp_header.psh = 0;
	message->tcp_header.ack = 0;
	message->tcp_header.urg = 0;
	message->tcp_header.window = htons(5840);/* maximum allowed window size */
	message->tcp_header.check = 0;
	message->tcp_header.urg_ptr = 0;
	add_tcp_options(message, trace);
}

void		add_tcp_options(t_message *message, t_trace *trace)
{
	int		options_size;
	char	opt;
	size_t		iphdr_size = 0;

	if (trace->use_ip_header)
		iphdr_size = IPHDR_SIZE;
	options_size = 2;
	opt = 1;
	ft_memcpy(message->data + iphdr_size + trace->protocol->len, &opt, 1);
	//End of option list
	opt = 0;
	ft_memcpy(message->data + iphdr_size + trace->protocol->len + 1, &opt, 1);
	message->packet_len += options_size;
	message->len += options_size;
}

void		serialize_tcp_header(t_message *message, t_trace *trace, size_t iphdr_size)
{
	int psize = sizeof(struct pseudoheader) + sizeof(struct tcphdr) + message->packet_len;
	char tcpcsumblock[psize];

	ft_memcpy(message->data + iphdr_size, &message->tcp_header, trace->protocol->len);
	ft_memset(tcpcsumblock, 0, psize);

	message->pseudoheader.src = message->ip_header.src.s_addr;
	message->pseudoheader.dst = message->ip_header.dest.s_addr;
	message->pseudoheader.zero = 0;
	message->pseudoheader.protocol = trace->protocol->proto;
	message->pseudoheader.tcp_length = htons(trace->protocol->len + message->packet_len);

	ft_memcpy(tcpcsumblock, &message->pseudoheader, sizeof(struct pseudoheader));
  	ft_memcpy(tcpcsumblock + sizeof(struct pseudoheader), &message->tcp_header, sizeof(struct tcphdr));
	//add payload
	//ft_memcpy(tcpcsumblock + sizeof(struct pseudoheader), &message->tcp_header, message->packet_len);

	message->tcp_header.check = checksum(tcpcsumblock,  psize);

	ft_memcpy(message->data + iphdr_size, &message->tcp_header, trace->protocol->len);
}

void		deserialize_tcp_header(t_message *message, t_trace *trace)
{
	ft_memcpy(&message->tcp_header, message->data, trace->protocol->len);
	message->data = message->data + trace->protocol->len;
}
