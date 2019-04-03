byte dataMS,dataLS,dataMSB,dataLSB,manID1,manID2,manID3,manID4;
byte partID1,partID2,PED_MSB,PED_LSB;
int16 rh,tmp,ped=0,storage=0;
float drh,dtmp;

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
   i2c_write_sgl(0x3C,0x1A,0x00); //CNTL1 
   i2c_write_sgl(0x3C,0x41,0x10); //CNTL1 
   i2c_write_sgl(0x3C,0x42,0x27); //CNTL1 
   i2c_write_sgl(0x3C,0x44,0x2C); //CNTL1 
   i2c_write_sgl(0x3C,0x37,0x7B); //CNTL1
   i2c_write_sgl(0x3C,0x26,0x61); //CNTL1
   i2c_write_sgl(0x3C,0x20,0x20); //CNTL1
   i2c_write_sgl(0x3C,0x24,0x20); //CNTL1
   
   i2c_write_sgl(0x3C,0x43,0x66); //CNTL1
   i2c_write_sgl(0x3C,0x44,0x2C); //CNTL1
   i2c_write_sgl(0x3C,0x45,0x17); //CNTL1
   i2c_write_sgl(0x3C,0x46,0x1F); //CNTL1
   i2c_write_sgl(0x3C,0x47,0x24); //CNTL1
   i2c_write_sgl(0x3C,0x48,0x13); //CNTL1
   i2c_write_sgl(0x3C,0x49,0x0B); //CNTL1
   i2c_write_sgl(0x3C,0x4A,0x08); //CNTL1
   i2c_write_sgl(0x3C,0x4B,0x19); //CNTL1
   i2c_write_sgl(0x3C,0x4C,0x1C); //CNTL1
   
   i2c_write_sgl(0x3C,0x1A,0xC2); //CNTL1
}


 
void setup_shock(){ //THIS SETUP UP THE CONTROL REGISTERS FOR 3G ACCEL

///////////////////////////   INTERRUPTS    /////////////////////////////////// 

   i2c_write_sgl(0x3C,0x19,0b10000010); //CNTL1 
   ////////////////////////    POWER UP     ///////////////////////////////////
   //i2c_write_sgl(0xA6,0x2D,0x00); //POWER CTL RESET
   //i2c_write_sgl(0xA6,0x2D,0x10); //POWER CTL STANDBY 
   i2c_write_sgl(0xA6,0x2D,0x08); //POWER CTL MEASURE

}
BYTE MANID(){ //HOLD MASTER MODE OPTION

   i2c_start();
   i2c_write(0x3C); //write
   i2c_write(0x00);//MAN ID
   i2c_start();
   i2c_write(0x3D); //read
   delay_us(20);
   manID1=i2c_read(1);
   manID2=i2c_read(1);
   manID3=i2c_read(1);
   manID4=i2c_read(0);
   i2c_stop();
   delay_us(20);

   printf(" MAN_ID %c|%c|%c|%c\n\r",manID1,manID2,manID3,manID4);
   //return(manID);
}

BYTE PARTID() { //HOLD MASTER MODE OPTION

   i2c_start();
   i2c_write(0x3C); //write
   i2c_write(0x01);//MAN ID
   i2c_start();
   i2c_write(0x3D); //read
   delay_us(20);
   partID1=i2c_read(1);
   partID2=i2c_read(0);
   i2c_stop();
   delay_us(20);

   printf(" PART_ID %c|%c\n\r",partID1,partID2);
   //return(manID);
}

BYTE PED_CNT() { //HOLD MASTER MODE OPTION

   i2c_start();
   i2c_write(0x3C); //write
   i2c_write(0x0E);//MAN ID
   i2c_start();
   i2c_write(0x3D); //read
   delay_us(20);
   PED_LSB=i2c_read(1);
   PED_MSB=i2c_read(0);
   i2c_stop();
   delay_us(20);
   ped=make16(PED_MSB,PED_LSB);
   printf("PEDOMETER CNT NEW=%LU TOTAL=%LU\n\r",ped,ped+storage);
   printf("NEW PED MATH CHECK MSB=%U LSB=%U\n\r",PED_MSB,PED_LSB);
   storage+=ped;
   //return(manID);
}
