/* USER CODE BEGIN Header */
/**
 * This whole project is in sync with its *.ioc file, and this includes the
 * ability to regenerate code. One thing that is not fully implemented is
 * handling of the touch screen input. Further on it will be added using default
 * BSP library. On the topic o BSP -- implementation of BSP in this project is a
 * custom implementation (it is not far from the oryginal, but some minor tweaks
 * were made).
 */

/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus_conf.h"
#include "modbus.h"
#include <stdio.h>
#include <string.h>
#include "stm32f746g_discovery_lcd.h"
//#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
//#include "stm32f746g_discovery_ts_remote.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SLAVE_ID 15 // TODO
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc3;

DMA2D_HandleTypeDef hdma2d;

LTDC_HandleTypeDef hltdc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */
uint32_t ADC3_buffer[2] = {0};

#define ADC_BUFFER_LENGTH 100
uint32_t uhADCxConvertedValue[ADC_BUFFER_LENGTH] = {0};

uint32_t adc_value = 0;
int c = 0 ;
char bufor[16] = {0};

TS_StateTypeDef TS_State;


uint8_t *resp;
uint16_t resplen;
MB_RESPONSE_STATE respstate;
uint8_t fan_on[]       = {0x00, 0x00, 0x03, 0xE8};
uint8_t fan_half[]     = {0x00, 0x00, 0x01, 0xF4};
uint8_t fan_off[]      = {0x00, 0x00, 0x00, 0x00};

uint8_t heater_on[]    = {0x00, 0x04, 0x03, 0xE8};
uint8_t heater_half[]  = {0x00, 0x04, 0x01, 0xF4};
uint8_t heater_var[]   = {0x00, 0x04, 0x01, 0xF4};
uint8_t heater_off[]   = {0x00, 0x04, 0x00, 0x00};

uint8_t get_temp[]     = {0x00, 0x00, 0x00, 0x01};
uint8_t get_temp_bad[] = {0x00, 0x01, 0x00, 0x01};

const uint16_t lcd_width					= 480;
const uint16_t lcd_height					= 282;

char txt1[50] = {0};

volatile uint32_t input = 0;
volatile uint32_t output = 0;

uint8_t UART_MB_rcvd = 0;
volatile uint8_t UART_MB_sending = 0;

char txt[200] = {0};
/* USER CODE END PV */


// DMC with global variables

float e = 0.0f;
float e_prev = 0.0f;
float u_i_prev = 0.0f;
float u_w_prev = 0.0f;
float u_prev = 0.0f;

int f_auto = 1;
int f_sensor = 0;
int f_communication = 0;
int f_temperature_low = 0;
int f_temperature_high = 0;

float y_zad = 35.0f;

const int Nu = 300;
const int N = 300;
const int lambda = 20;
int D = 650;

float dU = 0;
float dUP[649]; // D-1

float Ku[649] = {0.1241, 0.1314, 0.1375, 0.1426, 0.1467, 0.1501, 0.1528, 0.1550, 0.1566, 0.1579, 0.1588, 0.1593, 0.1596, 0.1597, 0.1596, 0.1593, 0.1588, 0.1583, 0.1576, 0.1569, 0.1561, 0.1552, 0.1543, 0.1533, 0.1523, 0.1513, 0.1502, 0.1491, 0.1480, 0.1469, 0.1458, 0.1447, 0.1436, 0.1424, 0.1413, 0.1402, 0.1391, 0.1379, 0.1368, 0.1357, 0.1346, 0.1335, 0.1324, 0.1313, 0.1302, 0.1291, 0.1281, 0.1270, 0.1260, 0.1249, 0.1239, 0.1228, 0.1218, 0.1208, 0.1198, 0.1188, 0.1178, 0.1168, 0.1159, 0.1149, 0.1139, 0.1130, 0.1120, 0.1111, 0.1102, 0.1093, 0.1083, 0.1074, 0.1065, 0.1057, 0.1048, 0.1039, 0.1030, 0.1022, 0.1013, 0.1005, 0.0996, 0.0988, 0.0980, 0.0972, 0.0963, 0.0955, 0.0947, 0.0939, 0.0932, 0.0924, 0.0916, 0.0908, 0.0901, 0.0893, 0.0886, 0.0879, 0.0871, 0.0864, 0.0857, 0.0850, 0.0842, 0.0835, 0.0828, 0.0821, 0.0815, 0.0808, 0.0801, 0.0794, 0.0788, 0.0781, 0.0775, 0.0768, 0.0762, 0.0755, 0.0749, 0.0743, 0.0737, 0.0730, 0.0724, 0.0718, 0.0712, 0.0706, 0.0700, 0.0695, 0.0689, 0.0683, 0.0677, 0.0672, 0.0666, 0.0661, 0.0655, 0.0650, 0.0644, 0.0639, 0.0633, 0.0628, 0.0623, 0.0618, 0.0612, 0.0607, 0.0602, 0.0597, 0.0592, 0.0587, 0.0582, 0.0578, 0.0573, 0.0568, 0.0563, 0.0558, 0.0554, 0.0549, 0.0545, 0.0540, 0.0536, 0.0531, 0.0527, 0.0522, 0.0518, 0.0514, 0.0509, 0.0505, 0.0501, 0.0497, 0.0492, 0.0488, 0.0484, 0.0480, 0.0476, 0.0472, 0.0468, 0.0464, 0.0460, 0.0457, 0.0453, 0.0449, 0.0445, 0.0442, 0.0438, 0.0434, 0.0431, 0.0427, 0.0423, 0.0420, 0.0416, 0.0413, 0.0409, 0.0406, 0.0403, 0.0399, 0.0396, 0.0393, 0.0389, 0.0386, 0.0383, 0.0380, 0.0377, 0.0373, 0.0370, 0.0367, 0.0364, 0.0361, 0.0358, 0.0355, 0.0352, 0.0349, 0.0346, 0.0343, 0.0340, 0.0338, 0.0335, 0.0332, 0.0329, 0.0326, 0.0324, 0.0321, 0.0318, 0.0316, 0.0313, 0.0310, 0.0308, 0.0305, 0.0303, 0.0300, 0.0298, 0.0295, 0.0293, 0.0290, 0.0288, 0.0285, 0.0283, 0.0281, 0.0278, 0.0276, 0.0274, 0.0271, 0.0269, 0.0267, 0.0265, 0.0262, 0.0260, 0.0258, 0.0256, 0.0254, 0.0252, 0.0250, 0.0248, 0.0245, 0.0243, 0.0241, 0.0239, 0.0237, 0.0235, 0.0233, 0.0231, 0.0230, 0.0228, 0.0226, 0.0224, 0.0222, 0.0220, 0.0218, 0.0216, 0.0215, 0.0213, 0.0211, 0.0209, 0.0208, 0.0206, 0.0204, 0.0202, 0.0201, 0.0199, 0.0197, 0.0196, 0.0194, 0.0192, 0.0191, 0.0189, 0.0188, 0.0186, 0.0185, 0.0183, 0.0181, 0.0180, 0.0178, 0.0177, 0.0175, 0.0174, 0.0173, 0.0171, 0.0170, 0.0168, 0.0167, 0.0165, 0.0164, 0.0163, 0.0161, 0.0160, 0.0159, 0.0157, 0.0156, 0.0155, 0.0153, 0.0152, 0.0151, 0.0150, 0.0148, 0.0147, 0.0146, 0.0145, 0.0143, 0.0142, 0.0141, 0.0140, 0.0139, 0.0138, 0.0136, 0.0135, 0.0134, 0.0133, 0.0132, 0.0131, 0.0130, 0.0129, 0.0128, 0.0127, 0.0125, 0.0124, 0.0123, 0.0122, 0.0121, 0.0120, 0.0119, 0.0118, 0.0117, 0.0116, 0.0115, 0.0114, 0.0113, 0.0112, 0.0112, 0.0111, 0.0110, 0.0109, 0.0108, 0.0107, 0.0106, 0.0105, 0.0104, 0.0103, 0.0103, 0.0102, 0.0101, 0.0100, 0.0099, 0.0098, 0.0098, 0.0097, 0.0096, 0.0095, 0.0094, 0.0094, 0.0093, 0.0092, 0.0091, 0.0090, 0.0090, 0.0089, 0.0088, 0.0087, 0.0087, 0.0086, 0.0085, 0.0085, 0.0084, 0.0083, 0.0082, 0.0082, 0.0081, 0.0080, 0.0080, 0.0079, 0.0078, 0.0078, 0.0077, 0.0076, 0.0076, 0.0075, 0.0075, 0.0074, 0.0073, 0.0073, 0.0072, 0.0072, 0.0071, 0.0070, 0.0070, 0.0069, 0.0069, 0.0068, 0.0067, 0.0067, 0.0066, 0.0066, 0.0065, 0.0065, 0.0064, 0.0064, 0.0063, 0.0063, 0.0062, 0.0061, 0.0061, 0.0060, 0.0060, 0.0059, 0.0059, 0.0058, 0.0058, 0.0057, 0.0057, 0.0057, 0.0056, 0.0056, 0.0055, 0.0055, 0.0054, 0.0054, 0.0053, 0.0053, 0.0052, 0.0052, 0.0052, 0.0051, 0.0051, 0.0050, 0.0050, 0.0049, 0.0049, 0.0049, 0.0048, 0.0048, 0.0047, 0.0047, 0.0047, 0.0046, 0.0046, 0.0045, 0.0045, 0.0045, 0.0044, 0.0044, 0.0044, 0.0043, 0.0043, 0.0043, 0.0042, 0.0042, 0.0041, 0.0041, 0.0041, 0.0040, 0.0040, 0.0040, 0.0039, 0.0039, 0.0039, 0.0038, 0.0038, 0.0038, 0.0037, 0.0037, 0.0037, 0.0037, 0.0036, 0.0036, 0.0036, 0.0035, 0.0035, 0.0035, 0.0034, 0.0034, 0.0034, 0.0034, 0.0033, 0.0033, 0.0033, 0.0033, 0.0032, 0.0032, 0.0032, 0.0031, 0.0031, 0.0031, 0.0031, 0.0030, 0.0030, 0.0030, 0.0030, 0.0029, 0.0029, 0.0029, 0.0029, 0.0028, 0.0028, 0.0028, 0.0028, 0.0027, 0.0027, 0.0027, 0.0027, 0.0027, 0.0026, 0.0026, 0.0026, 0.0026, 0.0025, 0.0025, 0.0025, 0.0025, 0.0025, 0.0024, 0.0024, 0.0024, 0.0024, 0.0024, 0.0023, 0.0023, 0.0023, 0.0023, 0.0023, 0.0022, 0.0022, 0.0022, 0.0022, 0.0022, 0.0022, 0.0021, 0.0021, 0.0021, 0.0021, 0.0021, 0.0020, 0.0020, 0.0020, 0.0020, 0.0020, 0.0020, 0.0019, 0.0019, 0.0019, 0.0019, 0.0019, 0.0019, 0.0019, 0.0018, 0.0018, 0.0018, 0.0018, 0.0018, 0.0018, 0.0017, 0.0017, 0.0017, 0.0017, 0.0017, 0.0017, 0.0017, 0.0016, 0.0016, 0.0016, 0.0016, 0.0016, 0.0016, 0.0016, 0.0016, 0.0015, 0.0015, 0.0015, 0.0015, 0.0015, 0.0015, 0.0015, 0.0015, 0.0014, 0.0014, 0.0014, 0.0014, 0.0014, 0.0014, 0.0014, 0.0014, 0.0014, 0.0014, 0.0013, 0.0013, 0.0013, 0.0013, 0.0013, 0.0013, 0.0013, 0.0013, 0.0013, 0.0013, 0.0013, 0.0012, 0.0012, 0.0012, 0.0012, 0.0012, 0.0012, 0.0012, 0.0012, 0.0012, 0.0012, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0010, 0.0010, 0.0010, 0.0010, 0.0010, 0.0009, 0.0009, 0.0009, 0.0009, 0.0008, 0.0008, 0.0008, 0.0007, 0.0007, 0.0007, 0.0006, 0.0006, 0.0006, 0.0005, 0.0005, 0.0004, 0.0004, 0.0003, 0.0003, 0.0003, 0.0002, 0.0002, 0.0001, 0.0001, 0.0000};
float Ke = 1.5617;
static float y = 0.0f;
static float ypp = 41.0f;
static float u = 0.0f;

static uint16_t raw_y = 2345;
static uint16_t raw_u = 0;



float our_DMC() {
	float Ku_dUP = 0;

	for(int i = 0; i < D - 1; i++) {
		Ku_dUP += Ku[i] * dUP[i];
	}

	dU = Ke*e - Ku_dUP;

	for(int i = D - 2; i > 0; i--){
		dUP[i] = dUP[i-1];
	}

	dUP[0] = dU;

	u = u_prev + dU;
	u_prev = u;

	return u;
}
// 480x272

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_LTDC_Init(void);
static void MX_FMC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */
void Communication_Mode(bool rx, bool tx){
	if(rx) HAL_UART_Receive_IT(&huart6, &UART_MB_rcvd, 1);

	if(tx && UART_MB_sending == 0) {
		UART_MB_sending = 1;
		SetCharacterReadyToTransmit();
	}
	if(!tx) UART_MB_sending = 0;
}
void Communication_Put(uint8_t ch){
	HAL_UART_Transmit_IT(&huart6, &ch, 1);
}

uint8_t Communication_Get(void){
	uint8_t tmp = UART_MB_rcvd;
	UART_MB_rcvd = 0;
	SetCharacterReceived(false);
	return tmp;
}

void Enable50usTimer(void){
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

void Disable50usTimer(void){
  HAL_NVIC_DisableIRQ(TIM4_IRQn);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == TS_INT_PIN){ // TOUCH SCREEN touched -- this is the place where you have to check where the touch screen is pressed
	  BSP_TS_GetState(&TS_State); /*!*/
	}
}

void DrawPointOfTouch(TS_StateTypeDef *TSS){
  static uint16_t lastx = 0;
  static uint16_t lasty = 0;
  BSP_LCD_SelectLayer(1);
  BSP_LCD_SetTextColor(LCD_COLOR_TRANSPARENT);
  BSP_LCD_DrawCircle(lastx,lasty, 3);
  BSP_LCD_DrawCircle(lastx,lasty, 2);
  if(TSS->touchDetected > 0){
	  lastx = TSS->touchX[0];
	  lasty = TSS->touchY[0];
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_DrawCircle(lastx,lasty, 3);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_DrawCircle(lastx,lasty, 2);
  }
  BSP_LCD_SelectLayer(0);
}

void DrawCalibrationCross(uint32_t x, uint32_t y){
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawLine(x-1, y-1, x-1, y-10); // upper left
	BSP_LCD_DrawLine(x-1, y+1, x-1, y+10); // upper right
	BSP_LCD_DrawLine(x+1, y-1, x+1, y-10); // lower left
	BSP_LCD_DrawLine(x+1, y+1, x+1, y+10); // lower right

	BSP_LCD_DrawLine(x-1, y-1, x-10, y-1); // lefter up
	BSP_LCD_DrawLine(x+1, y-1, x+10, y-1); // righter up
	BSP_LCD_DrawLine(x-1, y+1, x-10, y+1); // lefter down
	BSP_LCD_DrawLine(x+1, y+1, x+10, y+1); // righter down
}

void DrawHorizontal(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	int mid = y + (h / 2);

	BSP_LCD_DrawLine(x + 1, mid, x + w - 2, mid);
	BSP_LCD_DrawLine(x + 1, mid+1, x + w - 2, mid+1);
}

void DrawVertical(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	int mid = x + (w / 2);

	BSP_LCD_DrawLine(mid, y + 1, mid, y + h - 2);
	BSP_LCD_DrawLine(mid + 1, y + 1, mid + 1, y + h - 2);
}

void DrawMinus(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
	 BSP_LCD_SetTextColor(LCD_COLOR_RED);
	 BSP_LCD_FillRect(x, y, w, h);
	 DrawHorizontal(x, y, w, h);
}

void DrawPlus(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
	 BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	 BSP_LCD_FillRect(x, y, w, h);
	 DrawHorizontal(x, y, w, h);
	 DrawVertical(x, y, w, h);
}

void DrawAutoRegulation(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
  if(f_auto == 1){
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
  } else{
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  }
  
	BSP_LCD_FillRect(x, y, w, h);
}

void DrawFSensor(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  if(f_sensor == 1){
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
  } else{
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  }
  
	BSP_LCD_FillRect(x, y, w, h);
}

void DrawFCommunication(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  if(f_communication == 1){
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
  } else{
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  }
  
	BSP_LCD_FillRect(x, y, w, h);
}

void DrawFTemperatureLow(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  if(f_temperature_low == 1){
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
  } else{
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  }
  
	BSP_LCD_FillRect(x, y, w, h);
}

void DrawFTemperatureHigh(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  if(f_temperature_high == 1){
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
  } else{
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  }
  
	BSP_LCD_FillRect(x, y, w, h);
}

int touchDetectedZone(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	if(TS_State.touchDetected){
		if(TS_State.touchX[0] > x && TS_State.touchX[0] < x + w && TS_State.touchY[0] > y && TS_State.touchY[0] < y + h){
			return 1;
		}
	}

	return 0;
}
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc){
//  DrawPointOfTouch(&TS_State);    // TODO Delete if needed
//  DrawCalibrationCross(30, 30);   // TODO Delete if needed
//  DrawCalibrationCross(450, 30);  // TODO Delete if needed
//  DrawCalibrationCross(450, 240); // TODO Delete if needed
//  DrawCalibrationCross(30, 240);  // TODO Delete if needed

  char temperature_buf[50] = {0};
  char regulation_buf[50] = {0};
  char error_buf[50] = {0};
  char y_zad_buf[50] = {0};

  sprintf(temperature_buf,"T*C: %.2f", y);
  sprintf(y_zad_buf, "Tzad: %.2f", y_zad);
  sprintf(regulation_buf, "Sterowanie: %.2f", u);
//  sprintf(error_buf, "Uchyb T: %.2f", e);

  BSP_LCD_SetFont(&Font20);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DisplayStringAt(0, 5, (uint8_t*)temperature_buf, LEFT_MODE);
  BSP_LCD_DisplayStringAt(0, 40, (uint8_t*)y_zad_buf, LEFT_MODE);
  BSP_LCD_DisplayStringAt(0, 75, (uint8_t*)regulation_buf, LEFT_MODE);
  BSP_LCD_DisplayStringAt(0, 110, (uint8_t*)error_buf, LEFT_MODE);

  //draw y_zad +, -
  DrawMinus(350, 35, 30, 30);
  DrawPlus(400, 35, 30, 30);

  if(touchDetectedZone(350, 35, 30, 30)){
	  y_zad= y_zad - 1;
  }

  if(touchDetectedZone(400, 35, 30, 30)){
	  y_zad= y_zad + 1;
  }


  //draw sterowanie +, -
  DrawMinus(350, 70, 30, 30);
  DrawPlus(400, 70, 30, 30);

  if(touchDetectedZone(350, 70, 30, 30)){
  	  u= u - 1;
    }

    if(touchDetectedZone(400, 70, 30, 30)){
  	  u= u + 1;
    }

  DrawAutoRegulation(420, 70, 30, 30);

  if(touchDetectedZone(420, 70, 30, 30)){
    if(f_auto == 1){
      f_auto = 0
    } else {
      f_auto = 1
    }
  }

  DrawFSensor(420, 105, 30, 30)
  DrawFCommunication(420, 140, 30, 30)
  DrawFTemperatureLow(420, 175, 30, 30)
  DrawFTemperatureHigh(420, 210, 30, 30)

  HAL_LTDC_ProgramLineEvent(hltdc, 272);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	static int i=0;
	static uint32_t tmpval= 0;
	for(i=0,tmpval=0;i<ADC_BUFFER_LENGTH; ++i){
		tmpval += uhADCxConvertedValue[i];
	}
	adc_value = tmpval/ADC_BUFFER_LENGTH;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART6){
		SetCharacterReceived(true);
		HAL_UART_Receive_IT(&huart6, &UART_MB_rcvd, 1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART6)
		UART_MB_sending = 0;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		MB_SendRequest(SLAVE_ID, FUN_READ_INPUT_REGISTER, get_temp, 4); // Nie powinno być 0?
		respstate = MB_GetResponse(SLAVE_ID, FUN_READ_INPUT_REGISTER, &resp, &resplen, 300);
		if(respstate != RESPONSE_OK) {
            f_communication = 1;
            // zastanowić czy nie dać tutaj wywołania funkcji do rysowania
            // while(1)
        } else {
            f_communication = 0;
			raw_y = resp[1]*0x100+resp[2];
			y = raw_y/100.0f;
		}

        // Awarie i Alarmy

        if(y>=60.0f && y<=125.0f ) {
            f_temperature_high=1;
        } else {
            f_temperature_high=0;
        }
        if(y<=25.0f && y>=-55.0f ) {
            f_temperature_low=1;
        } else {
            f_temperature_low=0;
        }
		if(y>=125.0f || y<=-55.0f ) {
            f_sensor=1;
        } else {
            f_sensor=0;
        }

        // Sterowanie

		e = y_zad - y;

        if(f_auto){
            u =  our_DMC();
        }

		e_prev = e;

		/* aplikacja ograniczen na sygnal sterujacy */
		if(u >   50.0f) u =  50.0f;
		if(u <  -50.0f) u = -50.0f;

		/* skalowanie z -50..50 do 0..1000 */
		raw_u = (uint16_t)(u+50.0f)*10; // przejscie z -2048 - 2047 do 0 - 4095

		/* przygotowanie wiadomosci MODBUS */
		heater_var[2] = (raw_u&0xFF00)>>8; // pierwszy bajt
		heater_var[3] = (raw_u&0x00FF)>>0; // drugi bajt

		/* wyslanie wiadomosci */
		MB_SendRequest(SLAVE_ID, FUN_WRITE_SINGLE_REGISTER, heater_var, 4);

		/* odczyt odpowiedzi i sprawdzenie jej poprawnosci */
		respstate = MB_GetResponse(SLAVE_ID, FUN_WRITE_SINGLE_REGISTER, &resp, &resplen, 300);
		if(respstate != RESPONSE_OK) while(1);

		/* komunikacja z komputerem */
		while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
		sprintf(txt,"U=%.2f;Y=%.2f;\n",u,y); // 22 znaki
		if(HAL_UART_Transmit_IT(&huart1, (uint8_t*)txt, strlen(txt))!= HAL_OK) Error_Handler();
	}
	if (htim->Instance == TIM3){ // timer odpowiedzialny za aktualizacje MB i odliczanie timeout'u
		MB();
		TimeoutTick();
	}
	if (htim->Instance == TIM4){ // timer odpowiedzialny za odliczanie kwantow 50us
		Timer50usTick();
	}
	if (htim->Instance == TIM5){ // ...
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  HAL_Delay(100); /*! Delay so that LCD will not restart during initialisation !*/
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_LTDC_Init();
  MX_FMC_Init();
  MX_DMA2D_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  BSP_LCD_SetFont(&Font20); // choose size of the font: Font8, Font12, Font16, Font20, Font24
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE); // each character has background!!!
  BSP_TS_Init(0,0); // initialisation of TouchScreen -- arguments are irrelevant
  BSP_TS_ITConfig(); // to cancel exti interrupts from the touch screen comment this line

  HAL_ADC_Start_DMA(&hadc3, ADC3_buffer, 2);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SelectLayer(1);
  BSP_LCD_Clear(LCD_COLOR_TRANSPARENT);
  HAL_LTDC_ProgramLineEvent(&hltdc, 272);

  if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)uhADCxConvertedValue, ADC_BUFFER_LENGTH) != HAL_OK)
	  Error_Handler();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  MB_Config(115200);

	while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
	while(HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX);

	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim5);

  HAL_Delay(100); // wait for everything to set up before the controller loop starts


  MB_SendRequest(SLAVE_ID, FUN_WRITE_SINGLE_REGISTER, fan_half, 4);
	respstate = MB_GetResponse(SLAVE_ID, FUN_WRITE_SINGLE_REGISTER, &resp, &resplen, 300);
	if(respstate != RESPONSE_OK) while(1);
	HAL_Delay(900);
  HAL_TIM_Base_Start_IT(&htim2);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		sprintf(txt1,"Test input = %ld",input);
//		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

//		BSP_TS_GetState(&TS_State);
//		++c;
//		sprintf((char*)bufor, "Test %d", c);
//		HAL_Delay(100);

//		BSP_LCD_DisplayStringAt( 10, 10, (uint8_t*)bufor, LEFT_MODE);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 2;
  hadc3.Init.DMAContinuousRequests = ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 40;
  hltdc.Init.VerticalSync = 9;
  hltdc.Init.AccumulatedHBP = 53;
  hltdc.Init.AccumulatedVBP = 11;
  hltdc.Init.AccumulatedActiveW = 533;
  hltdc.Init.AccumulatedActiveH = 283;
  hltdc.Init.TotalWidth = 565;
  hltdc.Init.TotalHeigh = 285;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 480;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 272;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0xC0000000;
  pLayerCfg.ImageWidth = 480;
  pLayerCfg.ImageHeight = 272;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 480;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 272;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg1.Alpha = 255;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg1.FBStartAdress = 0xC0000000+480*272*4;
  pLayerCfg1.ImageWidth = 480;
  pLayerCfg1.ImageHeight = 272;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */
  BSP_LCD_LayerDefaultInit(0, pLayerCfg.FBStartAdress);
  BSP_LCD_LayerDefaultInit(1, pLayerCfg1.FBStartAdress);
  /* Assert display enable LCD_DISP pin */
  HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);

  /* Assert backlight LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);
  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 10800-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 108-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 10;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 108-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 50;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 10800-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 10000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_9B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_EVEN;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */
  FMC_SDRAM_CommandTypeDef Command;

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 6;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 6;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
  __IO uint32_t tmpmrd =0;
  /* Step 3:  Configure a clock configuration enable command */
  Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 4: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);

  /* Step 5: Configure a PALL (precharge all) command */
  Command.CommandMode = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 6 : Configure a Auto-Refresh command */
  Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 8;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 7: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_2           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 8: Set the refresh rate counter */
  /* (15.62 us x Freq) - 20 */
  /* Set the device refresh counter */
  hsdram1.Instance->SDRTR |= ((uint32_t)((1292)<< 1));

  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PK3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pin : PI12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PI13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PG6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PF7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
