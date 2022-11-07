#include <cstdio>

#include "mbed.h"

Thread thread_master;
Thread thread_slave;

I2C m_i2c(D14, D15);
I2CSlave i2c_slave(A4, A5);
char slave_address = 0xA0;
char buf[10];
char mode = 0;
char msg[2] = {0x00, 0x00};
char value = 0;

void slave() {
    i2c_slave.address(slave_address);
    while (1) {
        int receive_code = i2c_slave.receive();
        switch (receive_code) {
            case I2CSlave::ReadAddressed:
                if (mode == 1) {  // mode!=0
                    value = value + 1;
                    msg[0] = mode;
                    msg[1] = value;
                    printf("Slave Write: mode=%d, value=%d\n", mode, value);
                    i2c_slave.write(msg, 2);
                } else if (mode == 2) {
                    value = value + 2;
                    msg[0] = mode;
                    msg[1] = value;
                    printf("Slave Write: mode=%d, value=%d\n", mode, value);
                    i2c_slave.write(msg, 2);
                } else {
                    msg[0] = mode;
                    msg[1] = value;
                    printf("Slave Write: mode=%d, value=%d\n", mode, value);
                    i2c_slave.write(msg, 2);
                }
                break;
            case I2CSlave::WriteGeneral:
                i2c_slave.read(buf, 2);
                mode = buf[0];
                value = buf[1];
                printf("Slave Read General: mode=%d, value=%d\n", mode, value);
                break;
            case I2CSlave::WriteAddressed:
                i2c_slave.read(buf, 2);
                mode = buf[0];
                value = buf[1];
                printf("Slave Read Addressed: mode=%d, value=%d\n", mode,
                       value);
                break;
        }
        for (int i = 0; i < 10; i++) {
            buf[i] = 0;  // Clear buffer
        }
    }
}

void master() {
    char mode = 0x00;
    char value = 0x01;
    char reg[2] = {0x00, 0x01};  // reg[0]: mode, reg[1]: value
    char ans[2] = {1, 1};

    for (int i = 0; i < 5; ++i, reg[0]++) {  // Run for 5 times
        printf("\n\nNew chapter starts from here.\n");
        m_i2c.write(slave_address, reg, 2);
        ThisThread::sleep_for(100ms);
        m_i2c.read(slave_address, ans, 2);
        printf("The ans from slave mode=%d, value=%d\n", ans[0], ans[1]);
    }
}

int main() {
    thread_master.start(master);
    thread_slave.start(slave);
}