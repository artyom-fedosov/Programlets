#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <time.h>


extern char *program_invocation_short_name;


void
handle_parameters (int argc, char *argv[]);

void
print_help (void);

void
print_inotify_event (struct inotify_event *ie);

void
start_listening (int argc, char *argv[]);


int
main (int argc, char *argv[])
{
    handle_parameters (argc, argv);
    start_listening (argc, argv);
}


void
handle_parameters (int argc, char *argv[])
{
    if (argc == 1)
      {
        print_help ();
        exit (EXIT_FAILURE);
      }
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h')
      {
        print_help ();
        exit (EXIT_FAILURE);
      }

    for (int i = 1; i < argc; ++i)
      {
        if (access(argv[i], R_OK) == -1)
          {
            perror("access");
            exit(EXIT_FAILURE);
          }
      }
}

void
print_help (void)
{
#define prog program_invocation_short_name
    printf ("usage: %s -h\n"
            "       %s [FILE]...\n",
            prog, prog);
#undef prog
}

void
print_inotify_event (struct inotify_event *ie)
{
    struct timeval time;
    char *time_str;

    if (gettimeofday (&time, NULL) == -1)
      {
        perror ("gettimeofday");
        exit (EXIT_FAILURE);
      }

    time_str = ctime (&time.tv_sec);
    if (time_str == NULL)
      {
        perror ("ctime");
        exit (EXIT_FAILURE);
      }

    printf("%s%s: ", time_str, ie->name);
    if (ie->mask & IN_ACCESS)
        printf ("File was accessed\n\n");
    if (ie->mask & IN_ATTRIB)
        printf ("File metadata changed\n\n");
    if (ie->mask & IN_CLOSE_WRITE)
        printf ("File opened for writing was closed\n\n");
    if (ie->mask & IN_CLOSE_NOWRITE)
        printf ("File opened read-only was closed\n\n");
    if (ie->mask & IN_CREATE)
        printf ("File/directory created inside watched directory\n\n");
    if (ie->mask & IN_DELETE)
        printf ("File/directory deleted from within watched directory\n\n");
    if (ie->mask & IN_DELETE_SELF)
        printf ("Watched file/directory was itself deleted\n\n");
    if (ie->mask & IN_MODIFY)
        printf ("File was modified\n\n");
    if (ie->mask & IN_MOVE_SELF)
        printf ("Watched file/directory waws itself moved\n\n");
    if (ie->mask & IN_MOVED_FROM)
        printf ("File moved out of watched directory\n\n");
    if (ie->mask & IN_MOVED_TO)
        printf ("File moved into watched directory\n\n");
    if (ie->mask & IN_OPEN)
        printf ("File was opened\n\n");
}

void
start_listening (int argc, char *argv[])
{
#define BUFFER_LENGTH 4096
    char buffer[BUFFER_LENGTH];
    int inotify_fd, wd;
    ssize_t read_bytes;
    char *ptr;
    struct inotify_event *ie;

    inotify_fd = inotify_init ();
    if (inotify_fd == -1)
      {
        perror ("inotify_init");
        exit (EXIT_FAILURE);
      }

    for (int i = 1; i < argc; ++i)
      {
        wd = inotify_add_watch (inotify_fd, argv[i], IN_ALL_EVENTS);
        if (wd == -1)
          {
            perror ("inotify_add_watch");
            exit (EXIT_FAILURE);
          }
      }

    while (1)
      {
        read_bytes = read (inotify_fd, buffer, BUFFER_LENGTH);
        if (read_bytes == -1)
          {
            perror ("read");
            exit (EXIT_FAILURE);
          }

        for (ptr = buffer; ptr < buffer + read_bytes; )
          {
            ie = (struct inotify_event *)ptr;
            print_inotify_event (ie);
            ptr += sizeof (struct inotify_event) + ie->len;
          }
      }
#undef BUFFER_LENGTH
}
