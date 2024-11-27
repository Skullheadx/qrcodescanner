#include "main.h"
#include <vector>
#include "stm32f4xx_hal.h"
#include <iostream>
// I2C and DMA Handles
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma;
const int WIDTH{50};
const int HEIGHT{50};

// Image Data Buffer
uint8_t imageData[WIDTH * HEIGHT * 2]; // Adjust the size as needed (YUV422: 2 bytes per pixel)
std::vector<std::vector<uint8_t>> grayscaleImage(HEIGHT, std::vector<uint8_t>(WIDTH)); // 2D vector to store Y values

// Function Prototypes
void SystemClock_Config(void);
void GPIO_Init(void);
void I2C_Init(void);
void DMA_Init(void);
void captureImage(void);
void processImage(void);
void Error_Handler(void){
	while(1){
		std::cout<<"error has occurred";
	}
};



int main(void) {
    // Initialize the HAL Library
    HAL_Init();

    // Configure the system clock
    SystemClock_Config();

    // Initialize GPIO, I2C, and DMA
    GPIO_Init();
    I2C_Init();
    DMA_Init();

    // Reset the camera module
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); // Assert reset
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // Deassert reset
    HAL_Delay(100);

    // Main loop
    while (1) {
        captureImage();
        processImage();
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

void GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure HREF (PA0) and VSYNC (PA1) as inputs
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure PWDN (PA2) and RST (PA3) as outputs
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure DCLK (PA6) as input
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // Adjust as necessary
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure SDA (PB9) and SCL (PB8) for I2C
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure D0-D7 (PB0-PB7) as inputs
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void I2C_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
}

void DMA_Init(void) {
    __HAL_RCC_DMA1_CLK_ENABLE();

    // Configure DMA for I2C TX
    hdma_i2c1_tx.Instance = DMA1_Stream6;
    hdma_i2c1_tx.Init.Channel = DMA_CHANNEL_1;
    hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK) {
        Error_Handler();
    }

    // Link DMA to I2C TX
    __HAL_LINKDMA(&hi2c1, hdmatx, hdma_i2c1_tx);

    // Configure DMA for I2C RX
    hdma_i2c1_rx.Instance = DMA1_Stream0;
    hdma_i2c1_rx.Init.Channel = DMA_CHANNEL_1;
    hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK) {
        Error_Handler();
    }

    // Link DMA to I2C RX
    __HAL_LINKDMA(&hi2c1, hdmarx, hdma_i2c1_rx);
}
void captureImage(void) {
    // Enable PWDN pin to power up the camera module
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_Delay(10); // Ensure the camera module is powered up

    // Wait for VSYNC to go high to start a new frame
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET);

    int row = 0, col = 0;
    // Start capturing image data line by line
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) { // While VSYNC is high
        while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) { // While HREF is high (valid data lines)
            // Read data from D0-D7 on GPIOB
            uint8_t pixelData =
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) << 0) |
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) << 1) |
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) << 2) |
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) << 3) |
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) << 4) |
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) << 5) |
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) << 6) |
                (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) << 7);

            // Store the pixel data in the image buffer
            imageData[row * WIDTH + col] = pixelData;

            // Increment the column count
            col++;
            if (col >= WIDTH) {
                col = 0;
                row++;
            }

            // Ensure row doesn't exceed the image size
            if (row >= HEIGHT) {
                break;
            }
        }
    }
}




void processImage(void) {
    int index = 0;

    // Iterate through each row and column of the image buffer
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            // Extract the Y (luminance) value from the YUV422 format
            uint8_t Y = imageData[index];

            // Store the Y value in the grayscaleImage 2D vector
            grayscaleImage[row][col] = Y;
            std::cout<<grayscaleImage[row][col];
            // Move to the next pixel (Y component is every other byte in YUV422)
            index += 2;
        }
        std::cout<<std::endl;
    }
}


