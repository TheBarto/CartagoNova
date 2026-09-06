#include <stdint.h>
#include <string.h>
#include <file_conf.h>
#include <EN_defines.h>

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <common.h>
#include <c_adc.h>
#include <event_gpio.h>
#include <c_pwm.h>

#include <queue.h>
#include <stack.h>

void captura_muestra(capture_conf_t *capt, uint8_t *pin_sensor, uint8_t tsub, uint8_t nss)
{
	printf("Cerramos todas las electrovalvulas\n");
	//close_all_electrovals();
	printf("Abrir las electrovalvulas que queramos\n");
	//open_electrovals(capt->vals, capt->tota_vals);

	time_t time1;
	float value_readed = 0;
	for(uint8_t i = 0; i < capt->stimulus; i++) {
		time(&time1);
		for(uint8_t j = 0; j < nss; j++) {
			adc_read_value(pin_sensor, &value_readed, 0);
			usleep(tsub);
		}
		usleep(difftime(time(NULL), time1));
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
	for(uint8_t i = 0; i < 4; i++) {
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
		/*gpio_setup(b.electrovals_pins[i],
		           Output,
		           PullDown,
		           0,
		           0);*/
		printf("Incializamos GPIO: %s\n", b.electrovals_pins[i]);
	}


	//PWM.start - Al iniciar la captura
	//pwm_start_channel(b.motor_pin, 100.0, 2000.0, Normal);
	printf("Iniciamos el PWM del motor\n");

	//PWM.stop
	//pwm_stop_channel(b.motor_pin);
	//printf("Paramos el PWM del motor\n");
	printf("PWM del sensor al 100% e iniciar o setear el ADC\n");

	for(uint8_t i = 0; i < b.total_nodes_read; i++) {
		queue_b_unqueue_value(&b.capture_config_vals, &node_b);
		node = (capture_conf_t *)get_struct_by_elem(node_b, capture_conf_t, node);

		printf("Cerramos todas las electrovalvulas que tengamos\n");

		printf("Abrimos las electrovalvulas que necesitemos\n");
		// CAPTURA DE UN ODORANTE
		for(uint8_t j = 0; j < stimulus; j++) {
			printf("Cogemos una muestra temporal\n");
			printf("Leemos del ADC -> supone que hay un bug y la medida es errónea\n");

			/* Cogemos para el total de submuestras (NM) que generará una muestra.
			   Una muestra está formada por una media de varias submuestras, no
			   de una sola lectura */
			for(uint8_t k = 0; k < b.n_subsamples; k++) {
				printf("Leemos valores del ADC y los sumamos todos en una variable\n");
				printf("Dormimos(sleep) un total de b.n_subsamples_capt_time/b.subsamples\n");
				/* Dormir un tiempo. El periodo de captura de una muestra esta almacenado
				 * en la variable b.n_subsamples_capt_time, y el total de submuestras que
				 * conforma una muestra está en subsamples. Hay que dividir el tiempo
				 * maximo entre el numero de capturas.
				 */
			}
			printf("Una vez obtenidas las submuestras, dividimos entre el total de submuestras\n
					y obtenemos el valor de la medida\n");
			printf("Calculamos la resistencia interna del sensor con la formula especifica\n");
			printf("Guardamos/mostramos los datos obtenidos\n");
			printf("Totamos otra muestra temporal y la restamos a la inicial, \n
					para saber el tiempo empleado en esta operacion\n");
		}
		printf("Con el tiempo empleado obtenido, restamos el tiempo de captura de UNA MUESTRA (no submuestra) al tiempo\n
				que hemos tardado en capturar la muestra (creo que no marcado)\n");
	}
}

