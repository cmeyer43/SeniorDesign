#ifndef MESSAGES_H
#define MESSAGES_H

enum message_types_t {
    REQUEST_BUTTON_STATE = 0,
    SEND_BUTTON_STATE,
    CONTROL_SERVO_1,
    CONTROL_SERVO_2,
    CONTROL_DC_1,
    CONTROL_DC_2,
    REQUEST_CAN_SEND,
    SEND_CAN_SEND
} typedef messages_t;

#endif
