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
	trace->addr = NULL;
	trace->port = 80;
	trace->launch = process_traceroute;
	trace->received = 0;
	trace->send = 0;
	trace->timeout.tv_sec = 5;
	trace->pid = (getpid() & 0xFFFF) | 0x8000;
	trace->ttl = 1;
	trace->mintime = 0;
	trace->totaltime = 0;
	trace->maxtime = 0;
	trace->max_hop = 30;
	trace->protocol = get_protocol(ICMP);
	trace->socket_type = INTERNAL_SOCK_FLUX;
	trace->retry = true;
	trace->use_ip_header = false;
# ifdef __linux__
	trace->use_ip_header = true;
#endif
	trace->dest_ip = NULL;
	trace->sweepminsize = trace->protocol->len;
	if (trace->use_ip_header)
		trace->sweepminsize += IPHDR_SIZE;
	trace->sweepmaxsize = trace->sweepminsize;
	struct in_addr local;
	local.s_addr = INADDR_ANY;
	trace->source_ip = ft_strdup(get_hostname_ipv4(&local));
	trace->write_message = NULL;
	trace->interval_number_connection = NB_DEFAULT_INTERVAL_CONNECTION;
	return (trace);
}

void		destruct_trace(t_trace *trace)
{
	int i = 0;

	while (i < FLAGS_SIZE)
	{
		ft_strdel(&trace->flags[i]->name);
		ft_strdel(&trace->flags[i]->error);
		ft_strdel(&trace->flags[i]->help);
		free(trace->flags[i]);
		i++;
	}
	free(trace->flags);
	if (trace->addr != NULL)
		free(trace->addr);
	if (trace->shost != NULL)
		ft_strdel(&trace->shost);
	if (trace->source_ip != NULL)
		ft_strdel(&trace->source_ip);
	if (trace->dest_ip != NULL)
		ft_strdel(&trace->dest_ip);
	if (trace->write_message != NULL)
		ft_strdel(&trace->write_message);
	free(trace->ip_tab);
	free(trace);
}

BOOLEAN		send_message(t_trace *trace, t_message *message)
{
	int		res;

	trace->send++;
	trace->start_time = get_current_time_millis();
	if (F_ASCII_DEBUG_MSG)
	{
		printf("\n   Packet > (%s) [", trace->dest_ip);
		message->tostring(message);
		printf("]\n");
	}
	res = sendto(trace->sock, message->data, message->len, MSG_DONTWAIT, (struct sockaddr*)trace->addr, sizeof(*trace->addr));
	
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
	int		i = 0;

	while (i < trace->interval_number_connection)
	{
		//Open socket connection
		initialize_socket_receiver_connection(trace);
		initialize_socket_sender_connection(trace);

		if (i == 0)
			ft_printf("%2d ", trace->ttl);
		trace->message = new_message(trace->sweepminsize);
		trace->message->serialize(trace->message, trace);

		if (F_WRITING && trace->write_message != NULL)
		{
			int len = ft_strlen(trace->write_message);

			if (len > (trace->message->packet_len - 1))
				len = (trace->message->packet_len - 1);
			if (len > 0) {
				ft_memcpy(trace->message->data + (trace->message->len - trace->message->packet_len) + 1, trace->write_message, len);
			}
		}

		send_message(trace, trace->message);
		if ((trace->ip_tab[i] = handle_message(trace)) != NULL)
		{
			if (ft_strcmp(trace->ip_tab[i], trace->dest_ip) == 0)
				trace->retry = false;
		}
		else
		{
			ft_printf(" *");
		}
		destruct_message(trace->message);
		trace->sequence++;
		//close socket connection
		close(trace->sock);
		close(trace->sock_snd);
		i++;
	}
	ft_printf("\n");
	return (trace->retry);
}

static void	print_traceroute_stats(t_trace *trace)
{
	float	mintime = 0;
	float	mediumtime = 0;
	float	maxtime = 0;

	if (trace->mintime != 0)
		mintime = (((float)trace->mintime) / 1000);
	if (trace->received != 0)
		mediumtime = (trace->totaltime / trace->received) == 0 ? 0 : (((float)(trace->totaltime / trace->received)) / 1000);
	if (trace->maxtime != 0)
		maxtime = (((float)trace->maxtime) / 1000);
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", mintime, mediumtime, maxtime, maxtime - mintime);
}

void		on_traceroute_finished(t_trace *trace)
{
	if (!F_TIME_INFO)
		return ;
	printf("----------------------------------------------------------\n");
	print_traceroute_stats(trace);
}

BOOLEAN		process_traceroute(t_trace *trace)
{
	if (!load_ip_tab(trace))
			return (false);
	while (trace->ttl <= trace->max_hop && trace->retry)
	{
		reset_ip_tab(trace);
		if (process_loop(trace) == false) {
			free_ip_tab(trace);
			on_traceroute_finished(trace);
			return (true);
		}
		free_ip_tab(trace);
		trace->ttl++;
	}
	return (true);
}
