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
    void                    (*prepare)();
	void					(*update_checksum)();
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
	int						packet_len;
}							t_trace;

# include "proto.h"

# define FLAGS_SIZE			6

# define F_MAXHOPS			trace->flags[0]->actif
# define F_FIRSTHOPS		trace->flags[1]->actif
# define F_PRINT_HOP_ADDR	trace->flags[2]->actif
# define F_SOCK_DEBUG		trace->flags[3]->actif
# define F_DONTROUTE		trace->flags[4]->actif
# define F_PROTOCOL			trace->flags[5]->actif

# define NB_TEST_CONNECTION	3

/*
** flags manager
*/
BOOLEAN						load_flags(t_trace *trace, int argc, char **argv);
BOOLEAN						load_flag_list(t_trace *trace);
BOOLEAN						icmp_initialize_connection(t_trace *trace, int ttl);
BOOLEAN						set_flags_values(t_trace *trace);

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
void						*prepare_packet_to_send(t_trace *trace, size_t size);
void						destruct_packet_send(t_packet *packet);

/*
** Handler
*/
struct in_addr				*icmp_handle_message(t_trace *trace);
struct in_addr				*icmp_process_received_packet(t_trace *trace, struct sockaddr_in *addr);

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

#endif
