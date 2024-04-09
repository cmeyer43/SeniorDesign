#ifndef MESSAGES_H
#define MESSAGES_H

enum message_types_t {
    REQUEST_BUTTON_STATE = 0,
    SEND_BUTTON_STATE, // 1
    CONTROL_SERVO_1,   // 2
    CONTROL_SERVO_2,   // 3
    CONTROL_DC_1,      // 4
    CONTROL_DC_2,      // 5
    SEND_CAN_SEND,     // 6
    RESPOND_CAN_SEND,  // 7
    REQUEST_CONTROL,   // 8
    SEND_CONTROL       // 9
} typedef messages_t;

enum {
    NONE = 0,
    FORWARD,
    BACKWARD
} typedef control_types_t;
#endif
