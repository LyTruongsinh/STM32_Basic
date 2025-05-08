#include "Button.h"


////--------------------Định nghĩa biến toàn cục---------------------//
//uint8_t ButtonX->btn_current;
//uint8_t ButtonX->btn_last = 0;
//uint8_t btn_filter = 0;
//uint8_t ButtonX->is_debouncing;
//uint32_t ButtonX->time_debouncing;
//uint8_t test;
//uint32_t ButtonX->time_start_press;
//uint8_t ButtonX->is_press_timeout;
//
//LedStatus led_status = LED_OFF;
////---------------------------------------------------------------//
void button_init(Button_Typedef* ButtonX, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	ButtonX->GPIOx = GPIOx;
	ButtonX->GPIO_Pin = GPIO_Pin;
	ButtonX->btn_last = 0;
	ButtonX->btn_filter = 0;
}


void button_handle(Button_Typedef * ButtonX) {
	uint8_t sta = HAL_GPIO_ReadPin(ButtonX->GPIOx, ButtonX->GPIO_Pin);
	if(sta != ButtonX->btn_filter) {
		ButtonX->btn_filter = sta;
		ButtonX->is_debouncing = 1;
		ButtonX->time_debouncing = HAL_GetTick();
	}

	//---------------------Tín hiệu được xác lập-----------------------------//
	if(ButtonX->is_debouncing && (HAL_GetTick() - ButtonX->time_debouncing) >= 15) {
		ButtonX->btn_current = ButtonX->btn_filter;
		ButtonX->is_debouncing = 0;
	}
	//---------------------Xử lý nhấn nhả------------------------------------//
	if(ButtonX->btn_current != ButtonX->btn_last) {
		if(ButtonX->btn_current == 0) { // nhả ra
			if(HAL_GetTick() - ButtonX->time_start_press <= 1000) {
				btn_press_short_callback(ButtonX);
			}
			btn_release_callback(ButtonX);
			ButtonX->is_press_timeout = 0;
		}
		else { // nhấn xuống
			ButtonX->is_press_timeout = 1;
			btn_pressing_callback(ButtonX);
			ButtonX->time_start_press = HAL_GetTick();
		}
		ButtonX->btn_last = ButtonX->btn_current;
	}
	//------------------------------Xử lý nhấn giữ--------------------------//
	if(ButtonX->is_press_timeout && (HAL_GetTick() - ButtonX->time_start_press >= 3000)) {
		btn_press_timeout_callback(ButtonX);
		ButtonX->is_press_timeout = 0;
	}
	//------------------------------Xử lý nhấn giữ--------------------------//
}


