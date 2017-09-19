/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gre_protocol_header.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/19 00:10:00 by jguyet            #+#    #+#             */
/*   Updated: 2017/09/19 00:10:01 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

void		prepare_gre_header(t_message *message, t_trace *trace)
{
	message->gre_header.flags = htons(0x2001);
	message->gre_header.proto = htons(trace->port);
	message->gre_header.length = 0;
	message->gre_header.callid = htons(trace->pid + trace->sequence);
}

void		update_gre_checksum(t_message *message, t_trace *trace, size_t iphdr_size)
{
	ft_memcpy(message->data + iphdr_size, &message->gre_header, trace->protocol->len);
	ft_memset(message->data + iphdr_size + trace->protocol->len, '0', message->packet_len);
}
