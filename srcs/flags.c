/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 21:26:39 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 21:26:41 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

BOOLEAN			has_help(int argc, char **argv)
{
	int i;

	i = 1;
	while (i < argc)
	{
		if (ft_strcmp(argv[i], "-help") == 0)
			return (true);
		if (ft_strcmp(argv[i], "--help") == 0)
			return (true);
		i++;
	}
	return (false);
}

BOOLEAN			has_illegal_option(int argc, char **argv)
{
	int i;

	i = 1;
	while (i < argc)
	{
		if (ft_strncmp(argv[i], "--", 2) == 0)
		{
			printf("ft_trace: illegal option -- -\n");
			return (true);
		}
		i++;
	}
	return (false);
}

BOOLEAN			print_help(t_trace *trace)
{
	int		i;

	i = 0;
	printf("usage: ft_traceroute [-");
	while (i < FLAGS_SIZE)
	{
		if (trace->flags[i]->special == false)
			printf("%s", trace->flags[i]->name);
		i++;
	}
	printf("]");
	i = 0;
	while (i < FLAGS_SIZE)
	{
		if (trace->flags[i]->special == true)
			printf(" %s", trace->flags[i]->help);
		i++;
	}
	printf("\n");
	return (false);
}

BOOLEAN			activ_flags(t_trace *trace, char *arg)
{
	int		i;
	int		o;

	i = -1;
	if (arg[0] != '-')
		return (true);
	while (++i < FLAGS_SIZE)
	{
		o = 1;
		if (trace->flags[i]->special == true)
			continue ;
		while (o < (int)ft_strlen(arg))
		{
			if (ft_strncmp(trace->flags[i]->name, arg + o,\
				ft_strlen(trace->flags[i]->name)) == 0)
			{
				trace->flags[i]->actif = true;
				break ;
			}
			o++;
		}
	}
	return (true);
}

BOOLEAN			select_value_special_flags(t_trace *trace,\
	int currentid, char *arg, char **argv, int argc)
{
	int		i;

	i = 0;
	if (arg[0] != '-')
		return (false);
	while (i < FLAGS_SIZE)
	{
		if (trace->flags[i]->special == false\
			|| ft_strcmp(trace->flags[i]->name, arg + 1) != 0)
		{
			i++;
			continue ;
		}
		if ((currentid + 1) >= argc) {
			print_help(trace);
			exit(0);
			return (false);
		}
		if (trace->flags[i]->type == 1 && !ft_is_string_numeric(argv[currentid + 1]))
		{
			char *error = ft_strjoin("ft_trace: ", trace->flags[i]->error);
			printf(error, argv[currentid + 1]);
			ft_strdel(&error);
			exit(0);
		}
		trace->flags[i]->actif = true;
		trace->flags[i]->value = ft_strdup(argv[currentid + 1]);
		return (true);
		i++;
	}
	return (false);
}

BOOLEAN			load_host(t_trace *trace, char *arg)
{
	struct sockaddr_in *in;

	if (arg[0] == '-')
		return (false);
	if (trace->shost != NULL)
	{
		print_help(trace);
		exit(0);
	}
	trace->shost = ft_strdup(arg);
	in = get_sockaddr_in_ipv4(trace->shost);
	if (in == NULL)
		return (false);
	trace->addr = *in;
	trace->destip = ft_strdup(get_hostname_ipv4(&trace->addr.sin_addr));
	return (true);
}

BOOLEAN			load_flags(t_trace *trace, int argc, char **argv)
{
	int			i;
	BOOLEAN		succes;

	i = 1;
	succes = true;
	if (has_illegal_option(argc, argv))
		succes = true;
	if (has_help(argc, argv))
	{
		print_help(trace);
		succes = false;
	}
	while (i < argc)
	{
		load_host(trace, argv[i]);
		activ_flags(trace, argv[i]);
		if (select_value_special_flags(trace, i, argv[i], argv, argc)) {
			i++;
		}
		i++;
	}
	return (succes);
}

static t_flag	*newflag(t_flag *f)
{
	t_flag *n;

	if (!(n = malloc(sizeof(t_flag))))
		return (NULL);
	n->actif = f->actif;
	n->name = ft_strdup(f->name);
	n->special = f->special;
	n->help = ft_strdup(f->help);
	n->value = NULL;
	n->type = f->type;
	n->error = ft_strdup(f->error);
	return (n);
}

BOOLEAN			load_flag_list(t_trace *trace)
{
	if (!(trace->flags = malloc(sizeof(t_flag) * FLAGS_SIZE)))
		return (false);
	trace->flags[0] = newflag(&(t_flag){false, "m", true, "[-m max_ttl]", NULL, 1, "invalid TTL: `%s'"});
	trace->flags[1] = newflag(&(t_flag){false, "M", true, "[-M first_ttl]", NULL, 1, "ft_traceroute: first ttl (%d) may not be greater than max ttl (%d)\n"});
	trace->flags[2] = newflag(&(t_flag){false, "n", false, NULL, NULL, 0, NULL});
	trace->flags[3] = newflag(&(t_flag){false, "d", false, NULL, NULL, 0, NULL});
	trace->flags[4] = newflag(&(t_flag){false, "r", false, NULL, NULL, 0, NULL});
	return (true);
}
