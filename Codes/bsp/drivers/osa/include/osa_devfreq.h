/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#ifndef __OSAL_DEVFREQ__H
#define __OSAL_DEVFREQ__H

typedef struct osa_devfreq_dev_status {
    /* both since the last measure */
    unsigned long total_time;
    unsigned long busy_time;
    unsigned long current_frequency;
    void *private_data;
} osa_devfreq_dev_status_t;

typedef struct osa_devfreq_para {
    unsigned long initial_freq;
    unsigned int polling_ms;
    unsigned long *freq_table;
    unsigned int max_state;
} osa_devfreq_para_t;

#endif /* __OSAL_DEVFREQ__H */
