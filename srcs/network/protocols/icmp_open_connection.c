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
	if (fd == EAFNOSUPPORT)
	{
		printf("Erreur : L'implémentation ne supporte pas la ");
		printf("famille d'adresses indiquée.\n");
		return (false);
	}
	if (fd == EPROTONOSUPPORT)
	{
		printf("Erreur : Le type de protocole, ou le protocole lui-même ");
		printf("n'est pas disponible dans ce domaine de communication.\n");
		return (false);
	}
	if (fd < 0)
	{
		printf("Erreur : Socket connection has failed.\n");
		return (false);
	}
	return (true);
}

static BOOLEAN	bind_error(void)
{
	printf("Error : Socket address is busy\n");
	return (false);
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
	struct sockaddr_in sock_addr; 

	opt = 1;

	trace->sock = socket(PROT_INTERNET_IPV4, NETWORK_FLUX, ICMP_PROTOCOL);//trace->protocol->proto);
	if (!socket_connection_is_estabilised(trace->sock))
		return (false);
	ft_memset(&sock_addr, '0', sizeof(struct sockaddr_in));
	sock_addr.sin_family = PROT_INTERNET_IPV4;
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	sock_addr.sin_port = htons(trace->port);
	if (bind(trace->sock, (struct sockaddr*)&sock_addr,\
		sizeof(struct sockaddr)) < 0)
		return (bind_error());
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
