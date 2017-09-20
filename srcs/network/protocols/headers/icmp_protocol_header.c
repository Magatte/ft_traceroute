/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_protocol_header.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/19 00:10:22 by jguyet            #+#    #+#             */
/*   Updated: 2017/09/19 00:10:24 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

void		prepare_icmp_header(t_message *message, t_trace *trace)
{
	message->icmp_header.type = ICMP_ECHO;
	message->icmp_header.un.echo.id = htons(trace->pid);
	message->icmp_header.un.echo.sequence = htons(trace->sequence);
	message->icmp_header.checksum = 0;
}

void		serialize_icmp_header(t_message *message, t_trace *trace, size_t iphdr_size)
{
	ft_memcpy(message->data + iphdr_size, &message->icmp_header, trace->protocol->len);
	ft_memset(message->data + iphdr_size + trace->protocol->len, '0', message->packet_len);
	message->icmp_header.checksum = checksum(message->data + iphdr_size, trace->protocol->len + message->packet_len);
	ft_memcpy(message->data + iphdr_size, &message->icmp_header, trace->protocol->len);
}

void		deserialize_icmp_header(t_message *message, t_trace *trace)
{
	ft_memcpy(&message->icmp_header, message->data, trace->protocol->len);
	message->data = message->data + trace->protocol->len;
}
