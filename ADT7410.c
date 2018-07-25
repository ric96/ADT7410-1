#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

int main()
{
    int file;
    int adapter_nr = 1;
    int addr = 0x48;
    char filename[50];

    snprintf(filename, sizeof(filename)-1, "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0) {
        perror("Cannot open i2c device");
        return 1;
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Cannot set slave address");
        close(file);
        return 1;
    }

    uint8_t reg = 0;
    char buf[256];
    ssize_t length;

    while(1)
   {

        if (write(file, &reg, 1) < 0) {
            perror("Cannot write to i2c device");
            close(file);
            return 0;
        }

        if ((length = read(file, buf, 4)) < 0) {
            perror("Cannot read from i2c device");
            close(file);
            return 0;
        }

        printf("length: %d\n", length);

        ssize_t i;
        for (i = 0; i < length; i++) {
            printf("[%02x] %02x\n", i, buf[i]);
        }

        if ((buf[2] & 0x80) == 0) {
            fprintf(stderr, "Not ready.\n");
            close(file);
            return 0;
        }
        int value = (buf[0] << 8) | buf[1];
        printf("%lf\n", ((double)value)/16/8);
	sleep(1);
    }

return 0;
}
