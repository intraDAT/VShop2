#ifndef __CARP_H
#define __CARP_H

#include <iostream>
#include <string>
#include <fstream> // for testing only

/** Provides methods to write messages to an output stream.  All members are
  * static; linking the object into an executable provides a global error sink.
  * The output stream is static and should be set using setStream(); if
  * it is not set, standard output is assumed.  All message methods (error(),
  * warning(), info()) accept one, two or three arguments.  If the first
  * argument is a null string, the contents of the internal buffer are used for
  * the text.
  */
class Carp {

  public:

    /** Sets the stream to which errors are written.
      */
    static void setStream( ostream *os );

    /** Indicates whether to print the date and time in messages.  The default
      * is false.
      */
    static void showDate( bool show );

    /** Returns a pointer to a character buffer that can be used to prepare
      * messages (with sprintf() for example).  The buffer is 256 characters
      * long.
      */
    static char *buf();

    /** Writes an error message.  Errors are typically used to indicate that
      * something has gone wrong and recovery is not possible.
      */
    static void error( const string &text,
                       const string &method = "",
                       const string &className = "" );

    /** Writes a warning message.  Warning are typically used to indicate that
      * something unexpected has occured but the program continutes to run.
      */
    static void warning( const string &text,
                         const string &method = "",
                         const string &className = "" );

    /** Writes an information message.
      */
    static void info( const string &text,
                      const string &method = "",
                      const string &className = "" );

  private:

    static void message( const string &type,
                         const string &className,
                         const string &method,
                         const string &text );

    static string date();

    static ostream *os_;
    static bool showDate_;
    static char buf_[ 256 ];
};

#endif

