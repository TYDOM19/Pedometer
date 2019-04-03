////////////////////////////////////////////////////////////////////////////////
////                    COTOPAXI Main 4                          ////
////
////     LOGS TIME,DATE,TEMP,ALTITUDE,HUMIDITY,SHOCK,FF
////     I2C ADRESSES AS FOLLOWS:
////     *ACK addr: 80 //SI7021    DEVICE ADRESS WORKS!
////     *ACK addr: A0 //24LC1025  DEVICE ADRESS WORKS!
////     -ACK addr: C0 //MPL3115A2 DEVICE ADRESS 
////     -ACK addr: AE //MCP7941N  DEVICE ADRESS
////     ADDED 2 TIMER INTERRUPTS AND TESTED USING GLOBAL DISSABLE
////     ADDING RDA 

////////////////////////////////////////////////////////////////////////////////
#include <18F26K20.h>
//#device adc=8                           
#FUSES NOWDT                    //No Watch Dog Timer
#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOPUT                    //No Power Up Timer
#FUSES NODEBUG                  //No Debug mode for ICD
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOWRT                    //Program memory not write protected
#FUSES NOMCLR                     //Master Clear pin enabled
       
#use delay(clock=8000000)
#use rs232(baud=2400,parity=N,xmit=PIN_A7,rcv=PIN_A6,bits=8,errors)
#use i2c(Master,Slow,sda=PIN_C4,scl=PIN_C3)

//#include <math.h>                     
//#include <stddef.h>                
//#include <stdlib.h>
#include <24256_TE.C>
#include <si7021.c> 

#define GRN_LED  PIN_A2
#define BLU_LED  PIN_A3
#define RED_LED  PIN_A4
#define BUTTON   PIN_B0
#define I2C_EN   PIN_C5

int1 sleepflag=0,feedalamaflag=0;
unsigned int8 event_cnt=0;
int16 TIK0=0,TIK1=0;
int16 lpage=0,pages=0;
int8 meas[30],val[8],value=0;
int32 a=0,c,g,i=0,max,addr=0;

#define BUFFER_SIZE 65
BYTE buffer[BUFFER_SIZE];
BYTE next_in = 0;
BYTE next_out = 0;


void update_eeprom(){
   lpage = read_eeprom(1000);//JUST WROTE THE UPDATE 20 SOME LINES ABOVE
   lpage++; //INITIALIZED TO8

   measure_RH();
   measure_temp();
/////////////////////////  OVERWRITE 1ST PAGE  ////////////////////////////////

   //if (lpage>= 1015){
   //   //syncd();
   //   lpage = 1;U
   //}
   write_eeprom(1000,lpage); //UPDATE THE PAGE
  

/////////////////////////  READ IT BACK TO SCREEN ///////////////////////////// 

/////////////////////////  WRITE LOG PAGE(S)  /////////////////////////////////
      meas[0]  = dataMSB;//RH
      meas[1]  = dataLSB;//RH
      meas[2]  = dataMS; //TEMP
      meas[3]  = dataLS; //TEMP

/////////////////////  FOR FACT CHECKING THE VARIABLE TYPES ////////////////////
   printf("New Page #%LU\n\r",lpage);
   for(c=0;c<4;c++){
      g=(c+(4*lpage)); //
      write_eeprom(g,meas[c]); 
      printf("value %LU %lU %lU %U\n\r",g,c,lpage,meas[c]);
   }
   printf("\n\r"); 

///////////////////////////////////////////////////////////////////////////////
   pages=read_eeprom(1000);
   
   for(a=1;a<pages;a++){ //USE POSITION STORED BY THE WRITE PAGE FUNCTION AS MAX PAGE READ 
      for (i=0; i<4; i++){ 
         addr = (i*a+4);
         value=read_eeprom(addr);
         //value=read_int16_eeprom(addr);
         printf("%03U, ",value );   
      }
      printf("\n\r"); 
   }  

///////////////////////////////////////////////////////////////////////////////  
      //shkcnt=0;//RESET AFTER EACH EEPROM WRITE
   
}

void main(){ 
   Si7021_init();
   //init_ext_eeprom();
   port_b_pullups(0b11111110);
   output_high(I2C_EN); 
   delay_ms(5);
   printf("\n\r**********************************************");
   printf("\n\r*          TY'S CUSTOM ELECTRONICS           *");
   printf("\n\r**********************************************");
   printf("\n\r                 PRESENTS THE                 ");  
   printf("\n\r**********************************************");
   printf("\n\r*          COTOPAXI WAYPOINT FINDER          *");
   printf("\n\r*         AUTHORS: TYSON E.| TYLER W.        *");
   printf("\n\r**********************************************");                                                          
   printf("\n\r"); 
   delay_ms(500); 
   
/////////////////////////////// CLEAR MEMORY ///////////////////////////////////
   
   //write_int16_eeprom(1,0);

//////////////////////////  SETUP TIMER 1 //////////////////////////////////////   
   setup_timer_0(RTCC_INTERNAL| RTCC_DIV_1); 
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);

   EXT_INT_EDGE(H_TO_L);  
   ENABLE_INTERRUPTS(INT_EXT);
   //ENABLE_INTERRUPTS(INT_TIMER0);
   ENABLE_INTERRUPTS(INT_TIMER1);
   ENABLE_INTERRUPTS(GLOBAL); 
   set_timer0(0);    // START TIMER1 OVER
   set_timer1(0);    // START TIMER1 OVER
   TIK1=0;           // START COUNTS OVER 
////////////////////////////////////////////////////////////////////////////////

   while(1){
 
      if(sleepflag==1){
         //DISABLE_INTERRUPTS(INT_TIMER0); 
         DISABLE_INTERRUPTS(INT_TIMER1); 
         printf("SNOOZING...\n\r");
         output_low(I2C_EN); 
         sleep();
         sleepflag=0;
         TIK0=0;
         TIK1=0;
         output_high(I2C_EN); 
         EXT_INT_EDGE(H_TO_L);
         //ENABLE_INTERRUPTS(INT_TIMER0);
         ENABLE_INTERRUPTS(INT_TIMER1);
         ENABLE_INTERRUPTS(GLOBAL); 
      }
      
      if(feedalamaflag==1){
         //DISABLE_INTERRUPTS(INT_TIMER0);
         DISABLE_INTERRUPTS(INT_TIMER1);
         DISABLE_INTERRUPTS(INT_EXT);
         DISABLE_INTERRUPTS(GLOBAL);   
         //if(!input(BUTTON)){
            delay_ms(50);
            printf("EAT YOUR FOOD TINA!\n\r");
            output_low(GRN_LED);
            output_high(RED_LED);
            delay_ms(250);
            output_low(BLU_LED);
            output_high(GRN_LED);
            delay_ms(250);
            output_low(RED_LED);
            output_high(BLU_LED);
            delay_ms(250);
            output_high(RED_LED);
            output_high(GRN_LED);
            output_high(BLU_LED);
            //measure_RH();
           // measure_temp();
            update_eeprom();
            TIK1=0;//KEEP RESETTING THE TIMEOUT TIMER1

         feedalamaflag=0;
         EXT_INT_EDGE(H_TO_L);
         //ENABLE_INTERRUPTS(INT_TIMER0);
         ENABLE_INTERRUPTS(INT_TIMER1);
         ENABLE_INTERRUPTS(INT_EXT);
         ENABLE_INTERRUPTS(GLOBAL);
      }
      
      if(TIK0>150){//61 OFLOWS PER SECOND ASSUMING 8MHZ AND 16BIT TIMER
         //OFLOW EVERY 5 SECONDS
         DISABLE_INTERRUPTS(GLOBAL);
         output_low(BLU_LED);
         delay_ms(200);
         output_high(BLU_LED);
         TIK0=0;
         ENABLE_INTERRUPTS(GLOBAL);
      } 
   
      if(TIK1>1800){
         DISABLE_INTERRUPTS(GLOBAL);
         printf("GOING TO SLEEP\n\r");
         output_low(RED_LED);
         delay_ms(500);
         output_high(RED_LED);
         sleepflag=1;
         TIK0=0;
         TIK1=0;
         ENABLE_INTERRUPTS(GLOBAL);
      }
   }
}


#int_TIMER0                //MONITORS 5 SECOND INACTIVITY LOOP
void TIMER0_isr() {
   TIK0++;
}

#INT_TIMER1                
void TIMER1_isr() {                           
   TIK1++;//OVERFLOWS - AUTO CLEARS
}

#INT_RDA
void RDA_isr()     {

   int t;

   buffer[next_in]=getc();
   t=next_in;
   next_in=(next_in+1) % BUFFER_SIZE;
   if(next_in==next_out)
     next_in=t;           // Buffer full !!
}

#INT_EXT
void EXT_isr()     {
   printf("\n\rlama fed\n\r");
   feedalamaflag=1;
}
