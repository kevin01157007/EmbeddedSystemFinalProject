#include <reg52.h>
#include <string.h>

#define DataPort P0
sbit LATCH1 = P2^2;
sbit LATCH2 = P2^3;

unsigned char code dofly_DuanMa[] = {
  0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
  0x77,0x7c,0x39,0x5e,0x79,0x71
};
unsigned char code dofly_WeiMa[] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
unsigned char TempData[8];
unsigned char password[4] = {1, 2, 3, 4};

unsigned char uart_buf[10];
unsigned char uart_index = 0;
unsigned char uart_char;


bit set_mode = 0;
unsigned char input_index = 0;
unsigned char input_buffer[4];

// �禡�ŧi
void Init_Timer0(void);
void InitUART(void);
void SendStr(unsigned char *s);
void Display(unsigned char FirstBit, unsigned char Num);
void DelayUs2x(unsigned char t);
void DelayMs(unsigned char t);

void main(void) {
    unsigned char i;
	bit clear = 0;
    Init_Timer0();
    InitUART();
    SendStr("8051 �K�X��Ұ�...\r\n");

    while (1) {
        // �P�_�O�_���짹����O
        if (uart_index == 5) {
            uart_buf[uart_index] = '\0';
            uart_index = 0;

            if (strcmp(uart_buf, "ENTER") == 0) {
				if(set_mode) {
					for (i = 0; i < 8; i++)
	                	TempData[i] = 0;
					set_mode = 0;
					SendStr("setup_finish");
				}
				else {
	                bit correct = 1;
	                for (i = 0; i < 4; i++)
	                    correct = correct && (input_buffer[i] == password[i]);
	
	                for (i = 0; i < 8; i++)
	                    TempData[i] = 0;
	
	                if (correct) {
	                    TempData[0] = 0x3f; // o
	                    TempData[1] = 0x73; // p
	                    TempData[2] = 0x79; // E
	                    TempData[3] = 0x54; // n
	                    SendStr("correct!");
	                } else {
	                    TempData[0] = 0x79; // E
	                    TempData[1] = 0x50; // r
	                    TempData[2] = 0x50; // r
	                    SendStr("wrong");
	                }
	
					clear = 1;
	                input_index = 0;
				}
            }

            else if (strcmp(uart_buf, "SET\r\r") == 0) {
                set_mode = 1;
                input_index = 0;
                for (i = 0; i < 8; i++)
                    TempData[i] = 0; // �i�J�]�w�Ҧ����M�����
                SendStr("password_setup");
            }

            else if (uart_buf[1] == '\r') {

                unsigned char num = uart_buf[0] - '0';

                // ��J�Ĥ@�ӼƦr�ɥ��M�����
                if (input_index == 0 && clear == 1) {
                    for (i = 0; i < 8; i++)
                        TempData[i] = 0;
                }

                if (set_mode) {
                    password[input_index] = num;
                    TempData[4 + input_index] = dofly_DuanMa[num];
                    input_index++;
                    if (input_index == 4) {
                        input_index = 0;

                    }
                } else {
                    input_buffer[input_index] = num;
                    TempData[4 + input_index] = dofly_DuanMa[num];
                    input_index++;
                    if (input_index >= 4)
                        input_index = 0;
                }
            }
        }
    }
}

/*--------------------- ��ܨ�� ---------------------*/
void Display(unsigned char FirstBit,unsigned char Num) {
    static unsigned char i = 0;
    DataPort = 0;
    LATCH1 = 1; LATCH1 = 0;
    DataPort = dofly_WeiMa[i + FirstBit];
    LATCH2 = 1; LATCH2 = 0;
    DataPort = TempData[i];
    LATCH1 = 1; LATCH1 = 0;
    i++;
    if (i == Num) i = 0;
}

/*--------------------- ���� ---------------------*/
void DelayUs2x(unsigned char t) { while (--t); }
void DelayMs(unsigned char t) {
    while (t--) {
        DelayUs2x(245);
        DelayUs2x(245);
    }
}

/*--------------------- Timer0 Init ---------------------*/
void Init_Timer0(void) {
    TMOD |= 0x01;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}
void Timer0_isr(void) interrupt 1 {
    TH0 = (65536 - 1000) / 256;
    TL0 = (65536 - 1000) % 256;
    Display(0, 8);
}

/*--------------------- UART Init ---------------------*/
void InitUART(void) {
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = 0xFD;
    TR1 = 1;
    ES = 1;
    EA = 1;
}
void SendByte(unsigned char dat) {
    SBUF = dat;
    while (!TI);
    TI = 0;
}
void SendStr(unsigned char *s) {
    while (*s) SendByte(*s++);
}

/*--------------------- UART ���_�B�z ---------------------*/
void UART_SER (void) interrupt 4 //��椤�_�A�ȵ{��
{
    unsigned char Temp;          //�w�q�{���ܶq 
   
   if(RI)                        //�P�_�O�������_����
     {
	  	RI=0;                      //�лx��M�s
	  	Temp=SBUF;                 //Ū�J�w�İϪ���
	//  P1=Temp;                   //��ȿ�X��P1�f�A�Ω��[��
    //  SBUF=Temp;                 //�Ⱶ���쪺�ȦA�o�^�q����
	  	uart_buf[uart_index] = Temp;
		uart_index++;
		if (uart_index == 10) uart_index = 0;
	 }
//   if(TI)                        //�p�G�O�o�e�лx��A�M�s
//     TI=0;
}


