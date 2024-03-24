#ifndef MESSAGES_H
#define MESSAGES_H

enum message_types_t {
    REQUEST_BUTTON_STATE = 0,
    SEND_BUTTON_STATE,
    CONTROL_SERVO_1,
    CONTROL_SERVO_2,
    CONTROL_DC_1,
    CONTROL_DC_2,
    SEND_CAN_SEND,
    RESPOND_CAN_SEND,
    REQUEST_CONTROL,
    SEND_CONTROL
} typedef messages_t;

enum {
    NONE = 0,
    FORWARD,
    BACKWARD
} typedef control_types_t
#endif
