#ifdef ENGLISH
  #define TEXT_DIALOGBOTTOM1 "[F10=OK/ESC=Cancel]"
  #define TEXT_DIALOGTOP1 " VShop Installation "
  #define TEXT_DIALOGBOTTOM2 "[RETURN/F10=OK][ESC=Cancel]"

  #define TEXT_KEYDIALOG1 " Enter VShop key"
  #define TEXT_KEYDIALOG2 " Key : "

  #define TEXT_DOMAINPWDIALOG1 " Enter Admin password "
  #define TEXT_DOMAINPWDIALOG2 " PW  : "
  
  #define TEXT_NAMEDIALOG1 " Enter shopname "
  #define TEXT_NAMEDIALOG2 " Name :"

  #define TEXT_ERROR " Error "

  #define TEXT_HTDOCSERROR1 "The directory   "
  #define TEXT_HTDOCSERROR2 "does not exist!  "
  #define TEXT_CGIBINERROR1 "The directory   "
  #define TEXT_CGIBINERROR2 "does not exist!  "

  #define TEXT_PARAMDIALOG " Enter parameters  "
  #define TEXT_PARAMDIALOG1 " servername           : "
  #define TEXT_PARAMDIALOG2 " CGI URL        (http): "
  #define TEXT_PARAMDIALOG3 " HTML directory (sys) : "
  #define TEXT_PARAMDIALOG4 " CGI directory  (sys) : "

  #define TEXT_MAINMENU1 "Show shops "
  #define TEXT_MAINMENU2 "Delete shop" 
  #define TEXT_MAINMENU3 "Create new shop"
  #define TEXT_MAINMENU4 "EXIT"

  #define TEXT_VERSIONFULL " VShop 1.6.1 Provider English "

  #define  ATEXT_MESSAGE1 "Creating tables..."
  #define  ATEXT_MESSAGE2 "Wrong password of sysdba 'ADMIN'" 
  #define  ATEXT_MESSAGE3 "Copy VShop-Engine..." 
  #define  ATEXT_MESSAGE4 "Error while copying VShop-Engine"
  #define  ATEXT_MESSAGE5 "Copy VShop-Administration..." 
  #define  ATEXT_MESSAGE6 "Error while copying VShop-Administration" 
  #define  ATEXT_MESSAGE7 "Createing tables..." 
  #define  ATEXT_MESSAGE8 "No, do not delete" 
  #define  ATEXT_MESSAGE9 "Yes, delete " 
  #define  ATEXT_MESSAGE10 "Delete shop"
  #define  ATEXT_MESSAGE12 "Deleting tables..."
  #define  ATEXT_MESSAGE13 "Deleting all pages..."
  #define  ATEXT_MESSAGE14 "Deleting VShop-Engine..."
  #define  ATEXT_MESSAGE15 "BACK TO STARTPAGE"
  #define  ATEXT_MESSAGE16 "The name must be shorter than 7 characters"

  #define TEXT_ERROR6 "To run this administration you have to be superuser!\n"
    
#else

  #define TEXT_DIALOGBOTTOM1 "[F10=OK/ESC=Abbruch]"
  #define TEXT_DIALOGTOP1 " VShop Administration"
  #define TEXT_DIALOGBOTTOM2 "[RETURN/F10=OK] [ESC=Abbruch]"

  #define TEXT_KEYDIALOG1 " VShopKey Eingabe "
  #define TEXT_KEYDIALOG2 " Key : "

  #define TEXT_DOMAINPWDIALOG1 " Admin-Passwort Eingabe "
  #define TEXT_DOMAINPWDIALOG2 " PW  : "
  
  /* stefan */
  #define TEXT_ORACLEPWDIALOG1 " Oracle Systemdaten "
  #define TEXT_ORACLEPWDIALOG2 " Oracle sys {Passwort}: "
  #define TEXT_ORACLEPWDIALOG3 " Service @{yourname} : "

  #define TEXT_NAMEDIALOG1 " Shopname Eingabe "
  #define TEXT_NAMEDIALOG2 " Name :"

  #define TEXT_ERROR " Fehler "
  #define TEXT_HTDOCSERROR1 "Das Verzeichnis "
  #define TEXT_HTDOCSERROR2 "existiert nicht !"
  #define TEXT_CGIBINERROR1 "Das Verzeichnis "
  #define TEXT_CGIBINERROR2 "existiert nicht !"
  #define TEXT_PARAMDIALOG " Parameter Eingabe "
  #define TEXT_PARAMDIALOG1 " Servername           : "
  #define TEXT_PARAMDIALOG2 " CGI URL              : "
  #define TEXT_PARAMDIALOG3 " HTML Verzeichnis     : "
  #define TEXT_PARAMDIALOG4 " CGI Verzeichnis      : "
  #define TEXT_MAINMENU1 "Shops anzeigen"
  #define TEXT_MAINMENU2 "Shop loeschen" 
  #define TEXT_MAINMENU3 "Neuen Shop erstellen"
  #define TEXT_MAINMENU4 "ENDE"

  #ifdef VSHOP_PARTNER
    #define TEXT_VERSIONFULL " VShop II Partner Deutsch "
  #else
    #ifndef DB_ORACLE
	    #define TEXT_VERSIONFULL " VShop II Provider Deutsch (Adabas D) "
	#else
	    #define TEXT_VERSIONFULL " VShop II Provider Deutsch (Oracle 8.i) "
	#endif
  #endif

  #define  ATEXT_MESSAGE1 "Erstelle Tabellen..."
  #define  ATEXT_MESSAGE2 "Falsches Passwort fuer Benutzer 'ADMIN'" 
  #define  ATEXT_MESSAGE3 "Kopiere VShop-Engine..." 
  #define  ATEXT_MESSAGE4 "Fehler beim Kopieren der Engine"
  #define  ATEXT_MESSAGE5 "Kopiere VShop-Administration..." 
  #define  ATEXT_MESSAGE6 "Fehler beim Kopieren der Administration" 
  #define  ATEXT_MESSAGE7 "Erstelle Tabellen..." 
  #define  ATEXT_MESSAGE8 "Shop nicht loeschen" 
  #define  ATEXT_MESSAGE9 "Shop loeschen" 
  #define  ATEXT_MESSAGE10 "Shop l˜schen"
  #define  ATEXT_MESSAGE12 "L˜sche Tabellen..."
  #define  ATEXT_MESSAGE13 "L˜sche alle Seiten..."
  #define  ATEXT_MESSAGE14 "L˜sche VShop-Engine..."
  #define  ATEXT_MESSAGE15 "ZURUECK ZUR HAUPTSEITE"
  #define  ATEXT_MESSAGE16 "Der Name darf nur 6 Zeichen lang sein"

  #define TEXT_ERROR6 "Um dieses Administrationsprogramm durchzufuehren mÅssen Sie Superuser sein !\n"
#endif


