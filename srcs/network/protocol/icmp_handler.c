/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/30 05:09:41 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/30 05:09:42 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

void				icmp_check_code(t_trace *trace)
{
	struct icmphdr *hdr;

	hdr = (struct icmphdr*)trace->packet;
	ft_printf("(code: %d) (type: %d)", hdr->code, hdr->type);
}

/*
** read le message icmp header-packet
*/
struct in_addr		*icmp_handle_message(t_trace *trace)
{
	int	ret;
	char in[1024];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

	if ((ret = recvfrom(trace->sock, &in, MSG_OOB, trace->sweepminsize, (struct sockaddr*)&from, &fromlen)) != -1)
	{
		icmp_check_code(trace);
		return (icmp_process_received_packet(trace, &from));
	}
	icmp_check_code(trace);
	//trace->packet
	return (NULL);
}

BOOLEAN				trace_tab_exists(t_trace *trace, struct in_addr *addr)
{
	int i;

	i = 0;
	while (i < NB_TEST_CONNECTION)
	{
		if (trace->ip_tab[i] == NULL)
		{
			i++;
			continue;
		}
		if (ft_strcmp(get_hostname_ipv4(addr), get_hostname_ipv4(trace->ip_tab[i])) == 0)
			return (true);
		i++;
	}
	return (false);
}

struct in_addr		*icmp_process_received_packet(t_trace *trace, struct sockaddr_in *addr)
{
	int				time_of;
	struct in_addr	*ip_addr;
	BOOLEAN			printhost;
	char			*ndd;

	
	if (!(ip_addr = (struct in_addr*)malloc(sizeof(struct in_addr))))
		return (NULL);
	ip_addr->s_addr = addr->sin_addr.s_addr;
	time_of = (get_current_time_millis() - trace->start_time);
	printhost = trace_tab_exists(trace, &addr->sin_addr);

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