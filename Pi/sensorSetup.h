#ifndef SENSORSETUP_H
#define SENSORSETUP_H
#include <gpiod.h>
#include <cerrno>

int setupIRsensor(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *event_buffer, int line_offset);

struct gpiod_line_request* request_input_lines(const char *chip_path, const unsigned int *offsets, unsigned int num_lines, const char *consumer, int debounce);

int gpiod_edge_event_buffer_read_fd(int fd, struct gpiod_edge_event_buffer *buffer, size_t max_events);
#endif
