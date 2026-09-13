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

static void gpio_main_simple_test()
{
	board_conf_t b;

	memset(&b, 0, sizeof(b));

	stack_b_init_stack(&b.free_nodes);
	queue_b_init_queue(&b.capture_config_vals);
	for(uint8_t i = 0; i < MAX_NUMBER_NODES; i++) {
		memset(&b.nodes[i], 0, sizeof(capture_conf_t));
		stack_b_stack_value(&b.free_nodes, &b.nodes[i].node);
	}

	int8_t r = read_config_file("file_test.txt", &b);

	init_module();

	for (uint8_t i = 0; i < MAX_TOTAL_ELECTROVALS; i++) {
		gpio_setup(b.electrovals_pins[i],
		           Output,
		           PullDown,
		           0,
		           0);
		printf("Incializamos GPIO: %s\n", b.electrovals_pins[i]);
	}

	gpio_output(b.electrovals_pins[0], High);
	gpio_output(b.electrovals_pins[1], Low);
	gpio_output(b.electrovals_pins[2], High);
	gpio_output(b.electrovals_pins[3], Low);
}

static int pure_capture_main()
{

	board_conf_t b;

	printf("Comenzamos main\n");

	memset(&b, 0, sizeof(b));

	stack_b_init_stack(&b.free_nodes);
	queue_b_init_queue(&b.capture_config_vals);
	for(uint8_t i = 0; i < MAX_NUMBER_NODES; i++) {
		memset(&b.nodes[i], 0, sizeof(capture_conf_t));
		stack_b_stack_value(&b.free_nodes, &b.nodes[i].node);
	}

	int8_t r = read_config_file("file_test.txt", &b);

	node_b_t *node_b = NULL;
	capture_conf_t *node = NULL;
	for(uint8_t i = 0; i < 4; i++) {
		list_get_b_node(&b.capture_config_vals,
		                i,
		                &node_b,
		                0);

		printf(" VALORES OBTENIDOS DESPUES ");
		node = (capture_conf_t *)get_struct_by_elem(node_b, capture_conf_t, node);

		printf("VALORES OBTENIDOS: \n");
		printf("-------------------------\n");
		printf("ESTIMULO: %d\nTEMPERATURA SENSOR(%%): %d\n", node->stimulus, node->sensor_heat);
		printf("SUCCION MOTOR: %d\nTOTAL ELECTROVALVULAS: %d\n", node->motor_suction, node->total_vals);
		for(uint8_t i = 0; i < node->total_vals; i++)
			printf("VALVULA[%d] = %d\n", i, node->vals[i]);
	}

	return 0;
	// Iniciamos las electrovalvulas.
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
	printf("Iniciar/setear el ADC\n");
	//adc_setup();

	for(uint8_t i = 0; i < b.total_nodes_read; i++) {
		queue_b_unqueue_value(&b.capture_config_vals, &node_b);
		node = (capture_conf_t *)get_struct_by_elem(node_b, capture_conf_t, node);

		printf("PWM del sensor al %d%%\n", node->sensor_heat);
		//pwm_set_channel_duty_cycle(b.sensor_pin, node->sensor_heat);

		printf("PWM del motor al %d%%\n", node->motor_suction);
		//pwm_set_channel_duty_cycle(b.motor_pin, node->sensor_heat);

		printf("Cerramos todas las electrovalvulas que tengamos: %d\n", b.total_pins_read);
		for(uint8_t j = 0; j < b.total_pins_read; j++) {
			printf("GPIO: %s\n", b.electrovals_pins[j]);
			//gpio_output(b.electrovals_pins[j], Low);
		}

		printf("Abrimos las electrovalvulas que necesitemos\n");
		for(uint8_t j = 0; j < node->total_vals; j++) {
			printf("GPIO: %s\n", b.electrovals_pins[node->vals[j]]);
			//gpio_output(b.electrovals_pins[j], High);
		}

		// CAPTURA DE UN ODORANTE
		double tsub = (double)b.n_subsamples_capt_time / (double)b.n_subsamples;
		for(uint8_t j = 0; j < node->stimulus; j++) {

			printf("Cogemos una muestra temporal\n");
			time_t time1;
			time(&time1);

			printf("Leemos del ADC -> supone que hay un bug y la medida es errónea\n");
			float value_readed = 0.0;
			float measure_readed = 0.0;

			//adc_read_value(b.sensor_pin, &value_readed, 0);

			/* Cogemos para el total de submuestras (NM) que generará una muestra.
			   Una muestra está formada por una media de varias submuestras, no
			   de una sola lectura */
			for(uint8_t k = 0; k < b.n_subsamples; k++) {
				//printf("Leemos valores del ADC y los sumamos todos en una variable\n");
				//adc_read_value(b.sensor_pin, &value_readed, 0);
				measure_readed+=value_readed;

				//printf("Dormimos(sleep) un total de b.n_subsamples_capt_time/b.subsamples: %f\n", tsub);
				usleep(tsub);
				/* Dormir un tiempo. El periodo de captura de una muestra esta almacenado
				 * en la variable b.n_subsamples_capt_time, y el total de submuestras que
				 * conforma una muestra está en subsamples. Hay que dividir el tiempo
				 * maximo entre el numero de capturas.
				 */
			}
			printf("Una vez obtenidas las submuestras, dividimos entre el total de submuestras\n y obtenemos el valor de la medida\n");
			measure_readed/=b.n_subsamples;

			printf("Calculamos la resistencia interna del sensor con la formula especifica\n");
			float resistance = (((b.sensor_volt*b.res_val)/(measure_readed/1000.0))-b.res_val);

			printf("Guardamos/mostramos los datos obtenidos\n");
			printf("Totamos otra muestra temporal y la restamos a la inicial, \npara saber el tiempo empleado en esta operacion\n");
			time_t time2;
			time(&time2);
			printf("time1: %ld --- time2: %ld\n", time1, time2);
			printf("Difftime entre capturas: %f\n", difftime(time2, time1));

		}
		printf("Con el tiempo empleado obtenido, restamos el tiempo de captura de UNA MUESTRA (no submuestra) al tiempo\n que hemos tardado en capturar la muestra (creo que no marcado)\n");
	}
}

int main(int argc, char *argv[])
{
	gpio_main_simple_test();
}
