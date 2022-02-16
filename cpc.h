/* begin of cpc.h */
#ifndef _CPC_H_1
#define _CPC_H_1 0

#define       sqlint2max               32767
#define       sqlint4min             - 2147483647
#define       sqlint4max               2147483647
#define       sqldbnamelengthmax       18
#define       sqlhostnamelengthmax     64
#define       sqlserverdblengthmax     18
#define       sqlservernodelengthmax   64
#define       sqlsnamelengthmax        12
#define       sqlnamelengthmax         18
#define       sqllnamelengthmax        32
#define       sqllinelengthmax         52
#define       sqlmacrolinelengthmax    30
#define       sqlgamax                  8
#define       sqlmamax                128
#define       sqlarglinelengthmax      132
#define       sqlmaxformkeys           20
#define       sqlmaxlabelkeys          96
#define       sqlmfetchmax             18
#define       sqlmfentrymax            56
#define       sqlnmax                 300
#define       sqldialogmax              4
#define       sqlvrpmx               8240
#define       sqlldmx                 250

/* dialog buffer */

#define       sqldccmax             32760
#define       sqldvcmax             32760
#define       sqlddgmax              5500
#define       sqldigmax             24000
#define       sqldvimax              5000
#define       sqldpdmax               600

/* SQLCODE macro for ANSI compatibility */

#define SQLCODE sqlca.sqlcode

/* SQLSTAE macro for ANSI compatibility */

#define SQLSTATE sqlca.sqlstate

/* Symbole fuer sqlvar - Eintraege */

/* colio: */
#define sqlinppar 0
#define sqloutpar 1
#define sqlinoutpar 2

/* colmode: */
#define sqlval    0
#define sqlundef  1

/* coltype: */
#define sqlfixed     0
#define sqlfloat     1
#define sqlchar      2
#define sqlbyte      3
#define sqldate      4
#define sqltime      5
#define sqlstring    6
#define sqlexpr      7
#define sqltimestamp 8
#define sqldbascii   9
#define sqldbebcdic 10
#define sqloldlongchar 11
#define sqloldlongbyte 12
#define sqlsmallint 15
#define sqlinteger  16
#define sqlvarchar  17
#define sqlescapechar 18
#define sqllong     19
#define sqllongbyte 20
#define sqlboolean  23


/* hostvartype: */
#define sqlvint2     0
#define sqlvint4     1
#define sqlvuns2    16
#define sqlvuns4    17
#define sqlvreal4    2
#define sqlvreal8    3
#define sqlvchar     7
#define sqlvstring2   15
#define sqlvstring1   20
#define sqlvint8      33
#define sqlvstring4   35

/* Macro Definitions for the VARCHAR Pseudotype: */

#define VARCHS(n) struct {unsigned short len; unsigned char arr [n];}
#define VARCHD(t,n) struct t {unsigned short len; unsigned char arr [n];}
#define VARCHR(t) struct t

/* Macro Definitions for the DECIMAL Pseudotype: */

#define DECIMS(n) struct {char arr [n];}

/* SAA sqlda Description: */

#if defined(sql_db2)
#ifndef SQLDASIZE
#define SQLDASIZE(n) (sizeof(struct sqlda) + (n-1)*sizeof(struct sqlvar))
#endif
#endif


typedef short          sqlint2;
typedef unsigned short sqluns2;


  typedef long           sqlint4;
  typedef unsigned long  sqluns4;

typedef float      sqlr4;
typedef double     sqlr8;
typedef char       sqlparsid   [12] ,
                   sqlsname      [sqlsnamelengthmax] ,
                   sqlnname      [sqlnamelengthmax] ,
                   sqllname      [sqllnamelengthmax] ,
                   sqllinetype   [sqllinelengthmax] ,
                   sqlmacroline  [sqlmacrolinelengthmax] ,
                   sqldbname     [sqldbnamelengthmax] ,
                   sqldbnode     [sqlhostnamelengthmax] ,
                   sqlserverdb   [sqlserverdblengthmax] ,
                   sqlservernode [sqlservernodelengthmax] ,
                   sqlhostname   [sqlhostnamelengthmax] ,
                   sqlargline    [sqlarglinelengthmax] ,
                   sqlformkeys   [sqlmaxformkeys] ,
                   sqllabelkeys  [sqlmaxlabelkeys] ,
                   sqluser [18],
                   sqlpassword [18] ,
                   sqlstatement_buffer [8240];
typedef sqlint4    sqlcrypt [6];
typedef DECIMS(1) DECIMAL1;
typedef DECIMS(2) DECIMAL2;
typedef DECIMS(2) DECIMAL3;
typedef DECIMS(3) DECIMAL4;
typedef DECIMS(3) DECIMAL5;
typedef DECIMS(4) DECIMAL6;
typedef DECIMS(4) DECIMAL7;
typedef DECIMS(5) DECIMAL8;
typedef DECIMS(5) DECIMAL9;
typedef DECIMS(6) DECIMAL10;
typedef DECIMS(6) DECIMAL11;
typedef DECIMS(7) DECIMAL12;
typedef DECIMS(7) DECIMAL13;
typedef DECIMS(8) DECIMAL14;
typedef DECIMS(8) DECIMAL15;

struct SQLCOL
{
        sqlint2 coltype;
        sqlint2 colmode;
        sqlint2 colio;
	sqlint2 fill;
        sqlint4 collen;
        sqlint2 colfrac;
        sqlint2 colpos;           /* column buffer position (relative) */
        sqlint2 coliolen;         /* column byte length */
        char colisnum;          /* (numeric column) ? 1 : 0 */
        char colislong;         /* (long column) ? 1 : 0 */
        sqlint2 colvartype;       /* host variable type */
        sqlint2 colindtype;       /* indicator variable type */
#if defined(__cplusplus) || defined(__STDC__)
        int (*coltobuf) (const struct SQLCOL *col, char *buf, const void *va,
                sqlint4 cl, sqlint4 vs, sqlint2 vl, sqlint2 vf);
        int (*colfrombuf) (const struct SQLCOL *col, const char *buf,
                void *va, sqlint4 cl, sqlint4 vs, sqlint2 vl, sqlint2 vf);
                         /* input and output conversion functions */
        int (*coltoindi) (sqlint4 ival, void *indi,sqlint2 indl,sqlint2 indf);
        int (*colfromindi) (sqlint4 *ival, const void *indi, sqlint2 indl,
                sqlint2 indf); /* indicator conversion functions */
#else
        int (*coltobuf) ();
        int (*colfrombuf) (); /* input and output conversion functions */
        int (*coltoindi) ();
        int (*colfromindi) (); /* indicator conversion functions */
#endif
};

struct SQLROW
{
        sqlint2 ireclen;  /* irecord length = Max (icolpos) + its (coliolen) */
        sqlint2 oreclen;  /* orecord length = Max (ocolpos) + its (coliolen) */
};

struct SQLRESTBUFLONG
{
        sqlint2 rbcolno;
        sqlint2 rbrestlen;
        sqlint4 rbhostlensum;
        sqlint4 rblongcolmax;
        char varpart [sqlvrpmx];
};

struct SQLMSG
{
        sqlint4 reserved;
        char protid;
        char msgclass;
        char rteflags;
        char residual;
        sqlint4 sendref;
        sqlint4 recvref;
        char fill [4];
        char msgcode [2];
        sqlint2 msgtype;
        char sendid [8];
        sqlint2 part1l;
        sqlint2 part2l;
        sqlint2 retcode;
        sqlint2 errcode;
        char varpart [sqlvrpmx];
};

struct SQLPCLD
{
#if defined(__cplusplus) || defined(__STDC__)
        void *ldhostaddr;
        void *ldindkaddr;
#else
        char *ldhostaddr;
        char *ldindkaddr;
#endif
        struct SQLCOL *ldcoladdr;
        sqlint4 ldhostlen;
        sqlint2 ldhosttype;
        sqlint2 ldindktype;
        sqlint2 ldindklen;
        sqlint2 ldindkfrac;
        sqlint2 ldrowno;
        sqlint2 ldcolno;
        char ldsfiotype;
        char ldsfdtype;
        sqlint2 ldsfbufpos;
        sqlint4 ldhostlensum;
};

struct OLDSQLSPLD
{
	char     lddsc [8];
        char     ldtabid [8];
        char     lddbno [2];
        char     ldrpl;
        char     ldchange;
        sqlint4  ldintpos;
        char     ldextr;
        char     ldshortsc;
        unsigned char ldvalind;
        char    ldvalmode;
        sqlint2 ldvalpos;
        sqlint2 ldvallen;
};
struct NEWSQLSPLD
{
	char    lddsc [8];
        char    ldsvno [2];
        char    ldchange;
        char    ldrpl;
        sqlint4 ldcurrpag;
        sqlint2 ldcurrpos;
	char    ldcolno;
	char    ldshowkind;
        sqlint4 ldintpos;
        char    ldinfoset;
        char    ldfill1;
        unsigned char ldvalind;
        char    ldvalmode;
        sqlint2 ldvalpos;
        sqlint2 ldvallen;
        sqlint4 ldfullen;
        char    ldfill2 [4];
        sqlint4 ldlastipos;
        sqlint4 ldrootcheck;
};

struct SQLSPLD
{
        sqlint2 ldkind;
        sqlint2 ldlength;
	union
	{
		struct OLDSQLSPLD o;
		struct NEWSQLSPLD n;
	} ld;
};

struct SQLLD
{
        sqlint2 ldmaxi;
        sqlint2 ldmaxo;
        sqlint2 ldindpc;
        sqlint2 ldindsp;
        sqlint2 ldcolkind;
        sqlint2 ldcount;
        char ldputvalsend;
        char allsend;
        sqlint2 ldretmaxi;   /** bf 20.12.94 */
        sqlint2 ldretmaxo;
        sqlint2 ldretindpc;
        sqlint2 ldretindsp;
	struct SQLRESTBUFLONG  *ldrestbufp;  /** bf **/
	struct SQLPCLD ldpc [sqlldmx];
        struct SQLSPLD ldsp [sqlldmx];
};

typedef struct
{
        char  ratermid [8];
        sqlint4  raprocessid;
        char  racomponent  [4];
        sqlint2 rasqlinit,
              raactsession,
              rakamode,
              ramesstype;
        sqlint4  raprofsec;
        struct SQLMSG sqlmsg;
	char rastmbuf [120];
}
sqlratype; /**/

typedef struct
{
 sqlhostname  tatracefn;
 sqlint4         tatraceno;
 sqlint4         tatracepprt;
 sqlint2        tatracety;
 sqlint2        tamaxsecoutp;
 sqlhostname  tamaxsecfn;
 sqlint4         tamaxsecno;
 sqlint4         tamaxsecpptr;
 char        tastartdate [8];
 char        taenddate [8];
 char        tastarttime [8];
 char        taendtime [8];
 sqlint2        tacnttrstm ;
 sqlint2        tastr80l;
 char       tastr80 [80];
}
sqltatype;  /**/

typedef struct     {
 sqlhostname   oamodtracefn;
 sqlint2         oamodtrace;
 sqlint2         oamodtracety;
 sqlint2         oamodsettrty;
 sqlint2         oamaxtrstm;
 sqlint2         oaprofile;
 sqlint2         oanotrtimeoutp;
 sqlint2         oanosdf;
 sqlint2         oauseropset;
 sqlint4          oatraceptr;
 sqlint2         oamaxsec;
}
sqloatype;  /**/

typedef struct     {
                      sqlint4       ga_reference;
                      sqlint2       ga_status;
                      sqlint2       ga_lockset;
                      sqluser       ga_userid;
                      sqlint2       ga_profinit;
                      sqlcrypt      ga_password;
                      sqlint2       ga_timeout;
                      sqlint2       ga_vaindex;
                      sqldbname     ga_serverdb;
                      sqlint2       ga_messcode;
                      sqlhostname   ga_servernode;
                      sqlnname      ga_userkey;
                      sqlint2       ga_nodeindex;
                      char          ga_sqlmode [8]; /**/
                      sqlint4       ga_cachelimit; /**/
                      sqlnname      gaatdbname;   /** at db **/
                      sqlint2       gafiller;
                   }
                   sqlgaentry;

typedef struct     {
  sqluser         gaopuserid;
  sqlint2         gaoplockset;
  sqlcrypt        gaoppassword;
  sqlint2         gamodlockset;
  sqlint2         gaoptimeout;
  sqlhostname     gaopservernode;
  sqldbname       gaopserverdb;
  sqlint2         gasqlconntyp;
  sqlint4         gaopcachelimit;
  sqlint2         gaxuserset;
  sqlint2         gafiller1;
  sqlgaentry    *gaentry [sqlgamax];
     }
     sqlgatype;   /**/
typedef struct     {
                      sqlint2       malen;
                      sqlmacroline  maval;
                   }
                   mavalue;
typedef mavalue    sqlmaentries [sqlmamax] ;
typedef struct     {
                      sqlint4       maversion;
                      sqlmaentries  maentry;
                   }
                   sqlmatype;
typedef struct     {
	              sqlint2          mfInit,
			               mfSelectno,
				       mfBufinit,
                                       mfBuflen,
                                       mfRecpos,
                                       mfReclen,
                                       mfPrevfaspec,
                                       mfLastfaspec;
                      sqlparsid        mfSelparsid;
                      sqlint2          mfRownotfound,
                                       mfRecposbeg;
                      sqlint4          mfMaxselrec,
                                       mfLastrecno,
                                       mfBufbegno,
                                       mfBufendno;
                      struct SQLMSG   *mfBufpointer;
                      sqlint2          mfsqlint4colpos,
                                       mfFiller;
		   }
                   sqlmfentry;
/*typedef char       sqlmfentry [sqlmfentrymax] ;*/
typedef struct     {
                      sqlint2       mfselcnt,
                                    mfetch;
                      sqlmfentry    mfentry [sqlmfetchmax] ;
                   }
                   sqlmftype;

typedef struct     {
                      sqlint2       xainit,
                                    xacmdinit,
                                    xakano;
                      sqlnname      xaprogn;
                      sqlint2       xaprogc;
                      sqlnname      xamodn;
                      sqlint2       xamodc,
                                    xalang,
                                    xabs,
                                    xatrace,
                                    xaprno;
                      sqlsname      xalcheck;
                      sqlnname      xaprname;
#if defined(__cplusplus) || defined(__STDC__)
                      void         *xasqldap;
#else
                      char         *xasqldap;
#endif
                      sqlint2       xacfetspec;
                      sqlint4       xacfetposc;
		      sqlint4	    fill;
                   }
                   sqlcxatype;    /**/

/* dialog buffer */

typedef char sqlcodecache    [sqldccmax];
typedef char sqlvaluecache   [sqldvcmax];
typedef char sqldialogglobal [sqlddgmax];
typedef char sqliglobal      [sqldigmax];
typedef char sqldiaprevars   [sqldvimax];
typedef char sqlprecompdca   [sqldpdmax];
typedef struct
{
        sqlint2         sqlcursor,
                        sqlrelkey,
                        fill,
                        sqlerrno;
        char            errmsg [64];
        sqlnname        action,
                        function1,
                        function2,
                        function3,
                        function4,
                        funct;
} sqldiaresult;
typedef struct     {
                      sqlint2 ldcc,
                              ldvc,
                              lddg,
                              ldig,
                              ldvi,
                              ldpd;
#if defined(__cplusplus) || defined(__STDC__)
                      void
#else
                      char
#endif
			     *pdcc,
                             *pdvc,
                             *pddg,
                             *pdig,
                             *pdvi,
                             *pdpd;
                      sqldiaresult diaresult;
                   }
                   sqldiaenv;

#if defined(sql_oracle) && ! defined(__cplusplus)
typedef struct sqlca {
#else
typedef struct     {
#endif
                      sqlint4       sqlenv;
                      char          sqlcaid [8] ;
#ifdef sql_oracle
                      sqlint4       sqlabc,
#else
                      sqlint4       sqlcabc,
#endif
                                    sqlcode;
#ifdef sql_oracle
                      struct {
                        sqluns2 sqlerrml;
                        char           sqlerrmc [70];
                        } sqlerrm;
#else
                      sqlint2       sqlerrml;
                      char          sqlerrmc [70] ;
#endif
                      char          sqlerrp [8] ;
                      sqlint4       sqlerrd [6] ;
#ifdef sql_oracle
                      char          sqlwarn [16];
#else
                      char          sqlwarn0,
                                    sqlwarn1,
                                    sqlwarn2,
                                    sqlwarn3,
                                    sqlwarn4,
                                    sqlwarn5,
                                    sqlwarn6,
                                    sqlwarn7,
                                    sqlwarn8,
                                    sqlwarn9,
                                    sqlwarna,
                                    sqlwarnb,
                                    sqlwarnc,
                                    sqlwarnd,
                                    sqlwarne,
                                    sqlwarnf;
#endif
                      char          sqlext [12] ;
                      sqlnname      sqlresn;
                      sqlint2       sqlcursor,
                                    sqlpfkey,
                                    sqlrowno,
                                    sqlcolno,
				    sqlmfetch;
		      sqlint4       sqltermref;
		      sqlint2       sqldiapre,
                                    sqldbmode,
                                    sqldatetime;
		      char	    sqlstate [6];
                      sqlargline    sqlargl;
                      sqlgatype     *sqlgap;
                      sqlratype     *sqlrap;
                      sqloatype     *sqloap;
                      sqlmatype     *sqlmap;
                      sqlmftype     *sqlmfp;
                      struct SQLLD  *sqlldp;
                      sqltatype     *sqltap;
                      sqldiaenv     *sqlplp;
                      sqlcxatype    sqlcxa;
                   }
                   sqlcatype;

#if defined (sql_sqldb) || defined (sql_default) || defined (sql_ansi)
typedef struct     {
                      sqlnname      colname;
                      sqlint2       colio,
                                    colmode,
                                    coltype;
                      sqlint4       collength;
                      sqlint2       colfrac,
			            colfiller,
                                    hostvartype,
                                    hostcolsize;
                      sqlint4       hostindicator;
#if defined(__cplusplus) || defined(__STDC__)
                      void         *hostvaraddr;
#else
                      char         *hostvaraddr;
#endif
                      sqlint4      *hostindaddr;
                      struct SQLCOL col;
              }
                   sqlvartype;
typedef struct     {
                      char          sqldaid [8] ;
                      sqlint4       sqlmax;
                      sqlint2       sqln,
                                    sqld;
                      sqlint4       sqlloop,
                                    sqloffset;
                      sqlint2       sqlkano,
                                    sqlprno,
                                    sqlkamode,
                                    sqlfill;
                      struct SQLROW sqlrow;
                      sqlvartype    sqlvar [sqlnmax] ;
                   }
                   sqldatype;

#endif

#if defined (sql_db2)
/**/
/*  SAA - sqlda Description */
/**/
struct sqlda       {
                   unsigned char sqldaid [8];
                   sqlint4       sqldabc;
                   sqlint2       sqln;
                   sqlint2       sqld;
                   sqlint4       sqlloop,
                                 sqloffset;
                   sqlint2       sqlkano,
                                 sqlprno,
                                 sqlkamode,
                                 sqlfill;
                   struct SQLROW sqlrow;
                   struct sqlvar {
                                 sqlint2        sqltype;
                                 sqlint2        sqllen;
                                 unsigned char *sqldata;
                                 sqlint2         *sqlind;
                                 struct SQLNAME {
                                                sqlint2         length;
                                                unsigned char data [30];
                                                } sqlname;
                                 struct SQLCOL col;
                         } sqlvar [1];
                   };
/**/
#endif

#ifdef sql_oracle
struct SQLDA {
          unsigned char sqldaid [8];
          sqlint4 sqldabc;
          sqlint4 sqlfill;
          sqlint4 sqlloop;
          sqlint4 sqloffset;
          sqlint2 sqlkano,
                  sqlprno;
          sqlint2 sqlkamode,
                  sqlfill2;
          struct SQLROW sqlrow;
          struct SQLCOL sqlcol [sqlnmax];
          long    N;   /* Descriptor size in number of entries         */
          char  **V;   /* Ptr to Arr of addresses of main variables    */
          long   *L;   /* Ptr to Arr of lengths of buffers             */
          short  *T;   /* Ptr to Arr of types of buffers               */
          short **I;   /* Ptr to Arr of addresses of indicator vars    */
          long    F;   /* Number of variables found by DESCRIBE        */
          char  **S;   /* Ptr to Arr of variable name pointers         */
          short  *M;   /* Ptr to Arr of max lengths of var. names      */
          short  *C;   /* Ptr to Arr of current lengths of var. names  */
          char  **X;   /* Ptr to Arr of ind. var. name pointers        */
          short  *Y;   /* Ptr to Arr of max lengths of ind. var. names */
          short  *Z;   /* Ptr to Arr of cur lengths of ind. var. names */
          /* only cobol */
          char  **FM;  /* Ptr to Arr of hostvar decimal pointers       */
          short  *FML; /* Ptr to Arr of lengths of hostvar decimal     */
          short  *FCLP;   /* nil  */
          short  *FCRC;   /* nil  */
          };

#ifndef __cplusplus
typedef struct SQLDA SQLDA;
#endif
#endif

enum SQLVAINDI  {SQLVASC = 0, SQLVACM, SQLVAST, SQLVAPT};

typedef struct
{
	sqlint2 va1indi;
	sqlint2 va1indva2;
	sqlint2 va1indva3;
	sqlint2 va1fill;
#if defined(__cplusplus) || defined(__STDC__)
	void *va1addr;
        void *va1fill1;
#else
	char *va1addr;
        char *va1fill1;
#endif
}
sqlvascalar;

typedef struct
{
	sqlint2 va1indi;
	sqlint2 va1indva2;
	sqlint2 va1indva3;
	sqlint2 va1fill;
	sqlint4 va1offs;
	sqlint4 va1fill2;
}
sqlvacomponent;

typedef struct
{
	sqlint2 va1indi;
	sqlint2 va1cmpcnt;
	sqlint4 va1size;
#if defined(__cplusplus) || defined(__STDC__)
	void *va1addr;
#else
	char *va1addr;
#endif
	sqlint4 va1ix;
}
sqlvastruct;

typedef struct
{
	sqlint2 va1indi;
	sqlint2 va1fill3;
	sqlint4 va1ix;
#if defined(__cplusplus) || defined(__STDC__)
	void *va1addr;
        void *va1fill5;
#else
	char *va1addr;
        char *va1fill5;
#endif
}
sqlvaptr;

typedef union
{
	sqlvascalar sc;
	sqlvacomponent cm;
	sqlvastruct st;
	sqlvaptr pt;
}
sqlva1en;

typedef struct     {
                   sqlint2 va2typ;
                   sqlint2 va2digit;
                   sqlint4 va2size;
                   sqlint2 va2frac;
                   sqlint2 va2const;
                   }
                   sqlva2en;
typedef struct     {
                   sqllname va3name;
                   sqlint2 va3naml;
                   sqlint2 va3fill;
                   }
                   sqlva3en;
typedef sqlva1en *sqlv1pointer;
typedef sqlva2en *sqlv2pointer;
typedef sqlva3en *sqlv3pointer;

enum SQLPARINDI {SQLPARLO = 0, SQLPARST,
		SQLPAREL, SQLPARPL, SQLPARFU};

typedef struct     {
                      sqlint2       pakind;
                      sqlint2       pava1index;
		      sqlint4       paloopcnt;
		      sqlint4       paloopmin;
}
	sqlparlooptyp;
typedef struct     {
                      sqlint2       pakind;
                      sqlint2       pavarst;
                      sqlint2       paindst;
                      sqlint2       paelcnt;
                      sqlint4       pafill4;
}
	sqlparstructyp;
typedef struct     {
                      sqlint2       pakind;
                      sqlint2       pavarno;
                      sqlint2       paindno;
                      sqlint2       pafill2;
                      struct SQLCOL * pasqlcolp;
}
	sqlparelemtyp;
typedef struct     {
                      sqlint2       pakind;
                      sqlint2       paarea;
                      sqlint2       papreline;
                      sqlint2       paindex;
                      sqlint4       pafill5;
}
	sqlparsqlpltyp;
typedef struct     {
                      sqlint2       pakind;
                      sqlint2       pacolid;
                      sqlint2       pacoltyp;
                      sqlint2       pava1ind;
                      sqlint4       pafill6;
}
	sqlparfuncpltyp;
typedef union     {
	sqlparlooptyp    lo;
	sqlparstructyp   st;
	sqlparelemtyp    el;
	sqlparsqlpltyp   pl;
	sqlparfuncpltyp  fu;
}
sqlparentry;
typedef sqlparentry *sqlparpointer;


typedef struct     {
                      sqlint2       katyp,
                                    kastate,
                                    kapacount,
                                    kapaindex,
                                    kastcount,
                                    kastindex,
                                    kamacro,
                                    kaprindex,
                                    kadiindex,
                                    kaparserrc;
                      sqlint4       kaversion;
                      sqlparsid     kaparsid;
                      sqlint2       kaparamcnt,
                                    kalineno,
                                    kafaindex,
                                    kaatindex,
                                    kacuindex,
                                    kamode;
                      struct SQLROW kasqlrow;
                   }
                   sqlkaentry;
typedef sqlkaentry *sqlkapointer;
typedef struct     {
                      char*         orsfpointer;
                      sqlint2       oropennew,
                                    orkaopenind;
                   }
                   sqlorentry;
typedef sqlorentry *sqlorpointer;
typedef struct     {
                      sqlint2       prvaindex,
                                    prstate;
                      sqlnname      prname;
                      sqlint2       prkaindex,
                                    prarea,
                                    prdescribe;
                      sqlnname      prcuname;
                      sqlint2       prcuindex,
                                    prstcount,
                                    prstindex;
                   }
                   sqlprentry;
typedef sqlprentry *sqlprpointer;
typedef struct     {
                      sqlint4       stllen;
                      sqllinetype   stline;
                   }
                   sqlstentry;
typedef sqlstentry *sqlstpointer;
typedef struct
{
        sqlint2 faselectno;
        sqlint2 famfindex;
        sqlint4 fapos;
        sqlint2 faspec;
        sqlint2 fafetch;
        sqlint2 fareclen;
        sqlint2 fill;
} sqlfaentry;
typedef sqlfaentry *sqlfapointer;
typedef struct
{
        sqlint2 fnkanomin;
        sqlint2 fnkanomax;
        sqlnname fnfilen;
        sqlint2 fnincllno;
} sqlfnentry;
typedef sqlfnentry *sqlfnpointer;
typedef struct
{
	sqlint2 sd1count;
	sqlint2 sd1index;
	sqlint2 sdcntcols;
	sqlint2 sdfiller;
	char sdgetheader [36];
	sqlint2 sdgetfiller;
	sqlint2 sdgetkeycount;
	sqlint2 sdgetcntcols;
	sqlint2 sdgetcntconvertcols;
}
sqlsdentry;
typedef sqlsdentry *sqlsdpointer;
typedef char sqlsd1entry [4];
typedef sqlsd1entry *sqls1pointer;
typedef struct     {
                      sqluser       diuserid;
                      sqlint2       difiller;
                      sqlnname      diapplname;
                      sqlnname      dimodname;
                      sqlint2       dimacrono;
                      sqlint2       dinacount,
                                    dinaindex,
                                    diopindex;
                   }
                   sqldientry;
typedef sqldientry *sqldipointer;
typedef struct     {
                      sqlnname      naname;
                      sqlint2       nalnam,
                                    nalow,
                                    nahigh,
                                    namode,
                                    nakind,
                                    naattr,
                                    napaind;
                   }
                   sqlnaentry;
typedef sqlnaentry *sqlnapointer;
typedef struct
{
        sqlint2         printcase,
                        newpage,
                        cpage,
                        linespace,
                        firstfeed,
                        prnflag;
        sqlnname        prname;
        sqlint2         filler;
} sqlopprint;
typedef struct     {
                      sqlint2       opbackground,
                                    oprestore,
                                    opinit;
                      char          opprompt [2] ;
                      sqlint2       opclear,
                                    opwindowline,
                                    opwindowcol,
                                    opviewline,
                                    opviewcol,
                                    opviewlen,
                                    opviewwidth,
                                    opmark,
                                    opaction,
                                    opinp;
                      sqlopprint    opprint;
                      char          opframehead [80];
                      sqlint2       opframe,
                                    opaccept;
                      sqlformkeys   opacceptkey;
                      sqllabelkeys  opacceptlab;
                      sqlint2       opinput [20];
                   }
                   sqlopentry;
typedef sqlopentry *sqloppointer;
typedef struct     {
                      sqlnname      atdbname;
                      sqlint2       atdbindex;
                      sqldbnode     atdbstring;
                      sqlint2       atgaindex,
                                    atdbnamix;    /** at db **/
                   }
                   sqlatentry;
typedef sqlatentry *sqlatpointer;
typedef struct     {
                      sqlnname      cuname;
                      sqlint2       cuatindex;
                      sqlint4       cuerrd3;
                      sqlint2       custate,
                                    cukind;
                   }
                   sqlcuentry;
typedef sqlcuentry *sqlcupointer;
typedef struct     {
                      sqlint2       xainit,
                                    xacmdinit,
                                    xakano;
                      sqlnname      xaprogn;
                      sqlint2       xaprogc;
                      sqlnname      xamodn;
                      sqlint2       xamodc,
                                    xalang,
                                    xabs,
                                    xatrace,
                                    xaprno;
                      sqlsname      xalcheck;
                      sqlnname      xaprname;
                      sqlint2       xastopcnt,
                                    xallen;
                      sqllinetype   xaline;
                      sqlint4       xatime;
                      sqlint2       xadescpos,
                                    xadesclen;
                      sqlparpointer  sqlpap;
                      sqlint4       sapfiller;
                      sqlkapointer  sqlkap;
                      sqlprpointer  sqlprp;
                      sqlstpointer  sqlstp;
                      sqlfapointer  sqlfap;
                      sqldipointer  sqldip;
                      sqlnapointer  sqlnap;
                      sqloppointer  sqlopp;
                      sqlatpointer  sqlatp;
                      sqlv1pointer  sqlv1p;
                      sqlv2pointer  sqlv2p;
                      sqlv3pointer  sqlv3p;
                      sqlsdpointer  sqlsdp;
                      sqls1pointer  sqls1p;
                      sqlint2       xaloopinp,
                                    xaloopout;
                      sqlcupointer  sqlcup;
                      sqlorpointer  sqlorp;
                      sqlint2       xaatcount,
                                    xacucount;
                      sqlfnpointer  sqlfnp;
#if defined(__cplusplus) || defined(__STDC__)
		      void *sqlsnp;
#else
		      char *sqlsnp;
#endif
                      char          xafill [4];
                   }
                   sqlxatype;
#endif
/* end of first part of cpc.h */

/* begin of second part of cpc.h */
#ifndef _CPC_H_2
#define _CPC_H_2 0

 /* ----------------------------------------------- */
 /*       CPC run time functions references         */
 /* ----------------------------------------------- */

#ifdef __OS2__
#define _CPC_FMOD _System
#else
#define _CPC_FMOD
#endif

#if defined(__STDC__) || defined(__cplusplus)
#ifdef __cplusplus
extern "C" {
#endif
void
_CPC_FMOD sqccatb (sqlatentry [], long, long, long, char*, char*),
_CPC_FMOD sqccchk(sqlcatype*, sqlxatype*, sqlint2*, long),
_CPC_FMOD sqls (sqlva1en*, long, long, long, long, void*),
_CPC_FMOD sqlr (sqlva1en*, long, long, void*),
_CPC_FMOD sqccv2b(sqlva2en [], long, long, long, long, long,
	long),
_CPC_FMOD sqccv3b(sqlva3en [], long, long, char*),
_CPC_FMOD sqcccab(sqlcatype*, long, long),
_CPC_FMOD sqccpab(sqlparentry [], long, long, long, long, long),
_CPC_FMOD sqcckab(sqlorentry [], sqlkaentry [], long, long, long,
	long, long, long, long, long, long, long, long, long, long, long,
	long),
_CPC_FMOD sqccprb(sqlprentry [], long, long, long, long, long,
	long, long, long, long, long, char*, char*),
_CPC_FMOD sqccstb(sqlstentry [], long, long, char*),
_CPC_FMOD sqccxab(sqlxatype*, long, long, long, long, long, char*, char*),
_CPC_FMOD sqccsdb(sqlsdentry [], long, long, long),
_CPC_FMOD sqccfnb(sqlfnentry [], long, long, long, long, char*),
_CPC_FMOD sqccmab(sqlcatype*, sqlxatype*, long, long, char*),
_CPC_FMOD sqccgab(sqlcatype*, long, long, long, char*, char*),
_CPC_FMOD sqccdib(sqldientry [], long, long, long, long, long,
	char*, char*, char*),
_CPC_FMOD sqcclos(sqlcatype*, long),
_CPC_FMOD sqcctrs(sqlcatype*, long),
_CPC_FMOD sqccmts(sqlcatype*, long, char*),
_CPC_FMOD sqccexe(sqlcatype*, sqlxatype*, long, long),
_CPC_FMOD sqccpro(sqlcatype*, sqlxatype*, long, long),
_CPC_FMOD sqccdsc(sqlcatype*, sqlxatype*, long, long),
_CPC_FMOD sqcccmd(sqlcatype*, sqlxatype*, long),
_CPC_FMOD sqcctrl(sqlcatype*, sqlxatype*, long),
_CPC_FMOD sqccbeg(sqlcatype*, sqlxatype*, long, long),
#if defined(sql_sqldb) || defined (sql_default) || defined (sql_ansi)
_CPC_FMOD sqccdam(sqlcatype*, sqldatype*),
_CPC_FMOD sqccdaa(sqlcatype*, sqldatype*),
#else
#ifdef sql_oracle
_CPC_FMOD sqccdaa(sqlcatype*, struct SQLDA*),
#endif
#endif
#if defined(sql_db2)
_CPC_FMOD sqccdaa(sqlcatype*, struct sqlda*),
#endif
_CPC_FMOD sqccend(sqlcatype*, sqlxatype*),
_CPC_FMOD sqccrcn(sqlcatype*),
_CPC_FMOD sqccstp(sqlcatype*, sqlxatype*),
_CPC_FMOD sqccexi(sqlcatype*, sqlxatype*),
_CPC_FMOD sqccprs(sqlcatype*, sqlxatype*, long, long),
_CPC_FMOD sqccos(long*),
_CPC_FMOD sqccrep(sqlcatype*, sqlxatype*, long, long),
_CPC_FMOD sqccque(sqlcatype*, sqlxatype*, long, long),
_CPC_FMOD sqccdex(sqlcatype*, sqlxatype*),
_CPC_FMOD sqccnab(sqlnaentry [], long, long, long, long, long,
	long, long, long, char*),
_CPC_FMOD sqcco1b(sqlopentry [], long, long, long, long, long,
	long, long, long, long, long, long, long, char*),
_CPC_FMOD sqcco2b(sqlopentry [], long, long, long, long, long,
	long, long, long, long, char*),
_CPC_FMOD sqcco3b(sqlopentry [], long, long, char*, char*),
_CPC_FMOD sqcco4b(sqlopentry [], long, long, char*),
_CPC_FMOD sqcco5b (sqlopentry [], long,
	long, long, long, long, long, long, long, long, long, long,
	long, long, long, long, long, long, long, long, long, long),
_CPC_FMOD sqccdlb(sqlcatype *sqlca, sqldiaenv*, sqlcodecache, sqlvaluecache,
	sqldialogglobal, sqliglobal, sqldiaprevars, sqlprecompdca,
	long, long, long, long, long, long),
_CPC_FMOD sqccdlg(sqlcatype*, sqlxatype*, long, long),
_CPC_FMOD sqccfrm(sqlcatype*, sqlxatype*, long, long);
#ifdef __cplusplus
}
#endif
static void
#ifdef sqlextern
sqlstart (sqlcatype*);
#else
sqlstart (void);
#endif
#else                 /* non ANSI C compilers */
extern void
_CPC_FMOD sqccchk(), _CPC_FMOD sqccdib(), _CPC_FMOD sqccdlg(),
_CPC_FMOD sqccdsc(), _CPC_FMOD sqccexe(), _CPC_FMOD sqccexi(),
_CPC_FMOD sqccgab(), _CPC_FMOD sqcckab(),
_CPC_FMOD sqccmab(), _CPC_FMOD sqccnab(),
_CPC_FMOD sqcco1b(), _CPC_FMOD sqcco2b(), _CPC_FMOD sqcco3b(),
_CPC_FMOD sqccpab(),
_CPC_FMOD sqccprb(), _CPC_FMOD sqccprs(),
_CPC_FMOD sqls   (), _CPC_FMOD sqlr   (),
_CPC_FMOD sqccstb(), _CPC_FMOD sqccbeg(), _CPC_FMOD sqcctrl(),
_CPC_FMOD sqcctrs(), _CPC_FMOD sqccv2b(), _CPC_FMOD sqccv3b(),
_CPC_FMOD sqccxab(), _CPC_FMOD sqccdex(), _CPC_FMOD sqcco4b(),
_CPC_FMOD sqccfrm(), _CPC_FMOD sqccend(), _CPC_FMOD sqccrcn(),
_CPC_FMOD sqccsdb(),
_CPC_FMOD sqccrep(), _CPC_FMOD sqccque(), _CPC_FMOD sqccpro(),
_CPC_FMOD sqcclos(), _CPC_FMOD sqccmts(),
_CPC_FMOD sqccos (), _CPC_FMOD sqccfna(),
_CPC_FMOD sqccfnb(), _CPC_FMOD sqcco5b(),
#ifdef sql_oracle
_CPC_FMOD sqccdaa(),
#else
_CPC_FMOD sqccdaa(), _CPC_FMOD sqccdam(),
#endif
_CPC_FMOD sqccatb(),
_CPC_FMOD sqccstp(), _CPC_FMOD sqcccab(),
_CPC_FMOD sqccdlb(), _CPC_FMOD sqcccmd();

static void

sqlstart();
#endif

#endif
/* end of second part of cpc.h */
/* begin of third part of cpc.h */
#ifndef _CPC_H_3
#define _CPC_H_3 0

 /* ----------------------------------------------- */
 /*     SQLCA                                       */
 /* ----------------------------------------------- */

#ifndef sqlextern
extern sqlcatype sqlca;

#if defined(sql_db2)
/* SAA sqlda description */
extern short sqlsize; /* number of sqlvars (sqln) */
extern struct SQLDA *sqlptr;
#endif

#endif

#ifndef DLL
#define sqlxap (&sqlxa)
#define sqlstcip (&sqlstci)
static sqlxatype sqlxa;
#if sqlva1mx > 0
static sqlva1en sqlva1 [sqlva1mx];
#endif
#if sqlva2mx > 0
static sqlva2en sqlva2 [sqlva2mx];
#endif
#if sqlva3mx > 0
static sqlva3en sqlva3 [sqlva3mx];
#endif
#if sqlpamax > 0
static sqlparentry sqlpa [sqlpamax];
#endif
#if sqlkamax > 0
static sqlkaentry sqlka [sqlkamax];
static sqlorentry sqlor [sqlkamax];
#endif
#if sqlprmax > 0
static sqlprentry sqlpr [sqlprmax];
#endif
#if sqlstmax > 0
static sqlstentry sqlst [sqlstmax];
#endif
#if sqlsdmx > 0
static sqlsdentry sqlsd [sqlsdmx];
#endif
#if sqlsd1mx > 0
static sqlsd1entry sqlsd1 [sqlsd1mx];
#endif
#if sqlfamx > 0
static sqlfaentry sqlfa [sqlfamx];
#endif
#if sqlfnmx > 0
static sqlfnentry sqlfn [sqlfnmx];
#endif
#if sqldimax > 0
static sqldientry sqldi [sqldimax];
#endif
#if sqlnamax > 0
static sqlnaentry sqlna [sqlnamax];
#endif
#if sqlopmax > 0
static sqlopentry sqlop [sqlopmax];
#endif
#if (sqldamax > 0) && (defined(sql_sqldb) || defined(sql_default)) || defined(sql_ansi)
static sqldatype sqlda;
#endif
#if sqlatmx > 0
static sqlatentry sqlat [sqlatmx];
#endif
#if sqlcumx > 0
static sqlcuentry sqlcu [sqlcumx];
#endif
static sqlint2 sqlstci = 0;

#else /* DLL */

enum SQLAREAS {SQLCAA = 0, SQLXAA, SQLVA1A, SQLVA2A, SQLVA3A, SQLPAA, SQLKAA,
SQLORA, SQLPRA, SQLSTA, SQLSDA, SQLSD1A, SQLFAA, SQLFNA, SQLDIA, SQLNAA,
SQLOPA, SQLDAA, SQLATA, SQLCUA, SQLSTCIA, SQLAREAMAX};

void *sqcaddr(int n, int numelem);

#ifdef sqlcap
#undef sqlcap
#endif
#define sqlcap ((sqlcatype*) sqcaddr(SQLCAA, sizeof(sqlcatype)))
#define sqlxap ((sqlxatype*) sqcaddr(SQLXAA, sizeof(sqlxatype)))
#define sqlstcip ((sqlint2 *) sqcaddr(SQLSTCIA, sizeof(sqlint2)))

#if sqlva1mx > 0
#define sqlva1 ((sqlva1en *) sqcaddr(SQLVA1A, sizeof(sqlva1en)*sqlva1mx))
#endif
#if sqlva2mx > 0
#define sqlva2 ((sqlva2en *) sqcaddr(SQLVA2A, sizeof(sqlva2en)*sqlva2mx))
#endif
#if sqlva3mx > 0
#define sqlva3 ((sqlva3en *) sqcaddr(SQLVA3A, sizeof(sqlva3en)*sqlva3mx))
#endif
#if sqlpamax > 0
#define sqlpa ((sqlparentry *) sqcaddr(SQLPAA, sizeof(sqlparentry)*sqlpamax))
#endif
#if sqlkamax > 0
#define sqlka ((sqlkaentry *) sqcaddr(SQLKAA, sizeof(sqlkaentry)*sqlkamax))
#define sqlor ((sqlorentry *) sqcaddr(SQLORA, sizeof(sqlorentry)*sqlkamax))
#endif
#if sqlprmax > 0
#define sqlpr ((sqlprentry *) sqcaddr(SQLPRA, sizeof(sqlprentry)*sqlprmax))
#endif
#if sqlstmax > 0
#define sqlst ((sqlstentry *) sqcaddr(SQLSDA, sizeof(sqlstentry)*sqlstmax))
#endif
#if sqlsdmx > 0
#define sqlsd ((sqlsdentry *) sqcaddr(SQLSDA, sizeof(sqlsdentry)*sqlsdmx))
#endif
#if sqlsd1mx > 0
#define sqlsd1 ((sqlsd1entry *) sqcaddr(SQLSD1A, sizeof(sqlsd1entry)*sqlsd1mx))
#endif
#if sqlfamx > 0
#define sqlfa ((sqlfaentry *) sqcaddr(SQLFAA, sizeof(sqlfaentry)*sqlfamx))
#endif
#if sqlfnmx > 0
#define sqlfn ((sqlfnentry *) sqcaddr(SQLFNA, sizeof(sqlfnentry)*sqlfnmx))
#endif
#if sqldimax > 0
#define sqldi ((sqldientry *) sqcaddr(SQLDIA, sizeof(sqldientry)*sqldimax))
#endif
#if sqlnamax > 0
#define sqlna ((sqlnaentry *) sqcaddr(SQLNAA, sizeof(sqlnaentry)*sqlnamax))
#endif
#if sqlopmax > 0
#define sqlop ((sqlopentry *) sqcaddr(SQLOPA, sizeof(sqlopentry)*sqlopmax))
#endif
#if (sqldamax > 0) && (defined(sql_sqldb) || defined(sql_default)) || defined(sql_ansi)
#define sqlda ((sqldatype *) sqcaddr(SQLDAA, sizeof(sqldatype)*sqldamax))
#endif
#if sqlatmx > 0
#define sqlat ((sqlatentry *) sqcaddr(SQLATA, sizeof(sqlatentry)*sqlatmx))
#endif
#if sqlcumx > 0
#define sqlcu ((sqlcuentry *) sqcaddr(SQLCUA, sizeof(sqlcuentry)*sqlcumx))
#endif
#endif /* DLL */
#endif
/* end of cpc.h */
