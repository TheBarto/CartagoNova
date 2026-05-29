#ifndef _EN_DEFINES_INCLUDE_H_
#define _EN_DEFINES_INCLUDE_H_

#include <stdint.h>
#include <queue.h>
#include <stack.h>

#include <stddef.h>

/* Obtenemos el offset del elemento dentro del struct. Con este valor restamos
 * a la posicion de memoria del struct, y obtenemos el primer elemento del struct.
*/
#define get_struct_by_elem(ptr_str, str_id, str_elem_name)                     \
        (((uintptr_t)ptr_str) - offsetof(str_id, str_elem_name))

#define MAX_TOTAL_ELECTROVALS ((uint8_t) 4)
#define MAX_SIZE_PIN_NAME     ((uint8_t) 7)
#define MAX_NUMBER_NODES      ((uint8_t) 15)
#define MAX_SIZE_FOLDER_NAME  ((uint8_t) 30)

typedef struct {
	uint16_t stimulus;
	uint8_t sensor_heat;
	uint8_t motor_suction;
	uint8_t total_vals;
	uint8_t vals[MAX_TOTAL_ELECTROVALS];
	node_b_t node;
} capture_conf_t;

typedef struct {
	uint16_t res_val;
	uint16_t board_limit_volt;
	uint16_t n_subsamples_capt_time;
	uint8_t n_subsamples;
	uint8_t sensor_volt;
	uint8_t TH_capture_time;
	uint8_t sensor_pin[MAX_SIZE_PIN_NAME];
	uint8_t TH_pin[MAX_SIZE_PIN_NAME];
	uint8_t motor_pin[MAX_SIZE_PIN_NAME];
	uint8_t folder[MAX_SIZE_FOLDER_NAME];
	uint8_t electrovals_pins[MAX_TOTAL_ELECTROVALS][MAX_SIZE_PIN_NAME];
	uint8_t total_nodes_read;
	queue_b_t capture_config_vals;
	stack_b_t free_nodes;
	capture_conf_t nodes[MAX_NUMBER_NODES];
} board_conf_t;

#endif // _EN_DEFINES_INCLUDE_H_
