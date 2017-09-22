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

BOOLEAN				check_packet(t_trace *trace, void *packet, int ret)
{
	t_message *message;

	if (!(message = deserialize_message(packet, trace, ret)))
		return (true);
	if (F_ASCII_DEBUG_MSG)
	{
		printf("   Packet < (%s) [", trace->dest_ip);
		message->tostring(message);
		printf("]\n");
	}
	free(message);
	return (true);
}

/*
** read le message icmp header-packet
*/
char		*handle_message(t_trace *trace)
{
	int	ret;
	char packet[trace->sweepminsize];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	
	if ((ret = recvfrom(trace->sock, &packet, trace->message->len, 0, (struct sockaddr*)&from, &fromlen)) != -1)
	{
		if (!check_packet(trace, packet, ret))
			return (handle_message(trace));
		return (process_received_message(trace, &from));
	}
	return (NULL);
}

char		*process_received_message(t_trace *trace, struct sockaddr_in *addr)
{
	float			time_of;
	char			*ip_addr;
	BOOLEAN			printhost;
	char			*ndd;
	float			n_response_time;
	char			*response_time;

	
	ip_addr = ft_strdup(get_hostname_ipv4(&addr->sin_addr));
	time_of = (get_current_time_millis() - trace->start_time);
	printhost = ip_tab_contains(trace, &addr->sin_addr);

	if (printhost == false || F_ASCII_DEBUG_MSG)
	{
		if (trace->ip_tab[0] != NULL && !F_ASCII_DEBUG_MSG)
			ft_printf("\n   ");
		if (F_ASCII_DEBUG_MSG)
			ft_printf("  ");
		ndd = get_hostname_by_in_addr(&addr->sin_addr);
		if (!F_PRINT_HOP_ADDR)
			ft_printf(" %s (%s)", ndd, ip_addr);
		else
			ft_printf(" %s ", ip_addr);
		ft_strdel(&ndd);
	}
	n_response_time = (time_of / (float)1000);
	if (n_response_time < 0)
		n_response_time = 0;
	response_time = ft_convert_double_to_string(n_response_time, 3);

	ft_printf(" %s ms ", response_time);
	ft_strdel(&response_time);
	trace->totaltime += time_of;
	if (trace->mintime == 0 || trace->mintime > time_of)
		trace->mintime = time_of;
	if (trace->maxtime == 0 || trace->maxtime < time_of)
		trace->maxtime = time_of;
	trace->received++;
	return (ip_addr);
}
