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

void		prepare_tcp_header(t_message *message, t_trace *trace)
{
	message->tcp_header.source = htons(trace->pid);
	message->tcp_header.dest = htons(trace->port + trace->sequence);
	message->tcp_header.sequence = (message->tcp_header.source << 16) | message->tcp_header.dest;
	message->tcp_header.ack = 0;
	message->tcp_header.th_off = 5;
	message->tcp_header.flags = TH_SYN;
	message->tcp_header.checksum = 0;
}

uint16_t	tcp_checksum(unsigned short len_tcp,
						     uint32_t saddr,
						     uint32_t daddr,
						     struct tcphdr *tcp_pkt)
{
	unsigned short *src_addr = (unsigned short *)&saddr;
	unsigned short *dest_addr = (unsigned short *)&daddr;

	unsigned char prot_tcp = 6;
	unsigned long sum = 0;
	int nleft = len_tcp;
	unsigned short *w;

	w = (unsigned short *)tcp_pkt;
	// calculate the checksum for the tcp header and tcp data
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}
	// if nleft is 1 there ist still on byte left.
	// We add a padding byte (0xFF) to build a 16bit word
	if (nleft > 0) {
		sum += *w & ntohs(0xFF00);
	}
	// add the pseudo header
	sum += src_addr[0];
	sum += src_addr[1];
	sum += dest_addr[0];
	sum += dest_addr[1];
	sum += htons(len_tcp);
	sum += htons(prot_tcp);
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	// Take the one's complement of sum
	return (unsigned short)(~sum);
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
	ft_memset(ptr_message + iphdr_size + trace->protocol->len, '0', size);
	message->tcp_header.checksum = tcp_checksum(trace->protocol->len, message->ip_header.src.s_addr,\
		message->ip_header.dest.s_addr, &message->tcp_header);//checksum(trace->protocol->len, trace->protocol->len + size);
	ft_memcpy(ptr_message + iphdr_size, &message->tcp_header, trace->protocol->len);
# endif
}
