#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/sysctl.h>
#include <unistd.h>

double get_process_runtime(const char *process_name) {
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t miblen = sizeof(mib) / sizeof(mib[0]);

    size_t size;
    sysctl(mib, miblen, NULL, &size, NULL, 0);

    struct kinfo_proc *procs = (struct kinfo_proc *)malloc(size);
    sysctl(mib, miblen, procs, &size, NULL, 0);

    int count = size / sizeof(struct kinfo_proc);
    double runtime = 0.0;

    for (int i = 0; i < count; i++) {
        if (strcmp(procs[i].kp_proc.p_comm, process_name) == 0) {
            struct timeval start_time = procs[i].kp_proc.p_starttime;
            struct timeval now;
            gettimeofday(&now, NULL);

            runtime = difftime(now.tv_sec, start_time.tv_sec) +
                      ((double)(now.tv_usec - start_time.tv_usec) / 1000000.0);

            break;
        }
    }

    free(procs);
    return runtime;
}

int main() {
    char program_name[256];  // Ändere dies in den Namen des Prozesses, den du überwachen möchtest
    printf("Programm Name: ");
    scanf("%255[^\t\n]", program_name);

    double runtime = get_process_runtime(program_name);

    int hours = (int)(runtime / 3600);
    int minutes = (int)((runtime - hours * 3600) / 60);
    int seconds = (int)(runtime - hours * 3600 - minutes * 60);

    printf("%s läuft seit %d Stunden, %d Minuten und %d Sekunden.\n", program_name, hours, minutes, seconds);

    return 0;
}
