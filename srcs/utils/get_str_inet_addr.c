/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_str_inet_addr.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/06 04:04:34 by jguyet            #+#    #+#             */
/*   Updated: 2017/06/06 04:04:35 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

char *get_hostname_ipv4(struct in_addr *addr)
{
	return (inet_ntoa(*addr));
}

char *get_hostname_ipv6(struct in6_addr *addr)
{
	char ipstr[INET6_ADDRSTRLEN];

	inet_ntop(PROT_INTERNET_IPV6, addr, ipstr, sizeof(ipstr));

	return (ft_strdup(ipstr));
}

char	*get_hostname_by_in_addr(const struct in_addr *addr)
{
	struct sockaddr_in	sa;
    socklen_t			len;
    char				hbuf[NI_MAXHOST];

	ft_memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = PROT_INTERNET_IPV4;
    sa.sin_addr.s_addr = addr->s_addr;
    len = sizeof(struct sockaddr_in);
    if (getnameinfo((struct sockaddr *) &sa, len, hbuf, sizeof(hbuf), 
        NULL, 0, 0)) {
        return (ft_strdup(""));
    }
	return (ft_strdup(hbuf));
}

struct sockaddr_in *get_sockaddr_in_ipv4(char *host)
{
	struct	addrinfo hints;
	struct	addrinfo *res;
	struct	sockaddr_in *ipv4;
	char	ipstr[INET6_ADDRSTRLEN];

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = PROT_INTERNET_IPV4;
	hints.ai_socktype = NETWORK_FLUX;
	if (getaddrinfo(host, NULL, &hints, &res) != 0)
		return (NULL);
	while (res)
	{
		if (res->ai_family != PROT_INTERNET_IPV4)
		{
			res = res->ai_next;
			continue ;
		}
		ipv4 = (struct sockaddr_in*)res->ai_addr;
		inet_ntop(PROT_INTERNET_IPV4, &(ipv4->sin_addr), ipstr, sizeof(ipstr));

		if (ipstr[0] != 0)
			return (ipv4);
		res = res->ai_next;
	}
	return (NULL);
}
