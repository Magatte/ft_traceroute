/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trace.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/06 01:48:05 by jguyet            #+#    #+#             */
/*   Updated: 2017/06/06 01:48:06 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

t_trace		*singleton_trace(void)
{
	static t_trace	*trace = NULL;

	if (trace != NULL)
		return (trace);
	if (!(trace = (t_trace*)malloc(sizeof(t_trace))))
		return (NULL);
	trace->sequence = 0;
	trace->shost = NULL;
	trace->destip = NULL;
	trace->port = 80;
	trace->launch = process_traceroute;
	trace->received = 0;
	trace->send = 0;
	trace->timeout.tv_sec = 5;
	trace->pid = (getpid() & 0xFFFF) | 0x8000;
	trace->ttl = 1;
	trace->sweepincrsize = 0;
	trace->sweepminsize = PACKET_X64;
	trace->sweepmaxsize = PACKET_X64;
	trace->mintime = 0;
	trace->totaltime = 0;
	trace->maxtime = 0;
	trace->max_hop = 30;
	trace->protocol = get_protocol(ICMP);
	trace->retry = true;
	trace->ping_interval = DEFAULT_PING_INTERVAL;
	if (!load_ip_tab(trace))
		return (NULL);
	return (trace);
}

void		destruct_trace(t_trace *trace)
{
	int i;

	i = 0;
	while (i < FLAGS_SIZE)
	{
		ft_strdel(&trace->flags[i]->name);
		ft_strdel(&trace->flags[i]->error);
		ft_strdel(&trace->flags[i]->help);
		i++;
	}
	free(trace->flags);
	if (trace->shost != NULL)
		ft_strdel(&trace->shost);
	ft_strdel(&trace->destip);
	free(trace->ip_tab);
	free(trace);
}

BOOLEAN		send_message(t_trace *trace, t_message *message)
{
	int		res;

	trace->send++;
	//trace->message = new_message(trace->sweepminsize);
	//trace->message->serialize(trace->message, trace);
	trace->start_time = get_current_time_millis();
	res = sendto(trace->sock, message->data, message->len, MSG_DONTWAIT, (struct sockaddr*)&trace->addr, sizeof(trace->addr));
	if (res < 0)
	{
		ft_fprintf(1, "ft_traceroute: sendto: Network is unreachable\n");
		return (false);
	}
	if (res != message->len)
	{
		ft_printf("ft_traceroute: wrote %s %d chars, ret=%d\n", trace->shost, message->len, res);
		return (false);
	}
	return (true);
}

BOOLEAN		process_loop(t_trace *trace)
{
	int i;
	char *save_addr;

	i = 0;
	save_addr = ft_strdup(trace->destip);
	while (i < NB_TEST_CONNECTION)
	{
		//Open socket connection
		initialize_socket_connection(trace);

		trace->message = new_message(trace->sweepminsize);
		trace->message->serialize(trace->message, trace);

		send_message(trace, trace->message);
		if ((trace->ip_tab[i] = icmp_handle_message(trace)) != NULL)
		{
			if (ft_strcmp(get_hostname_ipv4(trace->ip_tab[i]), save_addr) == 0)
				trace->retry = false;
		}
		else
		{
			ft_printf(" *");
		}
		free(trace->message->data);
		free(trace->message);
		free(trace->destip);
		trace->destip = ft_strdup(save_addr);
		trace->sequence++;
		//close socket connection
		close(trace->sock);
		i++;
	}
	ft_strdel(&save_addr);
	ft_printf("\n");
	return (trace->retry);
}

BOOLEAN		process_traceroute(t_trace *trace)
{
	printf("ft_traceroute to %s (%s), %d hops max, %d byte packets\n", trace->shost, trace->destip, trace->max_hop, trace->sweepminsize);
	printf("My pid = %d\n", htons(trace->pid));
	while (trace->ttl <= trace->max_hop && trace->retry)
	{
		reset_ip_tab(trace);
		if (process_loop(trace) == false) {
			free_ip_tab(trace);
			break ;
		}
		free_ip_tab(trace);
		trace->ttl++;
	}
	return (true);
}
