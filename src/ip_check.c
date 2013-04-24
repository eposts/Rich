#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int if_a_string_is_a_valid_ipv4_address(const char *str)
{
    struct in_addr addr;
    int ret;
    volatile int local_errno;

    errno = 0;
    ret = inet_pton(AF_INET, str, &addr);
    local_errno = errno;
    if (ret > 0)
        fprintf(stderr, "\"%s\" is a valid IPv4 address\n", str);
    else if (ret < 0)
        fprintf(stderr, "EAFNOSUPPORT: %s\n", strerror(local_errno));
    else 
        fprintf(stderr, "\"%s\" is not a valid IPv4 address\n", str);

    return ret;
}
