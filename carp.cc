#include "carp.h"
#include <strstream.h>
#include <iomanip.h>
#include <time.h>

ostream *Carp::os_ = 0;
bool Carp::showDate_ = false;
char Carp::buf_[ 256 ];

void Carp::setStream( ostream *os ) {
  os_ = os;
}

void Carp::showDate( bool show ) {
  showDate_ = show;
}

char *Carp::buf() {
  return buf_;
}

void Carp::error( const string &text,
                  const string &method = "",
                  const string &className = "" ) {
  message( "ERROR", className, method, text );
}

void Carp::warning( const string &text,
                           const string &method = "",
                           const string &className = "" ) {
  message( "WARNING", className, method, text );
}

void Carp::info( const string &text,
                 const string &method = "",
                 const string &className = "" ) {
  message( "INFO", className, method, text );
}

//
// Formats a message for output
//
void Carp::message( const string &type,
                    const string &className,
                    const string &method,
                    const string &text ) {
  if ( !os_ ) {
    cout << "[Carp::message] WARNING: Output stream hasn't been set; using "
            "cout.\n";
    os_ = &cout;
  }

  // build date string
  if ( showDate_ )
    *os_ << '[' << date() << "] ";

  // build message
  if ( !method.empty() )
    *os_ << '[';

  if ( !className.empty() )
    *os_ << className << "::";

  if ( !method.empty() )
    *os_ << method << "] ";

  *os_ << type << ": ";

  if ( text.empty() )
    *os_ << buf_;
  else
    *os_ << text;

  *os_ << endl;
}

//
// Returns a string containing the current date and time.
//
string Carp::date() {
  // set up string stream
  char s[ 32 ];
  ostrstream os( s, 32 );

  // get local time
  time_t t = time( 0 );
  struct tm *ts = localtime( &t );

  // set up formatting
  os.fill( '0' );

  // format date
  os << setw( 2 ) << ts->tm_mon << '/' <<
        setw( 2 ) << ts->tm_mday << '/' << 
        setw( 2 ) << ts->tm_year + 1900 <<
        ' ' <<
        setw( 2 ) << ts->tm_hour << ':' <<
        setw( 2 ) << ts->tm_min << ':' <<
        setw( 2 ) << ts->tm_sec << ends;
  
  return string( s );
}
