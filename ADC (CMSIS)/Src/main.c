#include "stm32f10x.h"

volatile uint16_t temp;

void ADC1_2_IRQHandler(void)
{
   if(ADC1->SR & ADC_SR_EOC)
   {
      // Inverting value
      GPIOC->ODR ^= GPIO_ODR_ODR13;
      // Conversion value
      temp = ADC1->DR;
      // Clear EOC Flag
      ADC1->SR = 0;
   }

}

int main()
{
    // Enable clock alternate functions
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    //NOJTAG: JTAG-DP Disabled and SW-DP Enabled
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    // Enable PORTC Periph clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    // Configure PC13 as Push Pull (CNF00) output
    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    // at max 2Mhz (MODE10)
    GPIOC->CRH |= GPIO_CRH_MODE13_1;

    // Enable PORTA Periph clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // Configure PA0 to analog input
    GPIOA->CRL &= ~GPIO_CRL_CNF0;
    GPIOA->CRL &= ~GPIO_CRL_MODE0;
    // ADC Clock speed at 12 MHz
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;
    // Enable ADC1 Periph clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN ;

    // Enable ADC1 Interrupt
    NVIC_EnableIRQ(ADC1_2_IRQn);
    // Interrupt priority
    NVIC_SetPriority(ADC1_2_IRQn, 1);
    
    // Enable Interrupt for end conversion
    ADC1->CR1 = ADC_CR1_EOCIE;
    
    // PA0
    ADC1->SQR1 = ADC_SQR1_L_0;
    ADC1->SQR2 = 0x00000000;
    ADC1->SQR3 = 0x00000000;

    // Enable start conversion for TRGO
    ADC1->CR2 &= ~ADC_CR2_EXTSEL;
    ADC1->CR2 = ADC_CR2_EXTSEL_2 | ADC_CR2_ADON | ADC_CR2_EXTTRIG ;

    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while(!(ADC1->CR2 & ADC_CR2_RSTCAL));  
    
    // Calibration
    ADC1->CR2 |= ADC_CR2_CAL;
    while(!(ADC1->CR2 & ADC_CR2_CAL));  
    
    // Enable TIM3 Periph clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    // Prescaller
    TIM3->PSC = 36000 - 1;
    // Counter
    TIM3->ARR = 1000 - 1;
    // Enable TRGO
    TIM3->CR2 &= ~TIM_CR2_MMS;
    TIM3->CR2 |= TIM_CR2_MMS_1;
    // Start and clear for full
    TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;

    while(1)
    {
      __NOP();
    }

}