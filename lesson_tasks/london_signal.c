#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void user1_handler (int sign)
{
    printf ("London is");
    fflush (stdout);
}

void user2_handler (int sign)
{
    printf (" the capital of great Britain\n");
    fflush (stdout);
}

int main()
{
    printf ("pid = %d\n", getpid());

    struct sigaction old_action = {};
    struct sigaction user1_action, user2_action;
    user1_action.sa_handler = user1_handler;
    user2_action.sa_handler = user2_handler;

    sigaction (SIGUSR1, &user1_action, &old_action);
    pause();

    sigaction (SIGUSR2, &user2_action, &old_action);
    pause();

    return 0;
}