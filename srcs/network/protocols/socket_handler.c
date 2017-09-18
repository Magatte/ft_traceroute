/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/30 05:09:41 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/30 05:09:42 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

void				check_packet(t_trace *trace, void *packet, int ret)
{
	struct icmphdr *hdr;

	if (trace->protocol->e_name != ICMP)
		return ;
# ifdef __linux__
	hdr = (struct icmphdr*)(packet + IPHDR_SIZE);
# else
	hdr = (struct icmphdr*)(packet);
# endif
	ft_printf("(code:%d), (type:%d), (ret:%d)", hdr->code, hdr->type, ret);
}

/*
** read le message icmp header-packet
*/
struct in_addr		*icmp_handle_message(t_trace *trace)
{
	int	ret;
	char packet[1024];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

	//MSG_OOB
	if ((ret = recvfrom(trace->sock, &packet, trace->message->len, 0, (struct sockaddr*)&from, &fromlen)) != -1)
	{
		//check_packet(trace, packet, ret);
		return (process_received_message(trace, &from));
	}
	return (NULL);
}

struct in_addr		*process_received_message(t_trace *trace, struct sockaddr_in *addr)
{
	int				time_of;
	struct in_addr	*ip_addr;
	BOOLEAN			printhost;
	char			*ndd;

	
	if (!(ip_addr = (struct in_addr*)malloc(sizeof(struct in_addr))))
		return (NULL);
	ip_addr->s_addr = addr->sin_addr.s_addr;
	time_of = (get_current_time_millis() - trace->start_time);
	printhost = ip_tab_contains(trace, &addr->sin_addr);

	if (printhost == false)
	{
		if (trace->ip_tab[0] != NULL)
			ft_printf("\n   ");
		ndd = get_hostname_by_in_addr(&addr->sin_addr);
		if (!F_PRINT_HOP_ADDR)
			ft_printf(" %s (%s)", ndd, get_hostname_ipv4(&addr->sin_addr));
		else
			ft_printf(" %s ", get_hostname_ipv4(&addr->sin_addr));
		ft_strdel(&ndd);
	}
	ft_printf(" %s ms ", ft_convert_double_to_string(((float)(time_of) / 1000), 3));
	trace->totaltime += time_of;
	if (trace->mintime == 0 || trace->mintime > time_of)
		trace->mintime = time_of;
	if (trace->maxtime == 0 || trace->maxtime < time_of)
		trace->maxtime = time_of;
	trace->received++;
	return (ip_addr);
}