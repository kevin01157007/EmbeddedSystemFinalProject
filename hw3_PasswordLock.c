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

// 函式宣告
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
    SendStr("8051 密碼鎖啟動...\r\n");

    while (1) {
        // 判斷是否收到完整指令
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
                    TempData[i] = 0; // 進入設定模式先清除顯示
                SendStr("password_setup");
            }

            else if (uart_buf[1] == '\r') {

                unsigned char num = uart_buf[0] - '0';

                // 輸入第一個數字時先清除顯示
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

/*--------------------- 顯示函數 ---------------------*/
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

/*--------------------- 延時 ---------------------*/
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

/*--------------------- UART 中斷處理 ---------------------*/
void UART_SER (void) interrupt 4 //串行中斷服務程序
{
    unsigned char Temp;          //定義臨時變量 
   
   if(RI)                        //判斷是接收中斷產生
     {
	  	RI=0;                      //標誌位清零
	  	Temp=SBUF;                 //讀入緩衝區的值
	//  P1=Temp;                   //把值輸出到P1口，用於觀察
    //  SBUF=Temp;                 //把接收到的值再發回電腦端
	  	uart_buf[uart_index] = Temp;
		uart_index++;
		if (uart_index == 10) uart_index = 0;
	 }
//   if(TI)                        //如果是發送標誌位，清零
//     TI=0;
}


