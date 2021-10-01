#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main() {
    int pid = getpid();
    int ppid = getppid();

    printf("My PID: %d\n", pid);
    printf("Shell PID: %d\n", ppid);

    char fileBuf[255];
    snprintf(fileBuf, sizeof(fileBuf), "/proc/%d/task/%d/children", ppid, ppid);

    FILE* tgFile = fopen(fileBuf, "r");
    if (tgFile != NULL) {
        int childPID;
        fscanf(tgFile, "%d", &childPID);
        while (!feof(tgFile) && !ferror(tgFile)) {
            if (childPID != pid) {
                printf("Killing Child: %d", childPID);
                kill(childPID, SIGKILL);
            }
            fscanf(tgFile, "%d", &childPID);
        }
        fclose(tgFile);
    }

    return 0;
}
