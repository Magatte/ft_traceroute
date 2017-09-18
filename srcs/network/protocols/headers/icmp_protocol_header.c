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

void		update_icmp_checksum(t_message *message, t_trace *trace,\
			void *ptr_message, size_t iphdr_size, size_t size)
{
	ft_memcpy(ptr_message + iphdr_size, &message->icmp_header, trace->protocol->len);
	ft_memset(ptr_message + iphdr_size + trace->protocol->len, '0', size);
	message->icmp_header.checksum = checksum(ptr_message + iphdr_size, trace->protocol->len + size);
	ft_memcpy(ptr_message + iphdr_size, &message->icmp_header, trace->protocol->len);
}
