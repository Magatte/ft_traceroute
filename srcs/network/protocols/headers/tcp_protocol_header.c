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

#define TCPSYN_LEN 20

/* This piece of code has been used many times in a lot of differents tools. */
/* I haven't been able to determine the author of the code but it looks like */
/* this is a public domain implementation of the checksum algorithm */
unsigned short in_cksum(unsigned short *addr,int len){
    
register int sum = 0;
u_short answer = 0;
register u_short *w = addr;
register int nleft = len;
    
/*
* Our algorithm is simple, using a 32-bit accumulator (sum),
* we add sequential 16-bit words to it, and at the end, fold back 
* all the carry bits from the top 16 bits into the lower 16 bits. 
*/
    
while (nleft > 1) {
sum += *w++;
nleft -= 2;
}

/* mop up an odd byte, if necessary */
if (nleft == 1) {
*(u_char *)(&answer) = *(u_char *)w ;
sum += answer;
}

/* add back carry outs from top 16 bits to low 16 bits */
sum = (sum >> 16) + (sum &0xffff); /* add hi 16 to low 16 */
sum += (sum >> 16); /* add carry */
answer = ~sum; /* truncate to 16 bits */
return(answer);

} /* End of in_cksum() */

void		prepare_tcp_header(t_message *message, t_trace *trace)
{
	message->tcp_header.source = htons(trace->pid);
	message->tcp_header.dest = htons(trace->port + trace->sequence);
	message->tcp_header.sequence = trace->sequence;
	message->tcp_header.ack = htonl(1);
	message->tcp_header.th_off = 5;
	message->tcp_header.flags = TH_SYN;
	message->tcp_header.checksum = 0;
}

void		update_tcp_checksum(t_message *message, t_trace *trace,\
			void *ptr_message, size_t iphdr_size, size_t size)
{
	(void)message;
	(void)trace;
	(void)ptr_message;
	(void)iphdr_size;
	(void)size;
# ifdef __linux__
	ft_memcpy(ptr_message + iphdr_size, &message->tcp_header, trace->protocol->len);
	//ft_memset(ptr_message + iphdr_size + trace->protocol->len, '0', size);

		 /* TCP Pseudoheader + TCP actual header used for computing the checksum */
  	char tcpcsumblock[ sizeof(struct pseudoheader) + sizeof(struct tcphdr)];

	message->pseudoheader.src = message->ip_header.src.s_addr;
	message->pseudoheader.dst = message->ip_header.dest.s_addr;
	message->pseudoheader.zero = 0;
	message->pseudoheader.protocol = message->ip_header.protocol;
	message->pseudoheader.tcplen = htons(sizeof(struct tcphdr));

	ft_memcpy(tcpcsumblock, &message->pseudoheader, sizeof(struct pseudoheader));
  	ft_memcpy(tcpcsumblock + sizeof(struct pseudoheader), &message->tcp_header, sizeof(struct tcphdr));

	message->tcp_header.checksum = checksum(tcpcsumblock,  sizeof(tcpcsumblock));

	ft_memcpy(ptr_message + iphdr_size, &message->tcp_header, trace->protocol->len);
# endif
}
