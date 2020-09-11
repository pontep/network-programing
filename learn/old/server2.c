#include <sys/types.h>
#include <sys/socket.h>

int main()
{

    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if ( fd == -1 ){
        // Error: unable to create socker.
        printf("Error: unable to create socker.\n");
    }
    return 0;
}