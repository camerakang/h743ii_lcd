#include "main.h"
#include "led.h"
#include "usart.h"
#include "sdram.h"  

#define SUCCESS 0
#define ERROR 1

uint8_t SDRAM_Test(void)
{
    uint32_t i = 0;         
    uint16_t ReadData = 0;  
    uint8_t  ReadData_8b;

    uint32_t ExecutionTime_Begin;        
    uint32_t ExecutionTime_End;      
    uint32_t ExecutionTime;              
    float    ExecutionSpeed;            

    printf("\r\n*****************************************************************************************************\r\n");       
    printf("\r\nSpeed test in progress>>>\r\n");

    // Writing >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    ExecutionTime_Begin = HAL_GetTick();  

    for (i = 0; i < SDRAM_Size/2; i++)
    {
        *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*i) = (uint16_t)i;  
    }
    ExecutionTime_End = HAL_GetTick();                                            
    ExecutionTime = ExecutionTime_End - ExecutionTime_Begin;               
    ExecutionSpeed = (float)SDRAM_Size /1024/1024 /ExecutionTime*1000;  
    
    printf("\r\nWritten with 16-bit data width, Size: %d MB, Time: %d ms, Speed: %.2f MB/s\r\n", SDRAM_Size/1024/1024, ExecutionTime, ExecutionSpeed);

    // Reading >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 

    ExecutionTime_Begin = HAL_GetTick();  

    for(i = 0; i < SDRAM_Size/2; i++)
    {
        ReadData = *(__IO uint16_t*)(SDRAM_BANK_ADDR + 2 * i);  
    }
    ExecutionTime_End = HAL_GetTick();                                            
    ExecutionTime = ExecutionTime_End - ExecutionTime_Begin;               
    ExecutionSpeed = (float)SDRAM_Size /1024/1024 /ExecutionTime*1000;  
    
    printf("\r\nReading complete, Size: %d MB, Time: %d ms, Speed: %.2f MB/s\r\n", SDRAM_Size/1024/1024, ExecutionTime, ExecutionSpeed);
    
    // Data verification >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   
        
    printf("\r\n*****************************************************************************************************\r\n");       
    printf("\r\nData verification in progress>>>\r\n");
    
    for(i = 0; i < SDRAM_Size/2; i++)
    {
        ReadData = *(__IO uint16_t*)(SDRAM_BANK_ADDR + 2 * i);  
        if(ReadData != (uint16_t)i)    
        {
            printf("\r\nSDRAM test failed!!\r\n");
            return ERROR;  
        }
    }
    
    printf("\r\n16-bit data width read and write passed, writing with 8-bit data width\r\n");
    for (i = 0; i < 255; i++)
    {
        *(__IO uint8_t*) (SDRAM_BANK_ADDR + i) = (uint8_t)i;
    }   
    printf("Write complete, reading data and comparing...\r\n");
    for (i = 0; i < 255; i++)
    {
        ReadData_8b = *(__IO uint8_t*) (SDRAM_BANK_ADDR + i);
        if(ReadData_8b != (uint8_t)i)    
        {
            printf("8-bit data width read and write test failed!!\r\n");
            printf("Please check the connection of NBL0 and NBL1\r\n");  
            return ERROR;  
        }
    }       
    printf("8-bit data width read and write passed\r\n");
    printf("SDRAM read and write test passed, system is normal\r\n");
    return SUCCESS;  
}

void SystemClock_Config(void);     
void MPU_Config(void);              

int main(void)
{
    MPU_Config();                
    SCB_EnableICache();     
    SCB_EnableDCache();     
    HAL_Init();                 
    SystemClock_Config();  
    LED_Init();                 
    USART1_Init();             
    
    MX_FMC_Init();             
    SDRAM_Test();              
        
    while (1)
    {
        LED1_Toggle;
        HAL_Delay(500);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 |RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    HAL_MPU_Disable();

    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = SDRAM_BANK_ADDR;
    MPU_InitStruct.Size             = MPU_REGION_SIZE_32MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER2;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
}
