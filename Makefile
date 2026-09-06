adc.o: source/c_adc.c source/common.c
	gcc -Iinclude/ -c c_adc.o source/c_adc.c source/common.c

pwm.o: source/c_pwm.c source/common.c
	gcc -Iinclude/ -c c_pwm.o source/c_pwm.c source/common.c

event_gpio.o: source/event_gpio.c source/common.c source/gpio.c source/c_pinmux.c
	gcc -Iinclude/ -c event_gpio.o source/event_gpio.c source/common.c source/gpio.c source/c_pinmux.c

uart.o: source/c_uart.c source/common.c
	gcc -Iinclude/ -c c_uart.o source/c_uart.c source/common.c

main: main.c source/file_conf.c list.o source/event_gpio.c source/c_pwm.c source/common.c source/c_pinmux.c source/c_adc.c
	gcc -g -Iinclude/ -o $@ $^
