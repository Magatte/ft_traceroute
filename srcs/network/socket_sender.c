/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_sender.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 02:30:43 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 02:30:45 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

BOOLEAN			initialize_socket_sender_connection(t_trace *trace)
{
	trace->sock_snd = socket(PROT_INTERNET_IPV4, trace->socket_type, trace->protocol->proto);//IPPROTO_RAW);
	if (!socket_connection_is_estabilised(trace->sock_snd))
	{
		if (trace->sock_snd && trace->socket_type == SOCK_RAW)
		{
			trace->socket_type = SOCK_DGRAM;
			printf("(Restart on SOCK_DGRAM socket type)\n");
			return (initialize_socket_sender_connection(trace));
		}
		return (false);
	}
	if (!bind_socket(trace))
		return (bind_error());
	if (!set_on_socket_sender_options(trace))
		return (set_socket_options_error());
	return (true);
}

BOOLEAN			bind_socket(t_trace *trace)
{
	struct sockaddr_in sock_addr; 

	ft_memset(&sock_addr, '0', sizeof(struct sockaddr_in));
	sock_addr.sin_family = PROT_INTERNET_IPV4;
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	sock_addr.sin_port = htons(trace->port);
	if (bind(trace->sock_snd, (struct sockaddr*)&sock_addr,\
		sizeof(struct sockaddr)) < 0)
		return (false);
	return (true);
}

BOOLEAN			set_on_socket_sender_options(t_trace *trace)
{
	int opt;
	int ttl;

	opt = 1;
	ttl = trace->ttl;
	if (setsockopt(trace->sock_snd, 0, TCP_IP_PACKET_HEADER_SERVICE,\
		&ttl, sizeof(ttl)) != 0)
		return (false);
	if (trace->use_ip_header)
	{
		if ((setsockopt(trace->sock_snd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt))) != 0)
			return (false);
		/*
		** L'option  SO_BROADCAST  demande  l'autorisation de pouvoir
        ** diffuser des datagrammes "broadcast" sur la  socket.  Dans
       	** les  premières versions du système, la diffusion broadcast
       	** etait une option privilégiée.
		*/
		/*if (setsockopt(trace->sock_snd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) != 0)
			return (false);*/
	}
	if (setsockopt(trace->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&trace->timeout, sizeof(trace->timeout)) != 0)
		return (false);
	return (true);
}
