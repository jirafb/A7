
#include "stm32f0xx_conf.h"
#include "LCD_A70.h"
#include "jusart.h"
#include "f7x9.h"


void pixel(void);
void line1(void);
void line2(void);
void Circle(void);

extern char Line_buffer[];


int main(void)
{

//uint16_t i;
//uint32_t cislo;
char ch;
uint8_t i;


 LCD_A70_Init();
 lcd_SetRowSpace(1);
 lcd_SetCharSpace(1);
 lcd_SelFont(&f7x9[0]);
 lcd_Clear();
 jusart_Config();
// lcd_printf("USART init  OK");

 lcd_DrawLine2(0,0,100,63,1);
 lcd_DrawLine2(100,0,0,63,1);
 lcd_DrawLine2(0,63,100,63,1);
 lcd_DrawLine2(100,63,100,0,1);
 lcd_DrawLine2(100,0,0,0,1);
 lcd_DrawLine2(0,0,0,63,1);

 lcd_DrawLine2(20,1,80,62,1);
 lcd_DrawLine2(80,1,20,62,1);
 lcd_DrawLine2(40,1,60,62,1);
 lcd_DrawLine2(60,1,40,62,1);

 lcd_DrawLine2(1,63,99,63,0);
 lcd_DrawLine2(100,62,100,1,0);
 lcd_DrawLine2(99,0,1,0,0);
 lcd_DrawLine2(0,1,0,62,0);
 lcd_DrawLine2(0,0,100,63,0);
 lcd_DrawLine2(100,0,0,63,0);

 lcd_DrawLine2(20,1,80,62,0);
 lcd_DrawLine2(80,1,20,62,0);
 lcd_DrawLine2(40,1,60,62,0);
 lcd_DrawLine2(60,1,40,62,0);


 while(1) {

   if(jusart_RXCR() != 0) {
     jusart_GetLine();
     if(Line_buffer[0] == '~') {
       switch(Line_buffer[1]) {
         case 'c': lcd_Clear();
                   break;
         case 'n': lcd_printf("\n");
                    break;
         case 'p': pixel();
                    break;
         case 'l': line1();
                    break;
         case 'L': line2();
                    break;
         case 'm':
         case 'M': Circle();
                    break;
         }

       }
     else {
       i = 0;
       while((ch = Line_buffer[i]) != 0) {
         lcd_printf("%c",ch);
         ++i;
         }
       }
     }


   }




/*
//LCD_A70_Fill(0xFF);
lcd_Clear();
lcd_SetPixel(0,0);
lcd_SetPixel(1,0);
lcd_SetPixel(2,0);
lcd_SetPixel(3,1);
lcd_SetPixel(4,1);
lcd_SetPixel(10,10);
lcd_SetPixel(10,11);
lcd_SetPixel(10,12);
lcd_SetPixel(10,13);

lcd_FillBlock(10,10,20,20);

lcd_ClearBlock(12,12,18,18);

lcd_SetXY(5,40);
lcd_printf("Hello World!\n");
lcd_InvertBlock(4,39,74,48);

lcd_InvertBlock(4,39,74,48);
lcd_InvertBlock(0,39,100,48);



 jusart_Puts("\nkontrast:\n");


    LCD_A70_OutC(0x21);
    LCD_A70_OutC(210);
    LCD_A70_OutC(0x20);
    LCD_A70_OutC(0x21);
    LCD_A70_OutC(215);
    LCD_A70_OutC(0x20);
    LCD_A70_OutC(0x21);
    LCD_A70_OutC(220);
    LCD_A70_OutC(0x20);
    LCD_A70_OutC(0x21);
    LCD_A70_OutC(225);
    LCD_A70_OutC(0x20);
    LCD_A70_OutC(0x21);
    LCD_A70_OutC(215);
    LCD_A70_OutC(0x20);
*/




  while(1) {
//    tmp = jusart_Scanf(254);
/*
    LCD_A70_OutC(0x21);
    LCD_A70_OutC(tmp);
    LCD_A70_OutC(0x20);
*/
    }
}



void pixel(void)
{
  uint8_t co,dig, i, x=0, y=0;
  uint16_t mezi;

 if(Line_buffer[2] == 0) return;
 if(Line_buffer[2] == 'r') co = 0;
 else co = 1;
 i=3; mezi=0;
 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Xmax) return;
   ++i;
   }
 if(dig == 0) return;
 x = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac
 while((dig = Line_buffer[i]) != 0) {
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Ymax) return;
   ++i;
   }
 y = mezi;
 if(co == 0) lcd_ResetPixel(x,y);
 else lcd_SetPixel(x,y);

}



void line1(void)
{
  uint8_t co,dig, i, x1=0,x2=0,y1=0,y2=0;
  uint16_t mezi;

 if(Line_buffer[2] == 0) return;
 if((Line_buffer[2] == 'r') || (Line_buffer[2] == 'R')) co = 0;
 else co = 1;
 i=3; mezi=0;
 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Xmax) return;
   ++i;
   }
 if(dig == 0) return;
 x1 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Ymax) return;
   ++i;
   }
 if(dig == 0) return;
 y1 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Xmax) return;
   ++i;
   }
 if(dig == 0) return;
 x2 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Ymax) return;
   ++i;
   }
 y2 = mezi;
 if(co == 0) lcd_DrawLine1(x1,y1, x2, y2, 0);
 else lcd_DrawLine1(x1,y1, x2, y2, 1);

}





void line2(void)
{
  uint8_t co,dig, i, x1=0,x2=0,y1=0,y2=0;
  uint16_t mezi;

 if(Line_buffer[2] == 0) return;
 if((Line_buffer[2] == 'r') || (Line_buffer[2] == 'R')) co = 0;
 else co = 1;
 i=3; mezi=0;
 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Xmax) return;
   ++i;
   }
 if(dig == 0) return;
 x1 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Ymax) return;
   ++i;
   }
 if(dig == 0) return;
 y1 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Xmax) return;
   ++i;
   }
 if(dig == 0) return;
 x2 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Ymax) return;
   ++i;
   }
 y2 = mezi;
 if(co == 0) lcd_DrawLine2(x1,y1, x2, y2, 0);
 else lcd_DrawLine2(x1,y1, x2, y2, 1);

}




void Circle(void)
{
  uint8_t co,dig, i, x0=0,y0=0,radius=0;
  uint16_t mezi;

 if(Line_buffer[2] == 0) return;
 if((Line_buffer[2] == 'r') || (Line_buffer[2] == 'R')) co = 0;
 else co = 1;
 i=3; mezi=0;
 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Xmax) return;
   ++i;
   }
 if(dig == 0) return;
 x0 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if(dig == ',') break;
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Ymax) return;
   ++i;
   }
 if(dig == 0) return;
 y0 = mezi;
 mezi = 0;
 ++i;					// preskoc oddelovac

 while((dig = Line_buffer[i]) != 0) {
   if((dig < '0') || (dig > '9')) return;
   mezi *= 10;
   mezi += (dig - '0');
   if(mezi > LCD_Ymax) return;
   ++i;
   }
 radius = mezi;
 if(co == 0) lcd_DrawCircle(x0,y0,radius);
 else lcd_DrawCircle(x0,y0,radius);

}





////////////////////////////////////////////////////////////////////////////////
//            Cortex-M0 Processor Exceptions Handlers                         //
////////////////////////////////////////////////////////////////////////////////

void NMI_Handler(void)
{
	while(1);
}

void HardFault_Handler(void)
{
	while(1);
}

void MemManage_Handler(void)
{
	while(1);
}

void BusFault_Handler(void)
{
	while(1);
}

void UsageFault_Handler(void)
{
	while(1);
}

void SVC_Handler(void)
{
	while(1);
}

void DebugMon_Handler(void)
{
	while(1);
}

void PendSV_Handler(void)
{
	while(1);
}

void SysTick_Handler(void)
{
	while(1);
}



