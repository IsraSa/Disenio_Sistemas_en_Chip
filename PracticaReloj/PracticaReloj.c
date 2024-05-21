/*
 * PracticaReloj.c
 *
 * Created: 16/05/2024 10:34:49 a. m.
 * Author: Samsung
 */

  #asm
    .equ __lcd_port=0x0B   //LCD en PuertoD
    .equ __lcd_EN=3
    .equ __lcd_RS=2
    .equ __lcd_D4=4
    .equ __lcd_D5=5
    .equ __lcd_D6=6
    .equ __lcd_D7=7
   #endasm

#include <stdio.h>
#include <io.h>
#include <delay.h>
#include <display.h>

unsigned char i=0;
signed char H=0,M=0,S=0;
char Cadena[17];

void tono(float);
void noTono();

void main(void)
{
    SetupLCD();
    PORTC = 0x1F;       // Pull ups de PC0 a PC4

    TCCR1A=0;
    TCCR1B=0x0A;            //Timer 1 en CTC con CK/8
    OCR1AH=31249/256;
    OCR1AL=31249%256;
    while (1)      // El ciclo completo tarda 0.25seg
    {
       //Imprimir hora en el LCD
       sprintf(Cadena, "%02i:%02i:%02i  ", H, M, S);
       MoveCursor(3, 0);
       StringLCDVar(Cadena);

       while(TIFR1.OCF1A==0);   //Espera a 0.25seg
       TIFR1.OCF1A=1;           //Borra Bandera
       i++;

       if (i==4)
       {
            i=0;
            S++;
            if(S==2)
                noTono();
            if (S==60)
            {
                S=0;
                M++;
                if (M==60)
                {
                    tono(400);
                    M=0;
                    H++;
                    if (H==24)
                        H=0;
                }

            }
       }
       //Revisar switches
       if(PINC.4 == 0 && H < 23){
            H++;
       }
       if(PINC.3 == 0 && H > 0){
            H--;
       }
       if(PINC.2 == 0 && M < 59){
            M++;
       }
       if(PINC.1 == 0 && M > 0){
            M--;
       }
       if(PINC.0 == 0){
            S = 0;
       }

    }
}
void tono(float frec){
    float cuentas;
    unsigned int cuentasEnt;
    DDRB.6 = 1;                      // PB1 de Salida
    TCCR0A = 0x40;                   // Timer 1 en CTC
    TCCR0B = 0x09;                   // Con pre-escalador de CK
    cuentas = 500000.0 / frec;       // Cálculo de cuentas
    cuentasEnt = cuentas;            // Nos quedamos con la parte entera de cuentas
    if (cuentas-cuentasEnt >= 0.5)   // Si la parte decimal es mayor o igual a 0.5
    {
        cuentasEnt++;               // Aumentamos la parte entera
    }
    OCR0A = (cuentasEnt -1);
}

void noTono(){
    TCCR0A = 0x00;                  // Apaga el tono
    TCCR0B = 0x00;                  // Apaga el tono
    PORTB.1 = 0;
}