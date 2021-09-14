/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include "math.h"
#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Address_PCF1 0x44//0x42
#define Address_PCF2 0x48//0x4A
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */

/*-------- INICIO VARIABLES DEL PROGRAMA --------*/
//RTC
uint8_t get_time[17];
//UBICACION DE SALIDAS
uint8_t salidaReles[] = {
		0xFF,0xFF,
		0xFB,0xFF,
		0xDF,0xFF,
		0xF3,0xFF,
		0xD7,0xFF,
		0x7B,0xFE,
		0xD5,0xFF,
		0x73,0xFE,
		0x95,0xFF,
		0x59,0xFE,
		0xC4,0xFF,
		0x43,0xFE,
		0x84,0xFF,
		0x11,0xFE,
		0x05,0xFE,
		0x01,0xFE,
		0x04,0xFE
		};

//Variables Modbus
uint16_t Tension[3];
uint16_t Corriente[3];
uint16_t Potencia[3];
uint8_t mensaje1[] = {2, 3, 0, 9, 0, 6, 21, 249};		/* Tension Canal A,B,C*/
uint8_t mensaje2[] = {2, 3, 0, 15, 0, 6, 245, 248};	/* Corriente Canal A,B,C*/
uint8_t mensaje3[] = {2, 3, 0, 39, 0, 6, 117, 240};	/*Potencia Canal A,B,C*/
uint8_t M_Tension[17];
uint8_t M_Corriente[17];
uint8_t M_Potencia[17];
uint8_t h_calculo = 0;
uint8_t h_envio = 0;
uint32_t mantisa = 0;
int16_t ex;
uint8_t devolucion;
uint8_t dato_a;
uint8_t dato_b;
uint8_t dato_c;
uint8_t dato_d;
uint8_t ubicacion;
uint8_t crc2 = 0;
uint8_t crc1 = 0;
uint16_t crcinicio;
uint8_t receptordei2c = 0;

//Varibales pulsadores
uint8_t est_up , est_down , est_left , est_right ;
uint8_t seg_up = 0;
uint8_t seg_down = 0;
uint8_t seg_left = 0;
uint8_t seg_right = 0;

//Variables menu
uint8_t menu_conf = 1;
uint8_t menu_auto = 0;
uint8_t menu_manu = 0;
uint8_t pag_menu = 0;
uint8_t g_menu = 0;
uint8_t m_cursor = 0;
uint8_t cursor_fila = 0;
uint8_t paso = 1;
uint8_t pag_anterior = 0;
uint8_t leer = 1;
uint8_t compsegundo;
//Variables de salida
uint8_t tiempo = 1;
uint8_t P_min = 1;
uint8_t P_max = 1;
uint8_t P_pas = 1;
uint8_t P_modo = 1;
uint8_t P_modo_franco = 1;
uint8_t P_sig = 0;
uint8_t direccion = 0;
uint8_t fin = 0;
uint8_t ultimo = 0;
uint8_t set_segundo;
uint8_t set_minuto;
uint8_t set_hors;
uint8_t i2c_tiempo_ok = 0;

//Variables Creacion de Caracteres
char customChar[] = {
  0x00,
  0x04,
  0x02,
  0x1F,
  0x02,
  0x04,
  0x00,
  0x00
};
char ambos[] = {
  0x04,
  0x0E,
  0x1F,
  0x00,
  0x00,
  0x1F,
  0x0E,
  0x04
};

char arriba[] = {
  0x04,
  0x0E,
  0x1F,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};
char abajo[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x1F,
  0x0E,
  0x04
};
/*-------- FIN VARIABLES DEL PROGRAMA --------*/

char paraBuffer[6];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

#define BUFFER_SIZE 50
char buffer[BUFFER_SIZE];  // to store strings..

int i=0;

static void User_I2C1_GeneralPurposeOutput_Init(I2C_HandleTypeDef* i2cHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(i2cHandle->Instance==I2C1)
    {
        /*   PB10     ------> I2C2_SCL; PB11     ------> I2C2_SDA */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}


static void User_I2C1_AlternateFunction_Init(I2C_HandleTypeDef* i2cHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(i2cHandle->Instance==I2C1)
    {
        /*   PB10     ------> I2C2_SCL; PB11     ------> I2C2_SDA */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

HAL_StatusTypeDef I2CResetBus(void)
{
    hi2c1.ErrorCode = HAL_I2C_ERROR_AF;
    /* 1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register */
    __HAL_I2C_DISABLE(&hi2c1);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* 2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR) */
    User_I2C1_GeneralPurposeOutput_Init(&hi2c1);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(1);

    /* 3. Check SCL and SDA High level in GPIOx_IDR */
    if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) != GPIO_PIN_SET)||(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_SET))
    {
#ifdef I2C_TEST
        printf("3.PB6=%d, PB7=%d\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6), HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7));
#endif
        return HAL_ERROR;
    }

    /* 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
     * 5. Check SDA Low level in GPIOx_IDR.
     * 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR)
     * 7. Check SCL Low level in GPIOx_IDR.
     * */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_Delay(1);
    if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) != GPIO_PIN_RESET)||(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_RESET))
    {
#ifdef I2C_TEST
        printf("4-7.PB6=%d, PB7=%d\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6), HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7));
#endif
        return HAL_ERROR;
    }

    /*
     * 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
     * 9. Check SCL High level in GPIOx_IDR.
     * 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
     * 11. Check SDA High level in GPIOx_IDR.
     */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(1);
    if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) != GPIO_PIN_SET)||(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_SET))
    {
#ifdef I2C_TEST
        printf("8-11.PB6=%d, PB7=%d\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6), HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7));
#endif
        return HAL_ERROR;
    }

    /* 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain. */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
    User_I2C1_AlternateFunction_Init(&hi2c1);

    /* 13. Set SWRST bit in I2Cx_CR1 register. */
    hi2c1.Instance->CR1 |=  I2C_CR1_SWRST;
    HAL_Delay(2);
    /* 14. Clear SWRST bit in I2Cx_CR1 register. */
    hi2c1.Instance->CR1 &=  ~I2C_CR1_SWRST;
    HAL_Delay(2);
    /* 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register */
    MX_I2C1_Init();
    __HAL_I2C_ENABLE(&hi2c1);
    HAL_Delay(2);
#ifdef I2C_TEST
    printf("I2CResetBus\r\n");
#endif
    hi2c1.ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c1.State = HAL_I2C_STATE_READY;
    //hi2c1.PreviousState = I2C_STATE_NONE;
    hi2c1.Mode = HAL_I2C_MODE_NONE;
    return HAL_OK;
}


int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}

void clear_buffer (void)
{
	for (int i=0; i<BUFFER_SIZE; i++) buffer[i] = '\0';
}

#define DS3231_ADDRESS 0xD0

uint8_t decToBcd(int val){
  return (uint8_t)( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
int bcdToDec(uint8_t val){
  return (int)( (val/16*10) + (val%16) );
}

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
} TIME;

TIME time;
void Set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t set_time[7];
	set_time[0] = decToBcd(sec);
	set_time[1] = decToBcd(min);
	set_time[2] = decToBcd(hour);
	set_time[3] = decToBcd(dow);
	set_time[4] = decToBcd(dom);
	set_time[5] = decToBcd(month);
	set_time[6] = decToBcd(year);

	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}

void Get_Time (void)
{
	HAL_Delay(1);
	uint8_t est_rtc;
	uint8_t conteo = 0;
	est_rtc = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 17, 200);
	while(est_rtc != HAL_OK){
		est_rtc = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 17, 50);
		conteo++;
		if(conteo > 2) {
			I2CResetBus();
			conteo = 0;
		}
	}
	HAL_Delay(1);
	time.seconds = bcdToDec(get_time[0]);
	time.minutes = bcdToDec(get_time[1]);
	time.hour = bcdToDec(get_time[2]);
	time.dayofweek = bcdToDec(get_time[3]);
	time.dayofmonth = bcdToDec(get_time[4]);
	time.month = bcdToDec(get_time[5]);
	time.year = bcdToDec(get_time[6]);
}
char buffer1[11];


void pcf1_send (uint8_t data){
	uint8_t dato1[1];
	dato1[0]= data;
	HAL_I2C_Master_Transmit(&hi2c1, Address_PCF1 , dato1, 1, 100);
}

void pcf2_send (uint8_t data){
	uint8_t dato2[1];
	dato2[0]= data;
	HAL_I2C_Master_Transmit(&hi2c1, Address_PCF2 , dato2, 1, 100);
}

void Pulsadores(void){
	est_right 	= HAL_GPIO_ReadPin(entrada1_GPIO_Port, entrada1_Pin);
	est_left 	= HAL_GPIO_ReadPin(entrada2_GPIO_Port, entrada2_Pin);
	est_down 	= HAL_GPIO_ReadPin(entrada3_GPIO_Port, entrada3_Pin);
	est_up 		= HAL_GPIO_ReadPin(entrada4_GPIO_Port, entrada4_Pin);

	//-------Configuracion de PULSADOR UP-------
	if((seg_up == 1)&&(est_up == 0)){
		seg_up = 0;
	}
	if((seg_up == 1)&&(est_up == 1)){
	    est_up = 0;
	}
	if((seg_up == 0)&&(est_up == 1)){
	    est_up = 1;
	    seg_up = 1;
	}
	//-------Configuracion de PULSADOR DOWN-------
	if((seg_down == 1)&&(est_down == 0)){
	    seg_down = 0;
	}
	if((seg_down == 1)&&(est_down == 1)){
	    est_down = 0;
	}
	if((seg_down == 0)&&(est_down == 1)){
	    est_down = 1;
	    seg_down = 1;
	}
	//-------Configuracion de PULSADOR LEFT-------
	if((seg_left == 1)&&(est_left == 0)){
	    seg_left = 0;
	}
	if((seg_left == 1)&&(est_left == 1)){
	    est_left = 0;
	}
	if((seg_left == 0)&&(est_left == 1)){
	    est_left = 1;
	    seg_left = 1;
	}
	//-------Configuracion de PULSADOR RIGHT-------
	if((seg_right == 1)&&(est_right == 0)){
	    seg_right = 0;
	}
	if((seg_right == 1)&&(est_right == 1)){
	    est_right = 0;
	}
	if((seg_right == 0)&&(est_right == 1)){
	    est_right = 1;
	    seg_right = 1;
	}
}

void Lcd_Cursor(void){
  //--------- EST_DOWN ---------
  if(est_down == 1){
    if(g_menu == 5){
      pag_menu = 5;
      paso--;
      if(paso<1){paso = 1;}
    }
    if(g_menu == 6){
      pag_menu = 6;
      paso--;
      if(paso<1){paso = 1;}
    }
    if(g_menu == 8){
      pag_menu = 8;
      paso--;
      if(paso>250){paso=0;}
    }
    if(cursor_fila==1){
      switch(g_menu){
        case 1: pag_menu = 2; cursor_fila = 0; break;
        case 2: pag_menu = 3; cursor_fila = 0; break;
        case 4: pag_menu = 4; cursor_fila = 0; paso = paso+2;
                if((paso>16)&&((pag_anterior == 1)||(pag_anterior == 2))){paso = 15; cursor_fila = 1;}
                if((paso >4)&&(pag_anterior == 3)){paso = 3; cursor_fila = 1;}
        break;
      }
    }
    else{ cursor_fila=1; }
    //--------- EST_UP ---------
  }else if(est_up == 1){
    if(g_menu == 5){
      pag_menu = 5;
      paso++;
      if(paso>16){paso = 16;}
    }
    if(g_menu == 6){
      pag_menu = 6;
      paso++;
      if(paso>3){paso = 3;}
    }
    if(g_menu == 8){
      pag_menu = 8;
      paso++;
      if(paso>=16){paso=16;}
    }
    if(cursor_fila == 0){
      switch(g_menu){
        case 2: pag_menu = 1; cursor_fila = 1; break;
        case 3: pag_menu = 2; cursor_fila = 1; break;
        case 4: pag_menu = 4; cursor_fila = 1; paso = paso-2;
              if((paso<1)||(paso > 250)){paso = 1; cursor_fila = 0;}
        break;
      }
    }
    else{cursor_fila=0;}
  }
  //--------- EST_RIGHT ---------
  if(est_right == 1){
    switch(g_menu){
      case 0:
            switch(cursor_fila){
              case 0: pag_menu = 1; break;
              case 1: pag_menu = 8; break;
            }
      break;
      case 1:
            switch(cursor_fila){
              case 0: pag_menu = 4; pag_anterior = 1;if(P_min % 2 == 0){paso = P_min -1; cursor_fila = 1;}else{paso = P_min; cursor_fila = 0;} break;  /*INSTRUCCION POTENCIA MIN*/
              case 1: pag_menu = 4; cursor_fila = 0; pag_anterior = 2; if(P_max % 2 == 0){paso = P_max -1; cursor_fila = 1;}else{paso = P_max; cursor_fila = 0;} break; /*INSTRUCCION POTENCIA MAX*/
            }
      break;
      case 2:
            switch(cursor_fila){
              case 0: pag_menu = 4; pag_anterior = 3; if(P_pas % 2 == 0){paso = P_pas -1; cursor_fila = 1;}else{paso = P_pas; cursor_fila = 0;} break; /*INSTRUCCION POTENCIA PASO*/
              case 1: pag_menu = 5; if(tiempo<5){paso = tiempo;}else if(tiempo >= 5 ){paso = (tiempo*0.2)+4;} break; /*INSTRUCCION TIEMPO PASO*/
            }
      break;
      case 3:
            switch(cursor_fila){
              case 0: pag_menu = 6; paso = P_modo; break; /*INSTRUCCION MODO ARRANQUE*/
              case 1: pag_menu = 7; break; /*INSTRUCCION INICIAR*/
            }
      break;
      case 4:
          switch(pag_anterior){
            case 1: if(cursor_fila == 0){P_min = paso;}else{P_min = paso+1;} pag_menu = 1; cursor_fila = 0;
            break;
            case 2: if(cursor_fila == 0){P_max = paso;}else{P_max = paso+1;} pag_menu = 1; cursor_fila = 1;
            break;
            case 3: if(cursor_fila == 0){P_pas = paso;}else{P_pas = paso+1;} pag_menu = 2; cursor_fila = 0;
            break;
          }
      break;
      case 5: pag_menu = 2; cursor_fila = 1;
      break;
      case 6: P_modo = paso; pag_menu = 3; cursor_fila = 0;
      break;
      case 8: lcd_clear();menu_conf = 0;menu_manu = 1;
              direccion = 2*paso;
              pcf1_send(salidaReles[direccion]);
              pcf2_send(salidaReles[direccion+1]);
              lcd_put_cur(0, 0);
              lcd_send_string("POT: ");
              uint16_t valor1 = 75*paso;
              uint8_t ent_valor1 = valor1*0.1;
              uint8_t un_valor1 = valor1 - (ent_valor1 * 10);
              sprintf(buffer,"%d",ent_valor1);
              lcd_send_string(buffer);
              //lcd.print(ent_valor1);
              lcd_send_string(".");
              sprintf(buffer,"%d",un_valor1);
              lcd_send_string(buffer);
              //lcd.print(un_valor1);
              lcd_send_string(" Kw");
              HAL_GPIO_WritePin(modbus_GPIO_Port, modbus_Pin, 1);
              h_envio = 1;
              h_calculo = 0;
              HAL_UART_Transmit_DMA(&huart3, mensaje1, 8);

      break;
    }
    //--------- EST_LEFT ---------
  }else if(est_left == 1){
    switch(g_menu){
      case 1: pag_menu = 0; cursor_fila = 0; break;
      case 2: pag_menu = 0; cursor_fila = 0; break;
      case 3: pag_menu = 0; cursor_fila = 0; break;
      case 4: switch(pag_anterior){
                case 1: pag_menu = 1; cursor_fila = 0;break;
                case 2: pag_menu = 1; cursor_fila = 1;break;
                case 3: pag_menu = 2; cursor_fila = 0;break;
              }
      break;
      case 5: pag_menu = 2; cursor_fila = 1; break;
      case 6: pag_menu = 3; cursor_fila = 0; break;
      case 7: pag_menu = 3; cursor_fila = 1; break;
      case 8: pag_menu = 0; cursor_fila = 1;
      	  pcf1_send(salidaReles[0]);
      	  pcf2_send(salidaReles[1]);
      break;
    }
  }
  //--------- CURSOR FILA ---------
  if(g_menu < 5){
	lcd_put_cur(!cursor_fila, 0);
    lcd_send_string(" ");
    lcd_put_cur(cursor_fila, 0);
    lcd_write(0);
  } else if(g_menu == 5){
	  lcd_put_cur(1, 0);
    if(paso == 1){lcd_write(2);}
    else if(paso == 16){lcd_write(3);}
    else {lcd_write(1);}
  }

}

void Escritura_SD(void){
	fresult = f_open(&fil, "DATOS.csv", FA_OPEN_ALWAYS | FA_WRITE);
	fresult = f_lseek(&fil, fil.fsize);
	sprintf (buffer1, "%02d/%02d/%04d", time.dayofmonth, time.month, 2000 + time.year);
	f_puts(buffer1, &fil);
	f_puts(",", &fil);
	sprintf (buffer1, "%02d:%02d:%02d", time.hour, time.minutes, time.seconds);
	f_puts(buffer1, &fil);
	f_puts(",", &fil);
	for(uint8_t z=0 ; z<3 ; z++){
		sprintf (buffer1, "%d", Tension[z]);
		f_puts(buffer1, &fil);
		f_puts(",", &fil);
	}
	for(uint8_t x=0 ; x<3 ; x++){
		sprintf (buffer1, "%d", Corriente[x]);
		f_puts(buffer1, &fil);
		f_puts(",", &fil);
	}
	for(uint8_t y=0 ; y<3 ; y++){
		if(y==2){
			sprintf (buffer1, "%d", Potencia[y]);
			f_puts(buffer1, &fil);
			f_puts("\n", &fil);
		}else{
			sprintf (buffer1, "%d", Potencia[y]);
			f_puts(buffer1, &fil);
			f_puts(",", &fil);
		}
	}
	/* Close file */
	fresult = f_close(&fil);
}

/*
void verificadorCRC(void){
	crcinicio = 0xFFFF;
	for(uint8_t z0 = 0; z0 < 15 ; z0++){
		crcinicio ^= recepcion[z0];
		for(uint8_t z1 = 0; z1 < 8 ; z1++){
			if((crcinicio & 0x0001) != 0){
				crcinicio >>= 1;
				crcinicio ^= 0xA001;
			}else{
				crcinicio >>= 1;
			}
		}
	}
	uint16_t ver1 = crcinicio;
	uint16_t ver2 = crcinicio;
	ver2 >>= 8;
	ver1 &= 0xFF;
	crc2= ver2;
	crc1= ver1;
	if((crc1 == recepcion[15])&&(crc2 == recepcion[16])){
		devolucion = 1;
	}else{
		devolucion = 0;
		fresult = f_open(&fil, "Error.txt", FA_OPEN_ALWAYS | FA_WRITE);
		f_lseek(&fil, fil.fsize);
		for(uint8_t z2 = 0; z2<17 ; z2++){
			sprintf (buffer1, "%d", recepcion[z2]);
			f_puts(buffer1, &fil);
			f_puts("\n", &fil);
		}
		sprintf (buffer1, "%d:%d", crc1, crc2);
		f_puts(buffer1, &fil);
		f_puts("\n", &fil);
		f_close(&fil);
	}
}



void Calculo_modbus(void){
	for(uint8_t vuelta=3;vuelta < 12; vuelta+=4){
		dato_a = recepcion[vuelta];
		dato_b = recepcion[1+vuelta];
		dato_c = recepcion[2+vuelta];
		dato_d = recepcion[3+vuelta];
		ex = 0;
		dato_a &= 0x7F;
		ex = ((dato_a*0x100)+dato_b)>>7;
		ex = ex-126;
		if(((dato_b>>7)&(0x1))== 1){
			mantisa = dato_b*0x10000;
			mantisa = mantisa + dato_c*0x100;
			mantisa = mantisa + dato_d;
		}else{
			mantisa = 0x800000;
			mantisa = mantisa + dato_b*0x10000;
			mantisa = mantisa + dato_c*0x100;
			mantisa = mantisa + dato_d;
		}
		if(ex <= 24 ){
			mantisa = mantisa/ pow(2,24-ex);
		}
		ubicacion = (vuelta+1) / 4;
		ubicacion -= 1;
		if(mensaje == 1){
			Tension[ubicacion] = mantisa;
		}else if(mensaje == 2){
			Corriente[ubicacion] = mantisa;
		}else if(mensaje == 3){
			mantisa /= 1000;
			Potencia[ubicacion] = mantisa;
		}
	}
}

void Modbus(void){
	if(HAL_UART_Receive_IT(&huart3, recepcion, 17) == HAL_OK){
		Get_Time();

		uint8_t errormensaje[1];
		while(HAL_UART_Receive(&huart3, errormensaje, 1, 10) == HAL_OK){}
		Calculo_modbus();
		mensaje++;
		if(mensaje == 4) {
			mensaje = 1;
			Enviar_Mensaje(mensaje);
			Escritura_SD();
			lcd_put_cur(1, 0);
			sprintf(buffer, "%6d", Potencia[0]);
			lcd_send_string(buffer);
		}else{
			Enviar_Mensaje(mensaje);
		}
		for (int i=0; i<17; i++) recepcion[i] = '\0';

		verificadorCRC();
		  if(devolucion == 1){
			lcd_put_cur(1, 8);
			lcd_send_string("C");
			Calculo_modbus();
			mensaje++;
			if(mensaje > 3) {
				mensaje= 1;
				Enviar_Mensaje(mensaje);
				Escritura_SD();
				lcd_put_cur(1, 0);
				sprintf(buffer, "%6d", Potencia[0]);
				lcd_send_string(buffer);
			}else{
				Enviar_Mensaje(mensaje);
			}
		}else{
			lcd_put_cur(1, 9);
			lcd_send_string("I");
			Enviar_Mensaje(mensaje);
		}

	}
}
*/

void Calculo_modbus(uint8_t mensaje){
	for(uint8_t vuelta=3;vuelta < 12; vuelta+=4){
		if(mensaje == 1){
			dato_a = M_Tension[vuelta];
			dato_b = M_Tension[1+vuelta];
			dato_c = M_Tension[2+vuelta];
			dato_d = M_Tension[3+vuelta];
		}else if(mensaje == 2){
			dato_a = M_Corriente[vuelta];
			dato_b = M_Corriente[1+vuelta];
			dato_c = M_Corriente[2+vuelta];
			dato_d = M_Corriente[3+vuelta];
		}else if(mensaje == 3){
			dato_a = M_Potencia[vuelta];
			dato_b = M_Potencia[1+vuelta];
			dato_c = M_Potencia[2+vuelta];
			dato_d = M_Potencia[3+vuelta];
		}
		ex = 0;
		dato_a &= 0x7F;
		ex = ((dato_a*0x100)+dato_b)>>7;
		ex = ex-126;
		if(((dato_b>>7)&(0x1))== 1){
			mantisa = dato_b*0x10000;
			mantisa = mantisa + dato_c*0x100;
			mantisa = mantisa + dato_d;
		}else{
			mantisa = 0x800000;
			mantisa = mantisa + dato_b*0x10000;
			mantisa = mantisa + dato_c*0x100;
			mantisa = mantisa + dato_d;
		}
		if(ex <= 24 ){
			mantisa = mantisa/ pow(2,24-ex);
		}
		ubicacion = (vuelta+1) / 4;
		ubicacion -= 1;
		if(mensaje == 1){
			Tension[ubicacion] = mantisa;
		}else if(mensaje == 2){
			Corriente[ubicacion] = mantisa;
		}else if(mensaje == 3){
			mantisa /= 1000;
			Potencia[ubicacion] = mantisa;
		}
	}
}


void Procesado(){
	if((h_calculo & 0x01)== 0x01){
		Calculo_modbus(1);
		h_calculo ^= 0x01;
	}else if((h_calculo & 0x02)== 0x02){
		Calculo_modbus(2);
		h_calculo ^= 0x02;
	}else if((h_calculo & 0x04)== 0x04){
		Calculo_modbus(3);

		h_calculo ^= 0x04;
	}
}

void MuestraTiempo(){
	sprintf(buffer, "%02d:%02d:%02d", time.hour, time.minutes, time.seconds);
	compsegundo = time.seconds;
	lcd_put_cur(1, 0);
	lcd_send_string(buffer);
}

void Automatico(void){
	if(time.seconds != compsegundo) {MuestraTiempo(); Escritura_SD();}
	Procesado();
  //Modbus();
  //lcd_put_cur(1, 0);
  //lcd.print(Potencia[0]);lcd_send_string("W      ");
  if(leer == 1){
	pcf1_send(salidaReles[direccion]);
	pcf2_send(salidaReles[direccion+1]);
    uint16_t valor1 = 75*P_sig;
    uint8_t ent_valor1 = valor1*0.1;
    uint8_t un_valor1 = valor1 - (ent_valor1 * 10);
    lcd_put_cur(0, 0);
    lcd_send_string("ACTUAL: ");
    sprintf(buffer,"%d",ent_valor1);
    lcd_send_string(buffer);//lcd.print(ent_valor1);
    lcd_send_string(".");
    sprintf(buffer,"%d",un_valor1);
    lcd_send_string(buffer);//lcd.print(un_valor1);
    lcd_send_string("Kw");
    set_segundo = time.seconds;
    set_minuto = time.minutes;
    set_minuto = set_minuto + tiempo;
    set_hors = time.hour;
    if(set_minuto >= 60){
      set_minuto = set_minuto - 60;
      set_hors = set_hors + 1;
    }
    leer = 0;
    if(ultimo == 0){
      if(P_modo == 1){
          P_sig = P_sig + P_pas;
          direccion = 2*P_sig;
          if(P_sig >= P_max){
            P_sig = P_max;
            direccion = 2*P_sig;
            ultimo = 1;
          }
      }else if(P_modo == 2){
          P_sig = P_sig - P_pas;
          direccion = 2*P_sig;
          if((P_sig<=P_min)||(P_sig>240)){
            P_sig = P_min;
            direccion = 2*P_sig;
            ultimo = 1;
          }
      }else if(P_modo == 3){
          if(P_modo_franco == 1){
              P_sig = P_sig + P_pas;
              direccion = 2*P_sig;
              if(P_sig >= P_max){
                P_sig = P_max;
                direccion = 2*P_sig;
                P_modo_franco = 0;
              }
          }else if(P_modo_franco == 0){
              P_sig = P_sig - P_pas;
              direccion = 2*P_sig;
              if((P_sig<=P_min)||(P_sig>240)){
                P_sig = P_min;
                direccion = 2*P_sig;
                ultimo = 1;
              }
          }
      }
    }else{fin = 1;}
  }
  if((time.seconds== set_segundo)&&(time.minutes== set_minuto)&&(time.hour == set_hors)){
    //Vuelve a repetir el leer
    if(fin == 1){
      menu_conf = 1;
      menu_auto = 0;
      menu_manu = 0;
      pag_menu = 0;
      g_menu = 0;
      m_cursor = 0;
      cursor_fila = 0;
      paso = 1;
      pag_anterior = 0;
      pcf1_send(salidaReles[0]);
      pcf2_send(salidaReles[1]);
      lcd_clear();
      lcd_put_cur(0, 0);
      lcd_send_string("FIN DE SECUENCIA");
      HAL_Delay(2000);
    }else{
      leer = 1;
    }
  }
}




void Manual(void){

	Procesado();
  //Modbus();
  //lcd_put_cur(1, 0);
  //lcd.print(Potencia[0]);lcd_send_string("W      ");

  if(est_left == 1){
      menu_conf = 1;
      menu_auto = 0;
      menu_manu = 0;
      pag_menu = 8;
      g_menu = 8;
  }
}



void Lcd_Menu(void){
  if(pag_menu == 0){
      lcd_clear();
      lcd_put_cur(0, 1);
      lcd_send_string("AUTOMATICO");
      lcd_put_cur(1, 1);
      lcd_send_string("MANUAL");
  }else if(pag_menu == 1){
      lcd_clear();
      lcd_put_cur(0, 1);
      lcd_send_string("POTENCIA MIN");
      lcd_put_cur(1, 1);
      lcd_send_string("POTENCIA MAX");
      lcd_put_cur(1, 15);
      lcd_write(3);
  }else if(pag_menu == 2){
      lcd_clear();
      lcd_put_cur(0, 15);
      lcd_write(2);
      lcd_put_cur(0, 1);
      lcd_send_string("POTENCIA PASO");

      lcd_put_cur(1, 1);
      lcd_send_string("TIEMPO PASO");
      lcd_put_cur(1, 15);
      lcd_write(3);
  }else if(pag_menu == 3){
      lcd_clear();
      lcd_put_cur(0, 15);
      lcd_write(2);
      lcd_put_cur(0, 1);
      lcd_send_string("MODO ARRANQUE");
      lcd_put_cur(1, 1);
      lcd_send_string("INICIAR");
  }else if(pag_menu == 4){
	  uint16_t valor1 = 75*paso;
      uint16_t valor2 = 75*(paso+1);
      uint8_t ent_valor1 = valor1*0.1;
      uint8_t ent_valor2 = valor2*0.1;
      uint8_t un_valor1 = valor1 - (ent_valor1 * 10);
      uint8_t un_valor2 = valor2 - (ent_valor2 * 10);
      lcd_clear();
      lcd_put_cur(0, 1);
      sprintf(buffer,"%d",ent_valor1);
      lcd_send_string(buffer);//lcd.print(ent_valor1);
      lcd_send_string(".");
      sprintf(buffer,"%d",un_valor1);
      lcd_send_string(buffer);//lcd.print(un_valor1);
      lcd_put_cur(1, 1);
      sprintf(buffer,"%d",ent_valor2);
      lcd_send_string(buffer);//lcd.print(ent_valor2);
      lcd_send_string(".");
      sprintf(buffer,"%d",un_valor2);
      lcd_send_string(buffer);//lcd.print(un_valor2);
  }else if(pag_menu == 5){
      if(paso<5){
        tiempo = paso;
      }else if(paso >= 5 ){
        tiempo = 5*(paso-4);
      }
      lcd_clear();
      lcd_put_cur(0, 1);
      lcd_send_string("TIEMPO POR PASO");
      lcd_put_cur(1, 1);
      sprintf(buffer,"%d",tiempo);
      lcd_send_string(buffer);//lcd.print(tiempo);
      lcd_send_string(" min");
  }else if(pag_menu == 6){
      lcd_clear();
      lcd_put_cur(0, 0);
      lcd_send_string("MODO DE ARRANQUE");
      lcd_put_cur(1, 0);
      switch(paso){
        case 1: lcd_write(2);
        	lcd_send_string("MIN-MAX");
        break;
        case 2: lcd_write(1);
        	lcd_send_string("MAX-MIN");
        break;
        case 3: lcd_write(3);
        	lcd_send_string("MIN-MAX-MIN");
        break;
      }
  }else if(pag_menu == 7){
    lcd_clear();
    menu_conf = 0;
    leer = 1;
    menu_auto = 1;
    ultimo = 0;
    fin = 0;
    if(P_modo == 1){
      P_sig = P_min;
      direccion = 2*P_min;
    }else if(P_modo == 2){
      P_sig = P_max;
      direccion = 2*P_max;
    }else if(P_modo == 3){
      P_sig = P_min;
      direccion = 2*P_min;
      P_modo_franco = 1;
    }
    compsegundo = time.seconds + 20;
    HAL_GPIO_WritePin(modbus_GPIO_Port, modbus_Pin, 1);
    h_envio = 1;
    h_calculo = 0;
    HAL_UART_Transmit_DMA(&huart3, mensaje1, 8);

  }else if(pag_menu == 8){
    uint16_t valor1 = 75*paso;
    uint8_t ent_valor1 = valor1*0.1;
    uint8_t un_valor1 = valor1 - (ent_valor1 * 10);
    lcd_clear();
    lcd_put_cur(0, 0);
    if(paso == 0){
    	lcd_write(2);
    }else if(paso == 16){
    	lcd_write(3);
    }else{lcd_write(1);}
    lcd_put_cur(0, 1);
    lcd_send_string("POTENCIA: ");
    sprintf(buffer,"%d",ent_valor1);
    lcd_send_string(buffer);//lcd.print(ent_valor1);
    lcd_send_string(".");
    sprintf(buffer,"%d",un_valor1);
    lcd_send_string(buffer);//lcd.print(un_valor1);
  }

  if(pag_menu != 254){
    g_menu = pag_menu;
  }
  pag_menu = 254;
}

void Ajuste(void){
  Lcd_Menu();
  Lcd_Cursor();
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  /*------------------------- INICIO DE LA PROGRAMACION -------------------------*/

  pcf1_send(0xFF);
  pcf2_send(0xFF);


  lcd_init();
  HAL_Delay (1000);
  lcd_create_character(0, customChar);
  lcd_create_character(1, ambos);
  lcd_create_character(2, arriba);
  lcd_create_character(3, abajo);
  lcd_put_cur(0, 0);
  lcd_send_string("Iniciando SD ...");
  HAL_Delay(1000);
  lcd_clear();
  fresult = f_mount(&fs, "/", 1);
  while(fresult != FR_OK) {
	  f_mount(NULL, "/", 1);
	  HAL_Delay(50);
	  fresult = f_mount(&fs, "/", 1);
	  lcd_put_cur(0, 0);
	  lcd_send_string("Inserte SD");
  }
  lcd_put_cur(0, 0);
  lcd_send_string("SD iniciado");
  HAL_Delay(1000);
  lcd_clear();
  fresult = f_open(&fil, "DATOS.csv", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
  f_close(&fil);
  if(fresult != FR_OK){
	  fresult = f_open(&fil, "DATOS.csv", FA_OPEN_ALWAYS | FA_WRITE);
	  /* Writing text */
	  fresult = f_puts("FECHA,HORA,TENSION A,TENSION B,TENSION C,CORRIENTE A,CORRIENTE B,CORRIENTE C,POTENCIA A,POTENCIA B,POTENCIA C", &fil);
	  f_puts("\n", &fil);
	  /* Close file */
	  fresult = f_close(&fil);
  }

  //HAL_TIM_Base_Start(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  Pulsadores();
	  Get_Time();
	  if(menu_conf == 1){
		  Ajuste();
	  }
	  if(menu_auto == 1){
	      Automatico();
	  }
	  if(menu_manu == 1){
	      Manual();
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
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 1-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|modbus_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 modbus_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|modbus_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : entrada4_Pin entrada2_Pin entrada1_Pin */
  GPIO_InitStruct.Pin = entrada4_Pin|entrada2_Pin|entrada1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : entrada3_Pin */
  GPIO_InitStruct.Pin = entrada3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(entrada3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	HAL_GPIO_WritePin(modbus_GPIO_Port, modbus_Pin, 0);
	if(h_envio == 1){
			HAL_UART_Receive_DMA(&huart3, M_Tension, 17);
	}else if(h_envio == 2){
			HAL_UART_Receive_DMA(&huart3, M_Corriente, 17);
	}else if(h_envio == 3){
			HAL_UART_Receive_DMA(&huart3, M_Potencia, 17);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_GPIO_WritePin(modbus_GPIO_Port, modbus_Pin, 1);
	if(h_envio == 1){
		h_envio = 2;
		h_calculo |= 0x01;
		HAL_UART_Transmit_DMA(&huart3, mensaje2, 8);
	}else if(h_envio == 2){
		h_envio = 3;
		h_calculo |= 0x02;
		HAL_UART_Transmit_DMA(&huart3, mensaje3, 8);
	}else if(h_envio == 3){
		h_envio = 1;
		h_calculo |= 0x04;
		HAL_UART_Transmit_DMA(&huart3, mensaje1, 8);
	}
}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
