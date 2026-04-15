/* This program executes specific command in specific intervals.
   The program is written according to the GNU Coding Standard. */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>


long seconds = 0;
char *command = NULL;
extern char *program_invocation_short_name;


void
handle_options (int argc, char *argv[]);

void
print_help (void);

void
execute_shell_command (void);


int
main (int argc, char *argv[])
{
    handle_options (argc, argv);

    while (1)
      {
        execute_shell_command ();
        sleep (seconds);
      }
}


void
handle_options (int argc, char *argv[])
{
    int option;
    char *endptr;
    while ( (option = getopt (argc, argv, "hi:c:") ) != -1)
      {
        switch (option)
          {
            case 'h':
                print_help ();
                break;
            case 'i':
                errno = 0;
                seconds = strtol (optarg, &endptr, 10);
                if (errno != 0)
                  {
                    perror ("strtol");
                    exit (EXIT_FAILURE);
                  }
                if (endptr == optarg || *endptr != '\0')
                  {
                    fprintf (stderr, "Invalid number: %s\n", optarg);
                    exit (EXIT_FAILURE);
                  }
                if (seconds <= 0)
                  {
                    fprintf (stderr, "Number must be greater than 0\n");
                    exit (EXIT_FAILURE);
                  }
                break;
            case 'c':
                command = optarg;
                break;
            default:
                break;
          }
      }
}

void
print_help (void)
{
#define prog program_invocation_short_name
    printf ("usage: %s -h\n"
            "       %s -i num -c comnd\n\n"
            "    -h                help message\n"
            "    -i seconds        interval duration in seconds\n"
            "    -c command        command to be executed\n",
            prog, prog);
#undef prog
}

void
execute_shell_command (void)
{
    if (system (command) == -1)
      {
        perror ("system");
        exit (EXIT_FAILURE);
      }
}
