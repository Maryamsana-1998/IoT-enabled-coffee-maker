 #include "tm4c123gh6pm.h"
 
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))

#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))

#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))

void SysTick_Init(void){

  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup

  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock

}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)

void SysTick_Wait(unsigned long delay){

  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait

  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears

  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag

  }

}

// 800000*12.5ns equals 10ms

void SysTick_Wait10ms(unsigned long delay){

  unsigned long i;

  for(i=0; i<delay; i++){

    SysTick_Wait(800000);  // wait 10ms

  }

}
void PLL_Init(void){

  // 0) Use RCC2

  SYSCTL->RCC2 |=  0x80000000;  // USERCC2

  // 1) bypass PLL while initializing

  SYSCTL->RCC2|=  0x00000800;  // BYPASS2, PLL bypass

  // 2) select the crystal value and oscillator source

  SYSCTL->RCC = (SYSCTL->RCC &~0x000007C0)   // clear XTAL field, bits 10-6

                 + 0x00000540;   // 10101, configure for 16 MHz crystal

  SYSCTL->RCC2 &= ~0x00000070;  // configure for main oscillator source

  // 3) activate PLL by clearing PWRDN

  SYSCTL->RCC2 &= ~0x00002000;

  // 4) set the desired system divider

  SYSCTL->RCC2 |= 0x40000000;   // use 400 MHz PLL

  SYSCTL->RCC2 = (SYSCTL->RCC2&~ 0x1FC00000)  // clear system clock divider

                  + (4<<22);      // configure for 80 MHz clock

  // 5) wait for the PLL to lock by polling PLLLRIS

  while((SYSCTL->RIS&0x00000040)==0){};  // wait for PLLRIS bit

  // 6) enable use of PLL by clearing BYPASS

  SYSCTL->RCC2 &= ~0x00000800;

}
#define SENSOR  (*((volatile unsigned long *)0x4002400C))

#define LIGHT   (*((volatile unsigned long *)0x400050FC))

// Linked data structure

struct State {

  unsigned long Out;

  unsigned long Time; 

  unsigned long Next[4];};

typedef const struct State STyp;

#define goN   0

#define waitN 1

#define goE   2

#define waitE 3

STyp FSM[4]={

 {0x21,3000,{goN,waitN,goN,waitN}},

 {0x22, 500,{goE,goE,goE,goE}},

 {0x0C,3000,{goE,goE,waitE,waitE}},

 {0x14, 500,{goN,goN,goN,goN}}};

unsigned long S;  // index to the current state

unsigned long Input;

int main(void){ volatile unsigned long delay;

  PLL_Init();       // 80 MHz, Program 10.1

  SysTick_Init();   // Program 10.2

  SYSCTL->RCGC2 |= 0x12;      // 1) B E

  delay = SYSCTL->RCGC2;      // 2) no need to unlock

  GPIOE->AMSEL &= ~0x03; // 3) disable analog function on PE1-0

  GPIOE->PCTL &= ~0x000000FF; // 4) enable regular GPIO

  GPIOE->DIR &= ~0x03;   // 5) inputs on PE1-0

  GPIOE->AFSEL &= ~0x03; // 6) regular function on PE1-0

  GPIOE->DEN |= 0x03;    // 7) enable digital on PE1-0

  GPIOB->AMSEL &= ~0x3F; // 3) disable analog function on PB5-0

  GPIOB->PCTL &= ~0x00FFFFFF; // 4) enable regular GPIO

  GPIOB->DIR |= 0x3F;    // 5) outputs on PB5-0

  GPIOB->AFSEL &= ~0x3F; // 6) regular function on PB5-0

  GPIOB->DEN |= 0x3F;    // 7) enable digital on PB5-0

  S = goN;  

  while(1){

    LIGHT = FSM[S].Out;  // set lights

    SysTick_Wait10ms(FSM[S].Time);

    Input = SENSOR;     // read sensors

    S = FSM[S].Next[Input]; 

  }

}
