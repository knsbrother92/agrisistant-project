//유량 include파일
#include "stm32f10x_lib.h"
#include "System_func.h"
#include "stm32f10x_it.h"

//온습도 include파일
#include"lib_sensor.h"

//함수선언 공용
extern u32 cap1_cnt;
extern u16 pull_wi;
char GpsString[80];
//void TIM3_IRQHandler(void);
void putch(unsigned char c);
int getch(u16 data, unsigned char key[6] );
void delay(u32 time);
int getch2(u16 data, unsigned char key[7] );

void timer_d();
void delay_opt(u16 time);

float flow_rate(u16 data_number);
unsigned int flow_speed(u16 flowrate);

//수위
void USART1_init(u32 bps);
void Delay(vu32 nCount);
void SerialPutChar(u8 c);
void SerialPutString(u8 *s);
unsigned int liquid_level(float value);

//GPS함수 선언
void get_send();
extern u16 pull;
extern int state;

u16 frequency = 0;
int cnt=0;
//메인함수
int main(void){
  //수위
  char i;
  int cnt;
  Init_STM32F103();   
  
  //수위
  unsigned short ADC_ConvertedValue;
  ADC_InitTypeDef     ADC_InitStructure;
  USART1_init(9600);
  SerialPutString("\n\r ADC Start!! \r\n");
  
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  //온습도
  timer_d();
  initialize_sht11_hanback();
  
  //유량
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 | RCC_APB1Periph_TIM2, ENABLE );
  //수위
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  RCC_ADCCLKConfig(RCC_PCLK2_Div2);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
   
  //ADC_DeInit(ADC1);
   
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
     
  // 6)
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5);

  //  7)
  ADC_Cmd(ADC1, ENABLE);
 
  //  8)
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));        
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  
  //GPS & 블루투스
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_0 ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //블루투스 인터럽트
  
  //유량
 
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  

 
  TIM_TimeBaseStructure.TIM_Period = 59999;
  TIM_TimeBaseStructure.TIM_Prescaler = 11;     // 1MHz Timer input 
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_PrescalerConfig( TIM2, 11, TIM_PSCReloadMode_Immediate);

  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);
  
  //GPIO_PinRemapConfig( GPIO_FullRemap_TIM2, ENABLE);
  //TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
  
  
  
  //  6)  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Clock = USART_Clock_Disable;
  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

  //블루투스 인터럽트
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
  //
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);


  u16 speed;
  u16 temp,humi;
  char a,b,c,d,e;
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  unsigned char result[8];
  
  while (1){
    int cntloop = 0;
    //온습도  
      temp = get_sht11_hanback_data(TEMP);
      cntloop = getch2(temp, result);
      for(int i = 0 ; i < cntloop ; i++){
        putch(result[i]);
      }
      SerialPutString(" C\r\n");
      delay_opt(10);
      
      
      
      humi = get_sht11_hanback_data(HUMI);
      cntloop = getch2(humi, result);
      for(int i = 0 ; i < cntloop ; i++){
        putch(result[i]);
      }
      SerialPutString(" %\r\n");
      delay_opt(10);
    
    //GPS
      
      
      for(int i=0;i<180;i++) {
      while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
        a = USART_ReceiveData(USART2);
        b = USART_ReceiveData(USART2);
        c = USART_ReceiveData(USART2);
        d = USART_ReceiveData(USART2);
        e  = USART_ReceiveData(USART2);
        
        if((a == 'G')&&(b == 'P')&&(c == 'G')&&(d == 'G')&&(e == 'A')) {
        USART_SendData(USART1, a);
        USART_SendData(USART1, b);
        USART_SendData(USART1, c);
        USART_SendData(USART1, d);
        USART_SendData(USART1, e);
          break;
        }
      }
      
      for(int i=0;i<180;i++){
        while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
        a = USART_ReceiveData(USART2);
        USART_SendData(USART1, a);
        b = USART_ReceiveData(USART2);
        USART_SendData(USART1, b);
        c = USART_ReceiveData(USART2);
        USART_SendData(USART1, c);
        d = USART_ReceiveData(USART2);
        USART_SendData(USART1, d);
        e  = USART_ReceiveData(USART2);
        USART_SendData(USART1, e);
        if((a == 'G')&&(b == 'P')&&(c == 'V')&&(d == 'T')&&(e == 'G'))
        {
          a = USART_ReceiveData(USART2);
          USART_SendData(USART1, a);
          if(a == 'G')
            break;
        }
        
      }
      get_send();
      SerialPutString("\r\n");
      delay_opt(100);
    
    //유속
      u16 value = pull;
      
      if (cnt == cap1_cnt){
        putch('0');
        SerialPutString(" L per Sec\r\n");
      }
      else {
        speed = flow_speed(value);
        cntloop = getch(value, result);
        for(int i = 0 ; i < cntloop ; i++){
          putch(result[i]);
        }
      }
      delay_opt(100);
      SerialPutString(" L per Sec\r\n");
      cnt = cap1_cnt;
    //수위
   
    unsigned int temp_value = 0;
    
    ADC_ConvertedValue = ADC_GetConversionValue(ADC1);
    temp_value = liquid_level(ADC_ConvertedValue);
    
    SerialPutString(" Water(L) : ");
    SerialPutChar(ADC_ConvertedValue/10000 + '0'); 
    SerialPutChar((ADC_ConvertedValue/1000)%10 + '0'); 
    SerialPutChar((ADC_ConvertedValue/100)%10 + '0'); 
    SerialPutChar((ADC_ConvertedValue/10)%10 + '0'); 
    SerialPutChar((ADC_ConvertedValue)%10 + '0');
    SerialPutString(" L \r\n\r\n");
    for(i=0;i<3;i++)
        Delay(0xAFFF);
    
  } //end of main
}


unsigned int flow_speed(u16 flowrate){ //유속 계산 함수 V = Q / A
  int flowspeed = 0;
  u16 diameter = 1.1;
  flowspeed = flowrate / diameter;
  return flowspeed;
}

float flow_rate(u16 interrupt_time){ //유량 계산 함수 Q = A * V
  //int first_time = 0;
  //int herts = 0;
 // float rate = 10;
  int flowrate = 0;
  
  const int liters = 2.0;
  
  //herts = ( liters / (interrupt_time - first_time))/8;
  //flowrate = herts * (2 / 15);
  if((interrupt_time ) == frequency) {
    if(cnt >= 10) {
      frequency = 0;
      return 0;
    }
    cnt++;
  }
  else
    cnt = 0;
  
  frequency = interrupt_time;
  flowrate = (frequency / 100);    // frequency for input
  
  return flowrate;
}

void putch(unsigned char c){  //문자열 출력 함수

  USART_SendData(USART1, c);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

int getch2(u16 data, unsigned char key[7] ){ // 문자열 입력 함수
  if((data <1000)&&(data >100)) {
  key[0] = (data/100+'0');
  key[1] = (data - ((key[0] - '0')*100))/10 + '0'; 
  key[2] = '.';
  key[3] = (data - ((key[0] - '0')*100) - ((key[1] - '0')*10)) + '0';
  key[4] = '\r';
  key[5] = '\n';
  return 6;
  }
  else if((data <100 ) && (data >10)){
    key[0] = (data/10) + '0';
    key[1] = '.';
    key[2] = (data - ((key[0] - '0') * 10)) +'0';
    key[3] = '\r';
    key[4] = '\n';
    return 5;
  }
}

int getch(u16 data, unsigned char key[6] ){ // 문자열 입력 함수
  if(data > 10000 ){
  key[0] = (data/10000+'0');
  key[1] = (data - ((key[0] - '0')*10000))/1000 + '0'; 
  key[2] = (data - ((key[0]- '0') *10000) - ((key[1] - '0')*1000))/100 + '0';
  key[3] = (data - ((key[0] - '0')*10000) - ((key[1] - '0')*1000) - ((key[2] - '0')*100))/10 + '0';
  key[4] = '\r';
  key[5] = '\n';
  return 6;
  }
  else if((data > 1000 ) && (data <10000)){
  key[0] = (data/1000+'0');
  key[1] = (data - ((key[0] - '0')*1000))/100 + '0'; 
  key[2] = (data - ((key[0] - '0')*1000) - ((key[1] - '0')*100))/10 + '0';
  key[3] = (data - ((key[0] - '0')*1000) - ((key[1] - '0')*100) - ((key[2] - '0')*10)) + '0';
  key[4] = '\r';
  key[5] = '\n';
  return 6;
  }
  else if((data <1000)&&(data >100)) {
    key[0] = (data/100+'0');
  key[1] = (data - ((key[0] - '0')*100))/10 + '0'; 
  key[2] = (data - ((key[0] - '0')*100) - ((key[1] - '0')*10)) + '0';
  key[3] = '\r';
  key[4] = '\n';
  return 5;
  }
  else if((data <100 ) && (data >10)){
    key[0] = (data/10) + '0';
    key[1] = (data - ((key[0] - '0') * 10)) +'0';
    key[2] = '\r';
    key[3] = '\n';
    return 4;
  }
  else {
    key[0] = data +'0';
    key[1] = '\r';
    key[2] = '\n'; 
    return 3;
  }
//  key[5] = '\n';
}

void timer_d(){
  
  TIM_TimeBaseInitTypeDef TIM2_TimeBaseInitStruct;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
 
  TIM2_TimeBaseInitStruct.TIM_Prescaler = 7200 -1;  
  TIM2_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM2_TimeBaseInitStruct.TIM_Period = 10-1;  
  TIM2_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM2,&TIM2_TimeBaseInitStruct);
  
}

void delay_opt(u16 time){
  u16 i; 
  TIM_Cmd(TIM2,ENABLE);    
  for(i=0;i<time;i++){
     while(TIM_GetFlagStatus(TIM2,TIM_IT_Update)==RESET);
     TIM_ClearFlag(TIM2,TIM_FLAG_Update);
  }
  TIM_Cmd(TIM2,DISABLE);

}

void get_send(){ //GPS 송신 수신함수
 unsigned char gps_data = 0;
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
  gps_data = USART_ReceiveData(USART2);
  USART_SendData(USART1, gps_data);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    
}

//밑으로 수위
unsigned int liquid_level(float value){
  float reading = value;
  const int Ohm_resister = 560;
  
  reading = (1023 / reading) - 1;
  reading = (unsigned int)(Ohm_resister / reading);
  return reading;
}

void USART1_init(u32 bps){
  
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //  6)  
  USART_InitStructure.USART_BaudRate = bps;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Clock = USART_Clock_Disable;
  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

  USART_Init(USART1, &USART_InitStructure);  

  USART_Cmd(USART1, ENABLE);
}

void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}


void SerialPutChar(u8 c)
{
  USART_SendData(USART1, c);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void SerialPutString(u8 *s)
{
  while (*s != '\0')
  {
    SerialPutChar(*s);
    s ++;
  }
}

