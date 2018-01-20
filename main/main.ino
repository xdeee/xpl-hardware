#include <Joystick.h>

struct Axis {
	int pin;
	int readings[numReadings];
	long int total;

	Axis(int p) {
		pin = p;
		for (int thisReading = 0; thisReading < numReadings; thisReading++) {
			readings[thisReading] = 0;
		}
		total = 0;
	}
};

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
	JOYSTICK_TYPE_GAMEPAD, 2, 0,
	true, true, true, false, false, false,
	false, true, false, false, false);

const bool testAutoSendMode = true;
const unsigned long gcLedTime = 100;

const int RXLED = 17; // The RX LED has a defined Arduino pin
const int TXLED = 30; // The TX LED has a defined Arduino pin

const int numReadings = 5; //Moving average 
int readIndex = 0; //Moving average index

unsigned long gNextTime = 0;
unsigned long gLedTimer = 0;

Axis axisX(A0);
Axis axisY(A2);
Axis axisThr(A1);

int buttGears = -1;

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

int filterAxisRead(Axis *axis) {

	axis->total -= axis->readings[readIndex];
	axis->readings[readIndex] = analogRead(axis->pin);
	axis->total += axis->readings[readIndex];

	int average = axis->total / numReadings;
	return average;
}

void loop() {

	unsigned long currentTime = millis();

	while (Serial.available() > 0) {
		float red = Serial.parseFloat();
		Serial.println(red, DEC);
	}

	//Gears
	int butt = digitalRead(2);
	if (butt != buttGears) {
		buttGears = butt;
		digitalWrite(RXLED, butt);
		// Joystick.setButton(0, !butt);
		Joystick.setZAxis(butt ? 0 : 1);
		Serial.println(  butt ? 0 : 1, DEC);
	}

	//Axes
	int valueThrottle = filterAxisRead(&axisThr);
	int valueAxisX    = filterAxisRead(&axisX);
	int valueAxisY    = filterAxisRead(&axisY);
	readIndex++;
	if (readIndex >= numReadings) {
		readIndex = 0;
	}

	Joystick.setThrottle(valueThrottle);
	Joystick.setXAxis(valueAxisX);
	Joystick.setYAxis(valueAxisY);

	// Serial.print(valueAxisX); Serial.print(" ");
	// Serial.print(valueAxisY); Serial.print(" ");
	// Serial.println(valueThrottle);
	Joystick.sendState();
	delay(5);

}
