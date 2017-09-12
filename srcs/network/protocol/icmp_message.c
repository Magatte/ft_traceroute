/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_message.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 04:19:25 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 04:19:27 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_trace.h"

#ifdef __linux__
static void				prepare_iphdr(t_packet *packet, t_trace *trace)
{
	packet->ip.src.s_addr = INADDR_ANY;

	if (!(inet_pton(AF_INET, trace->destip, &packet->ip.dest)))
	{
		printf("ft_trace: Can't set destination network address\n");
		exit(EXIT_FAILURE);
	}

	packet->ip.ttl = trace->ttl;
	packet->ip.protocol = IPPROTO_ICMP;
	packet->ip.version = 4;//ipv4
	packet->ip.hl = sizeof(struct iphdr) >> 2;
	packet->ip.pid = htons(trace->pid);
	packet->ip.service = 0;
	packet->ip.off = 0;
	packet->ip.len = sizeof(packet);
	packet->ip.checksum = 0;
	packet->ip.checksum = checksum(&packet->ip, sizeof(struct iphdr));
}
#endif



static void				prepare_header(t_packet *packet, t_trace *trace)
{
	packet->header.type = ICMP_ECHO;
	packet->header.un.echo.id = htons(trace->pid);
	packet->header.un.echo.sequence = htons(trace->sequence);
	packet->header.checksum = 0;
	if (packet->header.checksum == 0)
		packet->header.checksum = 0xffff;
}

void		*prepare_packet_to_send(t_trace *trace, size_t size)
{
	t_packet 	*packet;
	char		*pck;

	if (!(packet = (t_packet*)malloc(sizeof(t_packet))))
		return (NULL);
	ft_bzero(packet, sizeof(*packet));
#ifdef __linux__
	prepare_iphdr(packet, trace);
#endif
	prepare_header(packet, trace);
#ifdef __linux__
	pck = ft_strnew(IPHDR_SIZE + ICMP_HEADER_SIZE + size);
	ft_memcpy(pck, &packet->ip, IPHDR_SIZE);
	ft_memcpy(pck + IPHDR_SIZE, &packet->header, ICMP_HEADER_SIZE);
	ft_memset(pck + IPHDR_SIZE + ICMP_HEADER_SIZE, '0', size);
	packet->header.checksum = checksum(pck + IPHDR_SIZE, ICMP_HEADER_SIZE + size);
	ft_memcpy(pck + IPHDR_SIZE, &packet->header, ICMP_HEADER_SIZE);
#else
	pck = ft_strnew(ICMP_HEADER_SIZE + size);
	ft_memcpy(pck, &packet->header, ICMP_HEADER_SIZE);
	ft_memset(pck + ICMP_HEADER_SIZE, '0', size);
	packet->header.checksum = checksum(pck, ICMP_HEADER_SIZE + size);
	ft_memcpy(pck, &packet->header, ICMP_HEADER_SIZE);
#endif
	destruct_packet_send(packet);
	return (pck);
}

void					destruct_packet_send(t_packet *packet)
{
	free(packet);
}