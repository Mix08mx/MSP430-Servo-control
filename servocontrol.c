
#include <msp430g2553.h>
//Config servo
//
#define MCU_CLOCK 1000000
#define PWM_FREQUENCY 50
#define SERVO_STEPS 180 // Número de máximo de pasos que soporta el servo
#define SERVO_MIN 600 // El menor ciclo de trabajo que soporta
#define SERVO_MAX 2400 // El máximo ciclo de trabajo que soporta
int PWM_Period = (MCU_CLOCK / PWM_FREQUENCY); // Periodo del PWM
int PWM_Duty = 0; // %
/////////////////////////////////////////////////////////////////////////////////
#define TXD BIT2
#define RXD BIT1
#define TXLED BIT0
#define RXLED BIT6
// Se hace la declaración de todas las funciones que se van a ocupar en el programa
void delay ( unsigned int );

void ConfigUSCI(void);
void SetClock(void);
void DisplayUART(void);
void recibircadena(void);
void ConfigTimer(void);
unsigned int Rx_Data = 0;
int a,j,k,m;
unsigned int servo_lut[ SERVO_STEPS+1 ];
unsigned int ang=0;
unsigned int servo_stepval, servo_stepnow;
unsigned int i;
int display[]={0X0D,' ','P','o','s','i','c','i','o','n',':','-','-','-','°',0x0D};
/// display[11,12 y 13] de datos
int dispcadena[]={0X0D,' ','C','a','d','e','n','a',':','-','-','-','-','-','-','-','-
','-','-',0x0D}; /// display[11,12 y 13] de datos
int cadena[3];
int main( void )
{
SetClock();
servo_stepval = ( (SERVO_MAX - SERVO_MIN) / SERVO_STEPS );
servo_stepnow = SERVO_MIN;
// Llena un arreglo con valores del timer para cada angulo
for (i = 0; i < SERVO_STEPS; i++) {
servo_stepnow += servo_stepval;
servo_lut[i] = servo_stepnow;
}
P1DIR |= BIT6 + BIT0; // P1.6 = output
P1SEL |= (BIT6 + TXD + RXD); // P1.4 = TA1 output // P1.1 & 2
TA0, rest GPIO
P1SEL2 |= (RXD + TXD) ; // P1.1 = RXD, P1.2=TXD
P2DIR = (BIT0+BIT1+BIT2+BIT3);
ConfigUSCI();
ConfigTimer();
__enable_interrupt();
a=j=k=m=0;
while(1) //bucle infinito
{
recibircadena();
__bis_SR_register(LPM0_bits); // Enter LPM0, interrupts enabled
}
}
/////////////////////////////////////////////////////////////////////////////////////
//////////////
void recibircadena(void){
//delay(1000);
if(Rx_Data!=0x0D){
cadena[m]=Rx_Data-0x30;
display[m+11]=Rx_Data;
delay(500);
m++;
//Rx_Data=0;
}
else if(Rx_Data==0x0D)
{
for(j=0;j<17;j++){ //17
while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
UCA0TXBUF = display[j]; // Send 8-bit
character
}
ang=(cadena[0]*100)+(cadena[1]*10)+cadena[2];
TA0CCR1 = servo_lut[ang];
__delay_cycles(100000);
m=0;
//Rx_Data=0;
}
else if(Rx_Data==0)
{
while(1){
if(Rx_Data!=0x0D){
break;
}
}
m=0;
}
}
void SetClock(void){
WDTCTL = WDTPW + WDTHOLD;
DCOCTL = 0; // Select lowest DCOx and MODx settings<
BCSCTL1 = CALBC1_1MHZ; // Set range DCOCTL =
CALDCO_1MHZ;
DCOCTL = CALDCO_1MHZ;
}
void ConfigUSCI(void){
UCA0CTL1 = UCSWRST; //Place UCA0 in Reset to be
configured
UCA0CTL1 |= UCSSEL_2; // SMCLK
UCA0BR0 = 104; /// 9600 baudios?
UCA0BR1 = 0; // 1MHz 9600
UCA0MCTL = UCBRS0;
UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
}
void ConfigTimer(void)
{
TA0CCTL1 = OUTMOD_7; // TACCR1 reset/set
TA0CCR0 = PWM_Period-1; // PWM Period
TA0CCR1 = PWM_Duty; // TACCR1 PWM Duty Cycle
TA0CTL = TASSEL_2 + MC_1; // SMCLK, upmode
}
void delay(unsigned int ms)
{
while (ms--)
{
__delay_cycles(1000); // set for 16Mhz change it to 1000 for 1 Mhz
}
}
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
P1OUT |= RXLED;
Rx_Data = UCA0RXBUF;
delay(25);
P1OUT &= ~RXLED;
IFG2 = ~UCA0RXIFG;
}
