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


void Init_Uart(U08 Com, U32 Uart_Baud) //USART init �Լ�
{
	U16 Temp_UBRR;


	Temp_UBRR = F_CPU/(16L * Uart_Baud) - 1;      // ��� ������Ʈ ����
	// U2X = 0 �϶� (�Ϲݸ��)
	
	//---------------------------- UART0 �ʱ�ȭ --------------------------------
	if( Com == UART0 )
	
	{
		UBRR0H = (Temp_UBRR >> 8);              // ��żӵ� ����
		UBRR0L = (Temp_UBRR & 0x00FF);
		
		UCSR0A = (0<<RXC0)  | (1<<UDRE0);      // ����,�۽� ���º�Ʈ �ʱ�ȭ
		UCSR0B = (1<<RXEN0) | (1<<TXEN0);        // ����,�۽� ��� Ȱ��ȭ
		UCSR0C = (3<<UCSZ00);               // START 1��Ʈ/DATA 8��Ʈ/STOP 1��Ʈ
		
		cbi( DDRE, 0 );                         // RXD0 �� �Է����� ����
		sbi( DDRE, 1 );                         // TXD0 �� ������� ����
		
		#ifdef UART0_RX_INT_ENABLE            // UART0_RX_INT_ENABLE �����ø�
		sbi(UCSR0B, RXCIE0);                 // �������ͷ�Ʈ0 Ȱ��ȭ
		#endif
	}
}


void SerialPutchar(char ch) //�۽��ϴ� �Լ�
{
	while(!(UCSR0A & 0x20));
	UDR0 = ch;
}


unsigned char SerialGetchar(void) //�����ϴ� �Լ�
{
	while(!(UCSR0A & 0x80));
	return UDR0;
}


void PWM4(char ch) //PB4
{
	// OC0(PB4)��¼���
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
	// OC0(PB4)��¼���
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
	// OC0(PB4)��¼���
	double num;
	if(ch<0)
	ch = 0;
	else if(ch>100)
	ch = 100;
	num = ch*2.55;
	OCR1B = num;
}


void PWM7(char ch) //PB7void PWM2(char ch) //PWM1 ����
{
	// OC0(PB4)��¼���
	double num;
	if(ch<0)
	ch = 0;
	else if(ch>100)
	ch = 100;
	num = ch*2.55;
	OCR2 = num;
}




//PB6, PB7 ���α׷� ����� �� �ҵ���
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
