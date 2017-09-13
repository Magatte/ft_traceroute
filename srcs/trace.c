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
	trace->packet_len = 0;
	trace->retry = true;
	if (!(trace->ip_tab = (struct in_addr**)malloc(sizeof(struct in_addr*) * NB_TEST_CONNECTION)))
		return (NULL);
	ft_bzero(&trace->addr, sizeof(trace->addr));
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

void		reset_trace_tab(t_trace *trace)
{
	int i;

	i = 0;
	while (i < NB_TEST_CONNECTION)
	{
		trace->ip_tab[i++] = NULL;
	}
}

void		free_trace_tab(t_trace *trace)
{
	int i;

	i = 0;
	while (i < NB_TEST_CONNECTION)
	{
		if (trace->ip_tab[i] != NULL)
			free(trace->ip_tab[i]);
		i++;
	}
}

BOOLEAN		sendto_message(t_trace *trace)
{
	int		cc;

	trace->send++;
	trace->packet = prepare_packet_to_send(trace, trace->sweepminsize);
	trace->start_time = get_current_time_millis();
	cc = sendto(trace->sock, trace->packet, trace->packet_len, MSG_DONTWAIT, (struct sockaddr*)&trace->addr, sizeof(trace->addr));
	if (cc < 0 || cc != trace->packet_len)
	{
		if (cc < 0)
			ft_fprintf(1, "ft_traceroute: sendto: Network is unreachable\n");
		ft_printf("ft_traceroute: wrote %s %d chars, ret=%d\n", trace->shost, trace->packet_len, cc);
		return (false);
	}
	return (true);
}

BOOLEAN		process_three_request(t_trace *trace)
{
	BOOLEAN retry = true;
	int i;
	int responses;
	char *tmp;

	i = 0;
	responses = 0;
	tmp = ft_strdup(trace->destip);
	while (i < NB_TEST_CONNECTION)
	{
		//Open socket connection
		icmp_initialize_connection(trace, trace->ttl);
		sendto_message(trace);
		if (i == 0)
			ft_printf("%2d ", trace->ttl);
		if ((trace->ip_tab[i] = icmp_handle_message(trace)) != NULL) {
			responses++;
			if (ft_strcmp(get_hostname_ipv4(trace->ip_tab[i]), tmp) == 0)
				retry = false;
		} else {
			ft_printf(" *");
		}
		free(trace->packet);
		free(trace->destip);
		trace->destip = ft_strdup(tmp);
		trace->sequence++;
		//close socket connection
		close(trace->sock);
		i++;
	}
	ft_strdel(&tmp);
	ft_printf("\n");
	return (retry);
}

BOOLEAN		process_traceroute(t_trace *trace)
{
	printf("ft_traceroute to %s (%s), %d hops max, %d byte packets\n", trace->shost, trace->destip, trace->max_hop, trace->sweepminsize);
	while (trace->ttl <= trace->max_hop && trace->retry)
	{
		reset_trace_tab(trace);
		if (process_three_request(trace) == false) {
			free_trace_tab(trace);
			break ;
		}
		free_trace_tab(trace);
		trace->ttl++;
	}
	return (true);
}
