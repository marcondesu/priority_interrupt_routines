/*
  Objetivo: Utilizar das rotinas de interrupcao de alta e baixa prioridade do microcontrolador PIC18F4550
  para inverter o estado de dois LEDs.

  LED1 -> inverter o estado a cada segundo utilizando o TIMER0
  LED2 -> inverter o estado a cada 200ms (0,2s) utilizando o TIMER1
*/

#define LED1 (PORTB.RB0)
#define LED2 (PORTB.RB1)

void configTIMERS(){
//----------- TIMER0 -----------
/*
 CicloMaquina = Fosc/4 (em microssegundo)
*/
// Fosc = frequência do cristal oscilador do circuito em MHz

/*
 TempoEstouro = CicloMaquina * Prescaler * (Modo - CargaInicial)

  O resultado da equação precisa ser convertido para hexadecimal/binário para que seja possível atribuir o valor
 inicial da contagem no TIMER.
  No modo 16 bits, vêm 2 valores, no nosso caso 0B DC, onde 0B será o mais significativo e DC o
 menos significativo.
*/
// TempoEstouro -> tempo desejado para o final da contagem do TIMER
// Prescaler -> divide a frequência do clock (1:2, 1:4, 1:8, etc), utilizamos apenas o fator de divisão (2, 4, 8, etc)
// Modo -> modo de contagem do TIMER, 8 bits ou 16 bits
// CargaInicial -> valor inicial da contagem do TIMER para que termine no TempoEstouro desejado 

// 1000ms (1s) = 0,5us * 32 *(65536 - x) -> x = 3036 (0B DC)
  T0CON = 0b00000100;     // 16 bits, 1:32, TIMER0 desligado
  TMR0H = 0x0B;           // atribui-se o valor pro bit mais significativo
  TMR0L = 0xDC;           // atribui-se o valor pro bit menos significativo

  T0CON.TMR0ON = 1;       // TIMER0 ligado
  INTCON.TMR0IF = 0;      // zera flag de overflow

//----------- TIMER1 -----------
//200ms = 0,5us * 1:32 * (65536 - x) -> x = 15536 (3C B0)
  T1CON = 0b10111000;     // 16 bits, 1:32, TIMER1 off
  TMR1H = 0x3C;
  TMR1L = 0xB0;

  T1CON.TMR1ON = 1;       // TIMER1 ligado
  PIR1.TMR1IF = 0;        // zera flag de overflow
}

// funcao para rotina de interrupcao de alta prioridade
void highInterruption() iv 0x0008 ics ICS_AUTO {
  if(INTCON.TMR0IF == 1){ // quando a flag de overflow for ativada...
    LED1 = ~LED1;         // inverte estado do LED1

    TMR0H = 0x0B;
    TMR0L = 0xDC;
    /*
      Reatribuimos os mesmos valores para a contagem do TIMER pois, caso contrario, na proxima contagem 
     iria iniciar do 0.
    */

    INTCON.TMR0IF = 0;    // zera a flag de overflow
  }
}

// funcao para rotina de interrupcao de baixa prioridade
void lowInterrupt() iv 0x0018 ics ICS_AUTO {
  if(PIR1.TMR1IF == 1){
    LED2 = ~LED2;

    TMR1H = 0x3C;
    TMR1L = 0xB0;
    PIR1.TMR1IF = 0;
  }
}

void configPIC(){
  ADCON1 = 0x0F;
  TRISB = 0;
  PORTB = 0;
}

void configITMRS(){
// TIMER0
  INTCON2.TMR0IP = 1;    // configura como alta prioridade
  INTCON.TMR0IE = 1;     // interrompe quando ocorre overflow
 
// TIMER1
  IPR1.TMR1IP = 0;       // configura como baixa prioridade
  PIE1.TMR1IE = 1;
}

void ConfigIGlobal(){
  RCON.IPEN = 1;         // habilita niveis de prioridade para interrupcoes
  INTCON.GIEH = 1;       // habilita altas prioridades para interrupcoes de perifericos
  INTCON.GIEL = 1;       // habilita baixas prioridades para interrupcoes de perifericos
}

void main() {
  configPIC();
  configTIMERS();
  configITMRS();
  ConfigIGlobal();
}