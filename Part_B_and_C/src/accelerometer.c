#include "SPI.h"
#include "SysTimer.h"
#include "accelerometer.h"

int16_t dataX; 
int16_t dataY; 
int16_t dataZ; 

void accWrite(uint8_t addr, uint8_t val){
	uint16_t write_message = 0; 
	write_message |= (((addr & ~(0xC0)) << 8)); //Address bits 0x3F
	write_message |= (val << 0); //Data bits
	
	SPI_Transfer_Data(write_message); 
}

uint8_t accRead(uint8_t addr){
	uint16_t read_message = 0; 
	read_message |= ((addr & ~(0x40))); //Address bits << 8 0x3F
	read_message |= 0x80; 

	
	return SPI_Transfer_Data(read_message<<8)&0xFF; 
}

void initAcc(void){
	//Set output data rate to 100Hz
	uint8_t ODR_addr = 0x2C; 
	uint8_t ODR_data = 0X0A;
	accWrite(ODR_addr, ODR_data); 

	//Set full resolution mode
	uint8_t FRM_addr = 0x31; 
	uint8_t FRM_data = 0X08;
	accWrite(FRM_addr, FRM_data);
	
	// enable measurement
	uint8_t EM_addr = 0x2D; 
	uint8_t EM_data = 0X08; //0x09
	accWrite(EM_addr, EM_data);
}

void readValues(double* x, double* y, double* z){
	// find scaler from data sheet
	double scaler = 4e-3; //mg/LSB
	dataX = 0; 
	dataY = 0; 
	dataZ = 0; 
	// read values into x,y,z using accRead
	dataX = (accRead(0x33)<< 8) | (accRead(0x32));
	dataY = (accRead(0x35)<< 8) | (accRead(0x34));
	dataZ = (accRead(0x37)<< 8) | (accRead(0x36));
	
	*x = dataX * scaler; 
	*y = dataY * scaler; 
	*z = dataZ * scaler;
}
