// thirtysecondtimer.ino by Costas 12 Dec 2016
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <SimpleTimer.h>      // library for SimpleTimer       

int pinState;
int TimeChoice;
//char lcd_message[];
byte control_pin = 22;
bool heat_state = false;
int state;
int CurrentDuration = 0;
int heating_run_timer;


char auth[] = "a8483d62fa0c4e3f96b906e722d4d3ca";
WidgetLCD lcd(V2);

SimpleTimer heat_check_timer; 
SimpleTimer heating_timer;




BLYNK_WRITE(V0){   // add a slider to your project on V0 range 0 to 30 (minutes)
  pinState = param.asInt();  // set variable as Slider value
  //Serial.println("pin state");
  //Serial.println(pinState);
  if (pinState == 0) {heat_off(); }

  if (pinState == 1){ digitalWrite(control_pin, HIGH); }
}

BLYNK_WRITE(V1){   // Get index of timer choice
  switch (param.asInt())
    {
      case 1: { // Item 1
        Serial.println("30 minutes heating selected");
       
        //LCDUpdate("Heating is on 30");
        heat_action(3000, 1);
        break;
      }
      case 2: { // Item 2
        Serial.println("45 minutes heating selected");
        //LCDUpdate("Heating is on 45");
        heat_action(4500, 1);
       
        break;
      }    
       case 3: { // Item 2
        Serial.println("60 minutes heating selected");
        //LCDUpdate("Heating is on 60");
        heat_action(6000, 1);
        
        break;
      }    
       case 4: { // Item 2
        Serial.println("90 minutes heating selected");
        //LCDUpdate("Heating is on 90");
        heat_action(9000, 1);
        
        break;
      }     
    }
  
   
  }
  
void LCDUpdate(String lcd_message) { // update the lcd and add new messages 
  lcd.print(0,0, lcd_message);
}
  
  
void heat_action(int duration, int action) { //action 0 = off, 1 = on

switch (action)
  {
    
  case 0: { // heating off
    digitalWrite (control_pin, LOW);
    //LCDUpdate("Turning heating off");
    heat_state = false;
    break;
  }
  
  case 1: { // heating on for duration

    CurrentDuration = duration;
    heating_timer.disable(heating_run_timer);
    heating_run_timer = heating_timer.setInterval(CurrentDuration, heat_off);
    digitalWrite(control_pin, HIGH);
    heat_state = true;
    heating_timer.enable(heating_run_timer);
    Serial.println("setting timer on");
    //LCDUpdate("Heating on");
  } 
  }
}
  
void show_heat_state () { //display state in LCD
state = digitalRead(control_pin);
lcd.clear();
if (state == 0){
  lcd.print(0,0, "Heating off");
  Blynk.virtualWrite(V0, 0);
}
if (state == 1) {
  lcd.print(0,0, "Heating on");
  Blynk.virtualWrite(V0, 1);
}
//Serial.println(state);

}

void heat_off() {

  heat_action(0,0);
}
  
  

void setup()
{
  pinMode(control_pin, OUTPUT);
  Serial.begin(9600); 
  Blynk.begin(auth);  // this is now a blocking function - more on this later
  
  lcd.clear();
  heat_check_timer.setInterval(5000, show_heat_state); // run the check every 5 seconds
  heating_run_timer = heating_timer.setInterval(CurrentDuration, heat_off);
  heating_timer.disable(heating_run_timer);
  
  
  
}

void loop()
{
  Blynk.run();
  heat_check_timer.run();
  heating_timer.run();
  
 
}
