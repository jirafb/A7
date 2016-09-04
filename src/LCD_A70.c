// varianta odladena s STM32F051R8T6
// obsahuje inicializaci STM32 periferii

#include "LCD_A70.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_rcc.h"
#include <stdio.h>
#include <stdarg.h>


const unsigned char maskL[] = {0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
const unsigned char maskR[] = {0xFF,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};
const unsigned char bib[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};			// bit in byte

unsigned char VRAM[LCD_Xmax+1][LCD_Pagemax+1];		// velka Video RAM

volatile static unsigned char  X = 0;				// souradnice, kam se vepise pristi znak
volatile static unsigned char  Y = 0;				// souradnice, kam se vepise pristi znak
static unsigned char CharSpace = 1;					// bitova mezera mezi znaky
static unsigned char RowSpace = 1;					// radkovani
static tFont Font;


//_________________________________________________________________________________________________
//______________________________________ LOW LEVEL FUNCTIONS ______________________________________


static void SWDelay(unsigned int d)
{
volatile unsigned int j;
volatile unsigned char k;

for(j = 0; j <= d; j++) {
  for(k = 0; k < 100; k++);
  }
}


// *********************************************************************
// *************************    LCD_A70_OutC    **************************
// *********************************************************************
// posle na LCD prikaz
// *********************************************************************
void LCD_A70_OutC(unsigned char c)
{
  LCD_CMMD;			// COMMAND
  LCD_CE_activate;	// pull ChipEnable to LOW
  SPI_SendData8(SPI1, c);
  while((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
  LCD_CE_deactivate;
  LCD_DATA;			// DATA )setri cas pro sekvenci dat
}


// *********************************************************************
// *************************    LCD_A70_OutD    **************************
// *********************************************************************
// posle na LCD data
// *********************************************************************
void LCD_A70_OutD(unsigned char d)
{
//  LCD_DATA;	// DATA						// <== implicitne
  LCD_CE_activate;	// pull ChipSelect to LOW
  SPI_SendData8(SPI1, d);
  while((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
  LCD_CE_deactivate;
}





// *********************************************************************
// ***********************    LCD_A70_FillRAM    *************************
// *********************************************************************
// zadanym vzorkem vyplni jen VRAM v MCU; neovlivni LCD
// *********************************************************************
void LCD_A70_FillRAM(unsigned char patt)
{
 unsigned char x,y;

 for(y=0; y<(LCD_Pagemax +1); y++) {
   for(x=0;x<(LCD_Xmax+1);x++) {
     VRAM[x][y] = patt;
     }
   }
}



// *********************************************************************
// *************************    LCD_A70_Fill    **************************
// *********************************************************************
// zadanym vzorkem vyplni LCD RAM i VRAM v MCU
// *********************************************************************
void LCD_A70_Fill(unsigned char patt)
{
 unsigned char pg,column;

 for(pg=0; pg<(LCD_Pagemax +1); pg++) {
   LCD_A70_OutC(0x80);    // column: 0
   LCD_A70_OutC(0x40 | pg); // page
   for(column=0;column<(LCD_Xmax+1);column++) {
     LCD_A70_OutD(patt);
	 VRAM[column][pg] = patt;
     }
   }
}



// *********************************************************************
// **********************    LCD_A70_Refresh    **************************
// *********************************************************************
// zkopiruje kompletni VRAM v MCU na LCD
// *********************************************************************
void LCD_A70_Refresh(void)
{
 unsigned char pg,column;

  for(pg=0; pg<(LCD_Pagemax +1); pg++) {
	LCD_A70_OutC(0x80);    // column
	LCD_A70_OutC(0x40 | pg); // page
    for(column=0;column<(LCD_Xmax+1);column++) {
    	LCD_A70_OutD(VRAM[column][pg]);
    }
  }
}




// *********************************************************************
// *************************    LCD_A70_Init    **************************
// *********************************************************************
// inicializuje GPIO a PSI, na kterych je LCD pripojeny
// pripoji napajeni; provede RESET a nastavi klidove hodnoty ridicich signalu
// CE na log.1 (displej neaktivni)
// RESET na log.1 (displej neni drzen v RESET stavu)
// C/D na log.1 (tj. zapisujeme data - tohle usetri cas pro sekvencnim zapisu dat)
// *********************************************************************
void LCD_A70_Init(void)
{
GPIO_InitTypeDef	GPIO_InitStructure;
SPI_InitTypeDef		SPI_InitStructure;

 RCC_AHBPeriphClockCmd(LCD_GPIO_CLOCK, ENABLE);			// GPIOx Periph clock enable
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
 GPIO_InitStructure.GPIO_Pin = LCD_DAT | LCD_CLK;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 GPIO_PinAFConfig(GPIOA, LCD_CLK_PS | LCD_DAT_PS, GPIO_AF_0);

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructure.GPIO_Pin = LCD_RES | LCD_CE | LCD_CD;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 // LCD SPI (SPI1)
 RCC_APB2PeriphClockCmd(LCD_SPI_CLOCK, ENABLE);			// for SPI1 only!!!
 SPI_I2S_DeInit(SPI1);
 SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
 SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
 SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
 SPI_Init(LCD_SPI, &SPI_InitStructure);
 SPI_Cmd(LCD_SPI, ENABLE);

// LCD_VCC_ON;
 SWDelay(10);
 LCD_RES_activate;
 LCD_CE_deactivate;
 SWDelay(100);
 LCD_DATA;
 LCD_RES_deactivate;
 SWDelay(10);

 LCD_A70_OutC(0x21);
 LCD_A70_OutC(0xDA);
 LCD_A70_OutC(0x14);
 LCD_A70_OutC(0x0B);
 LCD_A70_OutC(0x05);
 LCD_A70_OutC(0x20);
 LCD_A70_OutC(0x11);
 LCD_A70_OutC(0x0C);		// Display normal
// LCD_A70_OutC(0x0D);		// Display inverse

// LCD_A70_OutC(0x21);
// LCD_A70_OutC(198);			// kontrast
// LCD_A70_OutC(0x20);




}



//_________________________________________________________________________________________________
//_____________________________________ HIGH LEVEL FUNCTIONS ______________________________________



// *********************************************************************
// ************************    lcd_SetXY    ****************************
// *********************************************************************
// nastavi souradnice X a Y pro pristi znak
// *********************************************************************
void lcd_SetXY(unsigned char ax, unsigned char ay)
{
 X = ax;
 Y = ay;
}



// *********************************************************************
// ************************    lcd_GetXY    ****************************
// *********************************************************************
// vrati souradnice X a Y pro pristi znak
// *********************************************************************
void lcd_GetXY(unsigned char* ax, unsigned char* ay)
{
 *ax = X;
 *ay = Y;
}



// *********************************************************************
// ********************    lcd_SetCharSpace    *************************
// *********************************************************************
// nastavi mezeru mezi znaky v poctu pixelu
// *********************************************************************
void lcd_SetCharSpace(unsigned char aspace)
{
 CharSpace = aspace;
}



// *********************************************************************
// ********************    lcd_GetCharSpace    *************************
// *********************************************************************
// vrati mezeru mezi znaky v poctu pixelu
// *********************************************************************
unsigned char lcd_GetCharSpace(void)
{
 return CharSpace;
}



// *********************************************************************
// *********************    lcd_SetRowSpace    *************************
// *********************************************************************
// nastavi radkovani v poctu pixelu
// *********************************************************************
void lcd_SetRowSpace(unsigned char aspace)
{
 RowSpace = aspace;
}



// *********************************************************************
// *********************    lcd_SetRowSpace    *************************
// *********************************************************************
// vrati radkovani v poctu pixelu
// *********************************************************************
unsigned char lcd_GetRowSpace(void)
{
 return RowSpace;
}



// *********************************************************************
// ***********************    lcd_SelFont    ***************************
// *********************************************************************
// inicializuje font, kterym pro pristi zobrazeni
// *********************************************************************
void lcd_SelFont(const unsigned char* Ff)
{
 Font.Size = (*Ff++ * 256);
 Font.Size += *Ff++;
 Font.Width = *Ff++;
 Font.Height = *Ff++;
 Font.Bpp = *Ff++;
 Font.Fch = *Ff++;
 Font.Lch = *Ff++;
 Font.RLE = Ff;
 Font.Patt = Ff+(Font.Lch-Font.Fch+1);
}


// *********************************************************************
// ********************    lcd_GetCharSpace    *************************
// *********************************************************************
// vrati mezeru mezi znaky v poctu pixelu
// *********************************************************************
unsigned char lcd_GetFontHeight(void)
{
 return RowSpace;
}



// *********************************************************************
// ************************    lcd_Clear    ****************************
// *********************************************************************
// smaze LCD (i VRAM) a nastavi X i Y na 0
// *********************************************************************
void lcd_Clear(void)
{
 LCD_A70_Fill(0);
 X = Y = 0;
}



// *********************************************************************
// **********************    lcd_SetPixel    ***************************
// *********************************************************************
// zobrazi pixel na LCD (i ve VRAM)
// *********************************************************************
void lcd_SetPixel(uint8_t x, uint8_t y)
{
  uint8_t pg, bt;

 pg = y/8;  		// page
 bt = bib[y%8];		// bit in byte
 VRAM[x][pg] |= bt;
 LCD_A70_OutC(0x40 | (LCD_Pagemax-pg));			// adresace pro COG - page
 LCD_A70_OutC(0x80 | x);    						// column LSB  0000 X3X2X1X0
 LCD_A70_OutD(VRAM[x][pg]);
}





// *********************************************************************
// **********************    lcd_ResetPixel    ***************************
// *********************************************************************
// vymaze pixel na LCD (i ve VRAM)
// *********************************************************************
void lcd_ResetPixel(uint8_t x, uint8_t y)
{
  uint8_t pg, bt;

 pg = y/8;  		// page
 bt = bib[y%8];		// bit in byte
 VRAM[x][pg] &= ~bt;
 LCD_A70_OutC(0x40 | (LCD_Pagemax-pg));			// adresace pro COG - page
 LCD_A70_OutC(0x80 | x);    						// column LSB  0000 X3X2X1X0
 LCD_A70_OutD(VRAM[x][pg]);
}





// *********************************************************************
// **********************    lcd_ShowBlock    **************************
// *********************************************************************
// zkopiruje zvoleny blok z VRAM na LCD (rozliseni bloku operuje s bajty)
// *********************************************************************
unsigned char lcd_ShowBlock(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)		// zobrazi znak(y) v ramci zadane ortogonalni oblasti
{
 unsigned char Vx, Vy;

 if(x1 > LCD_Xmax) return 2;					// mimo displej
 if(y1 > LCD_Ymax) return 2;					// mimo displej
 if(x2 < x1) return 3;							// naopak!
 if(y2 < y1) return 3;							// naopak!
 if(x2 > LCD_Xmax) x2 = LCD_Xmax;
 if(y2 > LCD_Ymax) y2 = LCD_Ymax;
 Vx = x1;
// Vy = y1/8;							// tohle jsem opravil 25.12.2015
//  UpMask = maskR[y1 % 8];
//  BottMask = maskL[y2 % 8];
 for(Vy = y1/8; Vy <= y2/8; Vy++) {
   LCD_A70_OutC(0x40 | (LCD_Pagemax-Vy));			// adresace pro COG - page
   LCD_A70_OutC(0x80 | x1);    						// column LSB  0000 X3X2X1X0
   for(Vx = x1; Vx <= x2; Vx++) {
     LCD_A70_OutD(VRAM[Vx][Vy]);
     }
   }
 return 0;
}





// *********************************************************************
// *********************    lcd_ClearBlock    **************************
// *********************************************************************
// vynuluje zvoleny blok ve VRAM i na LCD (rozliseni bloku operuje s pixely)
// *********************************************************************
unsigned char lcd_ClearBlock(unsigned char x1, unsigned char y1,
							 unsigned char x2, unsigned char y2)
{
 unsigned char Vx, Vy,UpMask,BottMask,pr;

 if(x1 > LCD_Xmax) return 2;		// mimo displej
 if(y1 > LCD_Ymax) return 2;		// mimo displej
 if(x2 < x1) return 3;		// naopak!
 if(y2 < y1) return 3;		// naopak!
 if(x2 > LCD_Xmax) x2 = LCD_Xmax;
 if(y2 > LCD_Ymax) y2 = LCD_Xmax;
//  Vx = x1;
 Vy = y1/8;
 pr = (y2/8) - (y1/8) + 1;				// pocet fyzickych radku LCD, kterych se to tyka
//  UpMask = maskR[y1 % 8];				// bajt & UpMask vynuluje vsechny bity od y1 smerem dolu (smerem k y2)
 UpMask = ~maskL[y1 % 8];				// bajt & UpMask vynuluje vsechny bity od y1(vcetne) smerem dolu (smerem k y2)
 BottMask = ~maskR[(y2+1) % 8];		// bajt & BottMask vynuluje vsechny bity od y2(vcetne) smerem nahoru (smerem k y1)

// a ted pozor:
// muze to klidne byt blok na vysku jen jeden bit
// obecne na vysku uzsi nez fyzicky (8bitovy) radek displeje)

 if(pr == 1) {							// jen jeden fyzicky radek LCD
   UpMask = UpMask & BottMask;  		// maska jen pro bitovy vyrez
   }
// prvni radek:
 for(Vx = x1; Vx <= x2; Vx++) {
   VRAM[Vx][Vy] = (VRAM[Vx][Vy] & UpMask);
   }
// dalsi radky?
 if(pr == 1) {
   lcd_ShowBlock(x1,y1,x2,y2);
   return 0;
   }
 for(Vy = (y1/8)+1; Vy <= (y2/8)-1; Vy++) {
   for(Vx = x1; Vx <= x2; Vx++) {
	 VRAM[Vx][Vy] = 0;
     }
   }
// posledni radek
 for(Vx = x1; Vx <= x2; Vx++) {
   VRAM[Vx][Vy] = (VRAM[Vx][Vy] & BottMask);
   }
 lcd_ShowBlock(x1,y1,x2,y2);
 return 0;
}



// *********************************************************************
// **********************    lcd_FillBlock    **************************
// *********************************************************************
// vybarvi zvoleny blok ve VRAM i na LCD (rozliseni bloku operuje s pixely)
// *********************************************************************
unsigned char lcd_FillBlock(unsigned char x1, unsigned char y1,
							unsigned char x2, unsigned char y2)
{
 unsigned char Vx, Vy,UpMask,BottMask,pr;


 if(x1 > LCD_Xmax) return 2;				// mimo displej
 if(y1 > LCD_Ymax) return 2;				// mimo displej
 if(x2 < x1) return 3;						// naopak!
 if(y2 < y1) return 3;						// naopak!
 if(x2 > LCD_Xmax) x2 = LCD_Xmax;
 if(y2 > LCD_Ymax) y2 = LCD_Xmax;
//  Vx = x1;
 Vy = y1/8;
 pr = (y2/8) - (y1/8) + 1;					// pocet fyzickych radku LCD, kterych se to tyka
 UpMask = maskL[y1 % 8];					// bajt | UpMask nastavi vsechny bity >= y1 na log.1 (smerem k y2)
 BottMask = maskR[(y2+1) % 8];				// bajt | BottMask nastavi vsechny bity <= y2 na log.1 (smerem k y1)

// a ted pozor:
// muze to klidne byt blok na vysku jen jeden bit
// obecne na vysku uzsi nez fyzicky (8bitovy) radek displeje)

 if(pr == 1) {								// jen jeden fyzicky radek LCD
   UpMask = UpMask & BottMask;  			// maska jen pro bitovy vyrez
   }

// prvni radek:
 for(Vx = x1; Vx <= x2; Vx++) {
   VRAM[Vx][Vy] = (VRAM[Vx][Vy] | UpMask);
   }
// dalsi radky?
 if(pr == 1) {
   lcd_ShowBlock(x1,y1,x2,y2);
   return 0;
   }
 for(Vy = (y1/8)+1; Vy <= (y2/8)-1; Vy++) {
   for(Vx = x1; Vx <= x2; Vx++) {
     VRAM[Vx][Vy] = 0xFF;
     }
   }
// posledni radek
 for(Vx = x1; Vx <= x2; Vx++) {
   VRAM[Vx][Vy] = (VRAM[Vx][Vy] | BottMask);
   }
 lcd_ShowBlock(x1,y1,x2,y2);
 return 0;
}



// *********************************************************************
// *********************    lcd_InvertBlock    *************************
// *********************************************************************
// invertuje zvoleny blok ve VRAM i na LCD (rozliseni bloku operuje s pixely)
// *********************************************************************
unsigned char lcd_InvertBlock(unsigned char x1, unsigned char y1,
							  unsigned char x2, unsigned char y2)
{
 unsigned char Vx, Vy,UpMask,BottMask,pr;

 if(x1 > LCD_Xmax) return 2;				// mimo displej
 if(y1 > LCD_Ymax) return 2;				// mimo displej
 if(x2 < x1) return 3;						// naopak!
 if(y2 < y1) return 3;						// naopak!
 if(x2 > LCD_Xmax) x2 = LCD_Xmax;
 if(y2 > LCD_Ymax) y2 = LCD_Xmax;
//  Vx = x1;
 Vy = y1/8;
 pr = (y2/8) - (y1/8) + 1;					// pocet fyzickych radku LCD, kterych se to tyka
 UpMask = maskL[y1 % 8];					// bajt | UpMask nastavi vsechny bity >= y1 na log.1 (smerem k y2)
 BottMask = maskR[(y2+1) % 8];				// bajt | BottMask nastavi vsechny bity <= y2 na log.1 (smerem k y1)

// a ted pozor:
// muze to klidne byt blok na vysku jen jeden bit
// obecne na vysku uzsi nez fyzicky (8bitovy) radek displeje)

 if(pr == 1) {								// jen jeden fyzicky radek LCD
   UpMask = UpMask & BottMask;  			// maska jen pro bitovy vyrez
   }

// prvni radek:
 for(Vx = x1; Vx <= x2; Vx++) {
   VRAM[Vx][Vy] = (VRAM[Vx][Vy] ^ UpMask);
   }
// dalsi radky?
 if(pr == 1) {
   lcd_ShowBlock(x1,y1,x2,y2);
   return 0;
   }
 for(Vy = (y1/8)+1; Vy <= (y2/8)-1; Vy++) {
   for(Vx = x1; Vx <= x2; Vx++) {
     VRAM[Vx][Vy] ^= 0xFF;
     }
   }
// posledni radek
 for(Vx = x1; Vx <= x2; Vx++) {
   VRAM[Vx][Vy] = (VRAM[Vx][Vy] ^ BottMask);
   }
 lcd_ShowBlock(x1,y1,x2,y2);
 return 0;
}






// *********************************************************************
// ***********************    lcd_putchar    ***************************
// *********************************************************************
// zobrazi znak (na LCD i do VRAM)
// pracuje primo se souradnici X, takze ji vrati zmenenou
// Y zustane funkcni nezmenene
// zapisuje jen do VRAM a na zaver vola lcd_ShowBlock()
// *********************************************************************
unsigned char lcd_putchar(char CH)	// vraci 0, pokud je vse OK, jinak vrati chybovy kod
{
 unsigned int Offs;					// adresa ve FLASH, na ktere zacina bitovy obraz znaku
 unsigned int ASCII_C;				// slouzi k nalezeni Offs
 const unsigned char* ind;			// ukazatel na pocet sloupcu znaku (sirka v px)
 const unsigned char* Patt;			// ukazatel do souboru s fontem
 unsigned char H,HA;				// vyska fontu v pixelech
 unsigned char W;					// sirka znaku v pixelech
 unsigned char pBS;					// kde v bajtu zacina bitovy obraz naseho znaku
 unsigned char pBT;					// offset fyzickeho radku ve VRAM, kam zacnu zobrazovat znak
 unsigned char B;					// operacni bajt
 unsigned char Vy;					// pracovni index do VRAM
 unsigned char posun;
 unsigned char cx1,cx2,cy1,cy2;



 Vy = Y/8;														// X a Y jsou globalni souradnice
 if(X > LCD_Xmax) return 2;										// zkousim zobrazovat mimo displej (X)
 if(Vy > LCD_Pagemax) return 3;									// zkousim zobrazovat mimo displej (Y)
 if((CH < (int)Font.Fch ) || (CH > (int)Font.Lch)) return 1;	// font neobsahuje pozadovany znak
 H = Font.Height;
 Offs = 0;
 ind = Font.RLE;												// pocet bajtu, ktere obraz znaku zabira
 for(ASCII_C = Font.Fch; ASCII_C < CH; ASCII_C++) {				// hledame shodu ASCII kodu znaku, ktery zobrazime
   Offs += *ind++;
   }
 if(*ind == 0) return 1;										// character isn't defined
 Offs = Offs * H * (Font.Bpp & 0x7F);							// ted je Offset v bitech
 pBS = Offs % 8;  												// kolik bitu jeste patri obrazu predchoziho znaku
 Offs /= 8;                                  					// adresace s granularitou bajtu
 Patt = Font.Patt + Offs;										// Patt: 1.bajt;  *ind: char width
 pBT = Y % 8;
 //Vx = X;														// X a Y jsou globalni souradnice
 Vy = Y/8;
 W = *ind;
 HA = H;
 cx1 = X;
 cy1 = Y;
 cy2 = Y+H;

 while( W > 0) {
   while(HA > 0) {
     B = *Patt;
     if(pBT > pBS) {
       posun = pBT-pBS;					        // posun na cilovou pozici
       B <<= posun;						        // otazka: muze se tady stat, ze bude zapotrebi orez zleva?????????????????????
                                                // BTW, prave se zpracovalo 8-pBT pixelu
       B &= maskL[pBT];					        // odrizne pravou cast (od pBT dolu), protoze tam mohl byt zbytek minuleho sloupce
       if(HA < (8-pBT)) {                        // tzn. ze nastal konec sloupce
         B &= maskR[HA+pBT];                   // a zdroj mohl obsahovat uz i pattern nasledujiciho sloupce
         pBS += HA;                              // jen tolik bitu patrilo do aktualniho sloupce, zbytek uz patri nasledujicimu sloupci
         HA = 0;
         VRAM[X][Vy] |= B;
         break;
         }
       HA -= (8-pBT);                            // tady opet muze nastat situace, ze HA == (8-pBT), tedy HA bude 0
       pBS += (8-pBT);
       VRAM[X][Vy] |= B;
       Vy++;				// @ sekce "pBT > pBS"
       if(Vy > 8) return 3;							// zkousim zobrazovat mimo displej (Y)

       if(pBS == 0) Patt++;
       pBT = 0;                                  // prave jsme naplnili cilovy bajt, takze dalsi zacne od nuly
       }
     else if(pBT < pBS) {
       posun = pBS-pBT;					        // posun na cilovou pozici
       B >>= posun;						        // BTW, prave se zpracovalo 8-pBS pixelu
       B &= maskL[pBT];					        // odrizne pravou cast (od pBT dolu)
       if(HA < (8-pBS)) {                        // tzn. ze nastal konec sloupce
         B &= maskR[HA+pBT];                   // a zdroj mohl obsahovat uz i pattern nasledujiciho sloupce
         pBS += HA;                              // jen tolik bitu patrilo do aktualniho sloupce, zbytek uz patri nasledujicimu sloupci
         HA = 0;
         VRAM[X][Vy] |= B;
         break;
         }
       HA -= (8-pBS);                            // tady opet muze nastat situace, ze HA == (8-pBT), tedy HA bude 0
       pBT = (8-posun);
       VRAM[X][Vy] |= B;
       Patt++;				// @ sekce "pBT < pBS"
       pBS = 0;                                  // prave jsme vypleili zdrojovy bajt, takze dalsi zacne od nuly
       }
     else {          		// @ sekce "pBT = pBS"
       B &= maskL[pBT];					        // odrizne pravou cast (od pBT dolu)
       if(HA < (8-pBS)) {                        // tzn. ze nastal konec sloupce
         B &= maskR[HA+pBT];                   // a zdroj mohl obsahovat uz i pattern nasledujiciho sloupce
         pBS += HA;                              // jen tolik bitu patrilo do aktualniho sloupce, zbytek uz patri nasledujicimu sloupci
         HA = 0;
         VRAM[X][Vy] |= B;
         break;
         }
       HA -= (8-pBS);                            // tady opet muze nastat situace, ze HA == (8-pBT), tedy HA bude 0
       pBT = 0;                                  // vybrali jsme cely zbytek zdroje a zaplnili cely zbytek cile - zaciname od nul
       pBS = 0;
       VRAM[X][Vy] |= B;
       Patt++;
       if(HA > 0) Vy++;
       if(Vy > 8) return 3;						// zkousim zobrazovat mimo displej (Y)

       }
     } // while(HA > 0)
   X++;
   Vy = Y/8;
   if(X > LCD_Xmax) return 2;					// zkousim zobrazovat mimo displej (X)
   if(Vy > LCD_Pagemax) return 3;				// zkousim zobrazovat mimo displej (Y)

   pBT = Y % 8;
   HA = H;
   W--;
   } // while( W > 0)
 cx2 = X-1;
 lcd_ShowBlock(cx1,cy1,cx2,cy2);
 return 0;
}   // konec lcd_putchar()

// ------------------------- popis
//
// mam zadanou adresu prvniho bajtu s patternem
// znam pBS i pBT
// spoctu, kolik bitu z adresovaneho bajtu nalezi aktualnimu znaku:
// zdroj = 8-pBS
// tj. od pBS smerem k MSB, ale jen v pripade, ze zbyla vyska znaku neni mensi, nez toto cislo
// if(HA < zdroj) zdroj = HA.
// !!!! proc to kontrolovat i u zdroje, kdyz uz to kontroluji u cile?
// takze znam pocet vyuzitelnych bitu v adresovanem bajtu
// Tato informace se pouzije k rozeznani, kdy musim posunout ukazatel Patt na dalsi bajt s patternem
// Dal me ovsem zajima, kdy musim posunout ukazatel do VRAM
// cil = 8-pBT
// if(HA < cil) cil = HA

// 1. vzdy znovu nacist aktualni bajt s patternem
// 2. odkud (pBS)
// 3. kam (pBT)
// 4. rotovat
// 5. zjistit, jestli je konec sloupce
// 6. oriznout vysledek porad jeste v registru
// 7. zapsat do VRAM
// 8. vzdy spocitat nove pBS
// 9. upravit "HA"
// 10. pokud je konec sloupce, ukoncit smycku
// 11. v pripade potreby:
//     - posunout ukazatel do VRAM
//     - nacist novy bajt s patternem
// 12. spocitat nove pBS a pBT
// 13. smycka


// @ sekce "pBT > pBS"
// tahle sekce pracuje s pripadem pBT > pBS. tj zdroj se musi posunout vlevo
// mohou nastat nasledujici varianty:
//     a) aktualni zdrojovy bajt obsahuje vic "nasich" bitu, nez aktualni cilovy bajt potrebuje
//         - posun zpusobi, ze cil je nasycen a zdroj je vycerpan jen castecne
//         - reseni je inkrementovat ukazatel do VRAM (ukazat na novy cil) a jenom prepocitat pSB
//     b) aktualni zdrojovy bajt obsahuje presne tolik "nasich" bitu, kolik aktualni cilovy bajt potrebuje
//         - opet musim po provedeni operace inkrementovat ukazatel do VRAM
//         - je treba inkrementovat i ukazatel na zdroj (Pattern), pricemz pBS bude mit hodnotu 0
//     c) stav, kdy zdrojovy bajt obsahuje mene bitu, nez cil potrebuje nemuze nastat, protoze by neohlo by pBT > pBS
//
//     --> a) i b) lze sloucit s tim, ze VRAM inkrementuji vzdy a Patt inkrementuji podle vysledneho stavu pBS (bude-li nula)

// @ sekce "pBT < pBS"
// tahle sekce pracuje s pripadem pBT < pBS. tj zdroj se musi posunout vpravo
// mohou nastat nasledujici varianty:
//     a) pripad, kdy aktualni zdrojovy bajt obsahuje vic "nasich" bitu, nez aktualni cilovy bajt potrebuje
//        znamena, ze jsme dosahli konce sloupce a tento stav je jiz osetren vyse, takze zde nenastane
//     b) aktualni zdrojovy bajt obsahuje presne tolik "nasich" bitu, kolik aktualni cilovy bajt potrebuje
//         - mam dojem, ze to muze znamenat jen zase konec sloupce
//         - po provedeni operace musim inkrementovat ukazatel do VRAM (pokud je konec sloupce, zbytecne)
//         - je treba inkrementovat i ukazatel na zdroj (Pattern), pricemz pBS je treba nastavit na hodnotu 0
//         - HA bude 0 ;-)
//     c) aktualni zdrojovy bajt obsahuje mene bitu, nez aktualni cilovy bajt potrebuje
//         - posun zpusobi vycerpani zdroje, ale cil neni nasycen
//         - je treba inkrementovat Patt, aby se pri dalsim pruchodu zpracovaval nasledujici zdrojovy bajt
//         - pBS se nastavi na nulu (protoze se zacina s dalsim zdroj. bajtem)
//         - nemelo by byt zapotrebi odriznout levou cast, protoze rotaci by se mely zleva nasunout nuly
//         - NEINKREMENTUJE se ukazatel do VRAM!!!!!!!!!!!!!
//
//     --> b) a c) maji spolecne:
//         - inkrementaci Patt
//         - pBS na nulu

// @ sekce "pBT = pBS"
// V prvni rade neni treba rotovat
// Mohou nastat dva pripady:
//    a) zdroj obsahuje vsechny zbyle potrebne bity pro cil
//        - je treba odriznout zleva i zprava nepatricne bity
//        - zkopirovat zdroj do cile
//    b) cil potrebuje jeste i dalsi zdrojove bajty
//





// *********************************************************************
// ***************************    lcd_str    ***************************
// *********************************************************************
// zobrazi retezec, ukonceny 0, na ktery ukazuje pch
// *********************************************************************
unsigned char lcd_str(char * pch)
{
 unsigned char err,zx,zy;
// static unsigned char DisplayStartLine = 0x40;
// unsigned char rezerva_pro_posledni_znakovy_radek, scroll, Yposuv;
// ^^ zakomentovano 25.10.2013

// neni osetren pripad, ze Font neni inicializovan!
 while(*pch) {
   if(*pch == '\n') {					// odradkovani (provedu i "CR")
	 Y += (Font.Height + RowSpace);
     X = 0;
     if(Y > LCD_Ymax) return 1;				// tisk mimo LCD (Y)
     }
   else {
     zx = X; zy = Y;
     err = lcd_putchar(*pch);
     if(err == 3) {
       Y = zy + Font.Height + RowSpace;	// odradkujeme (znak se nevytiskl, protoze byl konec radku)
       X = 0;
       if(Y < (LCD_Ymax+1)) {			// pokud jsme jeste "uvnitr" LCD, zopakujeme tisk tehoz znaku o radek niz
         err = lcd_putchar(*pch);
	     if(err != 0) {
	       X = zx; Y = zy;				//  tisk se nezdaril - vracim koordinaty na puvodni hodnoty
	       return 1;
	       }
         }
       }
     X += CharSpace;					// pridam mezeru mezi znaky
     }
   pch++;
   }
 return 0;
}





// *********************************************************************
// *************************    lcd_printf    **************************
// *********************************************************************
// zobrazi formatovany retezec (VRAM + LCD)
// prebira standardni printf() pro potreby LCD
// *********************************************************************
void lcd_printf(const char * format, ...)
{
 char string[MaxStr];
 va_list myarg;

 va_start(myarg, format);
 vsprintf(string,format, myarg);
 va_end(myarg);
 lcd_str((char *)string);
}




// *********************************************************************
// ************************    lcd_DrawLine    *************************
// *********************************************************************
// nakresli usecku
// *********************************************************************
void lcd_DrawLine1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Color)
{

/*
//   uint8_t i,x,y,px,py;
//   int8_t sdx, sdy;
   uint8_t pg,bt,r,mensi,vetsi;

   int p, dx, dy, rozdil, posun_x, posun_y;

 if(x1 > LCD_Xmax) x1 = LCD_Xmax;
 if(x2 > LCD_Xmax) x2 = LCD_Xmax;
 if(y1 > LCD_Ymax) y1 = LCD_Ymax;
 if(y2 > LCD_Ymax) y2 = LCD_Ymax;

 if(x1 == x2) {        // Vertical Line
   if(y1 < y2) { mensi=y1; vetsi=y2; }
   else { mensi = y2; vetsi=y1; }
   for(r=mensi; r<=vetsi; r++) {
     pg = r/8;			// page
     bt = bib[r%8];	// bit in byte
     if(Color == 0) {
       VRAM[x1][pg] &= ~bt;
       } else {
       VRAM[x1][pg] |= bt;
       }
     LCD_A70_OutC(0x40 | (LCD_Pagemax-pg));			// adresace pro COG - page
     LCD_A70_OutC(0x80 | x1);    						// column LSB  0000 X3X2X1X0
     LCD_A70_OutD(VRAM[x1][pg]);
     }
   }
 else if(y1 == y2) {  	// Horizontal Line
   if(x1 < x2) { mensi=x1; vetsi=x2; }
   else { mensi = x2; vetsi=x1; }
   pg = mensi/8;			// page
   bt = bib[mensi%8];		// bit in byte
   LCD_A70_OutC(0x40 | (LCD_Pagemax-pg));			// adresace pro COG - page
   LCD_A70_OutC(0x80 | mensi);    						// column LSB  0000 X3X2X1X0
   for(r=mensi; r<=vetsi; r++) {
     if(Color == 0) {
       VRAM[x1][pg] &= ~bt;
       } else {
       VRAM[x1][pg] |= bt;
       }
     LCD_A70_OutD(VRAM[r][pg]);
     }
   }

*/

    int16_t dx, dy, rozdil, posun_x, posun_y;
    uint8_t pom;


// if(x1>x2) dx=x1-x2;
// else dx=x2-x1;

 if(x1>x2) {
   pom=x2;
   x2=x1;
   x1=pom;
   pom=y2;
   y2=y1;
   y1=pom;
   }
 dx=x2-x1;
 if(y1>y2) dy=y1-y2;
 else dy=y2-y1;
 rozdil=dx-dy;			// orignal pracuje i se zapornym cislem

 if(Color == 0) lcd_ResetPixel(x1,y1);
 else lcd_SetPixel(x1,y1);
 if (x1 < x2) posun_x = 1; else posun_x = -1;
 if (y1 < y2) posun_y = 1; else posun_y = -1;
 while ((x1 != x2) || (y1 != y2)) {
   int p = 2 * rozdil;
   if (p > -dy) {
     rozdil = rozdil - dy;
     x1 = x1 + posun_x;
     }
   if (p < dx) {
     rozdil = rozdil + dx;
     y1 = y1 + posun_y;
     }
   if(Color == 0) lcd_ResetPixel(x1,y1);
   else lcd_SetPixel(x1,y1);
   }




}






void lcd_DrawLine2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Color)
{
  int16_t p, rozdil, posun_y;
  uint8_t dx,dy;
  uint8_t mensi,vetsi,pg,bt,r;



 if(x1 == x2) {        // Vertical Line
   if(y1 < y2) { mensi=y1; vetsi=y2; }
   else { mensi = y2; vetsi=y1; }
   for(r=mensi; r<=vetsi; r++) {
     pg = r/8;			// page
     bt = bib[r%8];	// bit in byte
     if(Color == 0) {
       VRAM[x1][pg] &= ~bt;
       } else {
       VRAM[x1][pg] |= bt;
       }
     LCD_A70_OutC(0x40 | (LCD_Pagemax-pg));			// adresace pro COG - page
     LCD_A70_OutC(0x80 | x1);    						// column LSB  0000 X3X2X1X0
     LCD_A70_OutD(VRAM[x1][pg]);
     }
   }
 else if(y1 == y2) {  	// Horizontal Line
   if(x1 < x2) { mensi=x1; vetsi=x2; }
   else { mensi = x2; vetsi=x1; }
   pg = y1/8;			// page
   bt = bib[y1%8];		// bit in byte
   LCD_A70_OutC(0x40 | (LCD_Pagemax-pg));			// adresace pro COG - page
   LCD_A70_OutC(0x80 | mensi);    						// column LSB  0000 X3X2X1X0
   for(r=mensi; r<=vetsi; r++) {
     if(Color == 0) {
       VRAM[r][pg] &= ~bt;
       } else {
       VRAM[r][pg] |= bt;
       }
     LCD_A70_OutD(VRAM[r][pg]);
     }
   }

 else {
   if(x1>x2) {
     dx=x2;
     x2=x1;
     x1=dx;
     dy=y2;
     y2=y1;
     y1=dy;
     }
   dx=x2-x1;
   if(y1>y2) dy=y1-y2;
   else dy=y2-y1;
   rozdil=dx-dy;

   if(Color == 0) lcd_ResetPixel(x1,y1);
   else lcd_SetPixel(x1,y1);
   if (y1 < y2) posun_y = 1; else posun_y = -1;

   while ((x1 != x2) || (y1 != y2)) {
     p = rozdil << 1;
     if (p > -dy) {
       rozdil = rozdil - dy;
       x1++;
       }
     if (p < dx) {
       rozdil = rozdil + dx;
       y1 = y1 + posun_y;
       }
     if(Color == 0) lcd_ResetPixel(x1,y1);
     else lcd_SetPixel(x1,y1);
     }
   }
}



void lcd_DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius)
{
	int f = 1 - radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	lcd_SetPixel(x0, y0 + radius);
	lcd_SetPixel(x0, y0 - radius);
	lcd_SetPixel(x0 + radius, y0);
	lcd_SetPixel(x0 - radius, y0);
	while(x < y) {
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;
		lcd_SetPixel(x0 + x, y0 + y);
		lcd_SetPixel(x0 - x, y0 + y);
		lcd_SetPixel(x0 + x, y0 - y);
		lcd_SetPixel(x0 - x, y0 - y);
		lcd_SetPixel(x0 + y, y0 + x);
		lcd_SetPixel(x0 - y, y0 + x);
		lcd_SetPixel(x0 + y, y0 - x);
		lcd_SetPixel(x0 - y, y0 - x);
	}
}






