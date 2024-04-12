#include <HeliOS.h>

#define FG_INTERVAL_MS    500


// Failure generation mode
typedef enum {
  FM_ARBITRATION,
  FM_DATA
} FM_T;

FM_T fg_fm = FM_ARBITRATION;

// failure generation timer 
//    0 : no timed failure generation
int fg_time = FG_INTERVAL_MS;

// number of failures after receiving pattern
//    0 : pattern doesn't trigger failure
int fg_nfail = 0;

// skip N frames between pattern triggered failures
//    0 : generate consecutive failures
int fg_skip = 0;

// any failure has been generated
int fg_fire = 0;

// the number of timed failures
int fg_tn = 0;

// the number of pattern induced failures
int fg_pn = 0;

// the state of HeliOS initialization
int helios_state = 0;

void setup() {
  
  xTask failtask;

  Serial.setTimeout(100);
  Serial.begin(115200);
  while (!Serial) {}

  print_help();
  print_param();

  setup_task();
  if (helios_state){
    Serial.print("HeliOS init failure: ");
    Serial.println(helios_state);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available()){
    Serial.println("Ops");
    fetch_param();
    print_param();
  }

  if (fg_fire){
    print_fgstatus();
    fg_fire = 0;
  }
}

/*
  Generate failure at timer interrupt
*/
void failTask_main(xTask task_, xTaskParm parm_) {

  fg_tn++;
  fg_fire++;
  return;

}


void setup_task(){

  xTask blink;

  Serial.println("helios_1");
  if(ERROR(xSystemInit())) {
    helios_state = 1;
  }

  Serial.println("helios_2");
  if(ERROR(xTaskCreate(&blink, (const xByte *) "BLINKTSK", failTask_main, NULL))) {
    helios_state = 2;
  }

  Serial.println("helios_3");
  if(ERROR(xTaskWait(blink))) {
    helios_state = 3;
  }

  Serial.println("helios_4");
  if(ERROR(xTaskChangePeriod(blink, FG_INTERVAL_MS))) {
    helios_state = 4;
  }

  Serial.println("helios_5");
  if(ERROR(xTaskStartScheduler())) {
    helios_state = 5;
  }
  Serial.println("helios_6");

}

/*
  Read string from serial line, set operation parameters accordingly
*/

void fetch_param() {
  String teststr = Serial.readString();
  Serial.println(teststr);
}

void print_help() {
  Serial.println("Failure mode: Arbitration/Data");
  Serial.println("Trigger mode: ");
  Serial.println("  Timer at specified time - T55 : at each 55 ms");
  Serial.println("  Pattern N times         - N3 : 3 times after receiving pattern");
  Serial.println("  Pattern skip            - S10 : skip 10 frames before next failure generation");
  Serial.println();
}

void print_param() {

  Serial.print("Failure mode : ");

  if (fg_fm == FM_ARBITRATION){
    Serial.print("Arb ");
  }
  else {
    Serial.print("Data ");
  }

  if (fg_time){
    Serial.print("T_");
    Serial.print(fg_time);
    Serial.print(" ");
  }

  if (fg_nfail){
    Serial.print("N_");
    Serial.print(fg_nfail);
    Serial.print(" S_");
    Serial.print(fg_skip);
    Serial.print(" ");
  }

  Serial.println();

}

void print_fgstatus(){

  if (fg_fm == FM_ARBITRATION){
    Serial.print("FG_Arb ");
  }
  else {
    Serial.print("FG_Data ");
  }

  // the number of timer induced failures
  if (fg_time){
    Serial.print("T_N_");
    Serial.print(fg_tn);
    Serial.print(" ");
  }

  // the number of pattern induced failures
  if (fg_nfail){
    Serial.print("P_N_");
    Serial.print(fg_pn);
    Serial.print(" ");
  }

  Serial.println();

}