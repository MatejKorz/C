#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>  /* sockaddr */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop */
#include <unistd.h>     /* close */
#include <stdlib.h>     /* NULL, rand */
#include <stdio.h>      /* printf */
#include <stdint.h>     /* uint16_t */
#include <string.h>


typedef struct
{
    unsigned char data[ 512 ];
    int size;

} packet_t;

typedef struct
{
    uint16_t id;
    const char *domain;

} question_t;

typedef struct
{
    uint16_t id;
    uint16_t header_flags;
    uint32_t ttl;
    char addr[ 16 ];

} answer_t;

#define QR 0x8000
#define AA 0x400
#define TC 0x200
#define RD 0x100
#define RA 0x80

#define QTYPE 28
#define QCLASS 1

#define PACKET_MAXLEN 512
#define PACKET_MINLEN 38
#define HEADER_LEN 12

void init_packet(packet_t *packet) {
    packet->size = 0;
    memset(packet->data, 0, PACKET_MAXLEN);
}

void make_header(uint16_t id, unsigned char *header) {
    memset(header, 0, HEADER_LEN);
    uint16_t wrt = htons(id);
    memcpy(header, &wrt, sizeof(uint16_t));

    uint16_t flags = RD;
    wrt = htons(flags);
    memcpy(header + 2, &wrt, 2);

    uint16_t qdcount = 1;
    wrt = htons(qdcount);
    memcpy(header + 4, &wrt, 2);
}

void convert_domain(const char *domain, unsigned char *res) {
    int domain_len = 0;
    unsigned char *ptr = (unsigned char *) domain;
    while (*ptr != '\000') {
        domain_len++;
        ptr++;
    }
    char domain_cpy[domain_len + 1];
    int res_len = 0;
    memset(domain_cpy, 0, domain_len + 1);
    memset(res, 0, domain_len + 1);
    memcpy(domain_cpy, domain, domain_len);

    char *tok = strtok(domain_cpy, ".");
    while (tok) {
        unsigned char len = 0;
        char *ptr = tok;
        while (len < 255 && *ptr != '\000') {
            len++;
            ptr++;
        }
        memcpy(res + res_len, &len, 1);
        res_len++;
        memcpy(res + res_len, tok, (size_t) len);
        res_len += len;
        tok = strtok(NULL, ".");
    }
}

int create_query( const question_t *question, packet_t *packet ) {
    if (HEADER_LEN + strlen(question->domain) + 1 + 4 >= PACKET_MAXLEN) {
        return -1;
    }
    make_header(question->id, packet->data);
    packet->size += HEADER_LEN;

    convert_domain(question->domain, packet->data + packet->size);
    packet->size += strlen(question->domain) + 2;

    uint16_t wrt = htons(QTYPE);
    memcpy(packet->data + packet->size, &wrt, 2);
    packet->size += 2;

    wrt = htons(QCLASS);
    memcpy(packet->data + packet->size, &wrt, 2);
    packet->size += 2;

    return 0;
}

int process_answer( const packet_t *packet, answer_t *answer ) {
    if (packet->size < PACKET_MINLEN) {
        return -1;
    }
    uint16_t net;
    memcpy(&net, packet->data, 2);
    answer->id = ntohs(net);
    memcpy(&net, (packet->data + 2), 2);
    answer->header_flags = ntohs(net);

    uint16_t rcode = answer->header_flags & 0b0000000000001111;
    if (rcode != 0) {
        return -1;
    }

    memcpy(&net, (packet->data + 4), 2);
    uint16_t req_cnt = ntohs(net);

    memcpy(&net, (packet->data + 6), 2);
    uint16_t answer_cnt = ntohs(net);
    if (answer_cnt < 1) {
        return -1;
    }

    unsigned char *ptr = (unsigned char *) packet->data + HEADER_LEN;

    // skip requests
    while (req_cnt > 0) {
        while (*ptr != '\000') {
            ptr += *ptr + 1;
        }
        ptr += 5;
        req_cnt--;
    }

    uint8_t mask = 0xc0;
    while (*ptr != '\000') {
        if ((*ptr & mask)) {
            ptr++;
            break;
        }
        ptr += *ptr + 1;
    }
    ptr++;
    memcpy(&net, ptr, sizeof(uint16_t));
    uint16_t type = ntohs(net);
    memcpy(&net, ptr + 2, sizeof(uint16_t));
    uint16_t class = ntohs(net);
    uint32_t ttl_net;
    memcpy(&ttl_net, ptr + 4, sizeof(uint32_t));
    uint32_t ttl = ntohl(ttl_net);
    memcpy(&net, ptr + 8, sizeof(uint16_t));
    uint16_t rd_len = ntohs(net);
    if (type == 28 && class == 1) {
        answer->ttl = ttl;
        memcpy(answer->addr, ptr + 10, rd_len);
        return 0;
    }

    return -1;
}

#define INVALID_REQUEST 2
#define INVALID_ANSWER 3

int udig( struct sockaddr_in *host, const question_t *question, answer_t *answer ) {
    int rv = EXIT_SUCCESS;
    packet_t *send_packet = malloc(sizeof(packet_t));
    if (!send_packet) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    packet_t *recv_packet = malloc(sizeof(packet_t));
    if (!recv_packet) {
        perror("malloc");
        free(send_packet);
        return EXIT_FAILURE;
    }
    init_packet(send_packet);
    init_packet(recv_packet);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        rv = EXIT_FAILURE;
        goto err;
    }

    if (create_query(question, send_packet) == -1) {
        rv = INVALID_REQUEST;
        goto err;
    }

    if (sendto(sock_fd, send_packet->data, send_packet->size, 0, (struct sockaddr*) host, sizeof(*host)) == -1) {
        perror("sendto");
        rv = EXIT_FAILURE;
        goto err;
    }

    ssize_t byte_recv;
    struct sockaddr_in recv_addr;
    socklen_t recv_len = sizeof(recv_addr);
    byte_recv = recvfrom(sock_fd, recv_packet->data, PACKET_MAXLEN, 0, (struct sockaddr *) &recv_addr, &recv_len);
    if (byte_recv == -1) {
        perror("recvfrom");
        rv = EXIT_FAILURE;
        goto err;
    }

    recv_packet->size = byte_recv;

    if (process_answer(recv_packet, answer) == -1) {
        rv = INVALID_ANSWER;
        goto err;
    }

    if (answer->id != question->id) {
        rv = INVALID_ANSWER;
        fprintf(stderr, "Invalid ID\n");
        goto err;
    }

err:
    if (sock_fd != -1) close(sock_fd);
    free(send_packet);
    free(recv_packet);
    return rv;
}

/* tests */
#include <err.h>        /* err */
#include <unistd.h>     /* alarm */
#include <time.h>       /* time */

int main( int argc, char **argv )
{
    const char *host_ip_str = "1.1.1.1";

    const char *name_in_question = "seznam.cz";

    if ( argc == 3 )
    {
        host_ip_str = argv[ 1 ];
        name_in_question = argv[ 2 ];
    }
    else if ( argc > 3 )
    {
        fprintf( stderr, "usage: %s host question\n", *argv );
        return 1;
    }

    alarm( 5 );

    printf( "host → %s\n"
           "resolve → %s\n",
           host_ip_str, name_in_question );

    srand( time( 0 ) );
    question_t question;
    question.id = rand() % ( uint16_t )( -1 );
    question.domain = name_in_question;

    answer_t answer;

    struct sockaddr_in host;
    host.sin_family = AF_INET;
    host.sin_port = htons( 53 );
    if ( inet_pton(AF_INET, host_ip_str, &host.sin_addr ) != 1 )
        errx( 1, "invalid address '%s'", host_ip_str );

    int rv = udig( &host, &question, &answer );
    printf( "udig → %d\n", rv );
    if ( rv != 0 )
        return rv;

    char recv_addr[ INET6_ADDRSTRLEN + 1 ] = { 0 };
    if ( inet_ntop(AF_INET6, &answer.addr, recv_addr,
                  INET6_ADDRSTRLEN) == NULL )
        err( 1, "converting address from answer to string" );

    printf( "answer.id → 0x%x\n", ( int )answer.id );
    printf( "answer.header → 0x%x\n", ( int )answer.header_flags );
    printf( "answer.ttl → %d seconds\n", answer.ttl );
    printf( "answer.addr → %s\n", recv_addr );
    return rv;
}
