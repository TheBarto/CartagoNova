#include <stdint.h>
#include <string.h>
#include <file_conf.h>
#include <EN_defines.h>

#include <stdio.h>




void captura_muestra(capture_conf_t *capt, uint8_t *pin_sensor, uint8_t tsub, uint8_t nss)
{
	close_all_electrovals();
	open_electrovals(capt->vals, capt->tota_vals);

	for(uint8_t i = 0; i < capt->n_stimulus; i++) {
		//time_ini
		//adc_read(pin_sensor);
		for(uint8_t j = 0; j < nss; j++) {
			//adc_read(pin_sensor);
			//time_sleep(tsub);
		}
		//time_end
		//sleep(time_end-time_ini);
	}

	return;
}



int main(int argc, char *argv[])
{
	board_conf_t b;

	printf("Comenzamos main\n");

	stack_b_init_stack(&b.free_nodes);
	queue_b_init_queue(&b.capture_config_vals);
	for(uint8_t i = 0; i < MAX_NUMBER_NODES; i++) {
		memset(&b.nodes[i], 0, sizeof(capture_conf_t));
		stack_b_stack_value(&b.free_nodes, &b.nodes[i].node);
	}

	int8_t r = read_config_file("file_test.txt", &b);

	node_b_t *node_b = NULL;
	for(uint8_t i = 0; i < 7; i++) {
		list_get_b_node(&b.capture_config_vals,
		                i,
		                &node_b,
		                0);

		printf(" VALORES OBTENIDOS DESPUES ");
		capture_conf_t *node = (capture_conf_t *)get_struct_by_elem(node_b, capture_conf_t, node);

		printf("VALORES OBTENIDOS: \n");
		printf("-------------------------\n");
		printf("ESTIMULO: %d\nTEMPERATURA SENSOR(%): %d\n", node->stimulus, node->sensor_heat);
		printf("SUCCION MOTOR: %d\nTOTAL ELECTROVALVULAS: %d\n", node->motor_suction, node->total_vals);
		for(uint8_t i = 0; i < node->total_vals; i++)
			printf("VALVULA[%d] = %d\n", i, node->vals[i]);
	}

	// Iniciamos las electrovalvulas.
	//GPIO.setup(elec, GPIO.OUT);
	for (uint8_t i = 0; i < MAX_TOTAL_ELECTROVALS; i++) {
		gpio_setup(b.electrovalvulas[i], GPIO_Direction.Output, GPIO_Resistor.PullDown, 0, 0);
	}


	//PWM.start - Al iniciar la captura
	pwm_start_channel(b.motor_pin, 100.0, 2000.0, PWM_Polarity.Normal);

	//PWM.stop
	pwm_stop_channel(b.motor_pin);

	for(uint8_t i = 0; i < b->total_nodes_read; i++) {
		//queue_b_queue_get();
	}
}

