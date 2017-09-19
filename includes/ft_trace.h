/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_trace.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jguyet <jguyet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 02:45:20 by jguyet            #+#    #+#             */
/*   Updated: 2017/05/24 02:50:31 by jguyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TRACE_H
# define FT_TRACE_H

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <termios.h>
# include <stdio.h>
# include <fcntl.h>
# include <libft.h>
# include <printf.h>
# include <mapft.h>
# include <unistd.h>
# include <netdb.h>

# include "protocol_message.h"

typedef enum                e_proto_enum
{
    IP,
    ICMP,
    UDP,
    TCP,
    GRE,
    MAX_PROTO
}                           t_proto_enum;

# define MAX_PROTOCOLS MAX_PROTO

/*
** Flags
*/
typedef struct				s_flag
{
	BOOLEAN					actif;		/* is actif flag				*/
	char					*name;		/* name of flag					*/
	BOOLEAN					special;	/* has special arguments		*/
	char					*help;		/* text view on --help			*/
	char					*value;		/* arguments					*/
	int						type;		/* type of value				*/
	char					*error;		/* error message				*/
}							t_flag;

struct protocole
{
    t_proto_enum            e_name;
	char					*name;
    size_t                  len;
    int                     proto;
    void                    (*prepare_header)();
	void					(*serialize)();
};

/*
** trace struct
*/
typedef struct				s_trace
{
	char					*shost;		/* string hostargs				*/
	int						port;		/* port of connection			*/
	int						sock;		/* socket descriptor ID			*/
	struct sockaddr_in		addr;		/* sockaddr of destination		*/
	char					*destip;	/* ip of Destination			*/
	int						pid;		/* pid of current program		*/
	int						ttl;		/* time to live  				*/
	int						max_hop;
	int						sequence;	/* sequence id  				*/
	int						received;	/* total received messages  	*/
	int						send;		/* total sended messages  		*/
	int						sweepmaxsize;
	int						sweepminsize;
	int						sweepincrsize;
	t_flag					**flags;	/* map of flags					*/
	BOOLEAN					(*launch)();/* pointer of function launch	*/
	long					start_time;	/* timer						*/
	struct timeval			timeout;	/* timeout						*/
	int						mintime;	/* mintime						*/
	long					totaltime;	/* medium time					*/
	int						maxtime;	/* maxtime						*/
	struct in_addr			**ip_tab;
	const struct protocole	*protocol;
	t_message				*message;
	int						ping_interval;
	BOOLEAN					retry;
    BOOLEAN                 use_ip_header;
    int                     socket_type;
}							t_trace;

# define DEFAULT_PING_INTERVAL 3

# define FLAGS_SIZE			9

# define F_MAXHOPS			trace->flags[0]->actif
# define F_FIRSTHOPS		trace->flags[1]->actif
# define F_PRINT_HOP_ADDR	trace->flags[2]->actif
# define F_SOCK_DEBUG		trace->flags[3]->actif
# define F_DONTROUTE		trace->flags[4]->actif
# define F_PROTOCOL			trace->flags[5]->actif
# define F_PORT				trace->flags[6]->actif
# define F_DEFAULT			trace->flags[7]->actif
# define F_IP_HDR			trace->flags[8]->actif

# define NB_TEST_CONNECTION	1

/*
** Socket connection
*/
BOOLEAN						initialize_socket_connection(t_trace *trace);
# ifdef CONNECTION_FILE
BOOLEAN						bind_socket(t_trace *trace);
BOOLEAN						set_on_socket_options(t_trace *trace);
BOOLEAN						socket_connection_is_estabilised(int fd);
BOOLEAN						bind_error(void);
BOOLEAN						set_socket_options_error(void);
# endif

/*
** flags manager
*/
BOOLEAN						load_flags(t_trace *trace, int argc, char **argv);
BOOLEAN						load_flag_list(t_trace *trace);
BOOLEAN						set_flags_values(t_trace *trace);
BOOLEAN						print_help(t_trace *trace);

/*
** trace.c
*/
t_trace						*singleton_trace(void);
void						destruct_trace(t_trace *trace);
BOOLEAN						process_traceroute(t_trace *trace);
BOOLEAN						process_three_request(t_trace *trace);
BOOLEAN						sendto_message(t_trace *trace);

/*
** Messages
*/
t_message					*new_message(size_t size);
BOOLEAN						serialize_message(t_message *message, t_trace *trace);
void						destruct_message(t_message *packet);

/*
** Handler
*/
struct in_addr				*icmp_handle_message(t_trace *trace);
struct in_addr				*process_received_message(t_trace *trace, struct sockaddr_in *addr);

/*
** Utils
*/
unsigned short				checksum(void *b, int len);
long						get_current_time_millis();
char						*get_hostname_ipv4(struct in_addr *addr);
char						*get_hostname_ipv6(struct in6_addr *addr);
struct sockaddr_in			*get_sockaddr_in_ipv4(char *host);
char						*get_hostname_by_in_addr(const struct in_addr *addr);
const struct protocole    	*get_protocol(t_proto_enum e);
const struct protocole    	*get_protocol_by_name(char *name);

/*
** Array ip tab
*/
BOOLEAN						ip_tab_contains(t_trace *trace, struct in_addr *addr);
void						reset_ip_tab(t_trace *trace);
void						free_ip_tab(t_trace *trace);
BOOLEAN						load_ip_tab(t_trace *trace);

/*
** prococol headers
*/
void		                prepare_iphdr(t_message *message, t_trace *trace);
void                        prepare_icmp_header(t_message *message, t_trace *trace);
void		                prepare_udp_header(t_message *message, t_trace *trace);
void		                prepare_tcp_header(t_message *message, t_trace *trace);
void		                prepare_gre_header(t_message *message, t_trace *trace);

void					    update_icmp_checksum(t_message *message, t_trace *trace, size_t iphdr_size);
void					    update_udp_checksum(t_message *message, t_trace *trace, size_t iphdr_size);
void					    update_tcp_checksum(t_message *message, t_trace *trace, size_t iphdr_size);
void					    update_gre_checksum(t_message *message, t_trace *trace, size_t iphdr_size);

static const struct protocole protos[MAX_PROTOCOLS] = {
    {
        IP,
        "ip",
        sizeof(struct iphdr),
        DEFAULT_PROTOCOL,
        prepare_iphdr,
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
