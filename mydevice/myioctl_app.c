#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <unistd.h>

#define MYIOCTL_MAGIC 'k'
#define MYIOCTL_RESET _IO(MYIOCTL_MAGIC, 0)
#define MYIOCTL_GET_COUNT _IOR(MYIOCTL_MAGIC, 1, int)
#define MYIOCTL_INCREMENT _IOW(MYIOCTL_MAGIC, 2, int)
#define MAX_LINE_LEN 100

int main(){
    int fd = open("/dev/myioctl", O_RDWR);

    if(fd == -1){
        perror("Error opening myioctl device");
        return -1;
    }

    /*
    FILE *log_fp = fopen("log.txt", "r+") ;
    bool log_empty = false;

    if(log_fp != NULL){
        // Check whether log file is empty or not
        fseek(log_fp, 0, SEEK_END);
        if(ftell(log_fp) == 0) log_empty = true;
    }
    else{
        log_fp = fopen("log.txt", "w") ;
        log_empty = true;
    }
    
    // Read in last count value
    if(!log_empty){
        int add_count;
        int current_count;

        fseek(log_fp, 0, SEEK_SET);
        fscanf(log_fp, "%d", &add_count);
        ioctl(fd, MYIOCTL_GET_COUNT, &current_count);

        if(current_count == 0 && current_count != add_count){
            ioctl(fd, MYIOCTL_INCREMENT, &add_count);
        }
    }
    */

    // Example: Get the current counter value
    int counter_value;
    ioctl(fd, MYIOCTL_GET_COUNT, &counter_value);
    printf("Current counter value: %d\n", counter_value);

    // Example: Increment the counter by user input
    int increment_value;
    printf("Enter the value to increment the counter: ");
    if(scanf("%d", &increment_value) != 1){
        perror("Error redaing input");
        close(fd);
        //close(log_fp);
        return -1;
    } 
    
    ioctl(fd, MYIOCTL_INCREMENT, &increment_value);

    // Example: Get the updated counter value
    ioctl(fd, MYIOCTL_GET_COUNT, &counter_value);
    printf("Updated counter value: %d\n", counter_value);

    /*
    // Write the current count value back to log file
    fseek(log_fp, 0, SEEK_SET);
    fprintf(log_fp, "%d\n", counter_value);

    close(log_fp); */
    close(fd);

    return 0;
}
