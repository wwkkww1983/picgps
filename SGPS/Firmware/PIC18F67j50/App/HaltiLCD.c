
//#use fixed_io(d_outputs=PIN_D0, PIN_D1, PIN_D2, PIN_D3)
#use fast_io(d)
//#use fast_io(e)

#define LCD_WRX      PIN_D0
#define LCD_DC      PIN_D1
#define LCD_CSX      PIN_D2
#define LCD_RESX   PIN_D3
//#define LCD_VDDI   PIN_A2
#define LCD_RDX      PIN_B6
//#define LCD_VDD      PIN_C0
#define LCD_DATAOUT(data)      output_e(data)
#define LCD_DATAIN()		input_e()

#define LCDCMD_NOP   0x00
#define LCDCMD_SWRESET   0x01
#define LCDCMD_READ_DISPLAY_ID	0x04
#define LCDCMD_READ_DISPLAY_STATUS	0x09
#define LCDCMD_SLPIN   0x10   //Sleep in
#define LCDCMD_SLPOUT   0x11   //Sleep out
#define LCDCMD_PTLON   0x12   //Partial Mode On
#define LCDCMD_NORON   0x13   //Normal Mode On
#define LCDCMD_DSPOFF   0x28   //Display Off
#define LCDCMD_DSPON   0x29   //Display On
#define LCDCMD_CASET   0x2A   //Column address set
#define LCDCMD_PASET   0x2B   //Page address set
#define LCDCMD_RAMWR   0x2C   //Memory Write
#define LCDCMD_RGBSET   0x2D   //RGB table set
#define LCDCMD_RAMRD	0x2E	//Memory Read
#define LCDCMD_IDMOFF   0x38   //Idle mode off
#define LCDCMD_IDMON   0x39   //Idle mode On
#define LCDCMD_COLMOD   0x3A   //Interface pixel format
#define LCDCMD_MADCTL    0x36
#define LCDCMD_READ_ID1	 0xDA
#define LCDCMD_READ_ID2	 0xDB
#define LCDCMD_READ_ID3	0xDC	


#define MADCTL_HORIZ      0xA8
#define MADCTL_VERT       0x68

#define LCD_WIDTH      208
#define LCD_HEIGHT      176

int lcd_x;
int lcd_y;
int16 bkgColor;
int16 textColor;

void LCD_WriteData(byte data)
{
   output_high(LCD_DC);
   output_high(LCD_WRX);
   LCD_DATAOUT(data);
   output_low(LCD_WRX);
   delay_cycles(1);
   output_high(LCD_WRX);
}

#define FAST_WRITE(data)   LCD_DATAOUT(data);output_low(LCD_WRX);output_high(LCD_WRX);

void LCD_ReadData(byte *buf, int len)
{
	int i;
	output_high(LCD_DC);
	for(i=0; i<len; i++)
	{
		output_low(LCD_RDX);
		delay_cycles(1);
		output_high(LCD_RDX);
		*buf = LCD_DATAIN();
		buf++;
	}	
	
}
	
void LCD_WriteCommand(byte command)
{
   output_low(LCD_DC);
   output_high(LCD_WRX);
   LCD_DATAOUT(command);
   output_low(LCD_WRX);
   delay_cycles(1);
   output_high(LCD_WRX);
}

void LCD_SetWindow(int left, int top, int right, int bottom)
{
   LCD_WriteCommand(LCDCMD_CASET);
   LCD_WriteData(0);
   LCD_WriteData(left);
   LCD_WriteData(0);
   LCD_WriteData(right);
   LCD_WriteCommand(LCDCMD_PASET);
   LCD_WriteData(0);
   LCD_WriteData(top);
   LCD_WriteData(0);
   LCD_WriteData(bottom);
}

//Sleep in : 0.8mA
void LCD_SleepIn()
{
   LCD_WriteCommand(LCDCMD_SLPIN);
}

void LCD_SleepOut()
{
   LCD_WriteCommand(LCDCMD_SLPOUT);
}

//Idle mode: 1.5mA
void LCD_IdleMode()
{
   LCD_WriteCommand(LCDCMD_NORON);
   LCD_WriteCommand(LCDCMD_IDMON);
}

//Normal mode: 4mA
void LCD_NormalMode()
{
   LCD_WriteCommand(LCDCMD_NORON);
   LCD_WriteCommand(LCDCMD_IDMOFF);
}

void LCD_PartialMode()
{
   LCD_WriteCommand(LCDCMD_PTLON);
   LCD_WriteCommand(LCDCMD_IDMON);
}

void LCD_DisplayOff()
{
   output_low(LCD_CSX);
  LCD_WriteCommand(LCDCMD_SLPIN);
  LCD_WriteCommand(LCDCMD_DSPOFF);
   output_high(LCD_CSX);
}

void LCD_Shutdown()
{
   	LCD_DisplayOff();
   
   	delay_ms(50);
   output_high(LCD_RESX);	
}

void LCD_Init()
{
   int i=0;
   
   lcd_x = 0;
   lcd_y = 0;
   textColor = 0x0;
   bkgColor = 0xFFFF;
   
   set_tris_d(0b01110000);
   
   output_Low(LCD_CSX);
   output_low(LCD_RESX);
   //output_high(LCD_VDDI);
   //output_high(LCD_VDD);

   delay_ms(1);

   output_low(LCD_RESX);
   output_high(LCD_RESX);

//   output_high(LCD_RDX);
   output_high(LCD_WRX);
   output_high(LCD_DC);

   //Enable LCD
   output_low(LCD_CSX);

   delay_ms(10);

   LCD_WriteCommand(LCDCMD_NOP);
   LCD_WriteCommand(LCDCMD_SWRESET);   //Reset LCD
   delay_ms(120);
   LCD_WriteCommand(LCDCMD_SLPOUT);   //Sleep out
   LCD_WriteCommand(LCDCMD_DSPON);      //Display on
   LCD_WriteCommand(LCDCMD_NORON);      //Normal mode
   LCD_WriteCommand(LCDCMD_IDMOFF);   //Idle mode off
   //LCD_WriteCommand(LCDCMD_IDMON);
   LCD_WriteCommand(LCDCMD_COLMOD);   //Bits per Pixel
   //LCD_WriteData(0x02);   //8bit   
   LCD_WriteData(0x05);   //16bit
   
   LCD_WriteCommand(LCDCMD_MADCTL);   //Memory data acces control
   LCD_WriteData(MADCTL_HORIZ);    //X Mirror and BGR format
    //    lcdWrdata(0x98);

   LCD_WriteCommand(LCDCMD_RGBSET);   //Set RGB color lookup table
   for(i=0; i<32; i++)   //RED
      LCD_WriteData(i*2);
   for(i=0; i<64; i++)   ///GREEN
      LCD_WriteData(i);
   for(i=0; i<32; i++)   //BLUE
      LCD_WriteData(i*2);
}

//Text drawing functions
#include "ascii.h"

void LCD_Gotoxy(int x, int y)
{
   lcd_x = x;
   lcd_y = y;
}
void LCD_NewLine()
{
  lcd_x  = 0;
  lcd_y   += 14;
  if(lcd_y > LCD_HEIGHT)
     lcd_y -= 14;
  }

void LCD_SetTextColor(int16 nColor)
{
   textColor = nColor;
}

void LCD_SetBackgroundColor(int16 nColor)
{
   bkgColor = nColor;
}

void gps_FloodFill(int left, int top, int right, int bottom, int16 color)
{
   int16 i, numPixel;
  //select controller
  output_low(LCD_CSX);
  
   LCD_SetWindow(left,top, right, bottom);
   LCD_WriteCommand(LCDCMD_RAMWR);
   
   output_high(LCD_DC);
   numPixel = ((int16)(right-left+1))*((int16)(bottom-top+1));
   for(i=0; i<numPixel; i++)
   {
             FAST_WRITE(make8(color, 1));
             FAST_WRITE(make8(color, 0));
    }	
 
  output_high(LCD_CSX);
}

void LCD_ClearScreen()
{
   int16 i;
   //LCD_Floor(0,0, LCD_WIDTH-1, LCD_HEIGHT-1, bkgColor);
   //return;
  //select controller
  output_low(LCD_CSX);
  
   LCD_SetWindow(0,0, LCD_WIDTH-1, LCD_HEIGHT-1);
   LCD_WriteCommand(LCDCMD_RAMWR);
   
   output_high(LCD_DC);
   for(i=0; i<LCD_WIDTH*LCD_HEIGHT; i++)
   {
             FAST_WRITE(make8(bkgColor, 1));
             FAST_WRITE(make8(bkgColor, 0));
    }
    
  output_high(LCD_CSX);
}

/*****************************************************************************
 *
 * Description:
 *    Draw one character withc current foreground and background color
 *    at current xy position on display. Update x-position (+8).
 *
 ****************************************************************************/
void LCD_OutChar8(int data)
{
   int i,j, byteToShift;
   int16 mapOffset;
   
  //select controller
  output_low(LCD_CSX);

  if (data <= 127)
  {
    data -= 30;
    mapOffset = data;
    mapOffset *= 14;

   LCD_SetWindow(lcd_x, lcd_y, lcd_x+7, lcd_y+13);
    LCD_WriteCommand(LCDCMD_RAMWR);
   output_high(LCD_DC);
   
    for(i=0; i<14; i++)
    {
      byteToShift = charMap8x14[mapOffset++];
      for(j=0; j<8; j++)
      {
        if (byteToShift & 0x80)
           {
             //LCD_WriteData(textColor);
             FAST_WRITE(make8(textColor, 1));
             FAST_WRITE(make8(textColor, 0));
          }
        else
           {
             //LCD_WriteData(bkgColor);
             FAST_WRITE(make8(bkgColor, 1));
             FAST_WRITE(make8(bkgColor, 0));
           }
        byteToShift <<= 1;
      }
    }
  }

  //deselect controller
  output_high(LCD_CSX);

  lcd_x += 8;
}

void LCD_PutChar(int data)
{
  if (data == '\n')
    LCD_NewLine();
  else if (data != '\r')
  {
      LCD_OutChar8(data);
   }

}
