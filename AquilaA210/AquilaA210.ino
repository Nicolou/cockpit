#include <Bounce.h>
#include <Encoder.h>

/***************************************
/
/
/    teensy Xplane for Aquila A210
/
/ Author : Nicolas Montarnal
/ Date : 14-feb-2016
/
/ from : https://simelectronics.wordpress.com/
/ from : http://www.pjrc.com/teensy/td_flightsim.html
/
/ GPL licence
/
***************************************/

#define DEBUG false


//mapping pins
// Ignition
const int Pin_IgnOff = 7;
const int Pin_IgnLeft = 8;
const int Pin_IgnRight = 9;
const int Pin_IgnBoth = 10;
const int Pin_IgnStart = 11;
//lights
const int Pin_LandLight = 0;
const int Pin_StrobeLight = 1;
const int Pin_NavLight = 2;
const int Pin_PanelLight = 27;
//flaps
const int Pin_Flap_0 = 16;
const int Pin_Flap_1 = 15;
const int Pin_Flap_2 = 13;
//elec
const int Pin_Battery = 20;
const int Pin_Generator = 21;
const int Pin_Avionic = 23;
//engine
const int Pin_FuelPump = 22;
const int Pin_CarbHeat = 19;
//propeller
const int Analog_Pin_Prop = 7;
const float Prop_max_speed_rad_sec = 250;
const float Prop_min_speed_rad_sec = 83.75;
const int Prop_potar_max_value = 1023;
const int Prop_potar_min_value = 575;
//swap Encoder functionalities
const int Pin_SwapEncoderFuncInstruments = 5;
const int Pin_SwapEncoderFuncAvionic = 12;
//Radio-Nav
const int Pin_RadioSwitch = 17;
const int Pin_RadioMhz_L = 28;
const int Pin_RadioMhz_R = 29;
const int Pin_RadioKhz_L = 32;
const int Pin_RadioKhz_R = 33;
const int Pin_NavSwitch = 14;
const int Pin_NavMhz_L = 30;
const int Pin_NavMhz_R = 31;
const int Pin_NavKhz_L = 34;
const int Pin_NavKhz_R = 35;

// program Constants
// encoder reduction 
const int EncoderStepRadio = 6;
const int EncoderStepSquawk = 12;
const int EncoderStepInstruments = 1;
// Encoder functionality
enum ENCOD_FUNC {
  RADIO = 0,
  SQUAWK = 1,
  INSTRUMENTS = 2
};

// encoder defined:
Encoder radioMhz(Pin_RadioMhz_L, Pin_RadioMhz_R);
Encoder radioKhz(Pin_RadioKhz_L, Pin_RadioKhz_R);
Encoder navMhz(Pin_NavMhz_L, Pin_NavMhz_R);
Encoder navKhz(Pin_NavKhz_L, Pin_NavKhz_R);

// create anti-bounce 5ms (anti rebond) switch
// https://www.pjrc.com/teensy/td_libs_Bounce.html
// Ignition
Bounce bounceIgnOff(Pin_IgnOff, 5);
Bounce bounceIgnRight(Pin_IgnRight, 5);
Bounce bounceIgnLeft(Pin_IgnLeft, 5);
Bounce bounceIgnBoth(Pin_IgnBoth, 5);
Bounce bounceIgnStart(Pin_IgnStart, 5);
//lights
Bounce bounceLandLight(Pin_LandLight, 5);
Bounce bounceNavLight(Pin_NavLight, 5);
Bounce bounceStrobeLight(Pin_StrobeLight, 5);
Bounce bouncePanelLight(Pin_PanelLight, 5);
//elec
Bounce bounceBattery(Pin_Battery, 5);
Bounce bounceGenerator(Pin_Generator, 5);
Bounce bounceAvionic(Pin_Avionic, 5);
//flaps
Bounce bounceflap_0( Pin_Flap_0, 5);
Bounce bounceflap_1( Pin_Flap_1, 5);
Bounce bounceflap_2( Pin_Flap_2, 5);
//engine
Bounce bounceFuelPump(Pin_FuelPump, 5);
Bounce bounceCarbHeat(Pin_CarbHeat, 5);
//radio switch 
Bounce bounceRadioFlip(Pin_RadioSwitch, 25);
Bounce bounceNavFlip(Pin_NavSwitch, 25);
//swap function encoder
Bounce bounceSwapAvionicInstruments(Pin_SwapEncoderFuncInstruments, 5);
Bounce bounceSwapFuncAvionic(Pin_SwapEncoderFuncAvionic, 5);

// output dataref/commands
// the ignition key dataref does not engage the starter for some reason
// so we'll also use the normal engage_starter_1 command
FlightSimInteger xIgnPosition;
FlightSimCommand xEngineStart1;
FlightSimInteger xBattery;
FlightSimInteger xGenerator;
FlightSimInteger xAvionic;
FlightSimInteger xLandLight;
FlightSimInteger xNavLight;
FlightSimInteger xStrobeLight;
FlightSimInteger xPanelLight;
FlightSimFloat xCarbHeat;
FlightSimInteger xFuelPump;
FlightSimCommand xRadioFlip;
FlightSimCommand xRadioUp;
FlightSimCommand xRadioDown;
FlightSimCommand xRadioFineUp;
FlightSimCommand xRadioFineDown;
FlightSimCommand xNavFlip;
FlightSimCommand xNavUp;
FlightSimCommand xNavDown;
FlightSimCommand xNavFineUp;
FlightSimCommand xNavFineDown;
FlightSimInteger xSquawkCode;
FlightSimInteger xSquawkMode;
FlightSimCommand xSquawkSendId;
FlightSimFloat xBaroAltimeter;
FlightSimFloat xDirectionalGyroDrift;
FlightSimFloat xFlapRatio;
FlightSimFloat xPropeller;
FlightSimFloat xHeadingVOR1;

    

// per X-Plane DataRef docs
enum IGN_KEY_POSITION {
  IGN_OFF = 0,
  IGN_LEFT = 1,
  IGN_RIGHT = 2,
  IGN_BOTH = 3,
  IGN_START = 4
};
//remember some values for the encoders
int valsEncoders[3][4];

int encoderActiveFunction;

// remember propeller old value
int lastPropValue;



void setup() {

  Serial.println( " Teensy program start for cockpit Xplane ");
  Serial.println( " Version 1.0.2 ");
  
  pinMode (Pin_IgnOff, INPUT_PULLUP);
  pinMode (Pin_IgnLeft, INPUT_PULLUP);
  pinMode (Pin_IgnRight, INPUT_PULLUP);
  pinMode (Pin_IgnBoth, INPUT_PULLUP);
  pinMode (Pin_IgnStart, INPUT_PULLUP);
  pinMode (Pin_LandLight, INPUT_PULLUP);
  pinMode (Pin_NavLight, INPUT_PULLUP);
  pinMode (Pin_StrobeLight, INPUT_PULLUP);
  pinMode (Pin_PanelLight, INPUT_PULLUP);
  pinMode (Pin_Battery, INPUT_PULLUP);
  pinMode (Pin_Generator, INPUT_PULLUP);
  pinMode (Pin_Avionic, INPUT_PULLUP);
  pinMode (Pin_FuelPump, INPUT_PULLUP);
  pinMode (Pin_CarbHeat, INPUT_PULLUP);
  pinMode (Pin_RadioSwitch, INPUT_PULLUP);
  pinMode (Pin_NavSwitch, INPUT_PULLUP);
  pinMode (Pin_Flap_0, INPUT_PULLUP);
  pinMode (Pin_Flap_1, INPUT_PULLUP);
  pinMode (Pin_Flap_2, INPUT_PULLUP);
  pinMode (Pin_SwapEncoderFuncInstruments, INPUT_PULLUP);
  pinMode (Pin_SwapEncoderFuncAvionic, INPUT_PULLUP);
  

// create anti-bounce 5ms (anti rebond) switch
// https://www.pjrc.com/teensy/td_libs_Bounce.html

 
    //set dataPrefs 
    xIgnPosition = XPlaneRef("sim/cockpit2/engine/actuators/ignition_on[0]");
    xEngineStart1 = XPlaneRef("sim/starters/engage_starter_1");
    xBattery = XPlaneRef("sim/cockpit2/electrical/battery_on");
    xGenerator = XPlaneRef("sim/cockpit2/electrical/generator_on[0]");
    xAvionic = XPlaneRef("sim/cockpit2/switches/avionics_power_on");
    xLandLight = XPlaneRef("sim/cockpit2/switches/landing_lights_on");
    xNavLight = XPlaneRef("sim/cockpit2/switches/navigation_lights_on");
    xStrobeLight = XPlaneRef("sim/cockpit2/switches/strobe_lights_on");
    xPanelLight = XPlaneRef("sim/pcx/night_light_toggle");
    xCarbHeat = XPlaneRef("sim/cockpit2/engine/actuators/carb_heat_ratio");
    xFuelPump = XPlaneRef("sim/cockpit2/engine/actuators/fuel_pump_on[0]");
  
    xRadioFlip = XPlaneRef("sim/radios/com1_standy_flip");
    xRadioUp = XPlaneRef("sim/radios/stby_com1_coarse_up");
    xRadioDown = XPlaneRef("sim/radios/stby_com1_coarse_down");
    xRadioFineUp = XPlaneRef("sim/radios/stby_com1_fine_up");
    xRadioFineDown = XPlaneRef("sim/radios/stby_com1_fine_down");
    xNavFlip = XPlaneRef("sim/radios/nav1_standy_flip");
    xNavUp = XPlaneRef("sim/radios/stby_nav1_coarse_up");
    xNavDown = XPlaneRef("sim/radios/stby_nav1_coarse_down");
    xNavFineUp = XPlaneRef("sim/radios/stby_nav1_fine_up");
    xNavFineDown = XPlaneRef("sim/radios/stby_nav1_fine_down");
    
    
    xFlapRatio= XPlaneRef("sim/cockpit2/controls/flap_ratio");
    xPropeller= XPlaneRef("sim/cockpit2/engine/actuators/prop_rotation_speed_rad_sec_all");

    xSquawkCode = XPlaneRef("sim/cockpit/radios/transponder_code");
    xSquawkMode = XPlaneRef("sim/cockpit/radios/transponder_mode");
    xSquawkSendId = XPlaneRef("sim/cockpit/radios/transponder_id");

    xBaroAltimeter = XPlaneRef("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot");
    xDirectionalGyroDrift = XPlaneRef("sim/cockpit/gyros/dg_drift_vac_deg");
    xHeadingVOR1 = XPlaneRef("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot");
    
   for (int i=0; i <= 3; i++){
      for (int j=0; i <= 4; j++){
        valsEncoders[i][j] = 0;  
      } 
   } 
   encoderActiveFunction = RADIO;

   if (DEBUG) xSquawkCode = 7231;
   
}

void loop() {
  
  //read datas from xplane (not use yet, but one day some leds will light up)
  FlightSim.update();

  //Swap Encoder function -------------------------------------------------
  bounceSwapAvionicInstruments.update();bounceSwapFuncAvionic.update();
  //intruments swap button is prior than radioSquak sawp button
  //instrument swaping ON
  if (bounceSwapAvionicInstruments.fallingEdge()) {
    encoderActiveFunction = INSTRUMENTS;
    //reset saving counters
    readEncoderAnsSaveIntoTable();
  }//instrument swaping OFF
  else if (bounceSwapAvionicInstruments.risingEdge()) {
    //reset saving counters
    readEncoderAnsSaveIntoTable();
    if (digitalRead(Pin_SwapEncoderFuncAvionic)==LOW) {
      encoderActiveFunction = SQUAWK;
    }else{
      encoderActiveFunction = RADIO;
    }

  } //if button instruments swap is OFF
  else if (digitalRead(Pin_SwapEncoderFuncInstruments)==HIGH ) {
    if (bounceSwapFuncAvionic.fallingEdge()) {
      encoderActiveFunction = SQUAWK;
        
      //reset saving counters
      readEncoderAnsSaveIntoTable();
    }
    if (bounceSwapFuncAvionic.risingEdge()) {
      encoderActiveFunction = RADIO;
      //reset saving counters
      readEncoderAnsSaveIntoTable();
    }
    
  }else{
    encoderActiveFunction = INSTRUMENTS;
  }
 


  
  //manage ignition 
  bounceIgnOff.update(); bounceIgnRight.update(); bounceIgnLeft.update(); bounceIgnBoth.update(); bounceIgnStart.update();
 
  // non-blocking input to update ignition key position
  if (bounceIgnOff.fallingEdge()) xIgnPosition = IGN_OFF;
  if (bounceIgnLeft.fallingEdge()) xIgnPosition = IGN_LEFT;
  if (bounceIgnRight.fallingEdge()) xIgnPosition = IGN_RIGHT;
  if (bounceIgnBoth.fallingEdge()) xIgnPosition = IGN_BOTH;
  // engage starter using command as well as by moving key
  if (bounceIgnStart.fallingEdge()) {
    xIgnPosition = IGN_START;
    xEngineStart1.begin();
  }
  if (bounceIgnStart.risingEdge()) {
    xEngineStart1.end();
    xIgnPosition = IGN_START;
  }

  
  //manage elec
  bounceBattery.update(); bounceGenerator.update(); bounceAvionic.update();
  if (bounceBattery.fallingEdge()) {
    xBattery = true;  
  }else if (bounceBattery.risingEdge()) {
    xBattery = false;  
  }
  if (bounceGenerator.fallingEdge()) {
    xGenerator = true;  
  }else if (bounceGenerator.risingEdge()) {
    xGenerator = false;  
  }
  if (bounceAvionic.fallingEdge()) {
    xAvionic = true;  
  }else if (bounceAvionic.risingEdge()) {
    xAvionic = false;  
  }
  
  //manage Lights  
  bounceLandLight.update(); bounceNavLight.update(); bounceStrobeLight.update();bouncePanelLight.update();
  if (bounceLandLight.fallingEdge()) {
    xLandLight = true;  
  }else if (bounceLandLight.risingEdge()) {
    xLandLight = false;  
  }
    if (bounceNavLight.fallingEdge()) {
    xNavLight = true;  
  }else if (bounceNavLight.risingEdge()) {
    xNavLight = false;  
  }
  if (bounceStrobeLight.fallingEdge()) {
    xStrobeLight = true;  
  }else if (bounceStrobeLight.risingEdge()) {
    xStrobeLight = false;  
  }
  if (bouncePanelLight.fallingEdge()) {
    xPanelLight = true;  
  }else if (bouncePanelLight.risingEdge()) {
    xPanelLight = false;  
  }

  //flaps
  bounceflap_0.update(); bounceflap_1.update(); bounceflap_2.update();
  if (bounceflap_0.fallingEdge()) {
    xFlapRatio = 0;
  }else if (bounceflap_1.fallingEdge()) {
    xFlapRatio = 0.5;
  }else if (bounceflap_2.fallingEdge()) {
    xFlapRatio = 1;
  }
     
  //engine
  bounceFuelPump.update(); bounceCarbHeat.update();
  if (bounceFuelPump.fallingEdge()) {
    xFuelPump = true;  
  }else if (bounceFuelPump.risingEdge()) {
    xFuelPump = false;
  }
  if (bounceCarbHeat.fallingEdge()) {
    xCarbHeat = 1;  
  }else if (bounceCarbHeat.risingEdge()) {
    xCarbHeat = 0; 
  }
 

  //propeller
  int propVal = analogRead(Analog_Pin_Prop);
  //edges values of propeller value read 
  if (propVal > Prop_potar_max_value) propVal = Prop_potar_max_value;
  if (propVal < Prop_potar_min_value) propVal = Prop_potar_min_value;
  
  if (((lastPropValue + 2 ) < propVal ) || ( propVal < ( lastPropValue - 2 ))) {

    //remember
    lastPropValue = propVal;
    // function :  prop_speed_rad_sec = alpha * propval + Const
    float alpha, Const, prop_speed_rad_sec;
    alpha = ( Prop_max_speed_rad_sec - Prop_min_speed_rad_sec ) / ( Prop_potar_max_value - Prop_potar_min_value );
    Const =  Prop_max_speed_rad_sec - alpha * Prop_potar_max_value;

    prop_speed_rad_sec = alpha * propVal + Const;
    /* Serial.print(" propVal = ");
    Serial.print(propVal, DEC);
    Serial.print(" prop_speed_rad_sec = ");
    Serial.println(prop_speed_rad_sec);
    */
    xPropeller = prop_speed_rad_sec;
      
  }


  if (encoderActiveFunction == RADIO) {
          
          //radio --------------
          // flip Com and Nav
          bounceRadioFlip.update(); bounceNavFlip.update();
          if (bounceRadioFlip.fallingEdge()) {
            xRadioFlip.begin();
          }else if (bounceRadioFlip.risingEdge()) {
            xRadioFlip.end();  
          }
          if (bounceNavFlip.fallingEdge()) {
            xNavFlip.begin();  
          }else if (bounceNavFlip.risingEdge()) {
            xNavFlip.end();  
          }
          //encoder frequencies 
          int rMhz = radioMhz.read() / EncoderStepRadio;
          int rKhz = radioKhz.read() / EncoderStepRadio;
          int nMhz = navMhz.read() / EncoderStepRadio;
          int nKhz = navKhz.read() / EncoderStepRadio;
          
          if (rMhz > valsEncoders[0][0] ) {
            xRadioUp.once();
            if (  -1000 > rMhz || rMhz > 1000 ) {rMhz =0; radioMhz.write(0);} //check to not go to far
            valsEncoders[0][0]  = rMhz; //save value
          }else if (rMhz < valsEncoders[0][0] ) {
            if (  -1000 > rMhz || rMhz > 1000 ) {rMhz =0; radioMhz.write(0);}; //check to not go to far
            valsEncoders[0][0]  = rMhz; //save value
            xRadioDown.once();
          }
          
          if (rKhz > valsEncoders[0][1] ) {
            if (  -1000 > rKhz || rKhz > 1000 ) {rKhz =0; radioKhz.write(0);}; //check to not go to far
            valsEncoders[0][1] = rKhz; //save value
            xRadioFineUp.once();
          }else if (rKhz < valsEncoders[0][1] ) {
            if (  -1000 > rKhz || rKhz > 1000 ) {rKhz =0; radioKhz.write(0);}; //check to not go to far
            valsEncoders[0][1] = rKhz; //save value
            xRadioFineDown.once();
          }
          
          if (nMhz > valsEncoders[0][2] ) {
            if (  -1000 > nMhz || nMhz > 1000 ) {nMhz =0; navMhz.write(0);}; //check to not go to far
            valsEncoders[0][2] = nMhz; //save value
            xNavUp.once();
          }else if (nMhz < valsEncoders[0][2] ) {
            if (  -1000 > nMhz || nMhz > 1000 ) {nMhz =0; navMhz.write(0);}; //check to not go to far
            valsEncoders[0][2] = nMhz; //save value
            xNavDown.once();
          }
          
          if (nKhz > valsEncoders[0][3] ) {
            if (  -1000 > nKhz || nKhz > 1000 ) {nKhz =0; navKhz.write(0);}; //check to not go to far
            valsEncoders[0][3] = nKhz; //save value
            xNavFineUp.once();
          }else if (nKhz < valsEncoders[0][3] ) {
            if (  -1000 > nKhz || nKhz > 1000 ) {nKhz =0; navKhz.write(0);}; //check to not go to far
            valsEncoders[0][3] = nKhz; //save value
            xNavFineDown.once();
          }

          
    }else if (encoderActiveFunction == SQUAWK) {
          //squwak --------------
          // mode C
          bounceRadioFlip.update(); bounceNavFlip.update();
          if (bounceRadioFlip.fallingEdge()) {
            xSquawkMode= 2;
          }else if (bounceRadioFlip.risingEdge()) {
            xSquawkMode = 1;
          }// send ID
          if (bounceNavFlip.fallingEdge()) {
            xSquawkSendId.begin();  
          }else if (bounceNavFlip.risingEdge()) {
            xSquawkSendId.end();  
          }
          
          //encoder squawk 
          int a = radioMhz.read() / EncoderStepSquawk;
          int b = radioKhz.read() / EncoderStepSquawk;
          int c = navMhz.read() / EncoderStepSquawk;
          int d = navKhz.read() / EncoderStepSquawk;

          int sA = xSquawkCode / 1000 ;
          int sB = ( xSquawkCode % 1000 ) / 100 ;
          int sC = ( xSquawkCode % 100 ) /10 ;
          int sD = ( xSquawkCode % 10 );
          
          if (a > valsEncoders[1][0] ) {
                valsEncoders[1][0]  = a; //save value
                sA = ( sA + 1  ) % 8 ;
          }else if (a < valsEncoders[1][0]  ) {
                valsEncoders[1][0]  = a; //save value
                sA = ( sA - 1  );
                if (sA < 0 ) sA = 7;
          }          
          if (b > valsEncoders[1][1] ) {
                valsEncoders[1][1] = b; //save value
                sB = ( sB + 1  ) % 8 ;
          }else if (b < valsEncoders[1][1] ) {
                valsEncoders[1][1] = b; //save value
                sB = ( sB - 1  );
                if (sB < 0 ) sB = 7;
          }          
          if (c > valsEncoders[1][2] ) {
                 valsEncoders[1][2] = c; //save value
                sC = ( sC + 1  ) % 8 ;
          }else if (c <  valsEncoders[1][2] ) {
                 valsEncoders[1][2] = c; //save value
                sC = ( sC - 1  );
                if (sC < 0 ) sC = 7;
          }          
          if (d >  valsEncoders[1][3] ) {
                 valsEncoders[1][3] = d; //save value
                sD = ( sD + 1  ) % 8 ;
          }else if (d <  valsEncoders[1][3] ) {
                 valsEncoders[1][3] = d; //save value
                sD = ( sD - 1  );
                if (sD < 0 ) sD = 7;
          }
         
          xSquawkCode = sA*1000 + sB*100 + sC*10 + sD;

          
    }else if (encoderActiveFunction == INSTRUMENTS) {
          //encoder instruments 
          int a = radioMhz.read() / EncoderStepInstruments;
          //int b = radioKhz.read() / EncoderStepInstruments;
          int c = navMhz.read() / EncoderStepInstruments;
          int d = navKhz.read() / EncoderStepInstruments;

          //altimeter baro setting
          if (a >  valsEncoders[2][0] ) {
                valsEncoders[2][0] = a; //save value
                xBaroAltimeter = xBaroAltimeter + 0.0025;
          }else if (a < valsEncoders[2][0] ) {
                valsEncoders[2][0] = a; //save value
                xBaroAltimeter = xBaroAltimeter - 0.0025;
          }
          // directional gyro setting 
          if (c > valsEncoders[2][2] ) {
                valsEncoders[2][2] = c; //save value
                xDirectionalGyroDrift = xDirectionalGyroDrift + 0.25;
          }else if (c < valsEncoders[2][2] ) {
                valsEncoders[2][2] = c; //save value
                xDirectionalGyroDrift = xDirectionalGyroDrift - 0.25;
          }          

          // VOR 1 OBS setting 
          if (d > valsEncoders[2][3] ) {
                valsEncoders[2][3] = d; //save value
                xHeadingVOR1 = xHeadingVOR1 + 0.25;
          }else if (d < valsEncoders[2][3] ) {
                valsEncoders[2][3] = d; //save value
                xHeadingVOR1 = xHeadingVOR1 - 0.25;
          } 



          if (DEBUG) {
            Serial.print(" alti Hg = " ); Serial.print( xBaroAltimeter ) ;  Serial.print( ", directionnal gyro = ") ;  Serial.println( xDirectionalGyroDrift ) ;
          }
    }
  
}


//init valsEncoders with reading Encoder values
void readEncoderAnsSaveIntoTable() {
      //reset saving counters
      valsEncoders[0][0] = radioMhz.read() / EncoderStepRadio ;
      valsEncoders[0][1] = radioKhz.read() / EncoderStepRadio;
      valsEncoders[0][2] = navMhz.read() / EncoderStepRadio;
      valsEncoders[0][3] = navKhz.read() / EncoderStepRadio;
      
      valsEncoders[1][0] = radioMhz.read() /  EncoderStepSquawk ;
      valsEncoders[1][1] = radioKhz.read() /  EncoderStepSquawk;
      valsEncoders[1][2] = navMhz.read() /  EncoderStepSquawk;
      valsEncoders[1][3] = navKhz.read() /  EncoderStepSquawk;
      
      valsEncoders[2][0] = radioMhz.read() / EncoderStepInstruments;
      valsEncoders[2][1] = radioKhz.read() / EncoderStepInstruments;
      valsEncoders[2][2] = navMhz.read() / EncoderStepInstruments;
      valsEncoders[2][3] = navKhz.read() / EncoderStepInstruments;
      
}
