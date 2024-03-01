#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include "sensorSetup.h"

/* Request a line as input. */
struct gpiod_line_request* request_input_lines(const char *chip_path, const unsigned int *offsets, unsigned int num_lines, const char *consumer)
{
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *line_cfg;
	struct gpiod_chip *chip;
	unsigned int i;
	int ret;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_chip;

	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
	gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_RISING);
	/* Assume a button connecting the pin to ground, so pull it up... */
	gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
	/* ... and provide some debounce. */
	gpiod_line_settings_set_debounce_period_us(settings, 10000);

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;

	for (i = 0; i < num_lines; i++) {
		ret = gpiod_line_config_add_line_settings(line_cfg, &offsets[i],
							  1, settings);
		if (ret)
			goto free_line_config;
	}

	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

free_settings:
	gpiod_line_settings_free(settings);

close_chip:
	gpiod_chip_close(chip);

	return request;
}

static struct gpiod_line_request *request_input_line(struct gpiod_chip* chip,
                                            unsigned int offset,
                                            const char *consumer)
{
    struct gpiod_request_config *req_cfg = NULL;
    struct gpiod_line_request *request = NULL;
    struct gpiod_line_settings *settings;
    struct gpiod_line_config *line_cfg;
    int ret;

    settings = gpiod_line_settings_new();
    if (!settings)
    {
        printf("failed settings");
        goto free_settings;
    }

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_BOTH);
    gpiod_line_settings_set_debounce_period_us(settings, 10000);
    
    line_cfg = gpiod_line_config_new();
    if (!line_cfg)
    {
        printf("failed line config");
        goto free_line_config;
    }

    ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);

    if (ret)
    {
        printf("failed add settings");
        goto free_line_config;
    }

    if (consumer) 
    {
        req_cfg = gpiod_request_config_new();
        if (!req_cfg)
        {
            printf("failed request config");
            goto free_line_config;
        }
        gpiod_request_config_set_consumer(req_cfg, consumer);
    }
    
    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    gpiod_request_config_free(req_cfg);
free_line_config:
    gpiod_line_config_free(line_cfg);
free_settings:
    gpiod_line_settings_free(settings);

    return request;
}


int setupIRsensor(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *event_buffer, int line_offset)
{
    static const char* const chip_path = "/dev/gpiochip0";
    
    struct gpiod_chip_info *info;
    struct gpiod_chip *chip;

    //static const unsigned int line_offset = 17;

    //struct gpiod_edge_event_buffer *event_buffer;
    //struct gpiod_line_request *request;
    //struct gpiod_good_edge_event *event;
    int i, ret, event_buf_size;


    chip = gpiod_chip_open(chip_path);
    if (!chip)
    {
        printf("fail\n");
        return 1;
    }
    info = gpiod_chip_get_info(chip);
    if (!info)
    {
        printf("fail info\n");
    }

    //printf("%s, [%s] (%zu lines)\n", gpiod_chip_info_get_name(info),
    //        gpiod_chip_info_get_label(info),
    //        gpiod_chip_info_get_num_lines(info));
    request = request_input_line(chip, line_offset, "watch-line-value");
    if (!request)
    {
        printf("failed request");
        goto fail;
    }

    event_buf_size = 4;
    event_buffer = gpiod_edge_event_buffer_new(event_buf_size);
    if (!event_buffer)
    {
        printf("failed event buffer");
        goto fail;
    }
    
    //while(1)
    //{
    //    ret = gpiod_line_request_read_edge_events(request, event_buffer, event_buf_size);
    //    if (ret == -1)
    //    {
    //        printf("error reading event");
    //        goto fail;
    //    }
    //    for (i = 0; i < ret; i++)
    //    {
    //        event = gpiod_edge_event_buffer_get_event(event_buffer, i);
    //        switch (gpiod_edge_event_get_event_type(event))
    //        {
    //            case GPIOD_EDGE_EVENT_RISING_EDGE:
    //                printf("rising\n");
    //                break;
    //            case GPIOD_EDGE_EVENT_FALLING_EDGE:
    //                printf("falling\n");
    //                break;
    //            default:
    //                printf("?");
    //        }
    //    }
    //}
fail:
    gpiod_chip_info_free(info);
    gpiod_chip_close(chip);
    return 0;
}
