/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_flags_values.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/30 08:06:18 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/30 08:06:19 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

static BOOLEAN		print_error(char *error)
{
	ft_fprintf(1, "ft_traceroute: %s\n", error);
	return (false);
}


static BOOLEAN		print_error_args(char *error, int args)
{
	ft_fprintf(1, error, args);
	return (false);
}

static BOOLEAN		print_error_args_string(char *error, char *args)
{
	ft_fprintf(1, error, args);
	return (false);
}

static BOOLEAN		print_error_two_args(char *error, int arg, int arg2)
{
	ft_fprintf(1, error, arg, arg2);
	return (false);
}

BOOLEAN				set_flags_values(t_trace *trace)
{
	if (F_MAXHOPS)//"max-ttl"
		trace->max_hop = ft_atoi(trace->flags[0]->value);
	if (trace->max_hop > 255 || trace->max_hop < 0)
		return (print_error(trace->flags[0]->error));
	if (F_FIRSTHOPS)
		trace->ttl = ft_atoi(trace->flags[1]->value);
	if (trace->ttl > trace->max_hop || trace->ttl < 0)
		return (print_error_two_args(trace->flags[1]->error, trace->ttl, trace->max_hop));
	if (trace->ttl <= 0)
		return (print_error("ft_traceroute: first ttl must be > 0"));
	if (F_PROTOCOL)
	{
		trace->sweepminsize -= trace->protocol->len;
		if (trace->use_ip_header)
			trace->sweepminsize -= IPHDR_SIZE;
		trace->protocol = get_protocol_by_name(trace->flags[5]->value);
		if (trace->protocol == NULL)
			return (print_error_args_string(trace->flags[5]->error, trace->flags[5]->value));
		trace->sweepminsize = trace->protocol->len + trace->sweepminsize;
		if (trace->use_ip_header)
			trace->sweepminsize += IPHDR_SIZE;
		trace->sweepmaxsize = trace->sweepminsize;
	}
	if (F_PORT)
		trace->port = ft_atoi(trace->flags[6]->value);
	if (trace->port < 1 || trace->port > 32768)
		return (print_error_args(trace->flags[6]->error, trace->port));
	if (F_IP_HDR)
		trace->use_ip_header = !trace->use_ip_header;
	return (true);
}
