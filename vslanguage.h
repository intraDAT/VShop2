#ifdef ENGLISH

  #define TEXT_LIBC5DIALOG1 " There was not installed libc6 on your system"
  #define TEXT_LIBC5DIALOG2 " found. This could cause problems on execution."
  #define TEXT_LIBC5DIALOG3 " Please install libc6."
  #define TEXT_LIBC5DIALOG4 "                                                      "
  #define TEXT_LIBC5DIALOG5 "                        "
  #define TEXT_LIBC5DIALOG6 "      We recommend to abort installation now."

#ifdef VSHOP_DEMO
  #ifndef FRENCH
    #define TEXT_INSTALLDIALOG1 " This program installs VShop 2.1.1"
    #define TEXT_INSTALLDIALOG2 " on your system. Make sure a webserver"
    #define TEXT_INSTALLDIALOG3 " is installed and a user for Adabas exists. "
    #define TEXT_INSTALLDIALOG4 " This demo expires on January 31, 2001. "
    #define TEXT_INSTALLDIALOG5a " "
    #define TEXT_INSTALLDIALOG5b " More information is available at: "
    #define TEXT_INSTALLDIALOG6 " http://www.vshop.de or support@intradat.com"
  #else
    #define TEXT_INSTALLDIALOG1 " This program installs VShop 2.1.1 french "
    #define TEXT_INSTALLDIALOG2 " prerelease on your system. Make sure a webserver"
    #define TEXT_INSTALLDIALOG3 " is installed and a user for Adabas exists. "
    #define TEXT_INSTALLDIALOG4 " This demo expires on January 31, 2001. "
    #define TEXT_INSTALLDIALOG5a " "
    #define TEXT_INSTALLDIALOG5b " More information is available at: "
    #define TEXT_INSTALLDIALOG6 " http://www.vshop.de or support@intradat.com"
  #endif
#else
  #define TEXT_INSTALLDIALOG1 " Dieses Programm installiert VShop II auf Ihrem System. "
  #define TEXT_INSTALLDIALOG2 " Vor der Installation mu† ein WWW-Server installiert"
  #define TEXT_INSTALLDIALOG3 " und ein Benutzer fÅr Adabas D angelegt worden sein."
  #define TEXT_INSTALLDIALOG4 "  "
  #define TEXT_INSTALLDIALOG5a " "
  #define TEXT_INSTALLDIALOG5b " Weitere Informationen erhalten Sie unter "
  #define TEXT_INSTALLDIALOG6 " http://www.vshop.de oder http://support.intradat.com"
#endif

  #define TEXT_UEBERSCHREIBENDIALOG1 " VShop is already installed."
  #define TEXT_UEBERSCHREIBENDIALOG2 " To update this version abort now and"
  #define TEXT_UEBERSCHREIBENDIALOG3 " choose '/update' on the disc"

  #define TEXT_DIALOGBOTTOM1 "[F10=OK/ESC=Cancel]"
  #define TEXT_DIALOGTOP1 " VShop Installation "
  #define TEXT_DIALOGBOTTOM2 "[RETURN/F10=OK][ESC=Cancel]"
  #define TEXT_KEYDIALOG1 " VShop - enter key"
  #define TEXT_KEYDIALOG2 " Key : "
  #define TEXT_ADABASMENU1 "Adabas D is currently not installed"
  #define TEXT_ADABASMENU2 "Adabas D is already installed..."
  #define TEXT_ADABASMENU3 "Cancel installation   "
  #define TEXT_ADABASDIALOG1 " Adabas Installation"
  #define TEXT_ERRORINSTALL " Error "
  #define TEXT_ADABASUSERERROR1 " User     '"
  #define TEXT_ADABASUSERERROR2 "' does not exist."
  #define TEXT_ADABASDIRERROR1 "Directory  "
  #define TEXT_ADABASDIRERROR2 "does not exist.  "
  #define TEXT_ADABASPARAMDIALOG " Adabas Installation-parameter "
  #define TEXT_ADABASPARAMDIALOG1 " Adabas D directory   : "
  #define TEXT_ADABASPARAMDIALOG2 " Adabas D username    : "
  #define TEXT_HTTPDCONFERROR1 "The file  "
  #define TEXT_HTTPDCONFERROR2 "does not exist." 
  #define TEXT_WWWSERVERDIALOG  " Apache-server configuration"
  #define TEXT_WWWSERVERDIALOG1 "the server is not an Apache server"
  #define TEXT_WWWSERVERDIALOG2 "configuration is "
  #define TEXT_WWWSERVERDIALOG3 "configuration is /etc/httpsd/httpd.conf"
  #define TEXT_WWWSERVERDIALOG4 "configuration is /etc/httpd/conf/httpd.conf"
  #define TEXT_WWWSERVERDIALOG5 "configuration is /etc/httpsd/conf/httpd.conf"
  #define TEXT_WWWSERVERDIALOG6 "enter config-file manually :"
  #define TEXT_HTDOCSERROR1 "The directory   "
  #define TEXT_HTDOCSERROR2 "does not exist.  "
  #define TEXT_CGIBINERROR1 "The directory   "
  #define TEXT_CGIBINERROR2 "does not exist.  "
  #define TEXT_PARAMDIALOG " Enter parameters  "
  #define TEXT_PARAMDIALOG1 " servername           : "
  #define TEXT_PARAMDIALOG2 " CGI URL        (http): "
  #define TEXT_PARAMDIALOG3 " HTML directory (sys) : "
  #define TEXT_PARAMDIALOG4 " CGI directory  (sys) : "
  #define TEXT_MAINMENU1 "Key register"
  #define TEXT_MAINMENU2 "Change Adabas D user     " 
  #define TEXT_MAINMENU3 "Create a new VShop  "
  #define TEXT_MAINMENU4 "END!"

  #define TEXT_VERSIONDOWNLOAD " VShop II download english "
  #ifdef VSHOP_DEMO
    #ifdef FRENCH
      #define TEXT_VERSION " VShop II Demo french "
    #else
      #define TEXT_VERSION " VShop II Demo english "
    #endif 

  #elif VSHOP_FREE
    #define TEXT_VERSION " VShop II Light english "
  #elif VSHOP_BASIC
    #define WRONGGROUP "Wrong Usergroup"
    #define TEXT_VERSION " VShop II Basic english "
  #elif VSHOP_PROVIDER
    #ifdef WITHOUT_ADABAS
      #define TEXT_VERSION " VShop II Provider english (Without Adabas) "
    #else
      #define TEXT_VERSION " VShop II Provider english "
    #endif
  #elif VSHOP_PARTNER
    #define TEXT_VERSION " VShop II Partner english "

  #elif VSHOP_PROFESSIONAL
    #ifdef WITHOUT_ADABAS
      #define TEXT_VERSION " VShop II Professional english (Without Adabas) "
    #else
      #define TEXT_VERSION " VShop II Professional english "
    #endif
  #endif


  #define TEXT_NEWSHOPDIALOG " New Shop   "
  #define TEXT_NEWSHOPDIALOG1 " Only available in ProviderVersion!  "
  #define TEXT_ERRORINSTALL1 "Error while inserting new key         \n"
  #define TEXT_ERRORINSTALL2 "For installation of the download-version, Adabas D must be installed!    \n"
  #define TEXT_ERRORINSTALL3 "Installation finished\n"
  #define TEXT_ERRORINSTALL4 "Installation canceled   \n"
  #define TEXT_ERRORINSTALL5 "Configuration canceled   \n"
  #define TEXT_ERRORINSTALL6 "To run this installation you have to be superuser!                        \n"
  #define TEXT_ERRORINSTALL7 "Error while inserting the keys.."
  #define TEXT_INSTALL1 " Adabas Installation "
  #define TEXT_INSTALL2 " copying database ..."
  #define TEXT_INSTALL3 " decrunching database ..."
  #define TEXT_INSTALL4 "starting Adabas D installation..\n"
  #define TEXT_INSTALL5 "copying VShop..."
  #define TEXT_INSTALL6 "copying VShop-database ..."
  #define TEXT_INSTALL7 "converting  VShop-database ..."
  #define TEXT_INSTALL8 "copying VShop-Administration! "

  #define STEXT_YES1 "y"
  #define STEXT_YES2 "Y"
  #define STEXT_INSTALLADABAS1 "Is Adabas D already installed? (y/n): "
  #define STEXT_INSTALLADABAS2 "Is there a system user for Adabas D ? (y/n): "
  #define STEXT_IS "Is '"
  #define STEXT_INSTALLADABAS3 "' the user for Adabas D ? (y/n): "
  #define STEXT_INSTALLADABAS4 "Please enter the username for Adabas D :"
  #define STEXT_INSTALLADABAS4B "Please enter the username for Adabas D:"
  #define STEXT_INSTALLADABAS5 "' the directory, \nwhere Adabas D should be installed ? (y/n): "
  #define STEXT_INSTALLADABAS6 "Please enter the directory, \nwhere to install Adabas D :"
  #define STEXT_INSTALLADABAS7 "All Adabas D files must belong to a dedicated user (e.g. adabas) !\n"
  #define STEXT_INSTALLADABAS8 "This user must be created before starting the installation process\n"
  #define STEXT_INSTALLADABAS9 "or do it now in an extra session...\n\n"
  #define STEXT_INSTALLADABAS10 "Please create the user for Adabas D first\n\n"
  #define STEXT_INSTALLADABAS11 "The user '%s' does not exist on this system!\n\n"
  #define STEXT_INSTALLADABAS12 "Copying Adabas D...\n"
  #define STEXT_INSTALLADABAS13 "Installing  Adabas D...\n"
  #define STEXT_INSTALLADABAS14 "Configuring  Adabas D...\n"
  #define STEXT_INSTALLADABAS15 "Is '%s' the Adabas D directory ? (y/n): "
  #define STEXT_INSTALLADABAS16 "Please enter the top directory of the Adabas D installation : "
  #define STEXT_INSTALLWWW1 "Is Apache (ApacheSSL) your WWW-server ? (y/n): "
  #define STEXT_INSTALLWWW2 "The configuration file was not recognized automatically,\n"
  #define STEXT_INSTALLWWW3 "Is '%s' the configuration file for\nyour WWW-server ? (y/n): "
  #define STEXT_INSTALLWWW4 "Please enter the complete path and name of the configuration file (srm.conf)\nfor your WWW-server : "
  #define STEXT_INSTALLWWW5 "Is '%s' the configuration file for your\n WWW-server ? (y/n): "
  #define STEXT_INSTALLWWW6 "Please enter the complete path and name of the configuration file (srm.conf)\nfor your WWW-server : "
  #define STEXT_INSTALLWWW7 "Is '%s' (http://%s) the name \nof your WWW-server (y/n) ? : "
  #define STEXT_INSTALLWWW8 "Please enter the name of your WWW-server \n(e.g. www.intradat.com, or 194.152.32.29) :" 
  #define STEXT_INSTALLWWW9 "Is '%s' the HTML-directory (DocumentRoot) of\nyour WWW-Server (y/n) ? : " 
  #define STEXT_INSTALLWWW10 "Please enter the HTML-directory (DocumentRoot) \nof your WWW-server : "
  #define STEXT_INSTALLWWW11 "Is '%s' (http://%s%s) the URL of the CGI-directory (ScriptAlias) of\nyour WWW-server (y/n) ? : "
  #define STEXT_INSTALLWWW12 "Please enter the URL of the CGI-directory (ScriptAlias) of\nyour WWW-server  (e.g. /cgi-bin): " 
  #define STEXT_INSTALLWWW13 "Is '%s' the system-path of your CGI-directory\nof your WWW-server (y/n) ? : "
  #define STEXT_INSTALLWWW14 "Please enter the system-path to the CGI-directory of\nyour WWW-server : " 
  #define STEXT_INSTALLWWW15 "ERROR! - '%s' could not be opened to write data into! \n"
  #define STEXT_INSTALLWWW16 "The configuration file (srm.conf) could not be found!\n"
  #define STEXT_INSTALLWWW17 "The directory '%s' does not exist !\n"


  #define STEXT_INSTALL1 "Is '%s' the license-key for your version of VShop (y/n) ? : "
  #define STEXT_INSTALL2 "Please enter the license-key for your version of VShop: "
  #define STEXT_INSTALL3 "To run this installationprogram you must be the superuser !\n"

  #define TEXT_ENDE1 "Adabas was copied to %s.\n"
  #define TEXT_ENDE2 "The pages are available in %s.\n"
  #define TEXT_ENDE3 "To start VShop call http://%s/vshop/\n"
  #define TEXT_ENDE3b "ATTENTION! To administrate call http://%s/vshop/admin/\n"
  #define TEXT_ENDE4 "Please check the preferences in file '/etc/vshop.conf'.\n"
  #define TEXT_ENDE5 "For security reasons change the passwords of\nVShop administrators as described in the manual.\n\n"
  #define TEXT_ENDE6 "Congratulations, more information could be found at:\n"
  #define TEXT_ENDE7 "http://www.vshop.de.\n\nVShop - ecommerce for you.\n\n"


  #define DECIMALSYMBOL        '.'
  #define DECIMALSYMBOL_STRING "."
  #define DIGITSYMBOL          ','
  #define DIGITSYMBOL_STRING   ","
  #define CURRENCYNAME         "Dollar"
  #define CURRENCYSYMBOL       "#"
  #define WEEKDAY1             "sunday"
  #define WEEKDAY2             "monday"
  #define WEEKDAY3             "tuesday"
  #define WEEKDAY4             "wednesday"
  #define WEEKDAY5             "thursday"
  #define WEEKDAY6             "friday"
  #define WEEKDAY7             "saturday"
  #define MONTHNAME1           "january"
  #define MONTHNAME2           "february"
  #define MONTHNAME3           "march"
  #define MONTHNAME4           "april"
  #define MONTHNAME5           "may"
  #define MONTHNAME6           "june"
  #define MONTHNAME7           "july"
  #define MONTHNAME8           "august"
  #define MONTHNAME9           "september"
  #define MONTHNAME10          "october"
  #define MONTHNAME11          "november"
  #define MONTHNAME12          "december"
  #define TIMEFORMAT           "%H:%M"
  #define DATEFORMAT           "%m/%d/%Y"
  #define TIMESTAMPFORMAT      "%m/%d/%Y %H:%M"
  #define TEXT_MAINTENANCE1         "Deleting old sessions...<BR>\n"
  #define TEXT_MAINTENANCE2         "Updating sessions before %i.%.2i.%i %i:%.2i<BR>\n"
  #define TEXT_MAINTENANCE3         "Deleting sessionvars of session %s...<BR>\n"
  #define TEXT_MAINTENANCE4         "Deleting baskets of session %s"
  #define TEXT_MAINTENANCE5         "done.<BR>\n"
  #define TEXT_MAINTENANCE6         "No sessions found.<BR>\n"
   #define TEXT_ERROR1 "Demoversion is expired !"
  #define TEXT_ERROR2 "Variable DBROOT not set ! Please edit the configuration file"
  #define TEXT_ERROR3 "Variable TEMPLATEROOT not set ! Please edit the configuration file"
  #define TEXT_ERROR4 "Variable DBUSER not set ! Please edit the configuration file"
  #define TEXT_ERROR5 "Variable DBPASSWORD not set ! Please edit the configuration file"

  #define TEXT_ERROR6 "Session timed out !"
  #define TEXT_ERROR7 "back to startpage"

  #define TEXT_ERROR8 "Variable CGIBIN not set ! Please edit the configuration file"
  #define TEXT_ERROR9 "Variable BASEREF not set ! Please edit the configuration file"
  #define TEXT_ERROR10 "Can not open the file '%s' !"
  #define TEXT_ERROR11 "This Version \"%s\" is not registered for '%s'"
    #define TEXT_ERROR12 "Variable KEY not set ! Please edit the configuration file"
    #define TEXT_ERROR13 "the engin name is to long  !"
    #define TEXT_ERROR14 "Your IP '%s' is not allowed to access the administration area."

#else 

  #define TEXT_LIBC5DIALOG1 " Auf Ihrem System wurde keine installierte libc6"
  #define TEXT_LIBC5DIALOG2 " gefunden. Dies kann zu Problemen mit VShop fÅhren."
  #define TEXT_LIBC5DIALOG3 " Bitte installieren Sie die 'libc6' Dateien nach."
  #define TEXT_LIBC5DIALOG4 "                                                      "
  #define TEXT_LIBC5DIALOG5 "                        "
  #define TEXT_LIBC5DIALOG6 "      Es wird empfohlen das Setup nun abzubrechen !"

#ifdef VSHOP_DEMO
  #define TEXT_INSTALLDIALOG1 " Dieses Programm installiert VShop II Demo auf  "
  #define TEXT_INSTALLDIALOG2 " Ihrem System. Vor der Installation muss ein WWW-Server "
  #define TEXT_INSTALLDIALOG3 " installiert und ein Benutzer fÅr Adabas D angelegt "
  #define TEXT_INSTALLDIALOG4 " worden sein. Diese Version wird am 1.12.2000 ablaufen. "
  #define TEXT_INSTALLDIALOG5a " "
  #define TEXT_INSTALLDIALOG5b " Weitere Informationen erhalten Sie unter "
  #define TEXT_INSTALLDIALOG6 " http://www.vshop.de oder http://support.intradat.com"
#else
  /* stefan */
  #ifndef DB_ORACLE
	  #define TEXT_INSTALLDIALOG1 " Dieses Programm installiert VShop II auf Ihrem System. "
	  #define TEXT_INSTALLDIALOG2 " Vor der Installation muss ein WWW-Server installiert"
	  #define TEXT_INSTALLDIALOG3 " und ein Benutzer fÅr Adabas DDDD angelegt worden sein."
	  #define TEXT_INSTALLDIALOG4 "  "
	  #define TEXT_INSTALLDIALOG5a " "
	  #define TEXT_INSTALLDIALOG5b " Weitere Informationen erhalten Sie unter "
	  #define TEXT_INSTALLDIALOG6 " http://www.vshop.de oder http://support.intradat.com"
  #else
	  #define TEXT_INSTALLDIALOG1 " Dieses Programm installiert VShop II auf Ihrem System. "
	  #define TEXT_INSTALLDIALOG2 " Vor der Installation muss ein WWW-Server und eine"
	  #define TEXT_INSTALLDIALOG3 " Oracle Datenbank in der Version 8.i installiert sein."
	  #define TEXT_INSTALLDIALOG4 "  "
	  #define TEXT_INSTALLDIALOG5a " "
	  #define TEXT_INSTALLDIALOG5b " Weitere Informationen erhalten Sie unter "
	  #define TEXT_INSTALLDIALOG6 " http://www.vshop.de oder http://support.intradat.com"
  #endif
#endif

  #define TEXT_UEBERSCHREIBENDIALOG1 " VShop ist bereits auf Ihrem System installiert."
  #define TEXT_UEBERSCHREIBENDIALOG2 " Um Ihre Version zu erneuern brechen Sie bitte diese"
  #define TEXT_UEBERSCHREIBENDIALOG3 " Installation mit [ESC] ab und starten '/update'"

  #define TEXT_DIALOGBOTTOM1 "[F10=OK/ESC=Abbruch]"
  #define TEXT_DIALOGTOP1 " VShop Installation "
  #define TEXT_DIALOGBOTTOM2 "[RETURN/F10=OK] [ESC=Abbruch]"
  #define TEXT_KEYDIALOG1 " VShopKey Eingabe "
  #define TEXT_KEYDIALOG2 " Key : "
  #define TEXT_ADABASMENU1 "Adabas D ist noch nicht installiert"
  #define TEXT_ADABASMENU2 "Adabas D ist bereits installiert"
  #define TEXT_ADABASMENU3 "Installation abbrechen"
  #define TEXT_ADABASDIALOG1 " Adabas Installation"
  #define TEXT_ERRORINSTALL " Fehler "
  #define TEXT_ADABASUSERERROR1 " Benutzer '"
  #define TEXT_ADABASUSERERROR2 "' existiert nicht !"
  #define TEXT_ADABASDIRERROR1 "Verzeichnis"
  #define TEXT_ADABASDIRERROR2 "existiert nicht !"

  #define TEXT_ADABASPARAMDIALOG " Adabas Installationsparameter "
  #define TEXT_ADABASPARAMDIALOG1 " Adabas D Verzeichnis : "
  #define TEXT_ADABASPARAMDIALOG2 " Adabas D Benutzer    : "

   /* stefan */
  #define TEXT_ORACLEPARAMDIALOG " Oracle 8.i Installationsparameter "
  #define TEXT_ORACLEPARAMDIALOG1 " Oracle HOME : "
  #define TEXT_ORACLEPARAMDIALOG2 " Service @{your_name} : "
  #define TEXT_ORACLEPARAMDIALOG3 " Oracle sys {Passwort} : "
  #define TEXT_ORACLEPARAMDIALOG4 " Neuer {Benutzer} : "
  #define TEXT_ORACLEPARAMDIALOG5 " Neues {Passwort} : "
  #define TEXT_ORACLEDIRERROR1 "Verzeichnis"
  #define TEXT_ORACLEDIRERROR2 "existiert nicht !"
  #define TEXT_ORACLEPARAMHELP1 " Sqlplus muss sich wie folgt aufrufen lassen:"
  #define TEXT_ORACLEPARAMHELP2 " root$ sqlplus sys/change_on_install@yourname"
  #define TEXT_ORACLEPARAMHELP3 " Im Anschluss erscheint der SQL-Prompt: SQL>_"
  #define TEXT_ORACLEPARAMHELP4 "  BITTE UEBERPRUEFEN SIE DIES ZUERST, DANKE  "
  
  #define TEXT_HTTPDCONFERROR1 "Die Datei "
  #define TEXT_HTTPDCONFERROR2 "existiert nicht !" 
  #define TEXT_WWWSERVERDIALOG  " Apache-Server Konfiguration"
  #define TEXT_WWWSERVERDIALOG1 "Server ist kein Apache Server"
  #define TEXT_WWWSERVERDIALOG2 "Konfiguration ist"
  #define TEXT_WWWSERVERDIALOG3 "Konfiguration ist /etc/httpsd/httpd.conf"
  #define TEXT_WWWSERVERDIALOG4 "Konfiguration ist /etc/httpd/conf/httpd.conf"
  #define TEXT_WWWSERVERDIALOG5 "Konfiguration ist /etc/httpsd/conf/httpd.conf"
  #define TEXT_WWWSERVERDIALOG6 "Konfigurationsdatei manuell angeben :"
  #define TEXT_HTDOCSERROR1 "Das Verzeichnis "
  #define TEXT_HTDOCSERROR2 "existiert nicht !"
  #define TEXT_CGIBINERROR1 "Das Verzeichnis "
  #define TEXT_CGIBINERROR2 "existiert nicht !"
  #define TEXT_PARAMDIALOG " Parameter Eingabe "
  #define TEXT_PARAMDIALOG1 " Servername           : "
  #define TEXT_PARAMDIALOG2 " CGI URL              : "
  #define TEXT_PARAMDIALOG3 " HTML Verzeichnis     : "
  #define TEXT_PARAMDIALOG4 " CGI Verzeichnis      : "
  #define TEXT_MAINMENU1 "Key eingeben"
  #define TEXT_MAINMENU2 "Adabas D Benutzer aendern" 
  #define TEXT_MAINMENU3 "Neuen Shop erstellen"
  #define TEXT_MAINMENU4 "ENDE"

  #define TEXT_NEWSHOPDIALOG " Neuer Shop "
  #define TEXT_NEWSHOPDIALOG1 " Nur in der Providerversion mˆglich !"
  #define TEXT_ERRORINSTALL1 "Fehler beim EinfÅgen des neuen Keys !\n"
  #define TEXT_ERRORINSTALL2 "Fuer die Installation der Download-Version muss Adabas D vorhanden sein !\n"
  #define TEXT_ERRORINSTALL3 "Installation beendet\n"
  #define TEXT_ERRORINSTALL4 "Installation abgebrochen\n"
  #define TEXT_ERRORINSTALL5 "Konfiguration abgebrochen\n"
  #define TEXT_ERRORINSTALL6 "Um dieses Installationsprogramm durchzufuehren mÅssen Sie Superuser sein !\n"
  #define TEXT_ERRORINSTALL7 "Fehler beim EinfÅgen des Keys..."
  
  /* stefan */
  #define TEXT_ENDE1 "Adabas wurde nach %s kopiert.\n"
  #define TEXT_ENDE1b "Oracle User wurde installiert.\n"
  
  #define TEXT_ENDE2 "Die VShop Seiten liegen in %s.\n"
  #define TEXT_ENDE3 "Die Startseite des VShop finden Sie unter http://%s/vshop/\n"
  #define TEXT_ENDE3b "Den Administrationsbereich des VShop finden Sie unter http://%s/vshop/admin/\n"
  #ifdef VSL_ENGINE
    #define TEXT_ENDE4 "Bitte ÅberprÅfen Sie die Einstellungen in '/etc/vsl.conf'.\n"
  #else
    #define TEXT_ENDE4 "Bitte ÅberprÅfen Sie die Einstellungen in '/etc/vshop.conf'.\n"
  #endif
  #define TEXT_ENDE5 "éndern Sie die Passwˆrter der Datenbankbenutzer und des\nVShop Administrators wie im Handbuch beschrieben.\n\n"
  #define TEXT_ENDE6 "Viel Erfolg mit Ihrem Shop, weitere Informationen\n"
  #define TEXT_ENDE7 "finden Sie auch im Internet unter http://www.vshop.de.\n\nVShop - ecommerce for you.\n\n"
  
  #define TEXT_INSTALL1 " Adabas Installation "
  #define TEXT_INSTALL2 " Kopiere Datenbank..."
  #define TEXT_INSTALL3 " Entpacke Datenbank..."
  #define TEXT_INSTALL4 "Starte Adabas D Installation...\n"
  #define TEXT_INSTALL5 "Kopiere VShop..."
  /* stefan */
  #ifndef DB_ORACLE
	  #define TEXT_INSTALL6 "Kopiere VShop-Datenbank..."
  #else
	  #define TEXT_INSTALL6 "Installiere Oracle-VShopUser..."
  #endif
  #define TEXT_INSTALL7 "Konvertiere VShop-Datenbank..."
  #define TEXT_INSTALL8 "Kopiere VShop-Verwaltung...   "

  #define STEXT_YES1 "j"
  #define STEXT_YES2 "J"

  #define STEXT_UEBERSCHREIBEN1 "VShop ist bereits auf Ihrem System installiert.\n"
  #define STEXT_UEBERSCHREIBEN2 "Um Ihre Version zu erneuern beenden Sie bitte diese Installation\n"
  #define STEXT_UEBERSCHREIBEN3 "und starten '/update'"
  #define STEXT_UEBERSCHREIBEN4 "Soll die Installation fortgesetzt werden (VSHOP WIRD UEBERSCHRIEBEN!) ? (j/n): "
  
  #define STEXT_INSTALLADABAS1 "Ist Adabas D bereits installiert ? (j/n): "
  #define STEXT_INSTALLADABAS2 "Ist ein Benutzer fÅr Adabas D angelegt ? (j/n): "
  #define STEXT_IS "Ist '"
  #define STEXT_INSTALLADABAS3 "' der Benutzer von Adabas D ? (j/n): "
  #define STEXT_INSTALLADABAS4 "Bitte geben Sie den Benutzer ein, dem Adabas D \"gehˆren\" soll :"
  #define STEXT_INSTALLADABAS4B "Bitte geben Sie den Benutzer ein, dem Adabas D \"gehˆrt\" :"
  #define STEXT_INSTALLADABAS5 "' das Verzeichnis, \nin das Adabas D installiert werden soll? (j/n): "
  #define STEXT_INSTALLADABAS6 "Bitte geben Sie das Verzeichnis ein, \nin das Adabas D installiert werden soll :"
  #define STEXT_INSTALLADABAS7 "Die Adabas D Dateien mÅssen einem Benutzer (z.B. adabas) gehˆren.\n"
  #define STEXT_INSTALLADABAS8 "Dieser spezielle Benutzer muss vor der Installation von Adabas D \n"
  #define STEXT_INSTALLADABAS9 "angelegt worden sein.\n\n"
  #define STEXT_INSTALLADABAS10 "Bitte legen Sie erst einen Benutzer fÅr Adabas D an.\n\n"
  #define STEXT_INSTALLADABAS11 "Der Benutzer '%s' existiert nicht auf diesem System!\n\n"
  #define STEXT_INSTALLADABAS12 "Kopiere Adabas D...\n"
  #define STEXT_INSTALLADABAS13 "Installiere Adabas D...\n"
  #define STEXT_INSTALLADABAS14 "Konfiguriere Adabas D...\n"
  #define STEXT_INSTALLADABAS15 "Ist '%s' das Adabas D Verzeichnis ? (j/n): "
  #define STEXT_INSTALLADABAS16 "Bitte geben Sie das Adabas D Verzeichnis ein : "
  #define STEXT_INSTALLWWW1 "Ist Ihr WWW-Server der Apache (ApacheSSL) Server ? (j/n): "
  #define STEXT_INSTALLWWW2 "Die Konfigurationsdatei wurde nicht automatisch gefunden,\n"
  #define STEXT_INSTALLWWW3 "Ist '%s' die Umgebungs-Konfigurationsdatei \nIhres WWW-Servers ? (j/n): "
  #define STEXT_INSTALLWWW4 "Bitte geben Sie Pfad und Namen der Umgebungs-Konfigurationsdatei (srm.conf)\nIhres WWW-Servers ein : "
  #define STEXT_INSTALLWWW5 "Ist '%s' die Konfigurationsdatei \nIhres WWW-Servers ? (j/n): "
  #define STEXT_INSTALLWWW6 "Bitte geben Sie Pfad und Namen der Konfigurationsdatei \nIhres WWW-Servers ein : "
  #define STEXT_INSTALLWWW7 "Ist '%s' (http://%s) der Name \nIhres WWW-Servers (j/n) ? : "
  #define STEXT_INSTALLWWW8 "Bitte geben Sie den Namen Ihres WWW-Servers ein \n(z.B. www.intradat.com, oder 194.152.32.29) :" 
  #define STEXT_INSTALLWWW9 "Ist '%s' das HTML Verzeichnis \nIhres WWW-Servers (j/n) ? : " 
  #define STEXT_INSTALLWWW10 "Bitte geben Sie das HTML Verzeichnis  \nIhres WWW-Servers ein : "
  #define STEXT_INSTALLWWW11 "Ist '%s' (http://%s%s) die URL des CGI-Verzeichnises \nIhres WWW-Servers (j/n) ? : "
  #define STEXT_INSTALLWWW12 "Bitte geben Sie die URL des CGI-Verzeichnises  \nIhres WWW-Servers ein (z.B. /cgi-bin): " 
  #define STEXT_INSTALLWWW13 "Ist '%s' der Pfad des CGI-Verzeichnises \nIhres WWW-Servers (j/n) ? : "
  #define STEXT_INSTALLWWW14 "Bitte geben Sie den Pfad des CGI-Verzeichnises  \nIhres WWW-Servers ein : " 
  #define STEXT_INSTALLWWW15 "Die Datei '%s' kann nicht zum schreiben geˆffnet werden !\n"
  #define STEXT_INSTALLWWW16 "Die Umgebungs-Konfigurationsdatei (srm.conf) konnte nicht gefunden werden.\n"
  #define STEXT_INSTALLWWW17 "In das Verzeichnis '%s' kann nicht geschrieben werden !\n"


  #define STEXT_INSTALL1 "Ist '%s' der Key fuer Ihre Version des VShops (j/n) ? : "
  #define STEXT_INSTALL2 "Bitte geben Sie den Key fuer Ihre Version des VShops ein: "
  #define STEXT_INSTALL3 "Um dieses Installationsprogramm durchzufuehren muessen Sie Superuser sein !\n"


  #define DECIMALSYMBOL        ','
  #define DECIMALSYMBOL_STRING ","
  #define DIGITSYMBOL          '.'
  #define DIGITSYMBOL_STRING   "."
  #define CURRENCYNAME         "Deutsche Mark"
  #define CURRENCYSYMBOL       "DM"
  #define WEEKDAY1             "Sonntag"
  #define WEEKDAY2             "Montag"
  #define WEEKDAY3             "Dienstag"
  #define WEEKDAY4             "Mittwoch"
  #define WEEKDAY5             "Donnerstag"
  #define WEEKDAY6             "Freitag"
  #define WEEKDAY7             "Samstag"
  #define MONTHNAME1           "Januar"
  #define MONTHNAME2           "Februar"
  #define MONTHNAME3           "MÑrz"
  #define MONTHNAME4           "April"
  #define MONTHNAME5           "Mai"
  #define MONTHNAME6           "Juni"
  #define MONTHNAME7           "Juli"
  #define MONTHNAME8           "August"
  #define MONTHNAME9           "September"
  #define MONTHNAME10          "Oktober"
  #define MONTHNAME11          "November"
  #define MONTHNAME12          "Dezember"
  #define TIMEFORMAT           "%H:%M"
  #define DATEFORMAT           "%d.%m.%Y"
  #define TIMESTAMPFORMAT      "%d.%m.%Y %H:%M:%S"

  #define TEXT_MAINTENANCE1         "Lˆsche alle alten Sitzungen<BR>\n"
  #define TEXT_MAINTENANCE2         "Bearbeite Sitzungen vor %i.%.2i.%i %i:%.2i<BR>\n"
  #define TEXT_MAINTENANCE3         "Lˆsche die Variablen der Sitzung %s...<BR>\n"
  #define TEXT_MAINTENANCE4         "Lˆsche den Warenkorb der Sitzung %s"
  #define TEXT_MAINTENANCE5         "Fertig !<BR>\n"
  #define TEXT_MAINTENANCE6         "Keine Sitzungen gefunden !<BR>\n"

  #define TEXT_VERSIONDOWNLOAD " VShop II download Deutsch "
  #ifdef VSHOP_DEMO
    #define TEXT_VERSION " VShop II Demo Deutsch "

  #elif VSHOP_FREE
    #define TEXT_VERSION " VShop II Light Deutsch "
  #elif VSHOP_BASIC
    #define WRONGGROUP "Falsche Benutzergruppe"
    #define TEXT_VERSION " VShop II Basic Deutsch "
  #elif VSHOP_PROVIDER
    /* stefan */
	#ifndef DB_ORACLE
       #ifdef WITHOUT_ADABAS
         #define TEXT_VERSION " VShop II Provider Deutsch (Ohne Adabas) "
       #else
         #define TEXT_VERSION " VShop II Provider Deutsch "
       #endif
    #else
		#define TEXT_VERSION " VShop II Provider Deutsch (Oracle 8.i) "
	#endif
  #elif VSHOP_PARTNER
    #define TEXT_VERSION " VShop II Partner Deutsch "

  #elif VSHOP_PROFESSIONAL
    #ifdef WITHOUT_ADABAS
      #define TEXT_VERSION " VShop II Professional Deutsch (Ohne Adabas) "
    #else
      #define TEXT_VERSION " VShop II Professional Deutsch "
    #endif
  #endif

  #define TEXT_ERROR1 "Diese Demoversion ist abgelaufen. "
  #define TEXT_ERROR2 "Die Variable DBROOT ist nicht gesetzt ! Bitte editieren Sie die Konfigurationsdatei '/etc/vshop.conf'"
  #define TEXT_ERROR3 "Die Variable TEMPLATEROOT ist nicht gesetzt. Bitte editieren Sie die Konfigurationsdatei '/etc/vshop.conf'"
  #define TEXT_ERROR4 "Die Variable DBUSER ist nicht gesetzt. Bitte editieren Sie die Konfigurationsdatei '/etc/vshop.conf'"
  #define TEXT_ERROR5 "Die Variable DBPASSWORD ist nicht gesetzt. Bitte editieren Sie die Konfigurationsdatei '/etc/vshop.conf'" 
  #define TEXT_ERROR6 "Diese Sitzung ist abgelaufen. "
  #define TEXT_ERROR7 "ZurÅck zur Startseite"
  #define TEXT_ERROR8 "Die Variable CGIBIN ist nicht gesetzt. Bitte editieren Sie die Konfigurationsdatei '/etc/vshop.conf'"
  #define TEXT_ERROR9 "Die Variable BASEREF ist nicht gesetzt. Bitte editieren Sie die Konfigurationsdatei '/etc/vshop.conf'"
  #define TEXT_ERROR10 "Die Datei '%s' konnte nicht gˆffnet werden." 
  #define TEXT_ERROR11 "Diese Version \"%s\" ist nicht fÅr '%s' registriert.<BR>Bitte geben Sie Ihren Key Åber den Administrationsbereich ein."
  #define TEXT_ERROR12 "Die Konfigurationsdatei '/etc/vshop.conf' konnte nicht geˆffnet werden."
  #define TEXT_ERROR13 "Der Name der Engine ist ungÅltig lang."
  #define TEXT_ERROR14 "Ihre IPAdresse '%s' ist nicht fuer den Zugriff auf den Administrationsbereich berechtigt."


#endif

