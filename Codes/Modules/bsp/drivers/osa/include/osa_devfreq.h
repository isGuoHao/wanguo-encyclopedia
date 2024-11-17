#ifndef __OSA_DEVFREQ_H__
#define __OSA_DEVFREQ_H__

/**
 * @file osa_devfreq.h
 * @brief Device Frequency Management Structures and Definitions
 * @details This header file defines structures and types used for managing device frequencies.
 */

#include <linux/types.h>

/**
 * @struct osa_devfreq_dev_status
 * @brief Device frequency status structure
 * @details This structure holds the status information of a device's frequency management.
 */
typedef struct osa_devfreq_dev_status {
    unsigned long total_time;        /**< Total time in microseconds since the last measurement */
    unsigned long busy_time;         /**< Busy time in microseconds since the last measurement */
    unsigned long current_frequency; /**< Current frequency of the device in Hz */
    void *private_data;              /**< Private data pointer for driver-specific use */
} osa_devfreq_dev_status_t;

/**
 * @struct osa_devfreq_para
 * @brief Device frequency parameters structure
 * @details This structure holds the configuration parameters for device frequency management.
 */
typedef struct osa_devfreq_para {
    unsigned long initial_freq;      /**< Initial frequency of the device in Hz */
    unsigned int polling_ms;         /**< Polling interval in milliseconds */
    unsigned long *freq_table;       /**< Array of available frequencies in Hz */
    unsigned int max_state;          /**< Number of available frequency states */
} osa_devfreq_para_t;

#endif /* __OSA_DEVFREQ_H__ */
