#ifndef BUTTON_H
#define BUTTON_H
#include <stdint.h>
#include <main.h>

typedef struct {
	uint8_t btn_current;
	uint8_t btn_last;
	uint8_t btn_filter;
	uint8_t is_debouncing;
	uint32_t time_debouncing;
	uint8_t test;
	uint32_t time_start_press;
    uint8_t is_press_timeout;
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
} Button_Typedef;

//--------------------var led------------------------//

// Hàm xử lý nút nhấn
void button_init(Button_Typedef* ButtonX, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void btn_release_callback(Button_Typedef * ButtonX);
void btn_press_short_callback(Button_Typedef * ButtonX);
void btn_press_timeout_callback(Button_Typedef * ButtonX);
void btn_pressing_callback(Button_Typedef * ButtonX);
void button_handle(Button_Typedef* ButtonX);
void led1Blink1Hz();
void led2Blink5Hz();
void lefOff();
void led_handle();
#endif
