/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   protocol_messages.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 04:19:25 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 04:19:27 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

void		*new_message(t_trace *trace, size_t size)
{
	size_t		iphdr_size;

	if (!(trace->message = (t_message*)malloc(sizeof(t_message))))
			return (NULL);
	iphdr_size = 0;
	ft_bzero(trace->message, sizeof(t_message));
#ifdef __linux__
	iphdr_size = IPHDR_SIZE;
	prepare_iphdr(trace->message, trace);
#endif
	trace->message->len = trace->protocol->len + iphdr_size + trace->sweepminsize;
	trace->protocol->prepare(trace->message, trace);

	trace->message->data = ft_strnew(iphdr_size + trace->protocol->len + size);
#ifdef __linux__
	ft_memcpy(trace->message->data, &trace->message->ip_header, IPHDR_SIZE);
#endif
	trace->protocol->update_checksum(trace->message, trace, trace->message->data, iphdr_size, size);
	//destruct_packet_send(message);
	return (trace->message->data);
}


void			destruct_message(t_message *message)
{
	free(message);
}
