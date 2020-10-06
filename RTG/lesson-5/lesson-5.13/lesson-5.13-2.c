/*
 * Listing 5.13-1  Daemon demonstration using the daemon() system call
 * 
 *  This program does the following:
 *   - initializes the process as a daemon (using the daemon())
 *   - sets the sylog to accept messages
 *   - modifies signal handlers to send a message to the sys-logger in case of specific signals
 *   - waits for signals to occur
 * 
 * please run the following:
 * 
 * 1. compile the program gcc -ggdb lesson-5.13-2.c -o signal_deamon
 * 2. open a new terminal and run  - tail -f /var/log/syslog
 * 3. run the program, from the previous terminal ./signal_deamon
 * 4. send a SIGHUP signal to the daemon process - kill -1 <pid>, have a look at the logger
 * 
 *  To terminate the daemon process use kill -2 <pid> 
 * 
 */


#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
//umask
#include <sys/types.h>
#include <sys/stat.h>
 
#define DAEMON_NAME "simpledaemon"
 
 
void daemonShutdown();
void signal_handler(int sig);
void signal_configuration();

 
//////////////////////////////////////////////////////////// 
void signal_handler(int sig)
{
	switch(sig)
    {
        case SIGHUP:
            syslog(LOG_WARNING, "Received SIGHUP signal.");
            break;
        case SIGINT:
        case SIGTERM:
            syslog(LOG_INFO, "Daemon exiting");
            exit(EXIT_SUCCESS);
            break;
            
        default:
            syslog(LOG_WARNING, "Unhandled signal %s", strsignal(sig));
            break;
     }
}
//////////////////////////////////////////////////////////// 
void signal_configuration()
{
    struct sigaction newSigAction;
    sigset_t newSigSet;
                
    syslog(LOG_INFO, "daemon process created: %d\n",getpid() );       
 
    // Set up a signal handler 
    newSigAction.sa_handler = signal_handler;
    sigemptyset(&newSigAction.sa_mask);
    newSigAction.sa_flags = 0;
         
    // Signals to handle
    sigaction(SIGHUP, &newSigAction, NULL);     /* catch hangup signal */
    sigaction(SIGTERM, &newSigAction, NULL);    /* catch term signal */
    sigaction(SIGINT, &newSigAction, NULL);     /* catch interrupt signal */     
}
//////////////////////////////////////////////////////////// 
int main()
{
		/* Debug logging
        setlogmask(LOG_UPTO(LOG_DEBUG));
        openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
        */
        int nochdir=0;
        int noclose=0;
        int i=0;
        int ret =-1;
         
        /* Logging */
        setlogmask(LOG_UPTO(LOG_INFO));
        openlog(DAEMON_NAME, LOG_CONS | LOG_PERROR, LOG_USER);
         
        syslog(LOG_INFO, "Daemon starting up");
 
		ret = daemon(nochdir,noclose);
		if(ret == -1 )
        {
            // Couldn't open lock file 
            perror(" can't Daemonize");
            syslog(LOG_INFO, "can't Daemonize , exiting");
            exit(EXIT_FAILURE);
        }
        
        /* set signal configuration */
        signal_configuration();
         
        syslog(LOG_INFO, "Daemon running");
         
        while (1)
        {
            syslog(LOG_INFO, "daemon %d is still alive, running %d seconds \n",getpid(),(++i*10) );
             
            sleep(10);
        }
 }
//////////////////////////////////////////////////////////// 
