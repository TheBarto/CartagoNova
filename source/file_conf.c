#include <file_conf.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

static void parse_value(void *dst, uint8_t size_dst, uint8_t *src, uint8_t size_src)
{
	uint32_t dst_val = 0;
	for(uint8_t i = 0; i < size_src; i++) {
		dst_val*=10;
		dst_val+=src[i]-'0';
	}
	switch(size_dst) {
		case 1:
			*((uint8_t *)dst) = (uint8_t)dst_val;
			break;
		case 2:
			*((uint16_t *)dst) = (uint16_t)dst_val;
			break;
		case 4:
			*((uint32_t *)dst) = (uint32_t)dst_val;
			break;
		default:
			printf("ERROR 3");
			break;
	}
	return;
}

// USE THIS FUNCTION FOR BOTH TRANSFORMATIONS
static uint8_t parse_electrovals_pins(uint8_t *pins, uint8_t *stream, uint8_t parse)
{
	uint8_t* ptr_str = stream;
	uint8_t total_pins = 0;
	while(*ptr_str != '\n') {
		if(*ptr_str == ',') {
			if(parse == 0) {
				memcpy(pins, stream, sizeof(*stream)*(ptr_str-stream));
				pins+=(sizeof(*stream) * MAX_SIZE_PIN_NAME);
			} else {
				*pins = *stream-'0';
				pins++;
			}
			stream = (ptr_str + 1);
			total_pins++;
		}
		ptr_str++;
	}
	if(parse == 0)
		memcpy(pins, stream, sizeof(*stream)*((ptr_str-1)-stream));
	else
		*pins = *stream-'0';

	return total_pins;
}

static int analize_board_conf(uint8_t *stream, uint32_t size, board_conf_t *board)
{
	stream++;
	uint8_t *aux_ptr = stream;
	uint8_t pos = 0;
	while(*aux_ptr != '\n') {
		if(*aux_ptr == ';') {
			switch(pos) {
				case 0: parse_value(&board->res_val, sizeof(board->res_val), stream, (aux_ptr-stream)); break;
				case 1: parse_value(&board->n_subsamples, sizeof(board->n_subsamples), stream, (aux_ptr-stream)); break;
				case 2: parse_value(&board->n_subsamples_capt_time, sizeof(board->n_subsamples_capt_time), stream, (aux_ptr-stream)); break;
				case 3: memcpy(&board->sensor_pin, stream, sizeof(*stream)*(aux_ptr-stream)); break;
				case 4: parse_value(&board->sensor_volt, sizeof(board->sensor_volt), stream, (aux_ptr-stream)); break;
				case 5: parse_value(&board->board_limit_volt, sizeof(board->board_limit_volt), stream, (aux_ptr-stream)); break;
				case 6: memcpy(&board->TH_pin, stream, sizeof(*stream)*(aux_ptr-stream)); break;
				case 7: memcpy(&board->motor_pin, stream, sizeof(*stream)*(aux_ptr-stream)); break;
				case 8: parse_value(&board->TH_capture_time, sizeof(board->TH_capture_time), stream, (aux_ptr-stream)); break;
				case 9: memcpy(&board->folder, stream, sizeof(*stream)*(aux_ptr-stream)); break;
				case 10: board->total_pins_read = parse_electrovals_pins(&board->electrovals_pins[0][0], stream, 0); break;
				default: printf("ERROR 1\n"); return -1;
			}
			pos++;
			stream = (aux_ptr + 1);
		}
		aux_ptr++;
	}

	printf("VALORES OBTENIDOS CONFIGURACION: \n");
	printf("VALOR RESISTENCIA: %d\nN_SUBMUESTRAS: %d\n", board->res_val, board->n_subsamples);
	printf("TIEMPO CAPTURA SUBMUESTRAS: %d\nPIN SENSOR: %s\n", board->n_subsamples_capt_time, board->sensor_pin);
	printf("VOLTAJE SENSOR: %d\nLIMITE VOLTAJE PLACA: %d\n", board->sensor_volt, board->board_limit_volt);
	printf("PIN TyH: %s\nPIN MOTOR: %s\n", board->TH_pin, board->motor_pin);
	printf("TIEMPO CAPTURA TyH: %d\nCARPETA: %s\n", board->TH_capture_time, board->folder);
	for(uint8_t i = 0; i < board->total_pins_read; i++)
		printf("PIN VALVULAS[%d] = %s\n", i, board->electrovals_pins[i]);
	return 0;
}

static int analize_capture_conf(uint8_t *stream, uint32_t size, board_conf_t *board)
{
	stream++;
	uint8_t *aux_ptr = stream;
	uint8_t pos = 0;
	node_b_t *node_b = NULL;
	stack_b_unstack_value(&board->free_nodes, &node_b);
	capture_conf_t *node = (capture_conf_t *)get_struct_by_elem(node_b, capture_conf_t, node);
	while(*aux_ptr != '\n') {
		if(*aux_ptr == ';') {
			switch(pos) {
				case 0: parse_value(&node->stimulus, sizeof(node->stimulus), stream, (aux_ptr-stream)); break;
				case 1: parse_value(&node->sensor_heat, sizeof(node->sensor_heat), stream, (aux_ptr-stream)); break;
				case 2: parse_value(&node->motor_suction, sizeof(node->motor_suction), stream, (aux_ptr-stream)); break;
				case 3: parse_value(&node->total_vals, sizeof(node->total_vals), stream, (aux_ptr-stream)); break;
				case 4: parse_electrovals_pins(node->vals, stream, 1); break;
				default: printf("ERROR 2\n"); return -1;
			}
			pos++;
			stream = (aux_ptr + 1);
		}
		aux_ptr++;
	}

	queue_b_queue_value(&board->capture_config_vals, node_b);
	printf("VALORES OBTENIDOS: \n");
	printf("ESTIMULO: %d\nTEMPERATURA SENSOR(%): %d\n", node->stimulus, node->sensor_heat);
	printf("SUCCION MOTOR: %d\nTOTAL ELECTROVALVULAS: %d\n", node->motor_suction, node->total_vals);
	for(uint8_t i = 0; i < node->total_vals; i++)
		printf("VALVULA[%d] = %d\n", i, node->vals[i]);
	return 0;
}


int8_t read_config_file(uint8_t *name, board_conf_t *board)
{
	int16_t data_r = 0;
	uint8_t data[512];
	uint8_t *ptr_init = data;
	uint8_t *ptr_end = data;
	int32_t fd = open(name, (O_RDONLY));
	if(fd == -1)
		return -1;

	memset(board, 0, sizeof(board));
	do {
		data_r = read(fd, &data[0], sizeof(uint8_t)*512);
		// SI ERROR HAY QUE CERRAR TODO
		if(data_r == -1)
			return -1;
		printf("DATA: %s\n", data);
		printf("DATA_R: %d\n", data_r);
		while(*ptr_end != 0) {
			while((*ptr_end != '+') && (*ptr_end != '-'))
				ptr_end++;

			ptr_init = ptr_end;
			while((*ptr_end != '\n') && (*ptr_end != 0)) {
				if(*ptr_end == 0) {
					close(fd);
					return 0;
				}
				ptr_end++;
			}

			if(*ptr_init == '+')
				analize_board_conf(ptr_init, (ptr_end-ptr_init), board);
			else
				analize_capture_conf(ptr_init, (ptr_end-ptr_init), board);
			ptr_end++;
			ptr_init = ptr_end;
			// RESTAR LOS VALORES PARA RESTAR Y LLEGAR A 0
		}
	} while(data_r > 0);
	close(fd);
	return 0;
}

