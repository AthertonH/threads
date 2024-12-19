#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

/* Global storage for arguments */
struct {
    int threads;
    double limit;
} args;

int
main(int argc, char** argv) {
    char c;
    memset(&args, 0, sizeof(args));
    while ((c = getopt(argc, argv, "l:t:")) != -1) {
	switch (c) {
	case 'l':
	    args.limit = atol(optarg);
	    break;
	case 't':
	    args.threads = atoi(optarg);
	    break;
	default:
	    fprintf(stderr, "Unknown argument %c\n", c);
	    return -1;
	}
    }

    if ((args.threads <= 0) || (args.limit < 100)) {
	fprintf(stderr,
		"Threads (>0 ) and limit (>= 100) arguments required\n");
	return -1;
    }

    printf("Threads:%d Limit:%f\n", args.threads, args.limit);
    
    return 0;
}
