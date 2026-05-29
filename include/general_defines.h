#ifndef GENERAL_DEFINES_H
#define GENERAL_DEFINES_H

typedef enum {
	Input = 0,
	Output = 1
}GPIO_Direction;

typedef enum {
	Low = 0,
	High = 1
}GPIO_Value;

typedef enum {
	None = 0,
	PullDown = 1,
	PullUp = 2
} GPIO_Resistor;

typedef enum {
	Normal = 0,
	Inversed = 1,
} PWM_Polarity;

typedef enum {
	BBIO_OK, // No error
	BBIO_ACCESS, // Error accessing a file
	BBIO_SYSFS, // Some error with Sysfs files
	BBIO_CAPE, // some error with capes
	BBIO_INVARG, // Invalid argument
	BBIO_MEM,
	BBIO_GEN // General error
} BBIO_err;


#endif // GENERAL_DEFINES_H
