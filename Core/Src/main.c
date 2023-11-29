/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "math.h"
#include "vetor_senoide.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DOISPI 6.2831
#define MOV_ENCODER 25

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */



///////////////////// Maquina de estados//////////////////////
enum {

	desligado = 0, ligado, bypass

};

uint8_t estado_atual = desligado;
/////////////////////////////////////////////////////////////////////

uint32_t sin_wave_mod[TAM_SIN];
uint16_t i;
uint16_t cont = 0;
uint8_t time = 0;

///////////////////ENCODER///////////////
uint16_t contador_encoder = 0;
uint8_t variacao_encoder = 0;
/////////////////////////////////////////

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM10) {

		contador_encoder = (__HAL_TIM_GET_COUNTER(&htim4)); // ENCODER DE 5 A 50 s

		variacao_encoder = contador_encoder + MOV_ENCODER;

		cont++;

		if (cont == TAM_SIN - 1) {
			cont = 0;
		}

		//__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, sin_wave_mod[cont]);
		//__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, sin_wave_mod[cont]);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, sin_wave[cont]);

		__HAL_TIM_SET_PRESCALER(&htim10, variacao_encoder);

		//__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_, sin_wave_mod[cont]);

	}

	if (htim->Instance == TIM10) {

		time++;

	}


}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_TIM1_Init();
	MX_TIM10_Init();
	MX_TIM4_Init();
	/* USER CODE BEGIN 2 */

//////////////////DIVIDE SENOIDE////////////////////////
	/*
	 for (int j = 0; j < TAM_SIN; j++) {

	 sin_wave_mod[j] = sin_wave[j] / 2;

	 }
	 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

	//============================= Maquina de estados =============================//


		switch (estado_atual) {

		case desligado:

			HAL_TIM_Base_Stop_IT(&htim10);
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

			/////////Start Encoder//////////////
			HAL_TIM_Encoder_Stop(&htim4, TIM_CHANNEL_1);
			HAL_TIM_Encoder_Stop(&htim4, TIM_CHANNEL_2);

			//Deixa resistor ligado
			//Desliga led

			//if(botao){
			//time = 0;
			//estado_atual = ligado
			//}

			break;

		case ligado:

			HAL_TIM_Base_Start_IT(&htim10);
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

			/////////Start Encoder//////////////
			HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
			HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);

			//resistor ligado
			//liga led de circuito ligado
			//led de bypass desligado

			//if(time >= 3){
			//estado_atual = bypass
			//}

			break;

		case bypass:

			//aciona o bypass
			//liga led de bypass

			break;

		default:

			HAL_TIM_Base_Stop_IT(&htim10);
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

			/////////Start Encoder//////////////
			HAL_TIM_Encoder_Stop(&htim4, TIM_CHANNEL_1);
			HAL_TIM_Encoder_Stop(&htim4, TIM_CHANNEL_2);

			//Deixa resistor ligado
			//Desliga led

			break;

		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
