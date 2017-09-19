/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   udp_protocol_header.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/19 00:10:16 by jguyet            #+#    #+#             */
/*   Updated: 2017/09/19 00:10:17 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

void		prepare_udp_header(t_message *message, t_trace *trace)
{
	message->udp_header.source = htons(trace->pid);
	message->udp_header.dest = htons(trace->port + trace->sequence);
	message->udp_header.len = htons((u_short)(message->len - sizeof(struct iphdr)));
	message->udp_header.checksum = 0;
	message->udp_header.sec = trace->sequence;
	message->udp_header.ttl = trace->ttl;
	message->udp_header.tv = trace->timeout;
}

void		update_udp_checksum(t_message *message, t_trace *trace, size_t iphdr_size)
{
	ft_memcpy(message->data + iphdr_size, &message->udp_header, trace->protocol->len);
	ft_memset(message->data + iphdr_size + trace->protocol->len, '0', message->packet_len);
	message->udp_header.checksum = checksum(message->data + iphdr_size, trace->protocol->len + message->packet_len);
	ft_memcpy(message->data + iphdr_size, &message->udp_header, trace->protocol->len);
}