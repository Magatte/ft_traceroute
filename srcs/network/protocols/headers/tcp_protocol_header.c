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

void		update_tcp_checksum(t_message *message, t_trace *trace,\
			void *ptr_message, size_t iphdr_size, size_t size)
{
	ft_memcpy(ptr_message + iphdr_size, &message->tcp_header, trace->protocol->len);
	ft_memset(ptr_message + iphdr_size + trace->protocol->len, '0', size);
	message->tcp_header.checksum = checksum(ptr_message + iphdr_size, trace->protocol->len + size);
	ft_memcpy(ptr_message + iphdr_size, &message->tcp_header, trace->protocol->len);
}
