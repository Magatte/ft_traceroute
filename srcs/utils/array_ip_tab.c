/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_ip_tab.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/19 01:08:43 by jguyet            #+#    #+#             */
/*   Updated: 2017/09/19 01:08:46 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

BOOLEAN				ip_tab_contains(t_trace *trace, struct in_addr *addr)
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
		if (ft_strcmp(get_hostname_ipv4(addr),\
            get_hostname_ipv4(trace->ip_tab[i])) == 0)
			return (true);
		i++;
	}
	return (false);
}

void		reset_ip_tab(t_trace *trace)
{
	int i;

	i = 0;
	while (i < NB_TEST_CONNECTION)
	{
		trace->ip_tab[i++] = NULL;
	}
}

void		free_ip_tab(t_trace *trace)
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

BOOLEAN		load_ip_tab(t_trace *trace)
{
	if (!(trace->ip_tab = (struct in_addr**)malloc(\
        sizeof(struct in_addr*) * NB_TEST_CONNECTION)))
		return (false);
	ft_bzero(&trace->addr, sizeof(trace->addr));
    return (true);
}
