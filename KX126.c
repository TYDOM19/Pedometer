byte manID1,manID2,manID3,manID4;
int16 ped=0,storage=0;                                    
int8 PED_MSB,PED_LSB;         
                                  
#define DEV_ID 0x3C           
                                           
                               
void i2c_write_sgl(unsigned int8 ADR, unsigned int8 REG, unsigned int8 VAL){
   i2c_start();
   i2c_write(ADR); // adress of ic w/ write
   i2c_write(REG); // register adress 
   i2c_write(VAL); // data you want to send to that adress
   i2c_stop();
 }                                 
                                                  
void KX126_init(){                                       
                                                    
   output_float(PIN_C3);                                              
   output_float(PIN_C4);                                                   
   i2c_write_sgl(DEV_ID,0x1A,0x00); //CNTL1 - +/-2Gs, PC1 BIT SET TO '0'     
   i2c_write_sgl(DEV_ID,0x41,0x00); //PED_WM_L            
   i2c_write_sgl(DEV_ID,0x42,0x00); //PED_WM_H                                                                                                         
   i2c_write_sgl(DEV_ID,0x44,0x2C); //PED_CNTL2          
   i2c_write_sgl(DEV_ID,0x37,0x4B); //LP_CNTL                                       
   i2c_write_sgl(DEV_ID,0x26,0x00); //INC7 - NO STEP COUNTER   
   i2c_write_sgl(DEV_ID,0x20,0x00); //INC1                                                                    
   //i2c_write_sgl(0x3C,0x30,0x20); //FFTH - THRESHOLD SET TO 1G (20/FF)= 1/8 8 G FULL SCALE          
   //i2c_write_sgl(0x3C,0x31,0x20); //FFC - 1/ODR IF ODR = TIME OF FALL    
   //i2c_write_sgl(0x3C,0x32,0xC1); //FFCNTL - FF ENGINE ENABLED, ODR 25Hz  
   //i2c_write_sgl(0x3C,0x80,0x20); //INC4 - FF ENABLED ON INT1                                                         
   i2c_write_sgl(DEV_ID,0x24,0x00); //INC5                                                                                                                                  
   i2c_write_sgl(DEV_ID,0x43,0x66); //PED_CNTL1* - PED STEP DETECTION B[2-0] 2 = 4 STEPS TILL DETECTION STARTS                 
   i2c_write_sgl(DEV_ID,0x44,0x2C); //PED_CNTL2 - PED ODR 50(6) OR 100Hz(C)                       
   i2c_write_sgl(DEV_ID,0x45,0x17); //PED_CNTL3 - SCALING FACTOR HPF                 
   i2c_write_sgl(DEV_ID,0x46,0x1F); //PED_CNTL4 - MAX IMPACT ON FLOOR 0-15 B[3-0]
   i2c_write_sgl(DEV_ID,0x47,0x24); //PED_CNTL5* - MIN IMPACT ON FLOOR 0-255 
   i2c_write_sgl(DEV_ID,0x48,0x13); //PED_CNTL6 - MAX TIME INT FOR THE PEAK 0-63    
   i2c_write_sgl(DEV_ID,0x49,0x0B); //PED_CNTL7 - MIN TIME INT FOR PEAK 0-255         
   i2c_write_sgl(DEV_ID,0x4A,0x08); //PED_CNTL8 - TIME WINDOW, NOISE DELAY                    
   i2c_write_sgl(DEV_ID,0x4B,0x19); //PED_CNTL9 - TIME INT OVERFLOWS         
   i2c_write_sgl(DEV_ID,0x4C,0x1C); //PED_CNTL10* - MIN TIME SINGLE STRIDE 0-63                                                                                                                          
   //i2c_write_sgl(DEV_ID,0x1A,0xC2); ////CNTL1 - HIGH RES MODE,  +/-8Gs, PED ENABLED        
   i2c_write_sgl(DEV_ID,0x1A,0x82); ////CNTL1 - LOW RES MODE,  +/-8Gs, PED ENABLED   
}                                         
                                 
void set_kx126_configuration(int8 buf2, int8 buf3, int8 buf4, int8 buf5){                       
                                   
   i2c_write_sgl(DEV_ID,0x1A,0x00); //CNTL1 - +/-2Gs, PC1 BIT SET TO '0'     
   i2c_write_sgl(DEV_ID,0x41,0x10); //PED_WM_L   
   i2c_write_sgl(DEV_ID,0x42,0x27); //PED_WM_H    
   i2c_write_sgl(DEV_ID,0x44,0x2C); //PED_CNTL2                    
   i2c_write_sgl(DEV_ID,0x37,0x7B); //LP_CNTL       
   i2c_write_sgl(0x3C,0x26,0x00); //INC7 - NO STEP COUNTER          
   i2c_write_sgl(0x3C,0x20,0x00); //INC1                                                                   
   //i2c_write_sgl(0x3C,0x30,0x20); //FFTH - THRESHOLD SET TO 1G (20/FF)= 1/8 8 G FULL SCALE 
   //i2c_write_sgl(0x3C,0x31,0x20); //FFC - 1/ODR IF ODR = TIME OF FALL    
   //i2c_write_sgl(0x3C,0x32,0xC1); //FFCNTL - FF ENGINE ENABLED, ODR 25Hz  
   //i2c_write_sgl(0x3C,0x80,0x20); //INC4 - FF ENABLED ON INT1                                        
   i2c_write_sgl(0x3C,0x24,0x00); //INC5                                                                                                                              
   i2c_write_sgl(0x3C,0x43,buf2); //PED_CNTL1* - PED STEP DETECTION B[2-0] 2 = 4 STEPS TILL DETECTION STARTS                 
   i2c_write_sgl(0x3C,0x44,0x2C); //PED_CNTL2 - PED ODR 50(6) OR 100Hz(C)                       
   i2c_write_sgl(0x3C,0x45,0x17); //PED_CNTL3 - SCALING FACTOR HPF
   i2c_write_sgl(0x3C,0x46,buf3); //PED_CNTL4 - MAX IMPACT ON FLOOR 0-15 B[3-0]
   i2c_write_sgl(0x3C,0x47,buf4); //PED_CNTL5* - MIN IMPACT ON FLOOR 0-255 
   i2c_write_sgl(0x3C,0x48,0x13); //PED_CNTL6 - MAX TIME INT FOR THE PEAK 0-63    
   i2c_write_sgl(0x3C,0x49,0x0B); //PED_CNTL7 - MIN TIME INT FOR PEAK 0-255         
   i2c_write_sgl(0x3C,0x4A,0x08); //PED_CNTL8 - TIME WINDOW, NOISE DELAY                    
   i2c_write_sgl(0x3C,0x4B,0x19); //PED_CNTL9 - TIME INT OVERFLOWS         
   i2c_write_sgl(0x3C,0x4C,buf5); //PED_CNTL10* - MIN TIME SINGLE STRIDE 0-63     
                                                                       
   i2c_write_sgl(0x3C,0x1A,0xC2); ////CNTL1 - HIGH RES MODE,  +/-8Gs, PED ENABLED
   i2c_write_sgl(DEV_ID,0x1A,0x82); ////CNTL1 - HIGH RES MODE,  +/-8Gs, PED ENABLED   
                                                           
    printf("Reg_47=%U, Reg_4C=%U\n\r",buf2, buf3);     
   
} 
 

BYTE MANID(){ //HOLD MASTER MODE OPTION

   i2c_start();                                                        
   i2c_write(DEV_ID); //write
   i2c_write(0x00);//MAN ID                       
   i2c_start();
   i2c_write(DEV_ID+1); //read
   delay_us(20);
   manID1=i2c_read(1);
   manID2=i2c_read(1);
   manID3=i2c_read(1);               
   manID4=i2c_read(0);
   i2c_stop();
   delay_us(20);

   printf(" MAN_ID %c|%c|%c|%c\n\r",manID1,manID2,manID3,manID4);  
   return(manID1);
}
                                    
void PARTID(int8 *ptr1, int8 *ptr2) { //HOLD MASTER MODE OPTION

   
   i2c_start();            
   i2c_write(DEV_ID); //write 
   i2c_write(0x01);//MAN ID
   i2c_start();                   
   i2c_write(DEV_ID+1); //read        
   delay_us(20);
   *ptr1=i2c_read(1);          
   *ptr2=i2c_read(0);     
   i2c_stop();                                             
   delay_us(20);                                  

   printf(" PART_ID %u|%u\n\r",*ptr1,*ptr2); 
   
                             
   //return(manID);       
}                  
                                                               
void PED_CNT() { //HOLD MASTER MODE OPTION
   
   int8 pedMSB, pedLSB;
   
   i2c_start();                   
   i2c_write(DEV_ID); //write                                     
   i2c_write(0x0E);//MAN ID
   i2c_start();                                                                 
   i2c_write(DEV_ID+1); //read                                          
   pedLSB=i2c_read(1);                          
   pedMSB=i2c_read(0);                      
   i2c_stop();                     
   delay_us(20);                      
                                    
   ped=make16(pedMSB,pedLSB);                 
   printf("PEDOMETER CNT NEW=%LU TOTAL=%LU\n\r\n\r",ped,ped+storage);
   storage+=ped; 
   //return(PED_MSB); 
}      

BYTE PED_CNT2() { //HOLD MASTER MODE OPTION
                               
   i2c_start();                   
   i2c_write(DEV_ID); //write                                     
   i2c_write(0x0E);//MAN ID
   i2c_start();                                                                 
   i2c_write(DEV_ID+1); //read                                          
   PED_LSB=i2c_read(1);          
   PED_MSB=i2c_read(0);      
   i2c_stop();                                       
   delay_us(20);                      
   
   ped=make16(PED_MSB,PED_LSB);
   printf("YOU'BE WALKED %LU STEPS!\n\r\n\r",ped+storage);
   storage+=ped;
   return(PED_MSB);
}                         
   
void INT_SOURCE1() { //HOLD MASTER MODE OPTION
                                                              
   int8 source1;    
                                
   i2c_start();        
   i2c_write(0x3C); //write
   i2c_write(0x14);//INS1  
   i2c_start();                      
   i2c_write(0x3D); //read
   delay_us(20);
   source1=i2c_read(0);     
   i2c_stop();
   delay_us(20);                     
                                 
    printf("INT SOURCE 1 %X\n\r",source1);   

}      

void INT_SOURCE2() { //HOLD MASTER MODE OPTION
   
   int8 source2;    
                                
   i2c_start();                       
   i2c_write(0x3C); //write           
   i2c_write(0x15);//INS2  
   i2c_start();                      
   i2c_write(0x3D); //read
   delay_us(20);                                         
   source2=i2c_read(0);     
   i2c_stop();         
   delay_us(20);                     
   
    printf("INT SOURCE 2 %X\n\r",source2); 

}           

void FF_INT_CLEAR() { //HOLD MASTER MODE OPTION 
   
   int8 clear;                               
                                
   i2c_start();        
   i2c_write(DEV_ID); //write                 
   i2c_write(0x19);//INT_REL                     
   i2c_start();
   i2c_write(DEV_ID+1); //read
   delay_us(20);                                
   clear=i2c_read(0);               
   i2c_stop();                  
   delay_us(20);

} 
