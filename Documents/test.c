#define F_CPU 16000000UL
#define sbi(PORTX , BitX)   PORTX |=  (1 << BitX)   // SET   BIT
#define cbi(PORTX , BitX)   PORTX &= ~(1 << BitX)   // CLEAR BIT
#define U08 uint8_t
#define U32 uint32_t
#define U16 uint16_t
#define UART0 0
#define UART1 1
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <string.h>


void Init_Uart(U08 Com, U32 Uart_Baud);
void PWM4(char ch);
void PWM5(char ch);
void PWM6(char ch);
void PWM7(char ch);
void SerialPutchar(char ch);
unsigned char SerialGetchar(void);
char rev;
char data4 = 0;
char data5 = 0;
char data6 = 0;
char data7 = 0;
int recieve_data;


void Init_Uart(U08 Com, U32 Uart_Baud) //USART init 함수
{
	U16 Temp_UBRR;


	Temp_UBRR = F_CPU/(16L * Uart_Baud) - 1;      // 통신 보레이트 계산식
	// U2X = 0 일때 (일반모드)
	
	//---------------------------- UART0 초기화 --------------------------------
	if( Com == UART0 )
	
	{
		UBRR0H = (Temp_UBRR >> 8);              // 통신속도 설정
		UBRR0L = (Temp_UBRR & 0x00FF);
		
		UCSR0A = (0<<RXC0)  | (1<<UDRE0);      // 수신,송신 상태비트 초기화
		UCSR0B = (1<<RXEN0) | (1<<TXEN0);        // 수신,송신 기능 활성화
		UCSR0C = (3<<UCSZ00);               // START 1비트/DATA 8비트/STOP 1비트
		
		cbi( DDRE, 0 );                         // RXD0 핀 입력으로 설정
		sbi( DDRE, 1 );                         // TXD0 핀 출력으로 설정
		
		#ifdef UART0_RX_INT_ENABLE            // UART0_RX_INT_ENABLE 설정시만
		sbi(UCSR0B, RXCIE0);                 // 수신인터럽트0 활성화
		#endif
	}
}


void SerialPutchar(char ch) //송신하는 함수
{
	while(!(UCSR0A & 0x20));
	UDR0 = ch;
}


unsigned char SerialGetchar(void) //수신하는 함수
{
	while(!(UCSR0A & 0x80));
	return UDR0;
}


void PWM4(char ch) //PB4
{
	// OC0(PB4)출력설정
	double num;
	if(ch<0)
	ch = 0;
	else if(ch>100)
	ch = 100;
	num = ch*2.55;
	OCR0 = num;
}


void PWM5(char ch) //PB5
{
	// OC0(PB4)출력설정
	double num;
	if(ch<0)
	ch = 0;
	else if(ch>100)
	ch = 100;
	num = ch*2.55;
	OCR1A = num;
}


void PWM6(char ch) //PB6
{
	// OC0(PB4)출력설정
	double num;
	if(ch<0)
	ch = 0;
	else if(ch>100)
	ch = 100;
	num = ch*2.55;
	OCR1B = num;
}


void PWM7(char ch) //PB7void PWM2(char ch) //PWM1 패턴
{
	// OC0(PB4)출력설정
	double num;
	if(ch<0)
	ch = 0;
	else if(ch>100)
	ch = 100;
	num = ch*2.55;
	OCR2 = num;
}




//PB6, PB7 프로그램 종료될 때 불들어옴
int main (void)
{
	DDRE = 0xFE;
	Init_Uart(0, 57600);
	recieve_data = 0;
	DDRB = 0xFF;
	//PORTB = 0xFF;
	//char send = 'x';
	TCCR2 = 0b11100010;  //PB7
	TCCR0 = 0b11100010;  //PB4 // fast PWM1 mode : 3,6(WGM01,00) - 11, 5,4(COM ) - 11, 2-0(CS) - 010
	//TCCR1A |= (1<<WGM01)|(1<<COM1A1)|(1<<COM1B1); //PB5
	TCCR1A = 0xA2;
	TCCR1B |= (0<<CS10)|(1<<CS11)|(0<<CS12); //PB5
	//TCCR1A = 0x22;
	//TCCR1B = 0x19;
	PWM4(0);
	PWM5(0);
	PWM6(0);
	PWM7(0);
	while(1)
	{
		rev = SerialGetchar();
		switch(rev){
			case'A':
			data4 = SerialGetchar();
			if(data4=='X'){
				PWM4(0);
			}
			else{
				PWM4(data4);
			}
			break;
			
			case'B':
			data5 = SerialGetchar();
			if(data5=='X'){
				PWM5(0);
			}
			else{
				PWM5(data5);
			}
			break;
			
			case'C':
			data6 = SerialGetchar();
			if(data6=='X'){
				PWM6(0);
			}
			else{
				PWM6(data6);
			}
			break;
			
			case'D':
			rev = SerialGetchar();
			if(rev=='X'){
				PWM7(0);
			}
			else{
				PWM7(rev);
			}
			break;
			
			case'L':
			SerialPutchar(4);
			break;
			
			default:
			PWM4(0);
			PWM5(0);
			PWM6(0);
			PWM7(0);
			break;
		}
	}
	
	return 0;
}
