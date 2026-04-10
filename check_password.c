/* This program prompts user to enter username and password.
   It checks whether user credentials are correct.
   The program is written according to the GNU Coding Standard. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>
#include <errno.h>


void
printf_with_perror (const char *string, int string_length);
void
perror_and_exit_failure (const char *string);


int
main (void)
{
    char *username, *password, *encrypted;
    size_t username_length;
    long max_username_length;
    struct passwd *password_structure;
    struct spwd *shadow_password_structure;

    if (geteuid () != 0)
      {
        printf ("You are not root!\n");
        return EXIT_FAILURE;
      }

    max_username_length = sysconf (_SC_LOGIN_NAME_MAX);
    if (max_username_length == -1)
        perror_and_exit_failure ("sysconf");

    username = malloc (max_username_length);
    if (username == NULL)
        perror_and_exit_failure ("malloc");

    printf_with_perror ("Username: ", 10);

    if (fgets (username, max_username_length, stdin) == NULL)
        return EXIT_FAILURE;

    username_length = strlen (username);
    if (username[username_length - 1] == '\n')
        username[username_length - 1] = '\0';

    while (1)
      {
        errno = 0;
        password_structure = getpwnam (username);
        if (password_structure == NULL)
          {
            if (errno == EINTR)
                continue;
            perror_and_exit_failure ("getpwnam");
          }
        break;
      }

    shadow_password_structure = getspnam (username);
    if (shadow_password_structure == NULL && errno == EACCES)
      {
        printf ("No permission to read shadow password file\n");
        return EXIT_FAILURE;
      }

    if (shadow_password_structure != NULL)
        password_structure->pw_passwd = shadow_password_structure->sp_pwdp;

    password = getpass ("Password: ");

    encrypted = crypt (password, password_structure->pw_passwd);
    for (char *ptr = password; *ptr != '\0'; ++ptr)
        *ptr = '\0';
    if (encrypted == NULL)
        perror_and_exit_failure ("crypt");

    if (strcmp (encrypted, password_structure->pw_passwd) == 0)
      {
        printf ("Correct password\n");
        return EXIT_SUCCESS;
      }
    printf ("Incorrect password\n");
    return EXIT_FAILURE;
}


void
printf_with_perror (const char *string, int string_length)
{
    int printf_return_value;

    printf_return_value = printf (string);
    if (printf_return_value != string_length)
      {
        perror ("printf");
        exit (EXIT_FAILURE);
      }
}

void
perror_and_exit_failure (const char *string)
{
    perror (string);
    exit (EXIT_FAILURE);
}
