#ifndef SENSORSETUP_H
#define SENSORSETUP_H

int setupIRsensor(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *event_buffer, int line_offset);

struct gpiod_line_request* request_input_lines(const char *chip_path, const unsigned int *offsets, unsigned int num_lines, const char *consumer);
#endif
