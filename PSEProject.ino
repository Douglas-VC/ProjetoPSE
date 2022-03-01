/*
Projeto da Disciplina Projeto de Sistemas Embutidos
Integrantes:
Douglas Viana Coutinho
Felipe Aragão Nogueira de Freitas
Guilherme Rodrigues dos Santos
Guilherme Vinícius Amorim
*/

#include <math.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int counter = 10;
float total_medidas = 10;
float soma_medidas = 0;
float media = 0;
float temp;

int read_data(){
  uint8_t lower_bits, upper_bits;
  
  ADMUX = (1<<REFS0)|(1<<MUX0); //set up ASMUX register

  ADCSRA |= ( 1 << ADSC);  // enable conversion beginning 
  while(ADSC == 0x01); // Waits until the end of conversion

  lower_bits = ADCL; //read data1
  upper_bits = ADCH; // read data2
  
  return (upper_bits << 8) | lower_bits; // returns final data
}

float get_temp(float voltage){
  float r = 10000;
  float res = ((r*5)/voltage) - r;
 
  float T0 = 298.15;    
  float B = 3950.00;
  float R0 = 10000.00;
 
  float tempK = 1/(1/T0 + log(res/R0)/B); 
  return tempK - 273.15;
}

void setup(){
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // set up lcd 16x2
  lcd.init();
  lcd.backlight();
  lcd.setBacklight(HIGH);
  
  // set up push button registers for input:
  DDRB &= ~(1 << DDB4);  // set pin 12 as input
  PORTB |= (1 << PB4);   // set pin 12 pull up resistor
}

void loop() {

  //Using internal Pull up resistor for push button
  if(!(PINB & (1 << PB4))){    
	// restart counter allowing program 
	// to enter the loop and take new measurements
	soma_medidas = 0;
	media = 0;
	counter = 0;    
  }
  
 while (counter < total_medidas){     
	
	float voltage;
	int medida;

	medida = read_data(); // analog reading 
	
	//converts analog read to voltage value
	voltage = medida * (5.0 / 1023.0); 
   
	soma_medidas += voltage;    
	counter++;
 }  
   
  media = soma_medidas/total_medidas;
  
  Serial.print("Tensao media lida: ");
  Serial.print(media);  
  Serial.println(" ");  
	 
  temp = get_temp(media); 
  Serial.print("Temperatura (Celcius): ");
  Serial.print(temp);  
  Serial.println(" ");  
  
  lcd.setCursor(1,0);
  lcd.print("Temperature: ");
  lcd.setCursor(1,1);
  lcd.print(temp);
  lcd.println(" C");	  
}
