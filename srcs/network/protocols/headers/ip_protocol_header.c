/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ip_protocol_header.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/19 00:10:08 by jguyet            #+#    #+#             */
/*   Updated: 2017/09/19 00:10:10 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

void		prepare_iphdr(t_message *message, t_trace *trace)
{
	message->ip_header.src.s_addr = INADDR_ANY;

	if (!(inet_pton(AF_INET, trace->destip, &message->ip_header.dest)))
	{
		printf("ft_trace: Can't set destination network address\n");
		exit(EXIT_FAILURE);
	}

	message->ip_header.ttl = trace->ttl;
	message->ip_header.protocol = trace->protocol->proto;
	message->ip_header.version = 4;//ipv4
	message->ip_header.hl = sizeof(struct iphdr) >> 2;
	message->ip_header.pid = htons(trace->pid);
	message->ip_header.service = 0;
	message->ip_header.off = 0;
	message->ip_header.len = trace->protocol->len + IPHDR_SIZE;
	message->ip_header.checksum = 0;
	message->ip_header.checksum = checksum(&message->ip_header, sizeof(struct iphdr));
}