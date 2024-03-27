/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : FMC.c
 * Description        : This file provides code for the configuration
 *                      of the FMC peripheral.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "fmc.h"

/* USER CODE BEGIN 0 */
FMC_SDRAM_CommandTypeDef command; // 闂佺鐭囬崘銊у幀闂佸湱枪濞诧綁骞??
/******************************************************************************************************
 *	�?????? �?????? �??????: SDRAM_Initialization_Sequence
 *	闂佺ǹ绻堥崕杈亹濞戙垹鐭楅柛灞剧⊕濞??: hsdram - SDRAM_HandleTypeDef闁诲氦顫夐惌顔剧不閻斿吋鍎嶉柛鏇ㄥ亜缂嶄線姊洪幓鎺楃崪缂佽鲸绻堝畷锛??鍦О?閸愵亞鐭嗛柛婵嗗閺嗘澘鈽夐弬娆炬Ш婵炲牊鍞禿ram
 *				 Command	- 闂佺鐭囬崘銊у幀闂佸湱枪濞诧綁骞??
 *	�?????? �?????? �??????: �??????
 *	闂佸憡鍨兼慨銈夊汲閻旂厧�?夐柣鏃囶嚙閸??: SDRAM 闂佸憡鐟ラ崐褰掑汲閻斿吋�??鐎广儱娲ㄩ弸?
 *	�??????    �??????: 闂備焦婢樼粔鍫曟偪閸濈嚉RAM闂佺儵鏅濇灙闁告鍥х睄閻犻缚娅ｇ喊宥夋煕濠婂啯婀伴悽顖氱埣�?�曟岸鎳滈崹顐ゅ幍闂?????
 *******************************************************************************************************/

void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd = 0;

  /* Configure a clock configuration enable command */
  Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;  // �??????闂佸憡姘ㄩ弻鍢峈AM闂佸搫鍟悥濂稿�??
  Command->CommandTarget = FMC_COMMAND_TARGET_BANK; // 闂備緡鍋勯ˇ鎵??姘ュ妿閹茬増鎷呴悷棰佺帛闂佸憡甯掓晶搴♀枔閹达箑�?岄柛婵嗗閸??
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = 0;

  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT); // 闂佸憡鐟﹂崺濠囧�???闂佽法鍠曟慨銈囨暜閸洖�?嗛柤鎼佹涧閻�?綊鏌????
  HAL_Delay(1);                                          // 閻庣偣鍊涢崺鏍ь渻閸屾粎椹冲璺猴功缁??

  /* Configure a PALL (precharge all) command */
  Command->CommandMode = FMC_SDRAM_CMD_PALL;        // 婵☆偅婢�??氼剟宕㈤弽顓熷仺闂傚牊绋掗崵鎺楁�????
  Command->CommandTarget = FMC_COMMAND_TARGET_BANK; // 闂備緡鍋勯ˇ鎵??姘ュ妿閹茬増鎷呴悷棰佺帛闂佸憡甯掓晶搴♀枔閹达箑�?岄柛婵嗗閸??
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = 0;

  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT); // 闂佸憡鐟﹂崺濠囧�???闂佽法鍠曟慨銈囨暜閸洖�?嗛柤鎼佹涧閻�?綊鏌????

  /* Configure a Auto-Refresh command */
  Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE; // 婵炶揪缍?濞夋洟寮妶澶嬪殜妞ゅ繐瀚闂佸憡甯￠弨閬嶅蓟?
  Command->CommandTarget = FMC_COMMAND_TARGET_BANK;      // 闂備緡鍋勯ˇ鎵??姘ュ妿閹茬増鎷呴悷棰佺帛闂佸憡甯掓晶搴♀枔閹达箑�?岄柛婵嗗閸??
  Command->AutoRefreshNumber = 8;                        // 闂佺厧顨庢禍婊勬叏閳哄懎�?嗛梺鍨儐閻??濠电偛妫岄崜婵嬪�??
  Command->ModeRegisterDefinition = 0;

  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT); // 闂佸憡鐟﹂崺濠囧�???闂佽法鍠曟慨銈囨暜閸洖�?嗛柤鎼佹涧閻�?綊鏌????

  /* Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2 |
           SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
           SDRAM_MODEREG_CAS_LATENCY_3 |
           SDRAM_MODEREG_OPERATING_MODE_STANDARD |
           SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;   // 闂佸憡姊绘慨鎯归崶顬喖鍨惧畷鍥╊攨闁诲酣娼�??氼剟鎮洪妸鈺�?棷闁靛ǹ鍎查崵鎺楁�????
  Command->CommandTarget = FMC_COMMAND_TARGET_BANK; // 闂備緡鍋勯ˇ鎵??姘ュ妿閹茬増鎷呴悷棰佺帛闂佸憡甯掓晶搴♀枔閹达箑�?岄柛婵嗗閸??
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = tmpmrd;

  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT); // 闂佸憡鐟﹂崺濠囧�???闂佽法鍠曟慨銈囨暜閸洖�?嗛柤鎼佹涧閻�?綊鏌????

  HAL_SDRAM_ProgramRefreshRate(hsdram, 918); // 闂備焦婢樼粔鍫曟偪閸℃稑�?嗛梺鍨儐閻??�??????
}

/* USER CODE END 0 */

SDRAM_HandleTypeDef hsdram1;

/* FMC initialization function */
void MX_FMC_Init(void)
{
  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 3;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
  SDRAM_Initialization_Sequence(&hsdram1, &command); // 闂備焦婢樼粔鍫曟偪閸濈嚉RAM

  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, SET);
  KD024VGFPD094_init();
  /* USER CODE END FMC_Init 2 */
}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration
  PF0   ------> FMC_A0
  PF1   ------> FMC_A1
  PF2   ------> FMC_A2
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PF5   ------> FMC_A5
  PH2   ------> FMC_SDCKE0
  PH3   ------> FMC_SDNE0
  PH5   ------> FMC_SDNWE
  PF11   ------> FMC_SDNRAS
  PF12   ------> FMC_A6
  PF13   ------> FMC_A7
  PF14   ------> FMC_A8
  PF15   ------> FMC_A9
  PG0   ------> FMC_A10
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE8   ------> FMC_D5
  PE9   ------> FMC_D6
  PE10   ------> FMC_D7
  PE11   ------> FMC_D8
  PE12   ------> FMC_D9
  PE13   ------> FMC_D10
  PE14   ------> FMC_D11
  PE15   ------> FMC_D12
  PD8   ------> FMC_D13
  PD9   ------> FMC_D14
  PD10   ------> FMC_D15
  PD14   ------> FMC_D0
  PD15   ------> FMC_D1
  PG2   ------> FMC_A12
  PG4   ------> FMC_BA0
  PG5   ------> FMC_BA1
  PG8   ------> FMC_SDCLK
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PG15   ------> FMC_SDNCAS
  PE0   ------> FMC_NBL0
  PE1   ------> FMC_NBL1
  */
  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* Peripheral interrupt init */
  HAL_NVIC_SetPriority(FMC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FMC_IRQn);
  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* sdramHandle){
  /* USER CODE BEGIN SDRAM_MspInit 0 */

  /* USER CODE END SDRAM_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN SDRAM_MspInit 1 */

  /* USER CODE END SDRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();

  /** FMC GPIO Configuration
  PF0   ------> FMC_A0
  PF1   ------> FMC_A1
  PF2   ------> FMC_A2
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PF5   ------> FMC_A5
  PH2   ------> FMC_SDCKE0
  PH3   ------> FMC_SDNE0
  PH5   ------> FMC_SDNWE
  PF11   ------> FMC_SDNRAS
  PF12   ------> FMC_A6
  PF13   ------> FMC_A7
  PF14   ------> FMC_A8
  PF15   ------> FMC_A9
  PG0   ------> FMC_A10
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE8   ------> FMC_D5
  PE9   ------> FMC_D6
  PE10   ------> FMC_D7
  PE11   ------> FMC_D8
  PE12   ------> FMC_D9
  PE13   ------> FMC_D10
  PE14   ------> FMC_D11
  PE15   ------> FMC_D12
  PD8   ------> FMC_D13
  PD9   ------> FMC_D14
  PD10   ------> FMC_D15
  PD14   ------> FMC_D0
  PD15   ------> FMC_D1
  PG2   ------> FMC_A12
  PG4   ------> FMC_BA0
  PG5   ------> FMC_BA1
  PG8   ------> FMC_SDCLK
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PG15   ------> FMC_SDNCAS
  PE0   ------> FMC_NBL0
  PE1   ------> FMC_NBL1
  */

  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

  HAL_GPIO_DeInit(GPIOH, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);

  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15);

  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1);

  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1);

  /* Peripheral interrupt DeInit */
  HAL_NVIC_DisableIRQ(FMC_IRQn);
  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* sdramHandle){
  /* USER CODE BEGIN SDRAM_MspDeInit 0 */

  /* USER CODE END SDRAM_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN SDRAM_MspDeInit 1 */

  /* USER CODE END SDRAM_MspDeInit 1 */
}
/**
  * @}
  */

/**
  * @}
  */
