
#define BLYNK_PRINT Serial
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <RBD_Timer.h> // https://github.com/alextaujenis/RBD_Timer
//http://robotsbigdata.com/docs-arduino-timer.html

int TimeChoice;
byte control_pin = 22;
bool heat_state = false;
int state;
int heating_run_timer;
char button_choice;

char auth[] = "a8483d62fa0c4e3f96b906e722d4d3ca";
WidgetLCD lcd(V2);

RBD::Timer heating_timer;
RBD::Timer status_timer;
RBD::Timer blink_choice; //TODO add the code to blink the interval choice

void LCDUpdate(String lcd_message) { // update the lcd and add new messages
  lcd.print(0,0, lcd_message);
}

BLYNK_WRITE(V0) {// Do this when off button pressed
	if(param.asInt()) {
	  digitalWrite (control_pin, HIGH); //physically turn the pin on
		show_heat_state();
  } else {
    heat_off();
		show_heat_state();  
  }
}

BLYNK_WRITE(V3) {// Read 30 minute button
	if(param.asInt()){
		Serial.println("30 minutes heating selected");
			heat_action(30000, 1);
			show_heat_state();
			Blynk.virtualWrite(V3, LOW);
			button_choice = "V3";
		{ else {
			heat_off();
		}
}

BLYNK_WRITE(V4) { // Read 60 minute button
  if (param.asInt()){
		Serial.println("60 minutes heating selected");
		heat_action(60000, 1);
		show_heat_state();
		Blynk.virtualWrite(V4, LOW);
	} else {	
		heat_off();
	}
}

BLYNK_WRITE(V5) { // Read 90 minute button
  if (param.asInt()) {
		Serial.println("90 minutes heating selected");
		heat_action(90000, 1);
		show_heat_state();
		Blynk.virtualWrite(V5, LOW);
	} else {
		heat_off();
  }
}

BLYNK_WRITE(V6) { // Read 120 minute button
	switch (param.asInt()) {
		Serial.println("120 minutes heating selected");
		heat_action(120000, 1);
		show_heat_state();
		Blynk.virtualWrite(V6, LOW);
	} else {
		heat_off();
	}
}

void heat_action(unsigned long duration, int action) {
  //duration in ms; action 0 = off, 1 = on
  if(action){
		Serial.println("heat action on");
		heating_timer.setTimeout(duration);
		heating_timer.restart();
		digitalWrite(control_pin, HIGH);
		heat_state = true;
	} else {
		digitalWrite (control_pin, LOW); //physically turn the pin off
		Serial.println("Heating off");
		heating_timer.stop();
		Serial.println("Timer off");
		heat_state = false;
  }
}

int get_time_left(){
	unsigned long seconds_remaining;
	unsigned int minutes_remaining;
	seconds_remaining = heating_timer.getInverseValue() / 1000;
	//minutes_remaining = seconds_remaining / 60;
	return seconds_remaining;
}

void show_heat_state () { //display state in LCD

	unsigned long seconds_remaining;
	unsigned int minutes_remaining;

	state = digitalRead(control_pin);
	lcd.clear();
	if (!state){
	  lcd.print(0,0, "Heating off");
    Blynk.virtualWrite(V0, LOW);
    Blynk.setProperty(V0, "color", BLYNK_GREEN);
	  heating_timer.stop();
	} else {
		if (heating_timer.isActive()) {
			minutes_remaining = get_time_left();
      Blynk.virtualWrite(V0, HIGH);
      Blynk.setProperty(V0, "color", BLYNK_RED);
  	  lcd.print(0,0, "Heating on");
  	  lcd.print(0,1, minutes_remaining);
  	  lcd.print(4,1, "Minutes left");
		}	else {
			minutes_remaining = 0;
      lcd.print(0,0, "Heating on");
  	  lcd.print(0,1, "Constant");
		}
	}
}

void heat_off() { // turn the heat off
  //TODO is this really neccessary?
  heat_action(0,0);
}

void setup(){
  pinMode(control_pin, OUTPUT);
  Serial.begin(9600);
  Blynk.begin(auth);
  lcd.clear();
  status_timer.setTimeout(5000);
}

void loop(){
  Blynk.run();

  if(heating_timer.onRestart()) { heat_off(); }
  if(status_timer.onRestart()) { show_heat_state(); }


}
