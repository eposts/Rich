#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char const* argv[])
{
    int mode;
    struct flock fl = {F_WRLCK, SEEK_SET, 0, 0, 0};
    struct stat buf;
    int fd;

    if (stat("testfile.txt", &buf) < 0) {
        perror("Cannot stat file testfile.txt\n");
        return -1;
    }

    /* get currently set mode */
    mode = buf.st_mode;

    /* FIXME remove group execute permission from mode */
    mode &= ~(S_IEXEC >> 3);

    /* set 'set group id bit' in mode */
    mode |= S_ISGID;

    if (chmod("testfile.txt", mode) < 0) {
        perror("chmod error");
        return -1;
    }

    fl.l_pid = getpid();

    if ((fd = open("testfile.txt", O_RDWR)) == -1)
    {
        perror("open");
        return -1;
    }

    printf("Press <RETURN> to try get lock:");
    getchar();
    printf("Trying to get lock...");

    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl:");
        return -1;
    }
    printf("got lock\n");
    printf("Press <RETURN> to release lock\n");
    getchar();

    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK,&fl) == -1) {
        perror("fcntl");
        return -1;
    }
    printf("Unlocked.\n");
    close(fd);
}
