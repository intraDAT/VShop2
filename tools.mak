CFLAGS=-O3 -ansi -Wall -DVSADMIN -DVSHOP_PROVIDER -D__USE_XOPEN_EXTENDED -D_XOPEN_SOURCE -I /usr/lib/adabas/incl

GCC=gcc
GPP=g++

all: vsadmin vspasswd pips

pips: pips.cpp skey.o cgic.o adabas.o inifile.o hfct.o error.o 
	${GPP} ${CFLAGS} -o pips pips.cpp skey.o cgic.o adabas.o inifile.o hfct.o error.o /usr/lib/adabas/lib/pcrlib.a /usr/lib/adabas/lib/pcd1lib.a /usr/lib/adabas/lib/pcd2lib.a /usr/lib/adabas/lib/pcd3lib.a /usr/lib/adabas/lib/libsqlrte.a /usr/lib/adabas/lib/libsqlptc.a /usr/lib/adabas/lib/libncurses.a /usr/lib/adabas/lib/sqlca.a
	strip -s pips
	cp pips ../bin/pips

vsadmin: vsadmin.cpp skey.o adabas.o inifile.o inifile.h language.h 
	${GPP} ${CFLAGS} -o vsadmin skey.o vsadmin.cpp adabas.o inifile.o /usr/lib/adabas/lib/pcrlib.a /usr/lib/adabas/lib/pcd1lib.a /usr/lib/adabas/lib/pcd2lib.a /usr/lib/adabas/lib/pcd3lib.a /usr/lib/adabas/lib/libsqlrte.a /usr/lib/adabas/lib/libsqlptc.a /usr/lib/adabas/lib/sqlca.a -lmenu -lform -lncurses
	strip vsadmin
	cp vsadmin ../bin/provider

vspasswd: vspasswd.cpp skey.o adabas.o inifile.o inifile.h language.h 
	${GPP} ${CFLAGS} -o vspasswd vspasswd.cpp skey.o adabas.o inifile.o /usr/lib/adabas/lib/pcrlib.a /usr/lib/adabas/lib/pcd1lib.a /usr/lib/adabas/lib/pcd2lib.a /usr/lib/adabas/lib/pcd3lib.a /usr/lib/adabas/lib/libsqlrte.a /usr/lib/adabas/lib/libsqlptc.a /usr/lib/adabas/lib/sqlca.a -lmenu -lform -lncurses
	strip vspasswd
	cp vspasswd ../bin/demo
	cp vspasswd ../bin/free 
	cp vspasswd ../bin/basic 
	cp vspasswd ../bin/professional 
	cp vspasswd ../bin/provider 

skey.o: skey.c skey.h skey_proto.h skey_definitions.h 
	${GCC} ${CFLAGS} -c skey.c

inifile.o: inifile.h inifile.cpp 
	${GPP} ${CFLAGS} -c inifile.cpp

cgic.o: cgic.cpp cgic.h 
	${GCC} ${CFLAGS} -c cgic.cpp

error.o: error.c error.h 
	${GPP} ${CFLAGS} -c error.c

hfct.o: hfct.c hfct.h 
	${GCC} ${CFLAGS} -c hfct.c

db.o: db.cpp vsl.h db.h 
	${GPP} ${CFLAGS} -c db.cpp

adabas.o: adabas.C cgic.o 
	${GPP} ${CFLAGS} -c adabas.C

adabas.C: adabas.cpc
	cpc -d vshop -H nocheck -E cplus -c -w -u domain,domain adabas

clean:
	rm -f *.o adabas.pcl adabas.C vsadmin pips vspasswd *~ *.vsl.p gmon.out

