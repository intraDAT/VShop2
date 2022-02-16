GCC=gcc
GPP=g++

#       -g                      \
#       -DDEBUG                 \

INCLUDEPATH=-I/usr/lib/adabas/incl

CFLAGS=-O3 -ansi -Wall -D_GNU_SOURCE -DVSHOP_PROVIDER -D__USE_XOPEN_EXTENDED -D_XOPEN_SOURCE $(INCLUDEPATH)

CFLAGS=-O3                     \
       -ansi                   \
       -Wall                   \
       -DVSHOP_PROVIDER        \
       -DGERMAN                \
       -D_GNU_SOURCE           \
       -D__USE_BSD             \
       -D__USE_XOPEN_EXTENDED  \
       -D_XOPEN_SOURCE         \
       -DSUSE_7                \
       -I/usr/include          \
       -I/usr/lib/adabas/incl


ADABASLIBPATH=/usr/lib/adabas/lib
ADABASLIBS=${ADABASLIBPATH}/pcrlib.a     \
           ${ADABASLIBPATH}/pcd1lib.a    \
           ${ADABASLIBPATH}/pcd2lib.a    \
           ${ADABASLIBPATH}/pcd3lib.a    \
           ${ADABASLIBPATH}/libsqlrte.a  \
           ${ADABASLIBPATH}/libsqlptc.a  \
           ${ADABASLIBPATH}/sqlca.a      \
           ${ADABASLIBPATH}/libncurses.a

OBJECTS=vshop.o          \
        julian.o         \
        net-func.o       \
        shop-func.o      \
        skey.o           \
        smtp_vshop.o     \
        smtp_tcp_vshop.o \
        parse.o          \
        inifile.o        \
        error.o          \
        hfct.o           \
        cgic.o           \
        adabas.o         \
        vsl-exec.o       \
        base-func.o      \
        table-func.o     \
        db.o             \
        priceutil.o

# all #########################################################################

all: vsl-compile mtl-compile vshop install getkey vshoprpminstall

# binaries ####################################################################

getkey: getkey.c cgic.o skey.o 
	${GPP} ${CFLAGS} -static -o getkey getkey.c skey.o -lcrypt
	strip -s getkey

vshoprpminstall: vshoprpminstall.cpp vshoprpminstall.o
	${GPP} ${CFLAGS}  -static -o vshoprpminstall  vshoprpminstall.o 
	strip vshoprpminstall

install: install.c inifile.o inifile.h language.h 
	${GPP} ${CFLAGS} -DINSTALL -static -o install install.c inifile.o \
    -lmenu  -lform -lncurses
	strip install

vsl-compile: y.tab.o compiler.o scanner.o 
	${GCC} ${CFLAGS} -static -o vsl-compile scanner.c y.tab.o compiler.o \
    -rdynamic -lfl
	strip -s vsl-compile

mtl-compile: y.tab.o compiler.o scanner.o 
	${GCC} ${CFLAGS} -static -DMTLCOMPILE -o mtl-compile scanner.c \
    y.tab.o compiler.o -rdynamic -lfl
	strip -s mtl-compile

vshop: vslanguage.h ${OBJECTS}
	${GPP} ${CFLAGS} -o vshop -ldl -lm -lcrypt -rdynamic ${OBJECTS} ${ADABASLIBS}
	strip -s vshop

# objects ##################################################################### 

vshoprpminstall.o: vshoprpminstall.cpp
	${GPP}  -O3 -D__USE_BSD -D__USE_XOPEN -D__USE_XOPEN_EXTENDED  -c vshoprpminstall.cpp

skey.o: skey.c skey.h skey_proto.h skey_definitions.h
	${GCC} ${CFLAGS} -c skey.c

smtp_vshop.o: smtp_vshop.c smtp.h smtp_network.h smtp_network_proto.h \
              smtp_proto.h
	${GCC} ${CFLAGS} -c smtp_vshop.c

smtp_tcp_vshop.o: smtp_tcp_vshop.c smtp.h smtp_network.h smtp_network_proto.h \
                  smtp_proto.h
	${GCC} ${CFLAGS} -c smtp_tcp_vshop.c

parse.o: parse.h parse.c
	${GPP} ${CFLAGS} -c parse.c

julian.o: julian.h julian.c
	${GPP} ${CFLAGS} -c julian.c
	
inifile.o: inifile.h inifile.cpp
	${GPP} ${CFLAGS} -c inifile.cpp

shop-func.o: shop-func.cpp shop-func.h
	${GPP} ${CFLAGS} -c shop-func.cpp 
	
net-func.o: net-func.cpp net-func.h
	${GPP} ${CFLAGS} -c net-func.cpp 
	
vshop.o: vshop.cpp vshop.h
	${GPP} ${CFLAGS} -c vshop.cpp 

cgic.o: cgic.cpp cgic.h
	${GCC} ${CFLAGS} -c cgic.cpp

error.o: error.c error.h
	${GPP} ${CFLAGS} -c error.c

hfct.o: hfct.c hfct.h
	${GCC} ${CFLAGS} -c hfct.c

vsl-exec.o: vsl-exec.cpp vsl-exec.h m_code.h
	${GPP} ${CFLAGS} -c vsl-exec.cpp

base-func.o: base-func.cpp vsl.h
	${GPP} ${CFLAGS} -c base-func.cpp

table-func.o: table-func.cpp vsl.h db.h
	${GPP} ${CFLAGS} -c table-func.cpp

db.o: db.cpp vsl.h db.h
	${GPP} ${CFLAGS} -c db.cpp

scanner.o:scanner.c y.tab.h
	${GCC} ${CFLAGS} -c scanner.c

y.tab.o:y.tab.c y.tab.h
	${GCC} ${CFLAGS} -c y.tab.c

compiler.o:compiler.c compiler.h m_code.h
	${GCC} ${CFLAGS} -c compiler.c

y.tab.c y.tab.h:vsl.y m_code.h
	yacc -d vsl.y

adabas.o: adabas.C cgic.o
	${GPP} ${CFLAGS} -c adabas.C

priceutil.o: priceutil.h priceutil.cc
	${GPP} ${CFLAGS} -c priceutil.cc

adabas.C: adabas.cpc
	cpc -d vshop -H nocheck -E cplus -c -w -u domain,domain adabas

clean:
	rm -f *.o                                                  \
        y.tab.h y.tab.c y.output                             \
        adabas.pcl adabas.C                                  \
        vshop install getkey vsl-compile mtl-compile vsl.out vshoprpminstall \
        *~                                                   \
        *.vsl.p                                              \
        gmon.out

