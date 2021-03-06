#ifndef _LCD_A70_H_
#define _LCD_A70_H_

#include "stm32f0xx.h"


/*
 bitova orientace pixelu LCD Siemens A70

        ^^^^ konektor  (vrsek LCD)
      ..
 bit0  .						\
 bit1  .						|
 bit2  .						|
 bit3  .						|
 bit4  . 						 }  page x
 bit5  .						|
 bit6  .						|
 bit7  .						/
     ..
*/

// const unsigned char maskL[] = {0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
// const unsigned char maskR[] = {0xFF,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};
//
//  B | maskL[0] ... nastavi vsechny bity na log.1
//  B | maskL[1] ... nastavi bity b7..b1 na log.1, bit b0 zustane nezmenen
// "B | maskL[n]" ... n definuje, od ktereho bitu (vcetne), se vsechny vyssi bity (doleva) bajtu B nastavi na log.1

//  B & maskL[0] ... nezmeni vubec nic
//  B & maskL[1] ... vynuluje b0 (log.0)
//  B & maskL[2] ... vynuluje b1 a b0 (log.0)
//  B & maskL[n] ... n definuje bit, ktery jako posledni (smerem od MSB) zustane nezmenen, vsechny nizsi bity bajtu B jsou vynulovany

//  B | maskR[0] ... nastavi vsechny bity na log.1
//  B | maskR[1] ... nastavi bit 0 na log.1, ostatni bity se nezmeni
//  B | maskR[2] ... nastavi bit 0 a bit 1 na log.1, ostatni bity se nezmeni
// "B | maskR[n]" ... n definuje hranicni bit, ktery se jeste nemeni; vsechny nizsi bity (doprava) se nastavi na log.1


//  B & maskR[0] ... nezmeni vubec nic
//  B & maskR[1] ... bit 0 se nezmeni; vsechny vyssi bity se nuluji (log.0)
//  B & maskR[2] ... bity 0 a 1 se nezmeni; vsechny vyssi bity se nuluji (log.0)
//  B & maskR[n] ... n definuje hranicni bit; vsechny bity nizsi (mimo hranicni) zustanou nezmeneny
//                     a vsechny vyssi bity jsou nulovany


typedef struct {
  int					Size;		// font_Size_in_Bytes_over_all_including_Size_itself;
  unsigned char 		Width;		// font_Width_in_Pixels_for_fixed_drawing;
  unsigned char 		Height;		// font_Height_in_Pixel_for_all_Characters;
  unsigned char 		Bpp;		// font_Bits_per_Pixels (not BW only);
  unsigned char 		Fch;		// font_First_Char (in ASCII);
  unsigned char 		Lch;		// font_Last_Char;
  const unsigned char*	RLE;		// font_Char_Widths[font_Last_Char - font_First_Char +1];
									// for each character the separate width in pixels,
									// characters < 128 have an implicit virtual right empty row
									// characters with font_Char_Widths[] == 0 are undefined
  const unsigned char*   Patt;		// bit field of all characters
  } tFont;

#define LCD_Xmax			100		// 0 .. 100 = 101 pixels horizontal
#define LCD_Ymax			63		// 0 .. 63 = 64 pixels vertical
#define LCD_Pagemax			(LCD_Ymax/8)


// LCD pripojeno na GPIOA (SPI1)
#define LCD_GPIO_CLOCK		RCC_AHBPeriph_GPIOA
#define LCD_SPI_CLOCK		RCC_APB2Periph_SPI1
#define LCD_SPI				SPI1
#define LCD_PORT			GPIOA

#define LCD_RES				GPIO_Pin_3
#define LCD_CE				GPIO_Pin_4
#define LCD_CLK				GPIO_Pin_5
#define LCD_CD				GPIO_Pin_6
#define LCD_DAT				GPIO_Pin_7
#define LCD_CLK_PS			GPIO_PinSource5
#define LCD_DAT_PS			GPIO_PinSource7

#define LCD_RES_activate	GPIOA->BRR = LCD_RES		// RESET active in log. 0
#define LCD_RES_deactivate	GPIOA->BSRR = LCD_RES
#define LCD_CMMD			GPIOA->BRR = LCD_CD			// Command when C/D = log. 0
#define LCD_DATA			GPIOA->BSRR = LCD_CD		// Data when C/D = log. 1
#define LCD_CE_activate		GPIOA->BRR = LCD_CE			// CE active in log. 0
#define LCD_CE_deactivate	GPIOA->BSRR = LCD_CE

#define MaxStr				100		// maximalni delka retezce pro lcd_printf

//_________________________________________________________________________________________________
//______________________________________ LOW LEVEL FUNCTIONS ______________________________________

void LCD_A70_OutC(unsigned char c);			// posle na LCD prikaz
void LCD_A70_OutD(unsigned char d);			// posle na LCD data
void LCD_A70_FillRAM(unsigned char patt);	// zadanym vzorkem vyplni jen VRAM v MCU; neovlivni LCD
void LCD_A70_Fill(unsigned char patt);		// zadanym vzorkem vyplni LCD RAM i VRAM v MCU
void LCD_A70_Refresh(void);					// zkopiruje kompletni VRAM v MCU na LCD
void LCD_A70_Init(void);


//_________________________________________________________________________________________________
//_____________________________________ HIGH LEVEL FUNCTIONS ______________________________________

void 			lcd_SetXY(unsigned char ax, unsigned char ay);
void 			lcd_GetXY(unsigned char* ax, unsigned char* ay);
void 			lcd_SetCharSpace(unsigned char aspace);
unsigned char 	lcd_GetCharSpace(void);
void 			lcd_SetRowSpace(unsigned char aspace);
unsigned char 	lcd_GetRowSpace(void);
void 			lcd_SelFont(const unsigned char* Ff);
unsigned char 	lcd_GetFontHeight(void);
void 			lcd_Clear(void);
void			lcd_SetPixel(uint8_t x, uint8_t y);
void			lcd_ResetPixel(uint8_t x, uint8_t y);
unsigned char 	lcd_ShowBlock(unsigned char x1, unsigned char y1,
							  unsigned char x2, unsigned char y2);	// zobrazi blok z VRAM na LCD (bajtove rozliseni souradnic)
unsigned char 	lcd_ClearBlock(unsigned char x1, unsigned char y1,
							  unsigned char x2, unsigned char y2);	// vynuluje blok ve VRAM i na LCD (pixelove rozliseni)
unsigned char 	lcd_FillBlock(unsigned char x1, unsigned char y1,
							  unsigned char x2, unsigned char y2);	// vybarvi blok ve VRAM i na LCD (pixelove rozliseni)
unsigned char 	lcd_InvertBlock(unsigned char x1, unsigned char y1,
							  unsigned char x2, unsigned char y2);	// invertuje blok ve VRAM i na LCD (pixelove rozliseni) a zobrazi jej
unsigned char 	lcd_putchar(char CH);				// vraci 0, pokud je vse OK, jinak vrati chybovy kod
unsigned char 	lcd_str(char * pch);				// vytiskne retezec, ukonceny 0, na ktery ukazuje pch
void 			lcd_printf(const char * format, ...);
void			lcd_DrawLine1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Color);
void			lcd_DrawLine2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Color);
void			lcd_DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius);


#endif
