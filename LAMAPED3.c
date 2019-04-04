#include <18F26K20.h>     

#FUSES INTRC_IO            
#FUSES NOWDT                    
#FUSES NOMCLR                                            
#use delay(clock=8000000)      
#use rs232(baud=9600,parity=N,xmit=PIN_C7,rcv=PIN_C6)                                 
#use i2c(Master,Fast,sda=PIN_C4,scl=PIN_C3)                                                                       
                                    
#define FF        PIN_B0                                    
#define switch1   PIN_B4                                 
#define switch2   PIN_B5                                      
#define PGC       PIN_B6                           
#define PGD       PIN_B7          
#define LED       PIN_C0                                       
#define I2C_EN    PIN_A4                                           
#define RX        PIN_C6                                       
#define TX        PIN_C7     

#include <KX126.C>                       
#include <float.h>                                                           
#include <math.h>                                                                   
#include <stddef.h>                                                                
#include <stdlib.h>                                                                                 
#include <string.h>                                                      
#include <stdio.h>                                                
#include <stdint.h>                                 

int8 i,bl,x,cntr1=0;             
int8 status,val=0,SW1old,SW2old; 
int8 count=0,buffer1,buffer2,buffer3,buffer4,buffer5;                                          
int16 TIK1 = 0,cntr2=0;                                    
int1 sleepflag=0, sw1_flag=0,sw2_flag=0;    
int1 SWT1, SWT2,RDAflag=0,Pflag_Set=0,readflag=0,cnt_thresh_flag=0;          
                                                                                    
int8 get_ack_status(int8 address) 
{                
int8 status; 

i2c_start();                                      
status = i2c_write(address);  // Status = 0 if got an ACK 
i2c_stop(); 

if(status == 0)         
   return(TRUE); 
else                                      
   return(FALSE);     
}       
                           
void odom_reset(){
   ped = 0;
   storage = 0;                 
   cntr2 = 0;                              
   cnt_thresh_flag = 0;        
   output_low(LED);    
   printf("PEDOMETER RESET\n\r");
}    

                                           
//================================= 
void main(){                                                   
                         
                     
port_b_pullups(0b11001111); //PULLUPS ENABLED ON RB4-7
set_tris_b(0b11110000);//B4|B5 INPUTS ONLY
setup_comparator(NC_NC_NC_NC);                
val=input_b();           
setup_timer_0(RTCC_INTERNAL| RTCC_DIV_1); 
                                                                                                                
                                                                                                    
for(x=0;x<10;x++){                                                                             
   delay_ms(100);                              
   printf("\n\rStart in %U:\n\r",10-x);                   
 }                                      
                     
clear_interrupt(INT_RB);                                
ENABLE_INTERRUPTS(INT_RB|INT_RB4|INT_RB);//SW1|SW2 PRESSED           
ENABLE_INTERRUPTS(INT_TIMER0);   
ENABLE_INTERRUPTS(GLOBAL);  
set_timer0(0);    // START TIMER1 OVER        
TIK1=0;           // START COUNTS OVER 
                                                    
output_high(I2C_EN);                                            
delay_ms(50);                          
KX126_init();                                            
                                                  
                               
                                     
while(1){                                                                                                           
                       
   if(kbhit()==1){   
      TIK1=0; 
      DISABLE_INTERRUPTS(INT_RB);                         
      DISABLE_INTERRUPTS(INT_TIMER0); 
      DISABLE_INTERRUPTS(GLOBAL);        
      
      buffer1=getc();                                   
      buffer2=getc(); 
      buffer3=getc();              
      buffer4=getc();       
      buffer5=getc();  
      if((buffer1=='L')||(buffer1=='E')||(buffer1=='C')||(buffer1=='R')){
         ///printf("flagset\n\r");
         RDAflag=1;
      }                         
      else printf("Junk\n\r"); 
      TIK1=0;  
      ENABLE_INTERRUPTS(INT_RB|INT_RB4|INT_RB);//SW1|SW2 PRESSED
      ENABLE_INTERRUPTS(INT_TIMER0);   
      ENABLE_INTERRUPTS(GLOBAL);  
   }        
                             
   if(sleepflag==1){           
      DISABLE_INTERRUPTS(INT_TIMER0); 
      printf("SNOOZING AWAY...\n\r"); 
      output_low(I2C_EN);                             
      sleep(); 
      /////////////////////////////////////////////////////////////////////////
      delay_ms(50);                        
      printf("JUST WOKE UP...\n\r"); 
      sleepflag=0;                                
      //TIK0=0;                                 
      TIK1=0;            
      output_high(I2C_EN);    
      ENABLE_INTERRUPTS(INT_TIMER0);
      ENABLE_INTERRUPTS(GLOBAL);      
   }                                      
                                    
   if(TIK1>1000){                   
      output_high(LED);      
      delay_ms(300);                                                                                                                            
      output_low(LED);       
      sleepflag=1;            
      TIK1=0;                                   
   } 
                                           
   if(sw1_flag==1){              
      TIK1=0;          
      output_high(LED);                                                                 
      output_high(I2C_EN);    
      cntr1++;                               
      delay_ms(100);                         
      while(input(switch1))output_high(LED);  
      output_low(LED);      
      PED_CNT();                     
      if(ped+storage>=100)cnt_thresh_flag=1;    
      output_low(I2C_EN);                                 
      delay_ms(50);            
                    
      if(TIK1>100){                                     
         odom_reset();   
      }

      TIK1=0;    
      sw1_flag=0;                     
                                                 
   }                       
                    
   if(sw2_flag==1){ 
      output_high(LED); 
      output_high(I2C_EN);                     
      cntr2++;                        
      delay_ms(100);           
      while(input(switch2))output_high(LED);
      output_low(LED);                       
      TIK1=0;                                 
      delay_ms(50);              
      printf("DO GOOD COUNTER = %LU\n\r\n\r",cntr2);               
      count=0;
      TIK1=0;                                                 
      sw2_flag=0;
      }                                            
                                         
   if(RDAflag==1){                      
   
      DISABLE_INTERRUPTS(INT_RB);                         
      DISABLE_INTERRUPTS(INT_TIMER0);           
      DISABLE_INTERRUPTS(GLOBAL);               
      delay_ms(100);//kb debounce                                                                                               
      if (buffer1=='L'){// LINK                 
         delay_ms(50);                                        
         printf("L");           
         printf(" %U",buffer2);                    
         printf(" %U",buffer3);                     
         printf(" %U",buffer4);                    
         printf(" %U",buffer5);   
         printf("\n\r");                                                               
      }                                     
      if (buffer1=='E'){// EXIT                                
         delay_ms(50);                                        
         printf("E"); 
         printf(" %U",buffer2);                    
         printf(" %U",buffer3);   
         printf(" %U",buffer4);                    
         printf(" %U",buffer5);   
         printf("\n\r");  
      }                                             
      if (buffer1=='R'){// READ                                             
         delay_ms(50);                                        
         printf("R");  
         readflag=1;         
      }                                                                  
      if (buffer1=='C'){// CONFIGURE                              
         delay_ms(50);                                          
         printf("C");                                                   
         printf(" %U",buffer2);                    
         printf(" %U",buffer3);            
         printf(" %U",buffer4);                    
         printf(" %U",buffer5);   
         printf("\n\r");    
         pflag_set=1;  
      }                                                          
      if (buffer1=='P'){// PART ID      
         int x2,y2;           
         delay_ms(50);              
         printf("P");                
         PARTID(&x2,&y2);             
         printf(" %U",buffer2);                    
         printf(" %U",buffer3);   
         printf(" %U",buffer4);                    
         printf(" %U",buffer5);   
         printf(" %U",x2);         
         printf(" %U",y2);      
         printf("\n\r");
                                        
      }                                
                               
      if(pflag_set==1){                
         set_kx126_configuration(buffer2,buffer3,buffer4,buffer5);   
                          
         pflag_set=0;       
      }                            
                         
      if(readflag==1){        
         output_high(I2C_EN);
         delay_ms(100);               
         PED_CNT2();                                     
         output_low(I2C_EN);                                               
         printf("%U",PED_MSB);                    
         printf(" %U",PED_LSB);
         printf(" %U",buffer4);                    
         printf(" %U",buffer5);                   
         printf("\n\r");                                     
         readflag=0;                  
      }
                                
      RDAflag=0;  
      TIK1=0;  
      ENABLE_INTERRUPTS(INT_RB|INT_RB4|INT_RB);//SW1|SW2 PRESSED
      ENABLE_INTERRUPTS(INT_TIMER0);   
      ENABLE_INTERRUPTS(GLOBAL);  
   }     
                
   if(cnt_thresh_flag==1){ 
      delay_ms(1000);
      bl=0;                      
      if(ped+storage>500)bl=5;        
      else if(ped+storage>400)bl=4;         
      else if(ped+storage>300)bl=3;
      else if(ped+storage>200)bl=2;     
      else if(ped+storage>100)bl=1;                                 
      
      for(i=0;i<bl;i++){                  
         for(x=0;x<2;x++){    
            output_high(LED);      
            delay_ms(100);                                                                                                                            
            output_low(LED); 
            delay_ms(100);     
         }  
         delay_ms(1000); 
      }                    
     cnt_thresh_flag=0;
     }   
   }                                             
                                                    
}                                             
                                     
                         
#INT_RDA                  //INT RDA SERIAL DATA HAS ARRIVED          
void  RDA_isr(void){                                                    
   buffer1=getc(); //COMMAND
   RDAflag=1;                
}  

#INT_TIMER0                    
void TIMER0_isr() {                                           
   TIK1++;//OVERFLOWS - AUTO CLEARS           
   //printf("T\n\r"); 
}
                                  
#INT_RB //INTO EXTERNAL INTERRUPT  
void RB_isr(void){                  
   int8 current;            
   SWT1=input_state(SWITCH1);             
   SWT2=input_state(SWITCH2); 
   current=input_b();         
                                        
                                          
   if(SWT1 != SW1old){                               
      if(input(SWITCH1)){
         sw1_flag=1;     
           
      }                                                                    
      if(!input(SWITCH1)){          
          sw1_flag=1;                        
               
      }                                                                        
   }                             
                         
   if(SWT2 != SW2old){                               
      if(input(SWITCH2)){
         sw2_flag=1;                   
      }                                                                    
      if(!input(SWITCH2)){           
          sw2_flag=1;                                       
      }                                                                        
   }  
                                      
   SW1old=SWT1;                 
   SW2old=SWT2;            
     
}
                                                       
  

                    
                    
                    
