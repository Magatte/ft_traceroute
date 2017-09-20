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
	message->tcp_header.sequence = htons(trace->sequence);
	message->tcp_header.ack = htonl(1);
	message->tcp_header.th_off = 5;
	message->tcp_header.flags = TH_SYN;
	message->tcp_header.checksum = 0;
}

void		serialize_tcp_header(t_message *message, t_trace *trace, size_t iphdr_size)
{
	if (!trace->use_ip_header)
		return ;
	ft_memcpy(message->data + iphdr_size, &message->tcp_header, trace->protocol->len);
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

	ft_memcpy(message->data + iphdr_size, &message->tcp_header, trace->protocol->len);
}

void		deserialize_tcp_header(t_message *message, t_trace *trace)
{
	ft_memcpy(&message->tcp_header, message->data, trace->protocol->len);
	message->data = message->data + trace->protocol->len;
}
