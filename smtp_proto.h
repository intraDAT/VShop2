/*
 *
 * prototypes for smtp
 *
 *
 */

#ifdef __cplusplus 
extern "C" {
#endif

int smtp_data (int, char *, char *);
int smtp_set_error (char *, char *, char *, char *, char *, int);
int smtp_establishe (char *, int *);
int smtp_expn (int, char *, char *);
int smtp_header (char *, char *, char *, int, int, char *);
int smtp_subheader (char *, char *, char *, int, char *);
int smtp_helo (int, char *, char *);
void smtp_log (smtp_stack_t *, smtp_stack_t *, smtp_stack_t *, FILE *);
int smtp_mail (int, char *, char *);
int smtp_quit (int, char *);
int smtp_rcpt (int, char *, char *);
int smtp_rset (int, char *);
int smtp_saml (int, char *, char *);
int smtp_send (int, char *, char *);
int smtp_sm_mail (char *, char *, char *, char *, char **, headstr *, int, FILE *);
int smtp_soml (int, char *, char *);
char * smtp_spop (smtp_stack_t *);
int smtp_spush (smtp_stack_t *, char *);
int smtp_vrfy (int, char *, char *);

#ifdef __cplusplus 
}
#endif
