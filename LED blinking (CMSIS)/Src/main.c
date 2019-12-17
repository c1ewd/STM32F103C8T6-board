#include "stm32f10x.h"

void TIM4_IRQHandler(void)
{
    // Clear UIF flag
    TIM4->SR &= ~TIM_SR_UIF;
    // Inverting value
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}

int main()
{
  // Enable PORTC Periph clock  
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  // Configure PC13 as Push Pull (CNF00) output 
  GPIOC->CRH &= ~GPIO_CRH_CNF13;
  // at max 2Mhz (MODE10)
  GPIOC->CRH |= GPIO_CRH_MODE13_1;
  // Interrupt priority
  NVIC_SetPriority(TIM4_IRQn, 1);
  // Enable TIM4 interrupt
  NVIC_EnableIRQ(TIM4_IRQn);
  // Enable TIM4 Periph clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
  // Prescaller
  TIM4->PSC = 36000 - 1;
  // Counter
  TIM4->ARR = 1000 - 1;
  // Enable TIM4 interrupt
  TIM4->DIER = TIM_DIER_UIE;
  // Start and clear for full
  TIM4->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;
  
  while(1)
  {
    __NOP();
  }  
 
}

