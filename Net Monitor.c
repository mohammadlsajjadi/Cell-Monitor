/*****************************************************
This program was produced by the
CodeWizardAVR V2.03.9 Standard
Automatic Program Generator
© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Net Monitor
Version : 1.40
Date    : 2012/9/4
Author  : Sajjadi
Company : EPM
Comments: 


Chip type               : ATmega32L
Program type            : Application
AVR Core Clock frequency: 11.059200 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*****************************************************/

#include <mega32.h>
#include <Bitmap.h>
#include <delay.h>
#include <stdio.h>
#include <math.h>
#include <Font.h>

#define LCD_X_RES 127
#define LCD_Y_RES 63
#define LCD_DATA  PORTA
#define    rs     PORTB.2
#define    rw     PORTB.1
#define    en     PORTB.0
#define    cs1    PORTC.7
#define    cs2    PORTC.6
#define    rst    PORTC.5

#define    light  PORTC.4
#define    vlcd   PORTB.3
#define    igt    PORTD.2
//---------------------------------------------------------------
// Declare your global variables here
char tmp = 0;
int  comno = 0;
int  m = 0;
bit  shutdown = 0;
char bat[] = { 120,252,132,132,132,132,132,252,252,252,252,252 };
//---------------------------------------------------------------
// Standard Input/Output functions
#pragma used+
void init( void );
void batshow( void );
void display( void );
void write_right( unsigned char x );
void write_left( unsigned char x );
void address_right( int x , int y );
void address_left( int x , int  y);
void clear( void );
void lcd_line( unsigned char x1 , unsigned char y1 , unsigned char x2 , unsigned char y2 );
void lcd_put_char( unsigned char x , unsigned char y , unsigned char charac );
void lcd_put_str( unsigned char x , unsigned char y , flash char* str );
void lcd_put_ram( unsigned char x , unsigned char y , char* str );
unsigned int pow_int( unsigned int x , unsigned int y );
void lcd_pixel( unsigned char x , unsigned char y );
void putflash( char flash *str );
void shdown( void );
void shut_down( void );
void query( void );
void key43( void );
#pragma used-
//---------------------------------------------------------------
void init ( void )
    {
        int i = 0, com[4] =  { 0xc0 , 0xb8 , 0x40 , 0x3f };
        rs = 0;
        while ( i < 4 )
            {
                write_left ( com[i] );
                i++;
            }
        i = 0;
        while ( i < 4 )
            {
                write_right ( com[i] );
                i++;
            }
    }
//---------------------------------------------------------------    
void display( void )
    {
        unsigned char page[ 8 ] = { 0xb8 , 0xb9 , 0xba , 0xbb , 0xbc , 0xbd , 0xbe , 0xbf };
        int i = 0 , j = 0 , index = 0;
        while( index < 1024 )
            {
                address_left( page[i] , 0x40 );
                rs = 1;
                for( j = 0 ; j < 64 ; j++ )
                    {
                        write_left( bmp [ index ] );
                        index++;
                    }
                address_right( page[i] , 0x40 );
                rs = 1;
                for( j = 0 ; j < 64 ; j++ )
                    {
                        write_right( bmp [ index ] );
                        index++;
                    }
                i++;
            }
    }
//---------------------------------------------------------------
void batshow( void )
    {
        int i = 0 , j = 0;
        while( i < 12 )
            {
                address_right( 0xb8 , 0x72 );
                rs = 1;
                for( j = 0 ; j < 12 ; j++ )
                    {
                        write_right( bat [ i ] );
                        i++;
                    }
            }
    }
//---------------------------------------------------------------
void write_right ( unsigned char x )
    {
         cs1 = 0;   cs2 = 1;
         delay_us ( 10 );
         LCD_DATA = x;
         en = 1;
         delay_us ( 10 );
         en = 0;   cs1 = 0;   cs2 = 0;
    }
//---------------------------------------------------------------
void write_left ( unsigned char x )
    {
        cs1 = 1;   cs2 = 0;
        delay_us ( 10 );
        LCD_DATA = x;
        en = 1;
        delay_us ( 10 );
        en = 0;   cs1 = 0;   cs2 = 0;
    }
//---------------------------------------------------------------
void address_right ( int x , int y )
    {
        rs = 0;
        write_right ( x );
        write_right ( y );
    }
//---------------------------------------------------------------
void address_left ( int x , int y )
    {
        rs = 0;
        write_left ( x );
        write_left ( y );
    }
//---------------------------------------------------------------
void clear ( void )
    {
        unsigned char page[8] = { 0xb8 , 0xb9 , 0xba , 0xbb , 0xbc , 0xbd , 0xbe , 0xbf };
        int i = 0 , j = 0 , index = 0;
        while ( index < 1024 )
            {
                address_left ( page[i] , 0x40 );
                rs = 1;
                for ( j = 0 ; j < 64 ; j++ )
                    {
                        write_left ( 0 );
                        index++;
                    }
                address_right ( page[i] , 0x40 );
                rs = 1;
                for ( j = 0 ; j < 64 ; j++ )
                    {
                        write_right ( 0 );
                        index++;
                    }
                i++;
            }
    }
//---------------------------------------------------------------
void lcd_pixel( unsigned char x , unsigned char y )
    {
        if ( x > LCD_X_RES ) return;
        if ( y > LCD_Y_RES ) return;
        if( x < 64 )
            {
                #pragma warn-
                address_left( 64 + x , 0xbf - ( int )( y / 8 ) );
                #pragma warn+
                rs = 1;
                write_left( pow_int( 2 , 7 - ( int )fmod( y , 8 ) ) );
            }
        else
            {
                address_right( x , 0xbf - ( int )( y / 8 ) );
                rs = 1;
                write_right( pow_int( 2 , 7 - ( int )fmod( y , 8 ) ) );
            }
    }
//---------------------------------------------------------------
void lcd_line ( unsigned char x1 , unsigned char y1 , unsigned char x2 , unsigned char y2 )
    {
        float dx , dy , xx , yy , len;
        unsigned char l;
        xx = x1;   xx -= x2;   xx *= xx;
        yy = y1;   yy -= y2;   yy *= yy;
        len = sqrt( xx + yy );
        if( x1 > x2 ) dx = ( x1 - x2 ) / len;
        else dx = ( x2 - x1 ) / len;
        if( y1 > y2 ) dy = ( y1 - y2 ) / len;
        else dy = ( y2 - y1 ) / len;
        for( l = 0 ; l < len ; l++ )
            {
                if( x2 > x1 ) xx = ( -dx * l ) + x2;
                else xx = dx * l + x2;
                if( y2 > y1 ) yy = ( -dy * l ) + y2;
                else yy = dy * l + y2;
                lcd_pixel( xx , yy );
            }
    }
//---------------------------------------------------------------    
unsigned int pow_int( unsigned int x , unsigned int y )    // Calculate x^y
    {
        unsigned int i;   unsigned int result;
        result = 1;
        for( i = 0 ; i < y ; i++ ) result = result * x;
        return result;
    }
//---------------------------------------------------------------
void lcd_put_char ( unsigned char x , unsigned char y , unsigned char charac )
    {
        unsigned int i;
        if ( x > LCD_X_RES ) return;
        if ( y > LCD_Y_RES ) return;
        charac -= 32;
        if ( x < 64 )
            {
                for ( i = 0 ; i < 5 ; i++ )
                    {
                        if ( 64 + x + i > 127 )
                            {
                                address_right ( x + i , 0xbf - ( int )( y / 8 ) );
                                rs = 1;
                                write_right ( FontLookup [ charac ][ i ] );
                            }
                        else
                            {
                                #pragma warn-
                                address_left ( 64 + x + i , 0xbf - ( int )( y / 8 ) );
                                #pragma warn+
                                rs = 1;
                                write_left ( FontLookup [ charac ][ i ] );
                            }
                    }
            }
        else
            {
                for (i = 0 ; i < 5 ; i++ )
                    {
                        if ( x + i < 127 )
                            {
                                address_right ( x + i , 0xbf - ( int )( y / 8 ) );
                                rs = 1;
                                write_right ( FontLookup [ charac ][ i ] );
                            }
                    }
            }
    }
//---------------------------------------------------------------
void lcd_put_str ( unsigned char x , unsigned char y , flash char* str )
    {
        unsigned char i = 0;
        while ( *str )
            {
                lcd_put_char ( x + i * 6 , y , *str++ );
                i++;
            }
    }
//---------------------------------------------------------------    
void lcd_put_ram ( unsigned char x , unsigned char y , char* str )
    {
        unsigned char i = 0;
        while ( *str )
            {
                lcd_put_char ( x + i * 6 , y , *str++ );
                i++;
            }
    }
//---------------------------------------------------------------    
void putflash( char flash *str )
{
    char k;   #asm("wdr")
    while ( k = *str++ ) putchar( k );
    #asm("wdr")   putchar( 13 );
}
//---------------------------------------------------------------    
void key43 ( void )
    {
        delay_ms ( 20 );
        if ( ( PINB.4||PINB.5||PINB.6||PINB.7 ) == 0 ) return;
        light = 1;   m = 0;
        
        PORTC.0 = 0;   PORTC.1 = 0;   PORTC.2 = 1;
        
        if ( PINB.4 ){ comno = 0;  lcd_put_str ( 88,0,"PAGE 1" );}       // Key 1 Pressed
        if ( PINB.5 ){ comno = 27; lcd_put_str ( 88,0,"PAGE 4" );}       // Key 4 Pressed
        if ( PINB.6 ){ comno = 48; lcd_put_str ( 88,0,"PAGE 7" );}       // Key 7 Pressed
        if ( PINB.7 ){ comno = 0;  lcd_put_str ( 88,0,"PAGE 1" );}       // Key S Pressed 
        
        PORTC.0 = 0;   PORTC.1 = 1;   PORTC.2 = 0;                   
        
        if ( PINB.4 ){ comno = 13; lcd_put_str ( 88,0,"PAGE 2" );}       // Key 2 Pressed
        if ( PINB.5 ){ comno = 34; lcd_put_str ( 88,0,"PAGE 5" );}       // Key 5 Pressed
        if ( PINB.6 ){ comno = 0;  lcd_put_str ( 88,0,"PAGE 1" );}       // Key 8 Pressed
        if ( PINB.7 ){ comno = 0;  lcd_put_str ( 88,0,"PAGE 1" );}       // Key 0 Pressed  
        
        PORTC.0 = 1;   PORTC.1 = 0;   PORTC.2 = 0;          
        
        if ( PINB.4 ){ comno = 20; lcd_put_str ( 88,0,"PAGE 3" );}       // Key 3 Pressed
        if ( PINB.5 ){ comno = 41; lcd_put_str ( 88,0,"PAGE 6" );}       // Key 6 Pressed
        if ( PINB.6 ){ comno = 0;  lcd_put_str ( 88,0,"PAGE 1" );}       // Key 9 Pressed
        if ( PINB.7 ) shutdown = 1;                                      // Key M Pressed 
        
        PORTC.0 = 1;   PORTC.1 = 1;   PORTC.2 = 1;
    }
//---------------------------------------------------------------   
void shdown ( void )
    {
        light = 1;   clear ();   
        lcd_put_str ( 22 , 35 , "SHUTTING DOWN!" );
        putflash ( "AT" );   delay_ms ( 500 );
        putflash ( "AT^SMSO" );   delay_ms ( 1000 );
        shutdown = 0;   light = 0;   vlcd = 0;
        #asm("jmp 0x00")  
    } 
//---------------------------------------------------------------    
void shut_down ( void )
    {
        PORTC.0 = 1;   PORTC.1 = 0;   PORTC.2 = 0;
        
        if ( PINB.7 ) { delay_ms ( 800 );   if ( PINB.7 ) shdown(); }
        
        PORTC.1 = 1;   PORTC.2 = 1;   shutdown = 0;
    }
//---------------------------------------------------------------    
void query ( void )
    {
        int n = 0;   m++;
        
        do tmp = getchar();
        while ( tmp != ':' );
        
        while ( n != comno ) { tmp = getchar();   if ( tmp == ',' ) ++n; } 
            
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 50 , 56 , tmp );   else goto MNC;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 56 , 56 , tmp );   else goto MNC;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 62 , 56 , tmp );   else goto MNC;
        getchar(); 
        
        MNC:
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 50 , 48 , tmp );   else goto LAC;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 56 , 48 , tmp );   else goto LAC;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 62 , 48 , tmp );   else goto LAC;
        getchar();
            
        LAC:
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 50 , 40 , tmp );   else goto CELL;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 56 , 40 , tmp );   else goto CELL;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 62 , 40 , tmp );   else goto CELL;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 68 , 40 , tmp );   else goto CELL;
        getchar();
        
        CELL:
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 50 , 32 , tmp );   else goto BSIC;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 56 , 32 , tmp );   else goto BSIC;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 62 , 32 , tmp );   else goto BSIC;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 68 , 32 , tmp );   else goto BSIC;
        getchar();
        
        BSIC:
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 50 , 24 , tmp );   else goto CHANN;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 56 , 24 , tmp );   else goto CHANN;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 62 , 24 , tmp );   else goto CHANN;
        getchar();
            
        CHANN:
        lcd_put_str ( 56 , 16 , "      " );
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 56 , 16 , tmp );   else goto RXLEV;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 62 , 16 , tmp );   else goto RXLEV;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 68 , 16 , tmp );   else goto RXLEV; 
        getchar(); 
        
        RXLEV:
        lcd_put_str ( 50 , 8 , "      " );
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 50 , 8 , tmp );    else goto GETREST;
        tmp = getchar();   if ( tmp != ',' ) lcd_put_char ( 56 , 8 , tmp );
        
        GETREST:
        while ( UCSRA.7 ) getchar(); 
    }
//---------------------------------------------------------------
void main ( void )
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7 = Out Func6 = Out Func5 = Out Func4 = Out Func3 = Out Func2 = Out Func1 = Out Func0 = Out 
// State7 = 0 State6 = 0 State5 = 0 State4 = 0 State3 = 0 State2 = 0 State1 = 0 State0 = 0 
PORTA = 0x00;
DDRA = 0xFF;

// Port B initialization
// Func7 = In Func6 = In Func5 = In Func4 = In Func3 = Out Func2 = Out Func1 = Out Func0 = Out 
// State7 = T State6 = T State5 = T State4 = T State3 = 0 State2 = 0 State1 = 0 State0 = 0 
PORTB = 0x00;
DDRB = 0x0F;

// Port C initialization
// Func7 = Out Func6 = Out Func5 = Out Func4 = Out Func3 = Out Func2 = Out Func1 = Out Func0 = Out 
// State7 = 0 State6 = 0 State5 = 0 State4 = 0 State3 = 0 State2 = 0 State1 = 0 State0 = 0 
PORTC = 0x00;
DDRC = 0xFF;

// Port D initialization
// Func7 = In Func6 = In Func5 = In Func4 = In Func3 = In Func2 = Out Func1 = In Func0 = In 
// State7 = T State6 = T State5 = T State4 = T State3 = T State2 = 0 State1 = T State0 = T 
PORTD = 0x00;
DDRD = 0x04;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top = FFh
// OC0 output: Disconnected
TCCR0 = 0x00;
TCNT0 = 0x00;
OCR0 = 0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top = FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A = 0x00;
TCCR1B = 0x00;
TCNT1H = 0x00;
TCNT1L = 0x00;
ICR1H = 0x00;
ICR1L = 0x00;
OCR1AH = 0x00;
OCR1AL = 0x00;
OCR1BH = 0x00;
OCR1BL = 0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top = FFh
// OC2 output: Disconnected
ASSR = 0x00;
TCCR2 = 0x00;
TCNT2 = 0x00;
OCR2 = 0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR = 0x00;
MCUCSR = 0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK = 0x00;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA = 0x00;
UCSRB = 0x18;
UCSRC = 0x86;
UBRRH = 0x00;
UBRRL = 0x47;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR = 0x80;
SFIOR = 0x00;

// Start up
PORTC.2 = 1;
while ( 1 )
    {
        if ( PINB.7 )
            {
                delay_ms ( 1000 );
                if ( PINB.7 ) break;
            }
    };
PORTC.2 = 0; 

// Graphic LCD initialization
igt = 1;   delay_ms ( 300 );
igt = 0;   light = 1;   
rst = 1;   delay_ms ( 10 );
vlcd = 1;  delay_ms ( 10 );
init();    delay_ms ( 10 );
clear();
lcd_line ( 0 , 0 , 127 , 0 );
lcd_line ( 0 , 63 , 127 , 63 );
lcd_put_str ( 32 , 35 , "PAYAM MOWJE" );
lcd_put_str ( 13 , 25 , "TELECOMMUNICATION" );
delay_ms(3000);
clear();
display();
delay_ms ( 3000 );   
putflash ( "AT" );
delay_ms ( 1000 );
putflash ( "AT^SMOND = 1,3" );          // Period  =  1 Second
delay_ms ( 1000 );
clear();
lcd_put_str ( 88 , 0 , "PAGE 1" );
lcd_put_str ( 5 , 56 , "MCC:" );        // Mobile country code
lcd_put_str ( 5 , 48 , "MNC:" );        // Mobile network code
lcd_put_str ( 5 , 40 , "LAC:" );        // Location area code
lcd_put_str ( 5 , 32 , "CELLID:" );     // Cell identifier
lcd_put_str ( 5 , 24 , "BSIC:" );       // Base station identity code
lcd_put_str ( 5 , 16 , "CHANNEL:" );    // Absolute Frequency Channel Number
lcd_put_str ( 5 , 8 , "RSSI:" );        // Received signal level in dBm
lcd_put_char ( 40 , 8 , '-' );
batshow();

PORTC.0 = 1;   PORTC.1 = 1;   PORTC.2 = 1;

while (1)
    {
        if ( m == 60 ) light = 0;
        if ( PINB.4||PINB.5||PINB.6||PINB.7 ) key43();
        if ( UCSRA.7 ) query();
        if ( shutdown ) shut_down();
        if ( m >= 600 ) shdown();
    };
}