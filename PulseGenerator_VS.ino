#include <TimerOne.h>

#define PWM_PIN 11

uint16_t duty = 512;
//uint32_t period = 3000;//333
//uint32_t period = 2000;//500
//uint32_t period = 5000;//200
//uint32_t period = 500;//2000
//uint32_t period = 300;//3333
uint32_t period = 1000;//1000
uint32_t freq = 1000;
uint32_t periodInvert = period;
uint32_t icr;
uint32_t incomingBytes = 0;   // для хранения полученного байта
volatile uint8_t ad2, ad1, ad0; //байты частоты для функции на ассемблере

//unsigned long currentMillis = millis();
//uint16_t 

//unsigned long int v = 0;
//int vcount = 0;

const PROGMEM uint8_t  sinewave[] ={
	0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,0x98,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,0xae,
	0xb0,0xb3,0xb6,0xb9,0xbc,0xbf,0xc1,0xc4,0xc7,0xc9,0xcc,0xce,0xd1,0xd3,0xd5,0xd8,
	0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xed,0xef,0xf0,0xf2,0xf3,0xf5,
	0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfc,0xfd,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfe,0xfd,0xfc,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,
	0xf6,0xf5,0xf3,0xf2,0xf0,0xef,0xed,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,0xde,0xdc,
	0xda,0xd8,0xd5,0xd3,0xd1,0xce,0xcc,0xc9,0xc7,0xc4,0xc1,0xbf,0xbc,0xb9,0xb6,0xb3,
	0xb0,0xae,0xab,0xa8,0xa5,0xa2,0x9f,0x9c,0x98,0x95,0x92,0x8f,0x8c,0x89,0x86,0x83,
	0x80,0x7c,0x79,0x76,0x73,0x70,0x6d,0x6a,0x67,0x63,0x60,0x5d,0x5a,0x57,0x54,0x51,
	0x4f,0x4c,0x49,0x46,0x43,0x40,0x3e,0x3b,0x38,0x36,0x33,0x31,0x2e,0x2c,0x2a,0x27,
	0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,0x15,0x13,0x12,0x10,0x0f,0x0d,0x0c,0x0a,
	0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x03,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,
	0x09,0x0a,0x0c,0x0d,0x0f,0x10,0x12,0x13,0x15,0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,
	0x25,0x27,0x2a,0x2c,0x2e,0x31,0x33,0x36,0x38,0x3b,0x3e,0x40,0x43,0x46,0x49,0x4c,
	0x4f,0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x67,0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c
};

const PROGMEM char musor_mass[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


static inline void signalOUT(const uint8_t *signal) {
	asm volatile ("eor r18, r18   ;r18<-0"  "\n\t"
		"eor r19, r19   ;r19<-0"  "\n\t"
		"1:"            "\n\t"
		"add r18, %0  ;1 cycle"     "\n\t"
		"adc r19, %1  ;1 cycle"     "\n\t"
		"adc %A3, %2  ;1 cycle"     "\n\t"
		"lpm      ;3 cycles"  "\n\t"
		"out %4, __tmp_reg__  ;1 cycle" "\n\t"
		"sbis %5, 7   ;1 cycle if no skip" "\n\t"     // если стоит бит в переменной %5, то
		"rjmp 1b    ;2 cycles. Total 10 cycles" "\n\t" //пропустить эту команду ухода в цикл
		:
	: "r" (ad0), "r" (ad1), "r" (ad2), "e" (signal), "I" (_SFR_IO_ADDR(PORTD)), "I" (_SFR_IO_ADDR(PORTB))
		: "r18", "r19");
}


void setup() {	
	Serial.begin(115200);
	pinMode(PWM_PIN, OUTPUT);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A0, INPUT_PULLUP);

	for (int n = 0; n <sizeof(musor_mass); n++) { PORTD = musor_mass[n]; }
	TCCR0B = 0; //заглушить 0 таймер что б не мешал DDS
	TCCR1A = 0; TCCR1B = 0;

	signalOUT(sinewave);

	
	//Timer1.initialize(period);
	//Timer1.pwm(PWM_PIN, duty);		
	//Timer1.start();	
}

void setFreq(uint32_t freq) {
	//uint32_t val = 1000000/freq 
	Timer1.setPeriod(1000000 / freq);
	//Timer1.setPwmDuty(PWM_PIN, duty);	
	
}




void loop() {
#ifdef DEBUG



	/////////////////////////////////////////////////
	if (!digitalRead(2)) {
		delay(200);
		if (!digitalRead(8)) {
			period -= 10;
			Timer1.setPeriod(period);
		}
		else {
			period -= 1;
			Timer1.setPeriod(period);
		}

	}
	/////////////////////////////////////////////////
	if (!digitalRead(3)) {
		delay(200);
		if (!digitalRead(8)) {
			period += 10;
			Timer1.setPeriod(period);
		}
		else {
			period += 1;
			Timer1.setPeriod(period);
		}

	}
	/////////////////////////////////////////////////
	if (!digitalRead(11)) {
		delay(200);
		if (!digitalRead(8)) {
			duty += 100;
			Timer1.setPwmDuty(PWM_PIN, duty);
		}
		else {
			duty += 1;
			Timer1.setPwmDuty(PWM_PIN, duty);
		}

	}
	/////////////////////////////////////////////////
	if (!digitalRead(12)) {
		delay(200);
		if (!digitalRead(8)) {
			duty -= 100;
			Timer1.setPwmDuty(PWM_PIN, duty);
		}
		else {
			duty -= 1;
			Timer1.setPwmDuty(PWM_PIN, duty);
		}

	}

#endif // DEBUG

	PORTB &= ~(1 << 7);// флаг ухода в цикл DDS
	DDRD = 0xFF;//set D port as output

	/////////////////////////////////////////////////	
	if (!digitalRead(A0)) {
		delay(90);
		switch (analogRead(A1)) {
		case 0:
			freq += 100;
			period -= 100;
			periodInvert += 100;
			setFreq(freq);
			//Timer1.setPeriod(period);
			Timer1.setPwmDuty(PWM_PIN, duty);			
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		case 1:
			freq -= 100;
			period += 100;
			periodInvert -= 100;
			setFreq(freq);
			//Timer1.setPeriod(period);
			Timer1.setPwmDuty(PWM_PIN, duty);
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		case 2:
			duty += 100;
			Timer1.setPwmDuty(PWM_PIN, duty);
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		case 3:
			duty -= 100;
			Timer1.setPwmDuty(PWM_PIN, duty);
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		default:
			break;
		}
	}
	else {
		delay(90);
		switch (analogRead(A1)) {
		case 0:
			freq += 1;
			period -= 1;
			periodInvert += 1;
			setFreq(freq);
			//Timer1.setPeriod(period);
			Timer1.setPwmDuty(PWM_PIN, duty);
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		case 1:
			freq -= 1;
			period += 1;
			periodInvert -= 1;
			setFreq(freq);
			//Timer1.setPeriod(period);
			Timer1.setPwmDuty(PWM_PIN, duty);
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		case 2:
			duty += 1;
			Timer1.setPwmDuty(PWM_PIN, duty);
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		case 3:
			duty -= 1;
			Timer1.setPwmDuty(PWM_PIN, duty);
			Serial.print("Frequency: "); Serial.print(freq); Serial.println(" Hz");
			Serial.print("Duty: ");	Serial.print(duty); Serial.println(" %"); Serial.println();
			break;
		default:
			break;
		}
	}
	if (Serial.available() > 0) {  //если есть доступные данные	
		String s = Serial.readString();		
			if (s[0] == 'd'){
				duty = s.substring(1).toInt();
				Timer1.setPwmDuty(PWM_PIN, duty);
				Serial.print("Duty set to: "); Serial.print(duty); Serial.println(" %"); Serial.println();
			}
			if (s[0] == 's'){
				Serial.println("Waveform is set to: Sinewave"); Serial.println();
				signalOUT(sinewave);
				Serial.println("Waveform is set to: Sinewave"); Serial.println();
			}

			//char buf[] = { 0,0,0,0 };
			//Serial.readBytes(buf, 4);
			//if (buf[0] == 'F')
			//freq = Serial.parseInt();
			else{
				freq = s.toInt(); // считываем байты		
				setFreq(freq);
				Timer1.setPwmDuty(PWM_PIN, duty);
				Serial.print("Frequency set to: "); Serial.print(freq); Serial.println(" Hz"); Serial.println();
			}		
		
		
		
	}

}