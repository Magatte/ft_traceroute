/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_open_connection.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 02:30:43 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 02:30:45 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

#include <arpa/inet.h>

/*
** static boolean socket_connection_is_estabilised(int fd);
** return boolean
** if < 0 return false and print error message
*/
static BOOLEAN	socket_connection_is_estabilised(int fd)
{
	if (fd < 0)
	{
		printf("Erreur : Socket connection has failed.\n");
		return (false);
	}
	return (true);
}

/*
** boolean	icmp_initialize_connection(t_trace *trace, int ttl);
** Init icmp Socket connection on IPV4
** protocol ip/icmp
** setsockopt on tcp/ip header-packet service
** return boolean
*/
BOOLEAN			icmp_initialize_connection(t_trace *trace, int ttl)
{
	int opt;

	opt = 1;

	trace->sock = socket(PROT_INTERNET_IPV4, NETWORK_FLUX, ICMP_PROTOCOL);
	if (!socket_connection_is_estabilised(trace->sock))
		return (false);
	if (setsockopt(trace->sock, 0, TCP_IP_PACKET_HEADER_SERVICE,\
		&ttl, sizeof(ttl)) != 0)
		return (false);
# ifdef __linux__
	if ((setsockopt(trace->sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt))) != 0)
		return (false);
	if (setsockopt(trace->sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) != 0)
		return (false);
#endif
	if (F_SOCK_DEBUG)
		if (setsockopt(trace->sock, SOL_SOCKET, SO_DEBUG, (char*)&opt, sizeof(opt)) != 0)
			return (false);
	if (F_DONTROUTE)
		if (setsockopt(trace->sock, SOL_SOCKET, SO_DONTROUTE, (char*)&opt, sizeof(opt)) != 0)
			return (false);
	if (setsockopt(trace->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&trace->timeout, sizeof(trace->timeout)) != 0)
		return (false);
	return (true);
}
