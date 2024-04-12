#ifndef LINE_EVENT_H
#define LINE_EVENT_H
int gpiod_edge_event_buffer_read_fd(int fd,
        struct gpiod_edge_event_buffer *buffer,
        size_t max_events);

#endif
