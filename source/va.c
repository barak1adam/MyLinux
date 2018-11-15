/*
syntax:
=======
#include <cstdarg>
type va_arg( va_list argptr, type );
void va_end( va_list argptr );
void va_start( va_list argptr, last_parm );

The va_arg() macros are used to pass a variable number of arguments to a function.

    First, you must have a call to va_start() passing a valid va_list and the name of the last argument variable before the ellipsis ("..."). This first argument can be anything; one way to use it is to have it be an integer describing the number of parameters being passed.
    Next, you call va_arg() passing the va_list and the type of the argument to be returned. The return value of va_arg() is the current parameter.
    Repeat calls to va_arg() for however many arguments you have.
    Finally, a call to va_end() passing the va_list is necessary for proper cleanup.

 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#define NOTE 0
#define WARNING 1
#define MESSAGE_LEN 80

static void format_msg(char *, size_t, const char *, ...);
static int sum( int num, ... );

static const char *msgs[] = {
  "Msg # %04d  This is message number zero",
  "Msg # %04d  This message requires a %s"
};

int main()
{
   char msgbuf[84] = {0};
   int answer = sum( 4, 4, 3, 2, 1 );

   printf( "The answer is %d\n", answer );

   format_msg(msgbuf, MESSAGE_LEN, msgs[NOTE], NOTE);
   printf("The formatted message is: \"%s\" \n", msgbuf);

   format_msg(msgbuf, MESSAGE_LEN, msgs[WARNING], WARNING,
              "some string");
   printf("The formatted message is: \"%s\" \n", msgbuf);
   return 0;
}

static void format_msg(char *buf, size_t limit, const char *format,...)
{
   va_list args;
   /* Barak: Most important:
    * this is the call that init the va_list with all the dynamic ars recieved after format... */
   va_start(args, format),
   vsnprintf(buf, limit, format, args);
   va_end(args);
}

static int sum( int num, ... ) {
  int answer = 0;
  va_list argptr;

  va_start( argptr, num );

  for( ; num > 0; num-- ) {
    answer += va_arg( argptr, int );
  }

  va_end( argptr );

  return( answer );
}



