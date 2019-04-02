///////////////////////////////////////////////////////////////////////////////
////                         CMOS_BRKOUT1.C                               
////                            REV A BRD                                  
////                                                                                                                               
////        PIN ASSIGNMENTS MADE, ADDED TIE INS FOR ADC MEASUREMENT AND I2C
////        
///////////////////////////////////////////////////////////////////////////////     
                                                                           
#include <18F26K20.h>                                                                                                                                                                                   
#FUSES INTRC_IO, NOMCLR                                                                     
#use delay(clock=16Mhz, internal)                                                                                          

#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7)                                                                                                                  
#use i2c(Master,FORCE_SW,Slow,sda=PIN_B2,scl=PIN_B1,STREAM=M1)                                                                                                                   
                                                                                              
#define V35_RESET    PIN_A4                                                                                                                                                  
#define V33_RESET    PIN_A5                                                                        
#define RELAY        PIN_A6   //SPARE2                                                     
#define SPARE3       PIN_A7                                                        
#define ROWSHIFTIN   PIN_B0                                                        
#define ROWCLK       PIN_B1                                          
#define FPGA_SPARE1  PIN_B2                                                   
#define FPGA_SPARE2  PIN_B3                                      
#define V35_ALARM    PIN_B4                          
#define ICSP_CLK     PIN_B6         
#define ICSP_DATA    PIN_B7                          
#define SPARE4       PIN_C0          
#define SPARE5       PIN_C1                                         
#define LED_PWM      PIN_C2   
#define I2C_SCL      PIN_C3             
#define I2C_SDA      PIN_C4  
#define REED_SWCH    PIN_C5   //SPARE6 
#define RX           PIN_C6 
#define TX           PIN_C7    
                                                                          
#define ROW_WIDTH   13                                                   
#define ROW_WIDTH_2 8                                         
#define MAX_PAGES   1024                                                                     
                                                                                                                                           
                                                       
#include <si7021_STREAM.c>                                                                           
#include <float.h>                                                           
#include <math.h>                                                       
#include <stddef.h>                                                  
#include <stdlib.h>                                                                                 
#include <string.h>                                                      
#include <stdio.h>                                               
#include <stdint.h>                                                                    
                                                                               
int32 A;                                        
int16 B;                                                                   
unsigned int8 C;                                                                         
signed int8 D;               
int1 initflag=0,eraselog_flag=0,verify_flag=0;       
                                                                                                          
byte portval;                                                                              
                                    
void clear_ram_25(){  
   for(e=0;e<100;e++){//CLEAR ALL MEMORY 1ST 25 PAGES                         
      maxG[e]  =        0xFF;                               
      thestatus1[e] =   0xFF;         
      seconds[e] =      0xFF;                        
      minutes[e] =      0xFF;               
      hours[e] =        0xFF;  
      years[e] =        0xFF;                        
      months[e] =       0xFF;                        
      days[e] =         0xFF;        
      tempslsb[e] =     0xFF;                                  
      tempsmsb[e] =     0xFF;                                                                       
      humiditylsb[e] =  0xFF;      
      humiditymsb[e] =  0xFF;                    
      thestatus2[e] =   0xFF;
   }
}                                           
  
void get_last_twentyfive(){                                                        
   //printf("GET LAST 25 CALLED\n\r");                      
   clear_ram_25();                             
                                      
   page_num =  read_int16_eeprom(1);                                                     
   PG_LSB =     read_ext_eeprom(1);                                                                                              
   PG_MSB =     read_ext_eeprom(2);                                       
   tSEC =       read_ext_eeprom(3);                              
   tMIN =       read_ext_eeprom(4);                   
   tHR =        read_ext_eeprom(5);                                           
   tYEAR =      read_ext_eeprom(6);                            
   tMON =       read_ext_eeprom(7);                                          
   tDAY =       read_ext_eeprom(8);                   
   ////////////  NEW  ////////////                                      
   PRT_HRS =       read_int16_eeprom(9);                                                                                    
   PRT_HRS_LSB =   read_ext_eeprom(9);                             
   PRT_HRS_MSB =   read_ext_eeprom(10);                                   
   ET_MIN =        read_ext_eeprom(11);                                              
   BOOTCYCLES_LSB= read_ext_eeprom(12);                    
   BOOTCYCLES_MSB= read_ext_eeprom(13);               
   BOOTCYCLES=     read_int16_eeprom(12);  
   
   //printf("PAGE_NUM %LU\n\r",page_num);  
   
   starting_point= page_num-25;                      
                                                 
   if(starting_point<=0){                                    
      starting_point=0;}                    
                                
   else if(starting_point>400){                 
      starting_point=starting_point-400;}   
   
   else if(starting_point>300){                                
      starting_point=starting_point-300;} 
                                                            
   else if(starting_point>200){    
      starting_point=starting_point-200;}
                                                        
   else if(starting_point>100){                 
      starting_point=starting_point-100;}   
                         
   else                                                                            
      break;               
                       
   for(e=1;e<26;e++){                        
                                                                
      h=((897+(starting_point*128))+(e*128));                                              
                                                                         
                                     
      thestatus1[e] =   read_ext_eeprom(h+1);            
      seconds[e] =      read_ext_eeprom(h+2);                           
      minutes[e] =      read_ext_eeprom(h+3);                  
      hours[e] =        read_ext_eeprom(h+4);    
      years[e] =        read_ext_eeprom(h+5);                           
      months[e] =       read_ext_eeprom(h+6);                            
      days[e] =         read_ext_eeprom(h+7);               
      tempslsb[e] =     read_ext_eeprom(h+8);                                      
      tempsmsb[e] =     read_ext_eeprom(h+9);                                                                           
      humiditylsb[e] =  read_ext_eeprom(h+10);          
      humiditymsb[e] =  read_ext_eeprom(h+11);                         
      thestatus2[e] =   read_ext_eeprom(h+12); 
      if((thestatus1[e]!=0xFF) && (seconds[e]!=0xFF)){               
         maxG[e]=e;      
      }                                                         
      else                                                
         maxG[e]=0xFF;                              
                                                                  
 //////////////////////// TURNED THIS OFF TO SAVE TIME ////////////////////////                                                                                        
 //     printf(" PG -%05LU  h - %05LU  e - %05LU  page# - %03U\n\r",page_num,h,e,maxG[e]);     
   }
                                                                  
}                           
                                                   
                                                                       
                                                         
void main(){                                                             
                                                                                                                                    
                                                      
   for(x=0;x<10;x++){                 
      output_high(SPARE3);                       
      delay_ms(50);      
      output_low(SPARE3);        
      delay_ms(50);            
   }   

                                                 
///////////////////  SETUP TIMER1 & INITIALIZE INTERRUPTS  ////////////////////   
                                                                                                                    
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);                        
   set_timer1(0);    // START TIMER1 OVER                                                                                                   
   TICK1=0;          // START COUNTS OVER                          
   ENABLE_INTERRUPTS(INT_RB4);                                                         
   ENABLE_INTERRUPTS(INT_RB5);                                                   
   ENABLE_INTERRUPTS(INT_EXT);                                                       
   ENABLE_INTERRUPTS(INT_SSP);///ADDED BACK IN 7/5/17               
   ENABLE_INTERRUPTS(INT_TIMER1);                                   
   ENABLE_INTERRUPTS(GLOBAL);                                                                   
/////////////////////////////////////////////////////////////////////////////// 
   //clearflag=1; //ADDED 2/2 FOR DEBUGGGING WITH KENT          
   //sleepflag=1; //ADDED to GET around VEXT INT POS    
///////////////////////////////////////////////////////////////////////////////
                                                                       
   while (TRUE){                                                    
                                                                               
      if(sleepflag==1){   //PUTS BOARD TO SLEEP WHEN NOT CONNECTED TO EXT POWER     
         DISABLE_INTERRUPTS(INT_TIMER1);            
         DISABLE_INTERRUPTS(GLOBAL);         
                   
      ////////////////////////  POWER DOWN THE PERIPHERALS  ///////////////////                 
         output_low(I2C_EN);       //PWR DOWN                                                                                                    
         output_low(E2PWR);        //PWR DOWN                                                                
         output_low(HUMTEMP);      //PWR DOWN                            
         output_float(EEPROM_SCL);   //PWR DOWN                             
         output_float(EEPROM_SDA);   //PWR DOWN        
         output_low(LED);          //PWR DOWN                                    
         //printf("\n\POWERING DOWN\n\r");                        
                                                                                  
      /////////////////////////       SLEEP    ////////////////////////////////   
         sleep();                      
                                                  
      /////////////////////////  WAKEUP SEQUENCE  /////////////////////////////   
         sleepflag=0;                      
         output_high(HUMTEMP);     //PWR DOWN     
         delay_ms(80);
         output_high(I2C_EN);      //PWR DOWN                         
         output_high(E2PWR);       //PWR DOWN                                                         
                    
         output_float(EEPROM_SCL); //CLK                             
         output_float(EEPROM_SDA); //CLK  
         //output_low(ICSP_CLK);   // Not Used -- Except during ICSP programming         
         //output_low(ICSP_DATA);  // Not Used -- Except during ICSP programming   
         delay_ms(50);                                 
         //wake_accel();             // WAKE UP THE ACCEL - SET TO MEASURE                
         output_high(LED);         //PWR DOWN         
         //printf("\n\POWERING BACK UP\n\r");                                                             
         //ENABLE_INTERRUPTS(INT_SSP);                     
         //ENABLE_INTERRUPTS(GLOBAL);                         
         output_low(LED);    //PWR DOWN                              
         ENABLE_INTERRUPTS(INT_RB4);                                     
         ENABLE_INTERRUPTS(INT_RB5);                                
         ENABLE_INTERRUPTS(INT_EXT);                                                       
         ENABLE_INTERRUPTS(INT_SSP);///ADDED BACK IN 7/5/17        
         ENABLE_INTERRUPTS(INT_TIMER1);                              
         ENABLE_INTERRUPTS(GLOBAL);  
                                                   
         //accel_clone(); //ADDED IN 12/11/17 TO TEST     
      }                                                                     
                                                                                                                                          
                                                                   
                                                               
      if(VEXT_P_flag==1){  
         DISABLE_INTERRUPTS(INT_SSP);        
         DISABLE_INTERRUPTS(INT_TIMER1);    
         get_configuration();//ADDED 4_11_18                    
         DISABLE_INTERRUPTS(GLOBAL);       
         output_high(LED);                                                 
         delay_ms(10);                                
         //if(input(V_EXT) && (panel_type==3)){//4336Wv4 only                                                                       
         //   //printf("\n\rTETHER CONNECTED\n\r");                                                                                         
         //   start_panel_run_timer();                        
         //   timestamp_convert();                             
         //   get_last_twentyfive();                            
         //}                         
         if(input(V_EXT)){//ALL FUTURE PANELS                                                                       
            //printf("\n\rTETHER CONNECTED\n\r");                                                                                         
            start_panel_run_timer();                                   
            timestamp_convert();                       
            dump_data();                      
         }                                                    
         output_low(LED);                      
         TICK1=0;                                              
         VEXT_P_flag=0;                   
         //EXTflag=1;///ADDED THIS FOR TESTING IN ENVIRONMENTAL CHAMBER  
         ENABLE_INTERRUPTS(INT_SSP); 
         ENABLE_INTERRUPTS(INT_TIMER1); //CHANGED THIS TO KEEP TIMER1 DISABLED  
         ENABLE_INTERRUPTS(GLOBAL);                             
      }                                               
                                                                        
      if(VEXT_N_flag==1){ 
         DISABLE_INTERRUPTS(INT_SSP); 
         DISABLE_INTERRUPTS(INT_TIMER1);    
         DISABLE_INTERRUPTS(GLOBAL);   
         output_high(LED);                           
         delay_ms(10);                     
                                   
         if(!input(V_EXT)){ //TEMP HAS SWITCHED LOW AGAIN           
            //printf("\n\rTETHER DISCONNECTED\n\r\n\r");                                       
            update_panel_run_time(); 
         }                                          
                                                                 
         output_low(LED);                                                              
         TICK1=0;                 
         VEXT_N_flag=0;      
         ENABLE_INTERRUPTS(INT_SSP); 
         ENABLE_INTERRUPTS(INT_TIMER1);   
         ENABLE_INTERRUPTS(GLOBAL); 
      }                   
                                   
      if(OTEMP_P_flag==1){   
         DISABLE_INTERRUPTS(INT_SSP); 
         DISABLE_INTERRUPTS(INT_TIMER1);    
         DISABLE_INTERRUPTS(GLOBAL);
         utemp_flag=1;        
         update_eeprom();  //UPDATE THE EVENTS/SHOCKS/TIMESTAMPS                                                                                            
         OTEMP_P_flag=0;      
         TICK1=0;                 
         //EXTflag=1; ///ADDED THIS 11_29_17 
         ENABLE_INTERRUPTS(INT_SSP); 
         ENABLE_INTERRUPTS(INT_TIMER1);   
         ENABLE_INTERRUPTS(GLOBAL);      
      }                        
                                                       
      if(OTEMP_N_flag==1){                        
         DISABLE_INTERRUPTS(INT_SSP); 
         DISABLE_INTERRUPTS(INT_TIMER1);
         DISABLE_INTERRUPTS(GLOBAL);
         otemp_flag=1;   
         update_eeprom();  //UPDATE THE EVENTS/SHOCKS/TIMESTAMPS                                                                                                 
         OTEMP_N_flag=0;    
         TICK1=0;         
         //EXTflag=1; ///ADDED THIS 11_29_17 
         ENABLE_INTERRUPTS(INT_SSP); 
         ENABLE_INTERRUPTS(INT_TIMER1);
         ENABLE_INTERRUPTS(GLOBAL); 
      }  
                                                       
      if(EXTflag==1){                                                   
         DISABLE_INTERRUPTS(INT_SSP); 
         DISABLE_INTERRUPTS(INT_TIMER1);
         DISABLE_INTERRUPTS(GLOBAL);
         shock_flag=1;          
         update_eeprom();  //UPDATE THE EVENTS/SHOCKS/TIMESTAMPS           
         EXTflag=0;                                          
         TICK1=0;    
         ENABLE_INTERRUPTS(INT_SSP); 
         ENABLE_INTERRUPTS(INT_TIMER1);
         ENABLE_INTERRUPTS(GLOBAL);                                                                               
      }                                                            
                                                                
      if(new_time_flag==1){                                                       
         DISABLE_INTERRUPTS(INT_TIMER1);    
         DISABLE_INTERRUPTS(GLOBAL);
         timestamp_convert2();                    
         new_time_flag=0;  
         TICK1=0;                                 
         //ENABLE_INTERRUPTS(INT_TIMER1);   
         ENABLE_INTERRUPTS(GLOBAL);  
      }    
                                                   
      if(get_rtc_flag==1){                                               
         DISABLE_INTERRUPTS(INT_TIMER1);    
         DISABLE_INTERRUPTS(GLOBAL);                  
         timestamp_convert();                    
         get_rtc_flag=0;  
         TICK1=0;                       
         ENABLE_INTERRUPTS(INT_SSP);  
         //ENABLE_INTERRUPTS(INT_TIMER1);   
         ENABLE_INTERRUPTS(GLOBAL);
      }                                            
                                                                                                          
      if(TICK1>1800){     //30 SECONDS -1,800 OFLOWS                                
         output_high(LED);
         //printf("TIMER1 IS UP\n\r");    
         delay_ms(50);                                                
         TICK1=0;               
         sleepflag=1;               
      }                           
                                                               
   }                                                     
}                                                                       
                                                    
                                                                                                                                                                                           
#INT_SSP                  //I2C ACTIVITY ISR - 2 FLAGS ARE SET HERE                                                   
void ssp_interrupt(){                                                                                           
                                
   output_high(LED);                                      
                                                        
   state = i2c_isr_state();                                                   
   if(state== 0 )     //ADRESS MATCH W/ WRITE - read to clear int but tosses out the value       
      i2c_read(S1);                                                                                  
   if(state == 0x80)  //MATCH R/- read to clear int but tosses out the value   
      i2c_read(S1,2);                                  
   if(state >= 0x80){ //TRANSMIT RECIEVED, 0x81 to 0xFF RESPOND WITH I2CWRITE 
      hstateflag=1;   //SET FLAG - service the i2c_write with if, else-if statments                
      RCV_IN=0;       //***THIS RESETS THE COUNTER UPON 1ST CALL FOR I2C_READ FROM MASTER***
      //RCV_IN2=0;       
      //RCV_IN3=0;
   }                                                             
   else if(state > 0){ //MASTER HAS WRITTEN DATA reads in all values 1 to 7F                           
      rcv_buffer[state - 1] = i2c_read(S1);//fills up buffer[] 1 through 79   
      //this really means that it can take up to 78 i2c write commands in succession      
      sspflag=1;//MOVED THIS INTO LAST ELSE IF STATMENT 
   }
   output_low(LED);                                           
}                                          
                                                 
#INT_RB                   //INT PORT B4-7 EXTERNAL ISR - 4 FLAGS ARE SET HERE 
void  RB_isr(void){                                       
   // POLL PINS TO DETERMINE WHICH DIRECTION LOW 2 HIGH OR HIGH 2 LOW 
   output_high(LED);                     
   delay_ms(100); //DEBOUNCE                 
   ///////////////////////  READ THE STATES ///////////////////////////////////  
   vexternal=input_state(V_EXT);             
   tempsensor=input_state(O_TEMP);        
   portval=input_b();//have to read the port or at least B4-7 to clear the interrupt     
/////////////////////////  DID THEY CHANGE ////////////////////////////////////
   if(vexternal != vold){            
      if(input(V_EXT)){//TEMP HAS SWITCHED LOW AGAIN
         VEXT_P_flag=1;                                                
         //goto ending;               
      }                                                                    
      if(!input(V_EXT)){ //TEMP HAS SWITCHED LOW AGAIN           
         VEXT_N_flag=1;                       
         //goto ending;       
      }                                                                        
   }                                    
    //tempsensor=input_state(O_TEMP);                                    
   if(tempsensor != told){         
      if(input(O_TEMP)){//TEMP HAS SWITCHED LOW AGAIN     
         OTEMP_P_flag=1;                                                                      
      }                                                    
      if(!input(O_TEMP)){ //TEMP HAS SWITCHED LOW AGAIN           
         OTEMP_N_flag=1;                                       
      }                                      
      //ending: break;       
   }                                               
   output_low(LED);                                   
   vold=vexternal;   
   told=tempsensor;    
///////////////////////////////////////////////////////////////////////////////   
}                                                             
                                        
#INT_EXT                  //INTO EXTERNAL ISR - 1 FLAG IS SET HERE             
void  EXT_isr(void){                                                    
   output_high(LED);                                                                                
   delay_ms(20);                          
                                              
   if(input(ROWSHIFTIN )){                                  
      delay_ms(70); 
   }  
   if(input(ROWCLK)){                                  
      delay_ms(70); 
   }  
   //////////////////////////////// CLEAR INTERRUPT //////////////////////////////                            
                                   
/////////////////////////////////////////////////////////////////////////////// 
   }                           
   output_low(LED);                                                                                                            
   EXTflag=1;                             
                          
} 
                                     
#INT_TIMER1               //TIMER1  ISR - VALUE TICK1 INCREMENTED HERE                  
void  TIMER1_isr(void){                                               
   TICK1++;                                                                                                                                                            
}   
                                            
                           
                                                                    
