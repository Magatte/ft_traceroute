/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proto.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/12 21:38:45 by jguyet            #+#    #+#             */
/*   Updated: 2017/09/12 21:38:47 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROTO_H
# define PROTO_H

/*
** INTERNET PROTOCOL IP
*/
# define PROT_INTERNET_IPV4 AF_INET
# define PROT_INTERNET_IPV6 AF_INET6

/*
** Fluxs types
*/
# define BINARY_SOCK_FLUX   SOCK_STREAM
# define INTERNAL_NETWORK_FLUX SOCK_RAW
# define SMALL_NETWORK_FLUX SOCK_DGRAM

# ifdef __linux__
/*
** ON linux OS INTERNAL_NETWORK
*/
#  define NETWORK_FLUX      INTERNAL_NETWORK_FLUX
# else
/*
** On other OS SMALL_NETWORK_FLUX
*/
#  define NETWORK_FLUX      SMALL_NETWORK_FLUX
# endif

/*
** SOCK PROTOCOL
*/
# ifndef IPPROTO_GRE
#  define IPPROTO_GRE	47
# endif

# define DEFAULT_PROTOCOL   0
# define ICMP_PROTOCOL      IPPROTO_ICMP
# define TCP_PROTOCOL       IPPROTO_TCP
# define UDP_PROTOCOL       IPPROTO_UDP
# define GRE_PROTOCOL       IPPROTO_GRE

/*
** setsockopt options :
*/
# define EPROTONOSUPPORT    93
# define EAFNOSUPPORT       97

# define SOL_TCP            6
# define SOL_IP             0

# define TCP_KEEPCNT        5
# define TCP_KEEPIDLE       5
# define TCP_KEEPINTVL      1

/*
** ICMP MESSAGE TYPES
*/
# define ICMP_ECHOREPLY		0	/* Echo Reply			    */
# define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
# define ICMP_SOURCE_QUENCH	4	/* Source Quench			*/
# define ICMP_REDIRECT		5	/* Redirect (change route)	*/
# define ICMP_ECHO			8	/* Echo Request				*/
# define ICMP_TIME_EXCEEDED	11	/* Time Exceeded			*/
# define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
# define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
# define ICMP_TIMESTAMPREPLY 14	/* Timestamp Reply			*/
# define ICMP_INFO_REQUEST	15	/* Information Request		*/
# define ICMP_INFO_REPLY	16	/* Information Reply		*/
# define ICMP_ADDRESS		17	/* Address Mask Request		*/
# define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
# define NR_ICMP_TYPES		18

# define ICMP_MINLEN		28

/*
** Changes the default value set by the TCP/IP service provider in the
** TTL field of the IP header in outgoing datagrams.
** IP_TTL support is not required;
** to check whether IP_TTL is supported,
** use getsockopt to get current options.
** If getsockopt fails, IP_TTL is not supported.
*/
# define TCP_IP_PACKET_HEADER_SERVICE IP_TTL

/*
** unsigned typedef
*/
typedef	unsigned char		u_char;
typedef	unsigned short		u_short;
typedef	unsigned int		u_int;
typedef	unsigned long		u_long;


#ifdef __unix__
	#if BYTE_ORDER == BIG_ENDIAN
		#define htons(n) (n)
	#else
		#define htons(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
	#endif
#endif

/*
** ICMP message HEADER
*/
struct icmphdr
{
	u_char					type;		/* message type		*/
	u_char					code;		/* type sub-code	*/
	u_short					checksum;	/* sum of msglength	*/
	union
	{
		struct
		{
			u_short			id;			/* current processid*/
			u_short			sequence;	/* sequence id		*/
		}					echo;		/* echo datagram	*/
		unsigned int		gateway;	/* gateway address	*/
	}						un;			/* union			*/
};

struct udphdr
{
    u_short                 source;     /* source processid */
    u_short                 dest;       /* dest port        */
    u_short                 len;        /* udp header size  */
    u_short                 checksum;   /* sum of msglength */
};

/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
struct tcphdr {
	u_short                 source;		    /* source port */
	u_short                 dest;		    /* destination port */
	u_long                  sequence;		/* sequence number */
	u_long                  ack;			/* acknowledgement number */
#if BYTE_ORDER == LITTLE_ENDIAN 
	u_char                  th_x2:4,		/* (unused) */
		                    th_off:4;		/* data offset */
#endif
#if BYTE_ORDER == BIG_ENDIAN 
	u_char	                th_off:4,		/* data offset */
		                    th_x2:4;		/* (unused) */
#endif
	u_char	                flags;          /* options of type msg */
#define	TH_FIN              0x01
#define	TH_SYN              0x02
#define	TH_RST              0x04
#define	TH_PUSH             0x08
#define	TH_ACK              0x10
#define	TH_URG              0x20
	u_short                 win;			/* window */
	u_short                 checksum;			/* checksum */
	u_short	                urp;			/* urgent pointer */
};

struct grehdr
{
    u_short                 flags;      /* 0x2001           */
    u_short                 proto;      /* dest port        */
    u_short                 length;     /*                  */
    u_short                 callid;     /* pid + sequenceId */
};

struct iphdr
{
# if BYTE_ORDER == LITTLE_ENDIAN
	u_char                  hl:4,			/* header length */
			                version:4;		/* version */
# endif
# if BYTE_ORDER == BIG_ENDIAN
	u_char                  version:4,		/* version */
			                hl:4;			/* header length */
# endif
	u_char                  service;		/* type of service */
	u_short                 len;			/* total length */
	u_short                 pid;			/* identification */
	u_short                 off;			/* fragment offset field */
# define	                IP_DF 0x4000	/* dont fragment flag */
# define	                IP_MF 0x2000	/* more fragments flag */
	u_char                  ttl;			/* time to live */
	u_char                  protocol;		/* protocol */
	u_short                 checksum;		/* checksum */
	struct in_addr          src;		/* source and dest address */
	struct in_addr          dest;	/* source and dest address */
};

/*
** icmp packet struct
*/
# define ICMP_HEADER_SIZE	sizeof(struct icmphdr)
# define IPHDR_SIZE 		sizeof(struct iphdr)
# define PACKET_X64 		(60 - ICMP_HEADER_SIZE)

typedef struct				s_packet
{
# ifdef __linux__
	struct iphdr			ip;
# endif
	struct icmphdr			icmp_header;				/* header of message send 	*/
    struct udphdr           udp_header;
    struct tcphdr           tcp_header;
    struct grehdr           gre_header;
}							t_packet;

/*
** prepare functions
*/
# ifdef __linux__
void		                prepare_iphdr(t_packet *packet, t_trace *trace);
# endif
void                        prepare_icmp_header(t_packet *packet, t_trace *trace);
void		                prepare_udp_header(t_packet *packet, t_trace *trace);
void		                prepare_tcp_header(t_packet *packet, t_trace *trace);
void		                prepare_gre_header(t_packet *packet, t_trace *trace);

void					    update_icmp_checksum(t_packet *packet, t_trace *trace,\
						    void *final_packet, size_t iphdr_size, size_t size);
void					    update_udp_checksum(t_packet *packet, t_trace *trace,\
						    void *final_packet, size_t iphdr_size, size_t size);
void					    update_tcp_checksum(t_packet *packet, t_trace *trace,\
						    void *final_packet, size_t iphdr_size, size_t size);
void					    update_gre_checksum(t_packet *packet, t_trace *trace,\
						    void *final_packet, size_t iphdr_size, size_t size);

static const struct protocole protos[MAX_PROTOCOLS] = {
    {
        IP,
        "ip",
        sizeof(struct iphdr),
        DEFAULT_PROTOCOL,
# ifdef __linux__
        prepare_iphdr,
# else
        NULL,
# endif
        NULL

    },
    {
        ICMP,
        "icmp",
        sizeof(struct icmphdr),
        ICMP_PROTOCOL,
        prepare_icmp_header,
        update_icmp_checksum
    },
    {
        UDP,
        "udp",
        sizeof(struct udphdr),
        UDP_PROTOCOL,
        prepare_udp_header,
        update_udp_checksum
    },
    {
        TCP,
        "tcp",
        sizeof(struct tcphdr),
        TCP_PROTOCOL,
        prepare_tcp_header,
        update_tcp_checksum
    },
    {
        GRE,
        "gre",
        sizeof(struct grehdr),
        GRE_PROTOCOL,
        prepare_gre_header,
        update_gre_checksum
    }
};

#endif