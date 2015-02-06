#include <Bounce.h>
#include <Encoder.h>

/***************************************
/
/
/    teensy Xplane Dr400
/
/ Author : Nicolas Montarnal
/ Date : 3-feb-2015
/
/ from : https://simelectronics.wordpress.com/
/ from : http://www.pjrc.com/teensy/td_flightsim.html
/
/ under GPL
/
/**************************************/



//mapping pins
// Ignition
const int Pin_IgnOff = 7;
const int Pin_IgnLeft = 8;
const int Pin_IgnRight = 9;
const int Pin_IgnBoth = 10;
const int Pin_IgnStart = 11;
//lights
const int Pin_LandLight = 3;
const int Pin_TaxyLight = 2;
const int Pin_StrobeLight = 1;
const int Pin_NavLight = 0;
const int Pin_BeaconLight = 27;
//elec
const int Pin_Battery = 20;
const int Pin_Generator = 21;
const int Pin_Avionic = 23;
const int Pin_LandingGear = 12;
//engine
const int Pin_FuelPump = 22;
const int Pin_CarbHeat = 19;
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

// encoder reduction 
const int EncoderStep = 6;

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
Bounce bounceTaxyLight(Pin_TaxyLight, 5);
Bounce bounceNavLight(Pin_NavLight, 5);
Bounce bounceStrobeLight(Pin_StrobeLight, 5);
Bounce bounceBeaconLight(Pin_BeaconLight, 5);
//elec
Bounce bounceBattery(Pin_Battery, 5);
Bounce bounceGenerator(Pin_Generator, 5);
Bounce bounceAvionic(Pin_Avionic, 5);
Bounce bounceLandingGear(Pin_LandingGear, 5);

//engine
Bounce bounceFuelPump(Pin_FuelPump, 5);
Bounce bounceCarbHeat(Pin_CarbHeat, 5);
//radio switch 
Bounce bounceRadioFlip(Pin_RadioSwitch, 25);
Bounce bounceNavFlip(Pin_NavSwitch, 25);


// output dataref/commands
// the ignition key dataref does not engage the starter for some reason
// so we'll also use the normal engage_starter_1 command
FlightSimInteger xIgnPosition;
FlightSimCommand xEngineStart1;
FlightSimInteger xBattery;
FlightSimInteger xGenerator;
FlightSimInteger xAvionic;
FlightSimInteger xLandLight;
FlightSimInteger xTaxyLight;
FlightSimInteger xNavLight;
FlightSimInteger xStrobeLight;
FlightSimInteger xBeaconLight;
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
FlightSimCommand xLandingGearUp;
FlightSimCommand xLandingGearDown;

// per X-Plane DataRef docs
enum IGN_KEY_POSITION {
  IGN_OFF = 0,
  IGN_LEFT = 1,
  IGN_RIGHT = 2,
  IGN_BOTH = 3,
  IGN_START = 4
};
//remember some values for the encoders
int lastRM, ladtRK, lastNM, LastNK;



void setup() {
  
  pinMode (Pin_IgnOff, INPUT_PULLUP);
  pinMode (Pin_IgnLeft, INPUT_PULLUP);
  pinMode (Pin_IgnRight, INPUT_PULLUP);
  pinMode (Pin_IgnBoth, INPUT_PULLUP);
  pinMode (Pin_IgnStart, INPUT_PULLUP);
  pinMode (Pin_LandLight, INPUT_PULLUP);
  pinMode (Pin_TaxyLight, INPUT_PULLUP);
  pinMode (Pin_NavLight, INPUT_PULLUP);
  pinMode (Pin_StrobeLight, INPUT_PULLUP);
  pinMode (Pin_BeaconLight, INPUT_PULLUP);
  pinMode (Pin_Battery, INPUT_PULLUP);
  pinMode (Pin_Generator, INPUT_PULLUP);
  pinMode (Pin_Avionic, INPUT_PULLUP);
  pinMode (Pin_LandingGear, INPUT_PULLUP);
  pinMode (Pin_FuelPump, INPUT_PULLUP);
  pinMode (Pin_CarbHeat, INPUT_PULLUP);
  pinMode (Pin_RadioSwitch, INPUT_PULLUP);
  pinMode (Pin_NavSwitch, INPUT_PULLUP);
  

// create anti-bounce 5ms (anti rebond) switch
// https://www.pjrc.com/teensy/td_libs_Bounce.html

 
  //set dataPrefs 
  xIgnPosition = XPlaneRef("sim/cockpit2/engine/actuators/ignition_on[0]");
  xEngineStart1 = XPlaneRef("sim/starters/engage_starter_1");
  xBattery = XPlaneRef("sim/cockpit2/electrical/battery_on");
  xGenerator = XPlaneRef("sim/cockpit2/electrical/generator_on[0]");
  xAvionic = XPlaneRef("sim/cockpit2/switches/avionics_power_on");
  xLandLight = XPlaneRef("sim/cockpit2/switches/landing_lights_on");
  xTaxyLight = XPlaneRef("sim/cockpit2/switches/taxi_light_on");
  xNavLight = XPlaneRef("sim/cockpit2/switches/navigation_lights_on");
  xStrobeLight = XPlaneRef("sim/cockpit2/switches/strobe_lights_on");
  xBeaconLight = XPlaneRef("sim/cockpit2/switches/beacon_on");
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
   
   xLandingGearUp = XPlaneRef("sim/flight_controls/landing_gear_up");
   xLandingGearDown = XPlaneRef("sim/flight_controls/landing_gear_down");

   lastRM = ladtRK = lastNM = LastNK = 0;
  
}

void loop() {
  
  //read datas from xplane (not use yet, but one day some leds will light up)
  FlightSim.update();
  
   
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
  bounceLandLight.update(); bounceTaxyLight.update(); bounceNavLight.update(); bounceStrobeLight.update(); bounceBeaconLight.update();
  if (bounceLandLight.fallingEdge()) {
    xLandLight = true;  
  }else if (bounceLandLight.risingEdge()) {
    xLandLight = false;  
  }
  if (bounceTaxyLight.fallingEdge()) {
    xTaxyLight = true;  
  }else if (bounceTaxyLight.risingEdge()) {
    xTaxyLight = false;  
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
  if (bounceBeaconLight.fallingEdge()) {
    xBeaconLight = true;  
  }else if (bounceBeaconLight.risingEdge()) {
    xBeaconLight = false;  
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
  
  //landing Gear  
  bounceLandingGear.update();
  if (bounceLandingGear.fallingEdge()) {
    xLandingGearDown.once();
  }else if (bounceLandingGear.risingEdge()) {
    xLandingGearUp.once();
  }
  
  
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
  int rMhz = radioMhz.read() / EncoderStep;
  int rKhz = radioKhz.read() / EncoderStep;
  int nMhz = navMhz.read() / EncoderStep;
  int nKhz = navKhz.read() / EncoderStep;
  
  if (rMhz > lastRM ) {
    xRadioUp.once();
    if (  -1000 > rMhz || rMhz > 1000 ) {rMhz =0; radioMhz.write(0);} //check to not go to far
    lastRM = rMhz; //save value
  }else if (rMhz < lastRM ) {
    if (  -1000 > rMhz || rMhz > 1000 ) {rMhz =0; radioMhz.write(0);}; //check to not go to far
    lastRM = rMhz; //save value
    xRadioDown.once();
  }
  
  if (rKhz > ladtRK ) {
    if (  -1000 > rKhz || rKhz > 1000 ) {rKhz =0; radioKhz.write(0);}; //check to not go to far
    ladtRK = rKhz; //save value
    xRadioFineUp.once();
  }else if (rKhz < ladtRK ) {
    if (  -1000 > rKhz || rKhz > 1000 ) {rKhz =0; radioKhz.write(0);}; //check to not go to far
    ladtRK = rKhz; //save value
    xRadioFineDown.once();
  }
  
  if (nMhz >lastNM ) {
    if (  -1000 > nMhz || nMhz > 1000 ) {nMhz =0; navMhz.write(0);}; //check to not go to far
    lastNM = nMhz; //save value
    xNavUp.once();
  }else if (nMhz < lastNM ) {
    if (  -1000 > nMhz || nMhz > 1000 ) {nMhz =0; navMhz.write(0);}; //check to not go to far
    lastNM = nMhz; //save value
    xNavDown.once();
  }
  
  if (nKhz > LastNK ) {
    if (  -1000 > nKhz || nKhz > 1000 ) {nKhz =0; navKhz.write(0);}; //check to not go to far
    LastNK = nKhz; //save value
    xNavFineUp.once();
  }else if (nKhz < LastNK ) {
    if (  -1000 > nKhz || nKhz > 1000 ) {nKhz =0; navKhz.write(0);}; //check to not go to far
    LastNK = nKhz; //save value
    xNavFineDown.once();
  }
  

  
  
}
