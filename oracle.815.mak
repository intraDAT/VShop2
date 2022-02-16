CXXFLAGS=-O3
INCLUDEPATH=-I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/network/public -I$(ORACLE_HOME)/plsql/public
LIBPATH=-L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/network/lib
LIBS=-lclient8 -lclntsh -lcommon8 -lcore8 -lnls8

CFLAGS=-O3 -ansi -Wall -D_GNU_SOURCE -DDB_ORACLE -DVSHOP_PROVIDER -D__USE_XOPEN_EXTENDED -D_XOPEN_SOURCE $(INCLUDEPATH)

GCC=gcc
GPP=g++

all: vsl-compile mtl-compile vshop 

vsl-compile: y.tab.o compiler.o oracle.815.mak
	${GCC} ${CFLAGS} -static -o bin_815/vsl-compile scanner.c y.tab.o compiler.o -rdynamic -lfl
	strip -s bin_815/vsl-compile
#	mv vsl-compile bin/oracle

mtl-compile: y.tab.o compiler.o oracle.815.mak
	${GCC} ${CFLAGS} -static -DMTLCOMPILE -o bin_815/mtl-compile scanner.c y.tab.o compiler.o -rdynamic -lfl
	strip -s bin_815/mtl-compile
#	mv mtl-compile bin/oracle

vshop: vshop.o julian.o net-func.o shop-func.o smtp_vshop.o vslanguage.h smtp_tcp_vshop.o parse.o inifile.o error.o hfct.o cgic.o oracle.o vsl-exec.o base-func.o table-func.o db.o skey.o
	${GPP} ${CFLAGS} -rdynamic -o bin_815/vshop -lcrypt -ldl -lm julian.o net-func.o shop-func.o vshop.o smtp_vshop.o smtp_tcp_vshop.o parse.o inifile.o error.o hfct.o cgic.o vsl-exec.o base-func.o table-func.o db.o skey.o oracle.o $(LIBPATH) $(LIBS) 
	strip -s bin_815/vshop
	cp bin_815/vshop /usr/local/httpd/webspace/www.de.vshop.org/cgi-bin/vsorg
	cp bin_815/vshop /usr/local/httpd/webspace/intranet.intradat.com/cgi-bin/intra
	cp bin_815/vshop /usr/local/httpd/webspace/support.de.intradat.com/cgi-bin/supp
	cp bin_815/vshop /usr/local/httpd/webspace/partner.de.intradat.com/cgi-bin/part
	cp bin_815/vshop /usr/local/httpd/webspace/www.de.intradat.com/cgi-bin/www

skey.o: skey.c skey.h skey_proto.h skey_definitions.h oracle.815.mak
	${GCC} ${CFLAGS} -c skey.c

smtp_vshop.o: smtp_vshop.c smtp.h smtp_network.h smtp_network_proto.h smtp_proto.h oracle.815.mak
	${GCC} ${CFLAGS} -c smtp_vshop.c

smtp_tcp_vshop.o: smtp_tcp_vshop.c smtp.h smtp_network.h smtp_network_proto.h smtp_proto.h oracle.815.mak
	${GCC} ${CFLAGS} -c smtp_tcp_vshop.c

parse.o: parse.h parse.c oracle.815.mak
	${GPP} ${CFLAGS} -c parse.c

julian.o: julian.h julian.c oracle.815.mak
	${GPP} ${CFLAGS} -c julian.c
	
inifile.o: inifile.h inifile.cpp oracle.815.mak
	${GPP} ${CFLAGS} -c inifile.cpp

shop-func.o: shop-func.cpp shop-func.h oracle.815.mak
	${GPP} ${CFLAGS} -c shop-func.cpp 
	
net-func.o: net-func.cpp net-func.h oracle.815.mak
	${GPP} ${CFLAGS} -c net-func.cpp 
	
vshop.o: vshop.cpp vshop.h oracle.815.mak
	${GPP} ${CFLAGS} -c vshop.cpp 

cgic.o: cgic.cpp cgic.h oracle.815.mak
	${GCC} ${CFLAGS} -c cgic.cpp

error.o: error.c error.h oracle.815.mak
	${GPP} ${CFLAGS} -c error.c

hfct.o: hfct.c hfct.h oracle.815.mak
	${GCC} ${CFLAGS} -c hfct.c

vsl-exec.o: vsl-exec.cpp vsl-exec.h m_code.h oracle.815.mak
	${GPP} ${CFLAGS} -c vsl-exec.cpp

base-func.o: base-func.cpp vsl.h oracle.815.mak
	${GPP} ${CFLAGS} -c base-func.cpp

table-func.o: table-func.cpp vsl.h db.h oracle.815.mak
	${GPP} ${CFLAGS} -c table-func.cpp

db.o: db.cpp vsl.h db.h oracle.815.mak
	${GPP} ${CFLAGS} -c db.cpp

scanner.o:scanner.c y.tab.h oracle.815.mak
	${GCC} ${CFLAGS} -c scanner.c

y.tab.o:y.tab.c y.tab.h oracle.815.mak
	${GCC} ${CFLAGS} -c y.tab.c

compiler.o:compiler.c compiler.h m_code.h oracle.815.mak
	${GCC} ${CFLAGS} -c compiler.c

y.tab.c y.tab.h:vsl.y m_code.h oracle.815.mak
	yacc -d vsl.y

oracle.o: oracle.cc oracle.815.mak
	${GPP} $(CXXFLAGS) $(INCLUDEPATH) -c oracle.cc

oracle.c: oracle.pc
	proc code=cpp sys_include=(/usr/include,/usr/include/g++) include=(/usr/include,/usr/include/linux,/usr/include/g++,/usr/include/g++/stl,/usr/include/g++/gen,/usr/local/lib.gnu/gcc-lib/i586-pc-linux-gnu/egcs-2.91.66/include) oracle.pc
	
clean:
	rm -f *.o y.tab.h y.tab.c y.output oracle.c bin_815/vshop bin_815/install bin_815/getkey bin_815/vsl-compile bin_815/mtl-compile vsl.out *~ *.vsl.p gmon.out

