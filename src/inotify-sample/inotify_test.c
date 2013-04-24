#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/inotify.h>
#include <signal.h>

#include "event_queue.h"
#include "inotify_utils.h"
int keep_running;

/* This program will take as arguments one or more directory 
   or file names, and monitor them, printing event notifications 
   to the console. It will automatically terminate if all watched
   items are deleted or unmounted. Use ctrl-C or kill to 
   terminate otherwise.
*/

/* Signal handler that simply resets a flag to cause termination */
void signal_handler (int signum)
{
  keep_running = 0;
}

int main (int argc, char **argv)
{
  /* This is the file descriptor for the inotify watch */
  int inotify_fd;

  keep_running = 1;

  /* Set a ctrl-c signal handler */
  if (signal (SIGINT, signal_handler) == SIG_IGN)
    {
      /* Reset to SIG_IGN (ignore) if that was the prior state */
      signal (SIGINT, SIG_IGN);
    }

  /* First we open the inotify dev entry */
  inotify_fd = open_inotify_fd ();
  if (inotify_fd > 0)
    {

      /* We will need a place to enqueue inotify events,
         this is needed because if you do not read events
         fast enough, you will miss them. This queue is 
         probably too small if you are monitoring something
         like a directory with a lot of files and the directory 
         is deleted.
       */
      queue_t q;
      q = queue_create (128);

      /* This is the watch descriptor returned for each item we are 
         watching. A real application might keep these for some use 
         in the application. This sample only makes sure that none of
         the watch descriptors is less than 0.
       */
      int wd;


      /* Watch all events (IN_ALL_EVENTS) for the directories and 
         files passed in as arguments.
         Read the article for why you might want to alter this for 
         more efficient inotify use in your app.      
       */
      int index;
      wd = 0;
      printf("\n");
      for (index = 1; (index < argc) && (wd >= 0); index++) 
	{
	  wd = watch_dir (inotify_fd, argv[index], IN_ALL_EVENTS);
	  /*wd = watch_dir (inotify_fd, argv[index], IN_ALL_EVENTS & ~(IN_CLOSE | IN_OPEN) ); */
	}

      if (wd > 0) 
	{
	  /* Wait for events and process them until a 
             termination condition is detected
           */
	  process_inotify_events (q, inotify_fd);
	}
      printf ("\nTerminating\n");

      /* Finish up by closing the fd, destroying the queue,
         and returning a proper code
       */
      close_inotify_fd (inotify_fd);
      queue_destroy (q);
    }
  return 0;
}

