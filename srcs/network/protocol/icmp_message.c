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
void		prepare_iphdr(t_packet *packet, t_trace *trace)
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

void		prepare_icmp_header(t_packet *packet, t_trace *trace)
{
	packet->icmp_header.type = ICMP_ECHO;
	packet->icmp_header.un.echo.id = htons(trace->pid);
	packet->icmp_header.un.echo.sequence = htons(trace->sequence);
	packet->icmp_header.checksum = 0;
}

void		prepare_udp_header(t_packet *packet, t_trace *trace)
{
	packet->udp_header.source = htons(trace->pid);
	packet->udp_header.dest = htons(trace->port + trace->sequence);
	packet->udp_header.len = htons((u_short)(trace->packet_len - sizeof(struct iphdr)));
	packet->udp_header.checksum = 0;
	packet->udp_header.sec = trace->sequence;
	packet->udp_header.ttl = trace->ttl;
	packet->udp_header.tv = trace->timeout;
}

void		prepare_tcp_header(t_packet *packet, t_trace *trace)
{
	packet->tcp_header.source = htons(trace->pid);
	packet->tcp_header.dest = htons(trace->port + trace->sequence);
	packet->tcp_header.sequence = (packet->tcp_header.source << 16) | packet->tcp_header.dest;
	packet->tcp_header.ack = 0;
	packet->tcp_header.th_off = 5;
	packet->tcp_header.flags = TH_SYN;
	packet->tcp_header.checksum = 0;
}

void		prepare_gre_header(t_packet *packet, t_trace *trace)
{
	packet->gre_header.flags = htons(0x2001);
	packet->gre_header.proto = htons(trace->port);
	packet->gre_header.length = 0;
	packet->gre_header.callid = htons(trace->pid + trace->sequence);
}

void		update_icmp_checksum(t_packet *packet, t_trace *trace,\
			void *final_packet, size_t iphdr_size, size_t size)
{
	ft_memcpy(final_packet + iphdr_size, &packet->icmp_header, trace->protocol->len);
	ft_memset(final_packet + iphdr_size + trace->protocol->len, '0', size);
	packet->icmp_header.checksum = checksum(final_packet + iphdr_size, trace->protocol->len + size);
	ft_memcpy(final_packet + iphdr_size, &packet->icmp_header, trace->protocol->len);
}

void		update_udp_checksum(t_packet *packet, t_trace *trace,\
			void *final_packet, size_t iphdr_size, size_t size)
{
	ft_memcpy(final_packet + iphdr_size, &packet->udp_header, trace->protocol->len);
	ft_memset(final_packet + iphdr_size + trace->protocol->len, '0', size);
	packet->udp_header.checksum = checksum(final_packet + iphdr_size, trace->protocol->len + size);
	ft_memcpy(final_packet + iphdr_size, &packet->udp_header, trace->protocol->len);
}

void		update_tcp_checksum(t_packet *packet, t_trace *trace,\
			void *final_packet, size_t iphdr_size, size_t size)
{
	ft_memcpy(final_packet + iphdr_size, &packet->tcp_header, trace->protocol->len);
	ft_memset(final_packet + iphdr_size + trace->protocol->len, '0', size);
	packet->tcp_header.checksum = checksum(final_packet + iphdr_size, trace->protocol->len + size);
	ft_memcpy(final_packet + iphdr_size, &packet->tcp_header, trace->protocol->len);
}

void		update_gre_checksum(t_packet *packet, t_trace *trace,\
			void *final_packet, size_t iphdr_size, size_t size)
{
	ft_memcpy(final_packet + iphdr_size, &packet->gre_header, trace->protocol->len);
	ft_memset(final_packet + iphdr_size + trace->protocol->len, '0', size);
}

void		*prepare_packet_to_send(t_trace *trace, size_t size)
{
	t_packet 	*packet;
	char		*final_packet;
	size_t		iphdr_size;

	if (!(packet = (t_packet*)malloc(sizeof(t_packet))))
			return (NULL);
	iphdr_size = 0;
	ft_bzero(packet, sizeof(*packet));
#ifdef __linux__
	iphdr_size = IPHDR_SIZE;
	prepare_iphdr(packet, trace);
#endif
	trace->packet_len = trace->protocol->len + iphdr_size + trace->sweepminsize;
	trace->protocol->prepare(packet, trace);

	final_packet = ft_strnew(iphdr_size + trace->protocol->len + size);
#ifdef __linux__
	ft_memcpy(final_packet, &packet->ip, IPHDR_SIZE);
#endif
	trace->protocol->update_checksum(packet, trace, final_packet, iphdr_size, size);
	destruct_packet_send(packet);
	return (final_packet);
}


void			destruct_packet_send(t_packet *packet)
{
	free(packet);
}
