 
/*-----------------------------------------------
  �W�١G��f�q�H
  �����Gwww.doflye.net
  �s�g�Gshifang
  ����G2009.5
  �ק�G�L
  ���e�G�s���n��f�Ϊ�usb���f�ܹq���A�U���ӵ{�ǡA���}�q��
        ���}��f�ոյ{�ǡA�N�i�S�v�]�m��9600�A�L�_������
        ����11.0592MHz�A�o�e�M�����ϥΪ��榡�ۦP�A�p���ϥ�
        �r�ū��榡�A���_�쭫�ҵ{�ǡA�i�H�ݨ챵���� UART test�A�޳N�׾¡Gwww.doflye.net �Цb�o�e�Ͽ�J���N�H
		�M��b�o�e�ϵo�e���N�H���A�����Ϫ�^�P�˫H���A�����f���o�L�~
------------------------------------------------*/

#include<reg52.h> //�]�t�Y���A�@�뱡�p���ݭn��ʡA�Y���]�t�S��\��H�s�����w�q 
#include <string.h>                       
#define MAX 10
unsigned char buf[MAX];
unsigned char head = 0;


/*------------------------------------------------
                   ����n��
------------------------------------------------*/
void SendStr(unsigned char *s);

/*------------------------------------------------
                    ��f��l��
------------------------------------------------*/
void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: �Ҧ� 1, 8-bit UART, �ϯ౵��  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit ����
    TH1   = 0xFD;               // TH1:  ���˭� 9600 �i�S�v ���� 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 ���}                         
    EA    = 1;                  //���}�`���_
   // ES    = 1;                  //���}��f���_
}  

       
/*------------------------------------------------
                    �D���
------------------------------------------------*/
void main (void)
{
unsigned long i;
InitUART();


SendStr("UART TEST");

ES    = 1;                  //���}��f���_
while (1)                       
    {
 		if (head == 5)
		{
			buf[head] = '\0'; 
			head = 0;
			if ((strcmp(buf,"LEDON") == 0))	
				SendStr("REC_LED_ON");
			else if((strcmp(buf,"LEDOF") == 0))
				SendStr("REC_LED_OFF");	
				
		}

    }
}

/*------------------------------------------------
                    �o�e�@�Ӧr�`
------------------------------------------------*/
void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    �o�e�@�Ӧr�Ŧ�
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 ��ܦr�Ŧ굲���лx�A�q�L�˴��O�_�r�Ŧ꥽��
  {
  SendByte(*s);
  s++;
  }
//  SendByte(0x0d);
//  SendByte(0x0a);
}
/*------------------------------------------------
                     ��f���_�{��
------------------------------------------------*/
void UART_SER (void) interrupt 4 //��椤�_�A�ȵ{��
{
    unsigned char Temp;          //�w�q�{���ܶq 
   
   if(RI)                        //�P�_�O�������_����
     {
	  	RI=0;                      //�лx��M�s
	  	Temp=SBUF;                 //Ū�J�w�İϪ���
	//  P1=Temp;                   //��ȿ�X��P1�f�A�Ω��[��
    //  SBUF=Temp;                 //�Ⱶ���쪺�ȦA�o�^�q����
	  	buf[head] = Temp;
		head++;
		if (head == MAX) head = 0;
	 }
//   if(TI)                        //�p�G�O�o�e�лx��A�M�s
//     TI=0;
} 


 
