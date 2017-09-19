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

t_message	*new_message(size_t size)
{
	t_message	*message;

	if (!(message = (t_message*)malloc(sizeof(t_message))))
		return (NULL);
	ft_bzero(message, sizeof(t_message));
	message->packet_len = size;
	message->serialize = serialize_message;
	return (message);
}

BOOLEAN		serialize_message(t_message *message, t_trace *trace)
{
	size_t		iphdr_size = 0;

	if (trace->use_ip_header)
	{
		iphdr_size = IPHDR_SIZE;
		//prepare ip header
		prepare_iphdr(message, trace);
	}
	//set message total length
	message->len = trace->protocol->len + iphdr_size + message->packet_len;
	//prepare protocol header
	trace->protocol->prepare_header(message, trace);
	//serialize message
	if (!(message->data = ft_strnew(iphdr_size + trace->protocol->len + message->packet_len)))
		return (false);
	if (trace->use_ip_header)
	{
		ft_memcpy(message->data, &message->ip_header, IPHDR_SIZE);
	}
	trace->protocol->serialize(message, trace, iphdr_size);
	return (true);
}

void			destruct_message(t_message *message)
{
	free(message);
}
