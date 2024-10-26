#include "i2c_stub.h"

void Error_Handler();

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;

void I2C_MspInit()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	/* USER CODE BEGIN I2C2_MspInit 0 */

	/* USER CODE END I2C2_MspInit 0 */

	/** Initializes the peripherals clock
	*/
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
	PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_RCC_GPIOF_CLK_ENABLE();
	/**I2C2 GPIO Configuration
	PF0         ------> I2C2_SDA
	PF1         ------> I2C2_SCL
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* Peripheral clock enable */
	__HAL_RCC_I2C2_CLK_ENABLE();
	/* USER CODE BEGIN I2C2_MspInit 1 */

	/* USER CODE END I2C2_MspInit 1 */
}

#define I2C_TIMING_SM                     0x30E0628A
#define I2C_TIMING_FM                     0x20D01132
#define I2C_TIMING_FMP                    0x1080091A

void i2c_begin()
{
	I2C_MspInit();

	/* USER CODE BEGIN I2C2_Init 0 */

	/* USER CODE END I2C2_Init 0 */

	/* USER CODE BEGIN I2C2_Init 1 */

	/* USER CODE END I2C2_Init 1 */
	I2cHandle.Instance = I2C2;
	I2cHandle.Init.Timing = I2C_TIMING_SM;
	I2cHandle.Init.OwnAddress1 = 0;
	I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.OwnAddress2 = 0;
	I2cHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Analogue filter
	*/
	if (HAL_I2CEx_ConfigAnalogFilter(&I2cHandle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Digital filter
	*/
	if (HAL_I2CEx_ConfigDigitalFilter(&I2cHandle, 0) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C2_Init 2 */

	/* USER CODE END I2C2_Init 2 */
}

/* TODO: sendStop */
uint8_t i2c_endTransmission(bool sendStop)
{
	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY) {}
	/* Did acknowledge failure occur? */
	return (HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);
}

uint8_t i2c_read(uint8_t addr)
{
	uint8_t data = 0;
	/* Try receiving */
	if (HAL_I2C_Master_Receive(&I2cHandle, addr << 1, (uint8_t *)&data, sizeof(data), 1000) != HAL_OK)
		Error_Handler();

	/* On receive success, wait for end of transfer */
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY) {}
	return data;
}

uint16_t i2c_read2(uint8_t addr)
{
	uint16_t data = 0;
	/* Try receiving */
	if (HAL_I2C_Master_Receive(&I2cHandle, addr << 1, (uint8_t *)&data, sizeof(data), 1000) != HAL_OK)
		Error_Handler();

	/* On receive success, wait for end of transfer */
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY) {}
	return data;
}

uint32_t i2c_write(uint8_t addr, uint8_t data)
{
	/* Try transmitting */
	if (HAL_I2C_Master_Transmit(&I2cHandle, addr << 1, (uint8_t*)&data, sizeof(data), 1000) != HAL_OK)
		Error_Handler();

	/* On transmission success, wait for end of transfer */
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY) {}
	return 0;
}
uint32_t i2c_write2(uint8_t addr, uint8_t data1, uint8_t data2)
{
	uint8_t data[2];
	data[0] = data1;
	data[1] = data2;
	do
	{
		/* Try transmitting */
		if (HAL_I2C_Master_Transmit(&I2cHandle, addr << 1, (uint8_t*)&data, sizeof(data), 1000) != HAL_OK)
			Error_Handler();

		/* On transmission success, wait for end of transfer */
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY) {}
	}
	while (0);
	return 0;
}

bool i2c_deviceReady(uint8_t addr) {
	return HAL_I2C_IsDeviceReady(&I2cHandle, (addr << 1), 1, 1000) != HAL_OK;
}
