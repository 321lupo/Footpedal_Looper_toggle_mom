                                    //LCD & SHIELD Libraries
  #include <Wire.h>
  #include <Adafruit_MCP23017.h>
  #include <Adafruit_RGBLCDShield.h>
  Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
  #include <usb_keyboard.h>
                              // LCD backlight color
  #define RED 0x1
  #define YELLOW 0x3
  #define GREEN 0x2
  #define TEAL 0x6
  #define BLUE 0x4
  #define VIOLET 0x5
  #define WHITE 0x7
                              //Pushbuttons Debounce Library
  #include <Bounce.h>
                              //Midi Library
  #include <MIDI.h>
  const int MIDI_CHAN = 5;
  
  #define BUTTONBANK 6         //bankswitch button
  #define BUTTONSTOP 11        //stop button (not part of bank)
  #define BUTTONPLAY 23        //play (not part of bank)
  #define CTRLBUTTONS_N 12     //ctrl buttons used to trigger clips
  #define NUM_BANKS 2
  #define CTRL_DEL
  #define CTRL_STOP 72        //begin of control values available to footpedal (71-128)
  #define CTRL_PLAY 71
  #define CTRL_START 73       //start of ctrl values for buttons (number up in forloop)
  #define BOUNCE_T 50         //bounce time in ms
  int ctrlval[CTRLBUTTONS_N];  //control message bank, just to make sure that two buttons simultaneously clicked still get their momentary message sent
  int bank = 0;

  int buttonPins[] = {2,3,4,5,/*6,*/7,8,9,10,/*11,*/12,20,21,22,/*23*/};  //All button pins except bank, stop and play button
  // int buttons[CTRLBUTTONS_N];

  Bounce buttonBank = Bounce(BUTTONBANK, BOUNCE_T);                  
  Bounce buttonStop = Bounce(BUTTONSTOP, BOUNCE_T);
  Bounce buttonPlay = Bounce (BUTTONPLAY, BOUNCE_T);
  
  Bounce buttons[12] = {
    Bounce (buttonPins[0], BOUNCE_T), 
    Bounce (buttonPins[1], BOUNCE_T), 
    Bounce (buttonPins[2], BOUNCE_T), 
    Bounce (buttonPins[3], BOUNCE_T),
    Bounce (buttonPins[4], BOUNCE_T),
    Bounce (buttonPins[5], BOUNCE_T),
    Bounce (buttonPins[6], BOUNCE_T),
    Bounce (buttonPins[7], BOUNCE_T),
    Bounce (buttonPins[8], BOUNCE_T),
    Bounce (buttonPins[9], BOUNCE_T),
    Bounce (buttonPins[10], BOUNCE_T),
    Bounce (buttonPins[11], BOUNCE_T)
  };
  
  int bankadd = 0;

  unsigned long millisTime;                     //millis del-keystroke variables
  unsigned long delTime[CTRLBUTTONS_N];         //for now without arrays...         
  bool delBool[CTRLBUTTONS_N];
  int delHold = 2000;                           //time to hold down buttons to send del-msg
  unsigned long redoTime;
  bool redoBool;
  int redoHold = 2000;

  
void setup(){  
                             
  Serial.begin(9600);
  
  pinMode (BUTTONBANK, INPUT_PULLUP);
  pinMode (BUTTONSTOP, INPUT_PULLUP);
  pinMode (BUTTONPLAY, INPUT_PULLUP);
  
//  for(int i=0; i<CTRLBUTTONS_N; i++){                    //PULLUP FORLOOP
//    pinMode (buttonPins[i], INPUT_PULLUP);
//  }  
  
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(buttonPins[2], INPUT_PULLUP);
  pinMode(buttonPins[3], INPUT_PULLUP);
  pinMode(buttonPins[4], INPUT_PULLUP);
  pinMode(buttonPins[5], INPUT_PULLUP);
  pinMode(buttonPins[6], INPUT_PULLUP);
  pinMode(buttonPins[7], INPUT_PULLUP);
  pinMode(buttonPins[8], INPUT_PULLUP);
  pinMode(buttonPins[9], INPUT_PULLUP);
  pinMode(buttonPins[10], INPUT_PULLUP);
  pinMode(buttonPins[11], INPUT_PULLUP);
                          
  lcd.begin(16, 2);                         //LCD START
 
  lcd.clear();            
  lcd.setCursor(0, 0);
  lcd.print("START");
 
  Serial.println("serial start"); 

  for(int i=0; i<CTRLBUTTONS_N; i++){             //reset all del and redo Variables
    delTime[i]=0;
    delBool[i]=false;
  }
  redoTime=0;
  redoBool=false;
  
}

void loop(){

  millisTime = millis();                  //time var running WHATS THE MAX MILLIS VALUE?
  
  while (usbMIDI.read());   

  readButtons();

  if(buttonBank.fallingEdge()) {     //3 banks, Button+/-, Print LCD and Serial 
    bank++;
    bank %= NUM_BANKS;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bank ");
    lcd.print(bank);
    Serial.print("Bank ");
    Serial.println(bank);
    redoTime=millisTime;
    redoBool=true;
  }
  if (buttonBank.risingEdge()) {
      redoBool=false;
    }
  if (millisTime-redoTime>=redoHold && redoBool==true) {
      redoBool=false;
      Serial.println ("Redo ");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Redo ");
      Keyboard.set_modifier(MODIFIERKEY_CTRL);
      Keyboard.set_key1(KEY_Z);
      Keyboard.send_now();
      Keyboard.set_modifier(0);
      Keyboard.set_key1(0);
      Keyboard.send_now();
      bank=bank-1;
    }

  bankadd = bank * CTRLBUTTONS_N;     //ATTENTION THAT HE READS 12 HERE


  if (buttonStop.fallingEdge()) {
    usbMIDI.sendControlChange((CTRL_STOP), 127, MIDI_CHAN);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Stop ");
    Serial.println(String("ch=") + MIDI_CHAN + (", Control=") + (CTRL_STOP) + ("Velocity=127"));
  }
  if (buttonPlay.fallingEdge()) {
    usbMIDI.sendControlChange((CTRL_PLAY), 127, MIDI_CHAN);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Play ");
    Serial.println(String("ch=") + MIDI_CHAN + (", Control=") + (CTRL_PLAY) + ("Velocity=127"));
  }              
                     
  for(int i=0; i<CTRLBUTTONS_N; i++){   
    if (buttons[i].fallingEdge()) {
      usbMIDI.sendControlChange((CTRL_START+bankadd+i), 127, MIDI_CHAN);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" CTRL ");
      lcd.print (CTRL_START+bankadd+i);
      lcd.setCursor(0,1);
      lcd.print(" 127");
      Serial.println(String(" ch= ") + MIDI_CHAN + (", Control = ") + (CTRL_START+bankadd+i) + (" Velocity = 127 "));
      delTime[i]=millisTime;
      delBool[i]=true;
      ctrlval[i]=(CTRL_START+bankadd+i);
    }
    if (buttons[i].risingEdge()) {                                  //if the button gets released before delHold, delbool is false and the delete function cannot be sent
      
 //SEE IF THIS WORKS      
      //if (i==8||i==9||i==10||i==11 && (delBool[i]==true)){        //sends ctrl 0 (momentary midi msg) on the bottom four buttons
      if (ctrlval[i]==73 || ctrlval[i]==85 ||                          //sends ctrl 0 (momentary midi msg) on the top four buttons            
          ctrlval[i]==74 || ctrlval[i]==86 ||
          ctrlval[i]==75 || ctrlval[i]==87 ||
          ctrlval[i]==76 || ctrlval[i]==88 ||

          ctrlval[i]==77 || ctrlval[i]==89 ||                          //sends ctrl 0 (momentary midi msg) on the middle four buttons            
          ctrlval[i]==78 || ctrlval[i]==90 ||
          ctrlval[i]==79 || ctrlval[i]==91 ||
          ctrlval[i]==80 || ctrlval[i]==92 ||
          
          ctrlval[i]==81 || ctrlval[i]==93 ||                          //sends ctrl 0 (momentary midi msg) on the bottom four buttons            
          ctrlval[i]==82 || ctrlval[i]==94 ||
          ctrlval[i]==83 || ctrlval[i]==95 ||
          ctrlval[i]==84 || ctrlval[i]==96 &&  (delBool[i]==true)){
        usbMIDI.sendControlChange(ctrlval[i], 0, MIDI_CHAN);
        //usbMIDI.sendControlChange((CTRL_START+bankadd+i), 0, MIDI_CHAN);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" CTRL ");
        lcd.print (ctrlval[i]);
        lcd.setCursor(0,1);
        lcd.print(" 0");
        Serial.println(String(" ch= ") + MIDI_CHAN + (", Control = ") + (CTRL_START+bankadd+i) + (" Velocity = 0 MOMENTARY "));
      }

//SEE IF THIS WORKS
      delBool[i]=false;
      
    }                                                             
    if (millisTime-delTime[i]>=delHold && delBool[i]==true) {        //for each button send a "delete" control value (mapped to delete looper button) if delbool is true and pressure time longer than delHold (3s)
      delBool[i]=false;
      usbMIDI.sendControlChange((CTRL_START+bankadd+i+(CTRLBUTTONS_N*NUM_BANKS)), 127, MIDI_CHAN); //send a ctrl change that adds on top of the ctrl values already send by the buttons (plus the buttons*times bank)
      Serial.println ("3s Ctrl");                                                              //so the value of every del ctrl button is the value of the ctrl plus 30
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" CTRL ");
      lcd.print (CTRL_START+bankadd+i+(CTRLBUTTONS_N*NUM_BANKS));
      Serial.println(String(" ch= ") + MIDI_CHAN + (", Control = ") + (CTRL_START+bankadd+i+(CTRLBUTTONS_N*NUM_BANKS)) + (" Velocity = 127 "));
      delTime[i]=millisTime;
      delBool[i]=true;
      
 
    }
  }
}


void readButtons(){
  buttonBank.update();
  buttonStop.update();
  buttonPlay.update();
  for(int i=0; i<CTRLBUTTONS_N; i++){   
    buttons[i].update();
  }
}


