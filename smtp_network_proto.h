/*
 *
 * network prototypes
 *
 */

#ifdef __cplusplus 
extern "C" {
#endif


int tcp_connect (const char *, const char *);
int tcp_disconnect (int);
ssize_t tcp_read (int, char *);
ssize_t tcp_write (int, const void *, size_t);


#ifdef __cplusplus 
}
#endif
