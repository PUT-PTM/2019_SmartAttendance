
#include "stm32f4xx_hal.h"
#include "my_rc522.h"

extern SPI_HandleTypeDef hspi1;

uint8_t spi_send_byte(uint8_t byte) {
    unsigned char cmd[1];
    unsigned char out[1];

    cmd[0] = byte;
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t * ) & cmd, (uint8_t * ) & out, 1, 10);

    return out[0];
}

void write_register(uint8_t addr, uint8_t val) {
    addr = (addr << 1) & 0x7E;

    cs_reset();
    spi_send_byte(addr);
    spi_send_byte(val);
    cs_set();
}

uint8_t read_register(uint8_t addr) {
    uint8_t val;
    addr = ((addr << 1) & 0x7E) | 0x80;

    cs_reset();
    spi_send_byte(addr);
    val = spi_send_byte(0x00);
    cs_set();

    return val;
}

uint8_t check_for_card(uint8_t *id) {
    uint8_t status;
    status = request(PICC_REQIDL, id);
    if (status == MI_OK) {
        status = anticoll(id);
    }
    halt();
    return status;
}

uint8_t compare_ID(uint8_t *id1, uint8_t *id2) {
    for (uint8_t i = 0; i < 4; i++) {
        if (id1[i] != id2[i])
            return MI_ERR;
    }
    return MI_OK;
}

void set_mask(uint8_t reg, uint8_t mask) {
    write_register(reg, read_register(reg) | mask);
}

void clear_mask(uint8_t reg, uint8_t mask) {
    write_register(reg, read_register(reg) & (~mask));
}

uint8_t request(uint8_t mode, uint8_t *tag) {
    uint8_t status;
    uint16_t return_bytes;

    write_register(MFRC522_REG_BIT_FRAMING, 0x07);
    tag[0] = mode;
    status = to_card(PCD_TRANSCEIVE, tag, 1, tag, &return_bytes);
    if ((status != MI_OK) || (return_bytes != 0x10))
        status = MI_ERR;

    return status;
}

uint8_t to_card(uint8_t cmd, uint8_t *data_send, uint8_t size_send, uint8_t *data_receive, uint16_t *size_receive) {
    uint8_t status = MI_ERR;
    uint8_t interrupt_err = 0x00;
    uint8_t wait_err = 0x00;
    uint8_t last_bits;
    uint8_t n;
    uint16_t i;

    switch (cmd) {
        case PCD_AUTHENT:
            interrupt_err = 0x12;
            wait_err = 0x10;
            break;
        case PCD_TRANSCEIVE:
            interrupt_err = 0x70;
            wait_err = 0x30;
            break;
        default:
            break;
    }

    write_register(MFRC522_REG_COMM_IE_N, interrupt_err | 0x80);
    clear_mask(MFRC522_REG_COMM_IRQ, 0x80);
    set_mask(MFRC522_REG_FIFO_LEVEL, 0x80);
    write_register(MFRC522_REG_COMMAND, PCD_IDLE);

    for (i = 0; i < size_send; i++) {
        write_register(MFRC522_REG_FIFO_DATA, data_send[i]);
    }

    write_register(MFRC522_REG_COMMAND, cmd);

    if (cmd == PCD_TRANSCEIVE) {
        set_mask(MFRC522_REG_BIT_FRAMING, 0x80);
    }

    i = 2000;
    do {
        n = read_register(MFRC522_REG_COMM_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & wait_err));

    clear_mask(MFRC522_REG_BIT_FRAMING, 0x80);

    if (i != 0) {
        if (!(read_register(MFRC522_REG_ERROR) & 0x1B)) {
            status = MI_OK;
            if (n & interrupt_err & 0x01) {
                status = MI_NOTAGERR;
            }
            if (cmd == PCD_TRANSCEIVE) {
                n = read_register(MFRC522_REG_FIFO_LEVEL);
                last_bits = read_register(MFRC522_REG_CONTROL) & 0x07;
                if (last_bits) {
                    *size_receive = (n - 1) * 8 + last_bits;
                }
                else {
                    *size_receive = n * 8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > MFRC522_MAX_LEN) {
                    n = MFRC522_MAX_LEN;
                }
                for (i = 0; i < n; i++) {
                    data_receive[i] = read_register(MFRC522_REG_FIFO_DATA);
                }
            }
        }
        else {
            status = MI_ERR;
        }
    }
    return status;
}

uint8_t anticoll(uint8_t *id) {
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint16_t unLen;

    write_register(MFRC522_REG_BIT_FRAMING, 0x00);
    id[0] = PICC_ANTICOLL;
    id[1] = 0x20;
    status = to_card(PCD_TRANSCEIVE, id, 2, id, &unLen);
    if (status == MI_OK) {
        for (i = 0; i < 4; i++) {
            serNumCheck ^= id[i];
        }
        if (serNumCheck != id[i]) {
            status = MI_ERR;
        }
    }

    return status;
}

void CRC_calc(uint8_t *in_data, uint8_t size, uint8_t *out_data) {
    uint8_t i;
    uint8_t n;

    clear_mask(MFRC522_REG_DIV_IRQ, 0x04);
    set_mask(MFRC522_REG_FIFO_LEVEL, 0x080);

    for (i = 0; i < size; i++) {
        write_register(MFRC522_REG_FIFO_DATA, *(in_data + i));
    }

    write_register(MFRC522_REG_COMMAND, PCD_CALCCRC);

    i = 0xFF;
    do {
        n = read_register(MFRC522_REG_DIV_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x04));

    out_data[0] = read_register(MFRC522_REG_CRC_RESULT_L);
    out_data[1] = read_register(MFRC522_REG_CRC_RESULT_M);
}

void init(void) {
    reset();
    write_register(MFRC522_REG_T_MODE, 0x8D);
    write_register(MFRC522_REG_T_PRESCALER, 0x3E);
    write_register(MFRC522_REG_T_RELOAD_L, 30);
    write_register(MFRC522_REG_T_RELOAD_H, 0);
    write_register(MFRC522_REG_RF_CFG, 0x70);
    write_register(MFRC522_REG_TX_AUTO, 0x40);
    write_register(MFRC522_REG_MODE, 0x3D);
    antenna_on();
}

void reset(void) {
    write_register(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

void antenna_on(void) {
    uint8_t temp;

    temp = read_register(MFRC522_REG_TX_CONTROL);
    if (!(temp & 0x03)) {
        set_mask(MFRC522_REG_TX_CONTROL, 0x03);
    }
}

void antenna_off(void) {
    clear_mask(MFRC522_REG_TX_CONTROL, 0x03);
}

void halt(void) {
    uint16_t unLen;
    uint8_t buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CRC_calc(buff, 2, &buff[2]);
    to_card(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}









	
