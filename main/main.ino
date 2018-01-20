#include <Joystick.h>

#define MA_FACTOR 5

struct Axis {
public:
	int value;

private:
	uint8_t readIndex = 0;
	uint8_t pin;
	int readings[MA_FACTOR];
	long int total = 0;

public:
	Axis(uint8_t p) {
		pin = p;
		for (int thisReading = 0; thisReading < MA_FACTOR; thisReading++) {
			readings[thisReading] = 0;
		}
		total = 0;
		readIndex = 0;
	}

	int read() {
		total -= readings[readIndex];
		readings[readIndex] = analogRead(pin);
		total += readings[readIndex];

		readIndex++;
		if (readIndex >= MA_FACTOR) {
			readIndex = 0;
		}

		value = total / MA_FACTOR;
		return value;
	}

private:

};

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
	JOYSTICK_TYPE_GAMEPAD, 2, 0,
	true, true, true, false, false, false,
	false, true, false, false, false);

const bool testAutoSendMode = true;
const uint8_t RXLED = 17; // The RX LED has a defined Arduino pin
const uint8_t TXLED = 30; // The TX LED has a defined Arduino pin
const unsigned long gcLedTime = 100;
unsigned long gNextTime = 0;
unsigned long gLedTimer = 0;

Axis axisX(A0);
Axis axisY(A2);
Axis axisThr(A1);

uint8_t buttGears = -1;

void setup() {
	// put your setup code here, to run once:
	// Serial.begin(9600);
	Joystick.setXAxisRange(0, 1023);
	Joystick.setYAxisRange(0, 1023);
	Joystick.setZAxisRange(0, 1);
	Joystick.setThrottleRange(0, 1023);
	Joystick.begin(testAutoSendMode);

	pinMode(2, INPUT_PULLUP);
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(RXLED, OUTPUT); // Set RX LED as an output
	pinMode(TXLED, OUTPUT); // Set TX LED as an output
	digitalWrite(TXLED, HIGH); // set the LED off
	digitalWrite(RXLED, HIGH);
	RXLED0;
}

void loop() {

	// unsigned long currentTime = millis();

	while (Serial.available() > 0) {
		float red = Serial.parseFloat();
		Serial.println(red, DEC);
	}

	//Gears temp solution
	uint8_t butt = digitalRead(2);
	if (butt != buttGears) {
		buttGears = butt;
		digitalWrite(RXLED, butt);
		// Joystick.setButton(0, !butt);
		Joystick.setZAxis(butt ? 0 : 1);
		Serial.println(  butt ? 0 : 1, DEC);
	}

	//Axes
	Joystick.setThrottle(axisThr.read());
	Joystick.setXAxis(axisX.read());
	Joystick.setYAxis(axisY.read());

	// Serial.print(valueAxisX); Serial.print(" ");
	// Serial.print(valueAxisY); Serial.print(" ");
	// Serial.println(valueThrottle);
	Joystick.sendState();
	delay(5);

}
