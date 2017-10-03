#include "stm32l476xx.h"
extern void delay_us(int a);
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);
#define TIME_ARR_VAL 3000


int start_flag = 0;
void GPIO_Init(void) {
RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);
// UART
GPIOA->MODER = 0xABFF54FF;//PA4 IPUT PA567 OUTPUT
// BUTTON
GPIOC->MODER = 0xF3FFF3FF;//pc13input pc5 input


GPIOB->MODER = 0xFFFF6FBF;
}
void GPIO_init_AF(){
//TODO: Initial GPIO pin as alternate function for buzzer. You can choose to use C or assembly to finish this function.
	//GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL7;//AFR[0] LOW
	//GPIOB->AFR[0] |= (0b0010<<GPIO_AFRL_AFSEL7_Pos);//PB7 Alternate function mode
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL3;//AFR[0] LOW
	GPIOB->AFR[0] |= (0b0001<<GPIO_AFRL_AFSEL3_Pos);//PB3 Alternate function mode
}

void SystemClock_Config()
{
	// 10 MHZ
	RCC->CFGR = 0;
	RCC->CR |= RCC_CR_HSION; //turn on HSI16 oscillator
	RCC->CR &= ~RCC_CR_PLLON;//Disable PLLON
	while((RCC->CR & RCC_CR_PLLRDY) == 1);
	RCC->PLLCFGR = 0;
	RCC->PLLCFGR |= (40<<8); //PLLN
	RCC->PLLCFGR |= (7<<4); //PLLM
	RCC->PLLCFGR |= (3<<25);//PLLR
	RCC->PLLCFGR |= 2;

	RCC->CR |= RCC_CR_PLLON;//Enable PLLON
	RCC->PLLCFGR |= (1<<24);//Enable PLLREN
	RCC->CFGR |= 3; //PLL as system clock
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	SysTick_Config(4000000);

}
int score = 0;
void SysTick_Handler(void){
}
int display(int data, int num_digs)
{
	int i=1;
		if(data == 0){
			max7219_send(1,0);
			return 0;
		}
		//if(num_digs>8)return -1;
		while(i <= num_digs)
		{
			int temp;
			temp = data % 10;
			max7219_send(i+4,temp);
			data /= 10;
			i++;
		}
		return 0;
}
int how_digit(int sum)
{
	int digit=0;
	while(sum!=0)
	{
		sum/=10;
		digit++;
	}
	return digit;
}


void display_time(int data, int num_digs,int score,int score_digit)
{
	int i=1;
	while(i <= num_digs)
	{
			int temp;
			temp = data % 10;
			if(i==3)temp+=pow(2,7);
			max7219_send(i,temp);
			data /= 10;
			i++;
	}
	int j=1;
			if(score == 0){
				max7219_send(1+4,0);

			}
			while(j+4 <= num_digs)
			{
				int temp;
				temp = data % 10;
				max7219_send(j+4,temp);
				data /= 10;
				j++;
			}

		return ;
}
void Timer_init( TIM_TypeDef *timer){
//TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	timer->CR1 &= ~TIM_CR1_DIR;
	timer->CR1 |= (1<<TIM_CR1_DIR_Pos);
	//SET_REG(TIM2->CR1, TIM_CR1_DIR , TIM_COUNTERMODE_DOWN);//down counter
	timer->ARR = (uint32_t) TIME_ARR_VAL;//Reload value
	timer->PSC = (uint32_t) 39999;//Prescalser
	timer->EGR = TIM_EGR_UG;//Reinitialize the counter
}

void Timer4_init(){
//TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	TIM4->CR1 &= ~TIM_CR1_DIR;
	TIM4->CR1 |= (1<<TIM_CR1_DIR_Pos);
	//SET_REG(TIM2->CR1, TIM_CR1_DIR , TIM_COUNTERMODE_DOWN);//down counter
	TIM4->ARR = (uint32_t) 1680-1;//Reload value
	TIM4->PSC = (uint32_t) 1000-1;//Prescalser
	TIM4->EGR = TIM_EGR_UG;//Reinitialize the counter
}
int timerValue;
void Timer_start(TIM_TypeDef *timer){
//TODO: start timer and show the time on the 7-SEG LED.
	timer->CR1 |= TIM_CR1_CEN;//start timer
	//int pre_val = 0;
	int pre_val =  TIME_ARR_VAL;
	while(1){
		int value = GPIOA->IDR & (1<<4);
				if(value == 0){//HAVE THING
					score += 2;


					delay_us(10000);
				}
				display(score, how_digit(score));


		int timerValue = timer->CNT;//polling the counter value
		if(pre_val < timerValue){//check if times up
		//if(pre_val < timerValue){
			timer->CR1 &= ~TIM_CR1_CEN;
			return;
		}
		pre_val = timerValue;
		//int dis_val = TIME_SEC*100*timerValue/TIM_ARR_VAL;//convert counter value to time(seconds)
		int dis_val = timerValue;
		int how = how_digit(dis_val);
		//display(dis_val,how);//display the time on the 7-SEG LED
		if(how<3)
		{
				how = 3;
		}
		for(int j=how+1;j<=4;j++)
		{
			max7219_send(j,0xf);
		}

		display_time(dis_val,how,score, how_digit(score*10000));
		//display(score, how_digit(score));


	}
}

void timer_sing(uint32_t presc){
   //TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	TIM2->CR1 &= ~(TIM_CR1_DIR_Pos);
	TIM2->CR1 &= ~(TIM_CR1_CMS_Pos);
	//SET_REG(TIM2->CR1, TIM_CR1_DIR | TIM_CR1_CMS, TIM_COUNTERMODE_DOWN);// Edge-aligned mode, down counter
	TIM2->ARR = (uint32_t)100;//Reload value
	TIM2->PSC = (uint32_t)presc;//Prescaler
	TIM2->EGR = TIM_EGR_UG;//Reinitialize the counter
	TIM2->CR1 |= TIM_CR1_CEN;
}


void timer_turn(uint32_t presc){
   //TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM4->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	TIM4->CR1 &= ~(TIM_CR1_DIR_Pos);
	TIM4->CR1 &= ~(TIM_CR1_CMS_Pos);
	//SET_REG(TIM2->CR1, TIM_CR1_DIR | TIM_CR1_CMS, TIM_COUNTERMODE_DOWN);// Edge-aligned mode, down counter
	TIM4->ARR = (uint32_t)100;//Reload value
	TIM4->PSC = (uint32_t)presc;//Prescaler
	TIM4->EGR = TIM_EGR_UG;//Reinitialize the counter
	TIM4->CR1 |= TIM_CR1_CEN;
}
void EXTI_Setup(){
////PC13
	RCC->APB2ENR |= 0x1;
	SYSCFG->EXTICR[3] = 0x20;

	EXTI->RTSR1 |= (1<<13);
	EXTI->IMR1 |= (1<<13);

	/*SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PC;
	EXTI->IMR1 |= EXTI_IMR1_IM5;
	EXTI->RTSR1 &= ~EXTI_RTSR1_RT5;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT5;
	EXTI->RTSR1 |= EXTI_RTSR1_RT5;
	//EXTI->FTSR1 |= EXTI_FTSR1_FT5;


	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	NVIC_SetPriority(EXTI9_5_IRQn, 0);
	NVIC_EnableIRQ(EXTI9_5_IRQn);*/



	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void){
	if(EXTI->PR1 & (1<<13) && start_flag == 1){
		EXTI->PR1 |= (1<<13);

		Timer_init(TIM2);
		Timer_start(TIM2);


		timer_sing(40000/261);
		delay_us(15000);

		timer_sing(40000/293);
		delay_us(15000);

		timer_sing(40000/329);
		delay_us(10000);

		timer_sing(40000/392);
		delay_us(20000);

		timer_sing(40000/329);
		delay_us(20000);

		timer_sing(40000/261);
		delay_us(15000);

		timer_sing(40000/293);
		delay_us(15000);

		timer_sing(40000/329);
		delay_us(10000);

		timer_sing(40000/440);
		delay_us(20000);

		timer_sing(40000/349);
		delay_us(20000);

		timer_sing(40000/261);
		delay_us(15000);

		timer_sing(40000/293);
		delay_us(15000);

		timer_sing(40000/329);
		delay_us(10000);

		timer_sing(40000/493);
		delay_us(20000);

		timer_sing(40000/493);
		delay_us(20000);

		timer_sing(40000/523);
		delay_us(30000);



		///timer_sing(40000/392);
		//delay_us(30000);

		TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
			TIM2->CR1 &= ~(TIM_CR1_DIR_Pos);
			TIM2->CR1 &= ~(TIM_CR1_CMS_Pos);


			start_flag = 0;

			for(int i=1;i<=8;i++)
			{
					max7219_send(i,0xf);
			}


		//timer_sing(300000);//(40000/261);


	}
}



/*void EXTI9_5_IRQnHandler(void)
{
	if((EXTI->PR1 & EXTI_PR1_PIF5) == 0){

		EXTI->PR1 = EXTI_PR1_PIF5;
		start_flag = 1;
		for(int i=1;i<=8;i++)
			{

							max7219_send(i,0);
			}



	}
}*/

void PWM_channel_init(TIM_TypeDef *timer){
   //TODO: Initialize timer PWM channel
	timer->CCMR1 &= ~TIM_CCMR1_OC2M;
	timer->CCMR1 |= (0b0110 << TIM_CCMR1_OC2M_Pos);

	timer->CCMR1 &= ~TIM_CCMR1_OC2PE;//OCxPE
	timer->CCMR1 |= (0b1 << TIM_CCMR1_OC2PE_Pos);

	timer->CCR2 = 50;
	timer->CCER |= TIM_CCER_CC2E;

}
int main()
{
	GPIO_Init();
	//SystemClock_Config();
	max7219_init();
	EXTI_Setup();
	GPIO_init_AF();
	timer_sing(0);//(40000/261);
	PWM_channel_init(TIM2);
	//PWM_channel_init(TIM4);
	//Timer4_init();
	for(int i=1;i<=8;i++)
	{
			max7219_send(i,0xf);
	}
	while(1){
		int new_value = GPIOC->IDR & (1<<5);
		if(new_value == 0)
		{

		}
		else
		{

			start_flag = 1;
			for(int i=6;i<=8;i++)
			{
				max7219_send(i,0xf);
			}
			max7219_send(5,0);

			score = score - score;
			display_time(TIME_ARR_VAL,4,score, how_digit(score*10000));

		}

	}
}
