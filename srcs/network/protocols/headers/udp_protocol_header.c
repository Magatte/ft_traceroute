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

#include <sys/time.h>

void		prepare_udp_header(t_message *message, t_trace *trace)
{
	message->udp_header.source = htons(trace->pid);
	message->udp_header.dest = htons(trace->port + trace->sequence);
	message->udp_header.len = htons(message->len - sizeof(struct iphdr));
	message->udp_header.checksum = 0;
	message->udp_header.sec = trace->sequence;
	message->udp_header.ttl = trace->ttl;
	gettimeofday(&message->udp_header.tv, 0);
}

void		serialize_udp_header(t_message *message, t_trace *trace, size_t iphdr_size)
{
	ft_memcpy(message->data + iphdr_size, &message->udp_header, trace->protocol->len);
	message->udp_header.checksum = 0;
	ft_memcpy(message->data + iphdr_size, &message->udp_header, trace->protocol->len);

	int i = 0;
	ft_printf("\n");
	while (i < (int)trace->protocol->len)
	{
		if (ft_isascii((char)(message->data + iphdr_size + i)))
			ft_printf("%c", (char)(message->data + iphdr_size + i));
		else
			ft_printf(".");
		i++;
	}
	ft_printf("\n");
}

void		deserialize_udp_header(t_message *message, t_trace *trace)
{
	ft_memcpy(&message->udp_header, message->data, trace->protocol->len);
	message->data = message->data + trace->protocol->len;
}