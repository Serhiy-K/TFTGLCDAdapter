/*************************************************************************************
*
Description :  NeilScope3 Main Interrupt Service Routines
Version     :  1.0.0
Date        :  25.12.2012
Author      :  Left Radio                          
Comments:   :  This file provides template for all exceptions handler and 
*              peripherals interrupt service routine.
**************************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/


/**
  * @brief  This function handles RTC_IRQHandler .
  * @param  None
  * @retval : None
  */
void RTC_IRQHandler(void)
{
}


/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
//void TIM1_UP_IRQHandler(void)
//{
//}


/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
//void TIM2_IRQHandler(void)
//{
//}


/**
  * @brief  This function handles TIM4 interrupt request.
  * @param  None
  * @retval None
  */
/*
void TIM4_IRQHandler(void)
{
}
*/

/**
  * @brief  This function handles I2C ERROR interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_ER_IRQHandler(void)
{/*
	// Check on I2C2 SMBALERT flag and clear it
	if (I2C_GetITStatus(I2C1, I2C_IT_SMBALERT))	{
		I2C_ClearITPendingBit(I2C1, I2C_IT_SMBALERT);
	}
	// Check on I2C2 Time out flag and clear it
	if (I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_TIMEOUT);
	}
	// Check on I2C2 Arbitration Lost flag and clear it
	if (I2C_GetITStatus(I2C1, I2C_IT_ARLO))	{
		I2C_ClearITPendingBit(I2C1, I2C_IT_ARLO);
	}	
	// Check on I2C2 PEC error flag and clear it
	if (I2C_GetITStatus(I2C1, I2C_IT_PECERR)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_PECERR);
	} 
	// Check on I2C2 Overrun/Underrun error flag and clear it
	if (I2C_GetITStatus(I2C1, I2C_IT_OVR)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_OVR);
	} 
	// Check on I2C2 Acknowledge failure error flag and clear it
	if (I2C_GetITStatus(I2C1, I2C_IT_AF)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
	}
	// Check on I2C2 Bus error flag and clear it
	if (I2C_GetITStatus(I2C1, I2C_IT_BERR)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_BERR);
	}
*/
}


/**
  * @brief  This function handles ADC1_2 interrupt request.
  * @param  None
  * @retval None
  */
void ADC1_2_IRQHandler(void)
{
}


/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
}


/**
  * @brief  Message succsessful recived callback from USART state machine
  * @param  None
  * @retval None
  */
void ReceivedStateMachine_CompleteCallBack(uint8_t command_index, uint8_t* command_data)
{
}


/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel5_IRQHandler(void)
{

}


/**
  * @brief  This function handles EXTI9_5 interrupt request.
  * @param  None
  * @retval None
  */
/*
void EXTI9_5_IRQHandler(void)
{
}
*/


/***********************END OF FILE*******************************/
