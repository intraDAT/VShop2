/* 
 *
 * prototypes for  skey.c 
 *
 */

#ifdef __cplusplus 
  extern "C" {
#endif

int cryptpassword(char **, char *, char *);
int decryptpassword(char **, char *, char *);
int skey_addbin(skey_addbin_t *, char *, char *, char **, unsigned int *, unsigned int *, unsigned int *);
skey_addbin_t *skey_addbin_new(void);
int skey_compare(skey_compare_t *, char *, char *, unsigned *, unsigned *);
skey_compare_t *skey_compare_new(void);
int skey_convert(skey_convert_t *, char *, char **, unsigned int *);
skey_convert_t *skey_convert_new(void);
int skey_crypt(skey_crypt_t *, char *, char *, char **, unsigned int *, unsigned int *);
skey_crypt_t *skey_crypt_new(void);
void skey_dest_addbin(skey_dest_t *, void **, char *, char *, char *, unsigned int *, unsigned int *, unsigned int *);
void skey_dest_compare(skey_dest_t *, void **, char *, char *, unsigned int *, unsigned int *);
void skey_dest_convert(skey_dest_t *, void **, char *, char *, unsigned int *, unsigned int *);
void skey_dest_crypt(skey_dest_t *, void **, char *, char *, char *, unsigned int *, unsigned int *, unsigned int *);
void skey_dest_divbin(skey_dest_t *, void **, char *, char *, char *, char *, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
void skey_dest_mod(skey_dest_t *, void **, char *, char *, char *, char *, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
void skey_dest_mulbin(skey_dest_t *, void **, char *, char *, char *, unsigned int *, unsigned int *, unsigned int *);
skey_dest_t *skey_dest_new(void);
void skey_dest_read(skey_dest_t *, void **, char *, char *, unsigned int *, unsigned int *);
void skey_dest_subbin(skey_dest_t *, void **, char *, char *, char *, unsigned int *, unsigned int *, unsigned int *);
void skey_dest_unmod(skey_dest_t *, void **, char *, char *, char *, char *, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
int skey_divbin(skey_divbin_t *, char *, char *, char **, char **, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
skey_divbin_t *skey_divbin_new(void);
int skey_getmem(skey_getmem_t *, void **, void **, char **, unsigned int *, unsigned int *);
skey_getmem_t *skey_getmem_new(void);
int skey_mem_alloc(void **, unsigned int *);
int skey_mod(skey_mod_t *, char *, char *, char *, char **, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
skey_mod_t *skey_mod_new(void);
int skey_mulbin(skey_mulbin_t *, char *, char *, char **, unsigned int *, unsigned int *, unsigned int *);
skey_mulbin_t *skey_mulbin_new(void);
int skey_read(skey_read_t *, char *, char **, unsigned int *, unsigned int *);
skey_read_t *skey_read_new(void);
int skey_realptr(skey_dest_t *, void **, void *, void *);
int skey_rmlg(char **, unsigned int *);
int skey_rmlz(char **, unsigned int *);
int skey_store_ptr(skey_getmem_t *, void **, void **, void *, unsigned int *);
int skey_subbin(skey_subbin_t *, char *, char *, char **, unsigned int *, unsigned int *, unsigned int *);
skey_subbin_t *skey_subbin_new(void);
int skey_unmod(skey_unmod_t *, char *, char *, char *, char **, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
skey_unmod_t *skey_unmod_new(void);
int skey_unmod_test(unsigned int *);


#ifdef __cplusplus 
}
#endif
