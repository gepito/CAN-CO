#include <mcp2518fd_can.h>
#include <mcp2518fd_can_dfs.h>
#include <mcp_can_fd.h>

/*  MCP2517/8 send a can fd frame

ECLIPSE
    CAN FD Shield - https://www.longan-labs.cc/1030012.html
    CANBed FD - https://www.longan-labs.cc/1030009.html
    
    can-fd baud rate:

    CAN_125K_500K
    CAN_250K_500K
    CAN_250K_750K
    CAN_250K_1M
    CAN_250K_1M5
    CAN_250K_2M
    CAN_250K_3M
    CAN_250K_4M
    CAN_500K_1M
    CAN_500K_2M
    CAN_500K_3M
    CAN_500K_4M
    CAN_1000K_4M
*/

#include <SPI.h>

#define MAX_DATA_SIZE 64
#include <TimerOne.h>

#define FG_MIN_INTERVAL_MS    100

// the length of failure pulse upon timed event
#define FG_TIMED_PULSE_US     10

// SOF interrupt pin
const byte SOF_IT_PIN = 3;
// Failure generator pin
const byte FAILGEN_PIN = 7;

// pins for CAN-FD Shield
const byte SPI_CS_PIN = 9;
const byte CAN_INT_PIN = 2;


// Failure generation mode
typedef enum {
  FM_NONE,
  FM_ARBITRATION,
  FM_DATA
} FM_T;

FM_T fg_fmode = FM_NONE;

// failure generation timer 
//    0 : no timed failure generation
unsigned int fg_time = 0;

// number of failures after receiving pattern
//    0 : pattern doesn't trigger failure
int fg_nfail = 0;

// skip N frames between pattern triggered failures
//    0 : generate consecutive failures
int fg_skip = 0;

// any failure has been generated
int fg_fire = 0;

// the number of numbered/patterned/timed failures
int fg_num_n = 0;
int fg_num_p = 0;
int fg_num_t = 0;


// address of the CAN pattern that triggers failure generation
unsigned int fg_paddress = 0;

// address of the CAN pattern that triggers failure generation
// pattern data must match the first two bytes of the CAN frame
unsigned int fg_pdata = 0;

// parameters for counter based generation
int fg_nmin, fg_nmax;

void setup() {

  // set up GPIO ports
  pinMode(FAILGEN_PIN, OUTPUT);
  pinMode(SOF_IT_PIN, INPUT_PULLUP);
  // attach interrupt ONLY after setting MCP:PIN3 to SOF mode!!!
  // attachInterrupt(digitalPinToInterrupt(SOF_IT_PIN), fg_arb_pulse, RISING);
  // attachInterrupt(digitalPinToInterrupt(SOF_IT_PIN), fg_data_pulse, RISING);

  // deactivate failure generator
  digitalWrite(FAILGEN_PIN, LOW);

  // set up timer "task"
  Timer1.initialize(1500000);
  Timer1.attachInterrupt(fg_timer); // failTask_main to run every 0.15 seconds   
  Timer1.stop();

  Serial.setTimeout(100);
  Serial.begin(115200);
  while (!Serial) {}

  print_help();
  print_param();

}

void loop() {
  // main code here, to run repeatedly:

  if (Serial.available()){
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

  Latency (no processing, constant pulse width generation): 6 us

  Accuracy of  delayMicroseconds
  set   measured [us]
  2     4
  4     6
  8     10
  16    18

*/

/*
  Generate failure during the arbitration phase which is
  @ 500 KBPS: 22/62 us for Standard/Extended frames
  @ 1 MBSP: 11/31 us for Standard/Extended frames

*/
void fg_arb_pulse(){
// constants set for Standard 1/4 MBPS CAN_FD

  // use inline code, assure timingaccuracy!!!
  digitalWrite(FAILGEN_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(FAILGEN_PIN, LOW);

  fg_num_p++;
  fg_fire++;
  return;

}

/*
  Generate failure during the data phase which starts
  @ 500 KBPS: 22/62 us for Standard/Extended frames
  @ 1 MBSP: 11/31 us for Standard/Extended frames

*/
void fg_data_pulse(){
// constants set for Standard 1/4 MBPS CAN_FD

  delayMicroseconds(5);
  // use inline code, assure timingaccuracy!!!
  digitalWrite(FAILGEN_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(FAILGEN_PIN, LOW);

  fg_num_p++;
  fg_fire++;
  return;

}

void fg_timer() {

  // use inline code, assure timingaccuracy!!!
  digitalWrite(FAILGEN_PIN, HIGH);
  delayMicroseconds(FG_TIMED_PULSE_US - 2);
  digitalWrite(FAILGEN_PIN, LOW);

  fg_num_t++;
  fg_fire++;
  return;

}


/*
  Read string from serial line, set operation parameters accordingly
*/

void fetch_param() {
  char cmd;
  int param1, param2;
  String teststr = Serial.readString();
  int scanres = sscanf(teststr.c_str(),"%c %d %d %d %d", &cmd, &param1, &param2);

  switch (char(toupper(cmd))){

    case 'A':
      fg_fmode = FM_ARBITRATION;
      break;

    case 'D':
      fg_fmode = FM_DATA;
      break;

    case 'N':
      if (scanres >= 2){
        fg_nmin = param1;
        if (scanres >= 3){
          fg_nmax = param2;
        }
        else {
          fg_nmax = fg_nmin+1;
        }

        if (fg_fmode == FM_ARBITRATION){
          attachInterrupt(digitalPinToInterrupt(SOF_IT_PIN), fg_arb_pulse, RISING);
        }
        else if (fg_fmode == FM_DATA) {
          attachInterrupt(digitalPinToInterrupt(SOF_IT_PIN), fg_data_pulse, RISING);
        }

      }
      else {
        detachInterrupt(digitalPinToInterrupt(SOF_IT_PIN));
        fg_nmin = 0;
      }
      break;

    case 'P':
      if (scanres >= 2){
        fg_paddress = param1;
        if (scanres >= 2){
          fg_pdata = param2;
        }
        else {
          fg_pdata = 0;
        }
      }
      else {
        detachInterrupt(digitalPinToInterrupt(CAN_INT_PIN));
        fg_paddress = 0;
      }
      break;

    case 'S':
      if (scanres >= 3){
        fg_nfail = param1;
        fg_skip = param2;

        if (fg_fmode == FM_ARBITRATION){
          attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), fg_arb_pulse, RISING);
        }
        else if (fg_fmode == FM_DATA) {
          attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), fg_data_pulse, RISING);
        }

      }
      else {
        detachInterrupt(digitalPinToInterrupt(CAN_INT_PIN));
        fg_nfail = 0;
      }
      break;

    case 'T':
      if ((scanres >= 2) && (param1 > FG_MIN_INTERVAL_MS)){
        fg_time = param1;
        Timer1.initialize(long(fg_time) * 1000);
        Timer1.start();
      }
      else {
        fg_time = 0;
        Timer1.stop();
      }
      break;

    default:
      break;
  }

}

/*
  Arbitration/Data sets which part of the CAN frame is corrupted
  To activate pattern triggered failure generation, CAN address must be set
    CAN Data = 0 match each frames

  Frame number based failure generation random N is selected from <nmin> <nmax>
    N = <nmin> if no nmax is provided
  To activate timed failure generation, time has to be set to > 0 value

  Arbitration/Data setting doesn't apply for timed failure generation
*/
void print_help() {
  Serial.println("Failure mode  A : Arbitration");
  Serial.println("Failure mode  D : Data");
  Serial.println("Trigger mode: ");
  Serial.println("  Number of skip frames          : N <nmin> <nmax>");
  Serial.println("                       nmin = 29 : corrupt each 30-th frame");
  Serial.println("  Pattern is CAN Addr., CAN data : P <adr> <dta>");
  Serial.println("  Sequence at pattern            : S <count> <skip>");
  Serial.println("                       count = 3 : generate 3 failures after receiving pattern");
  Serial.println("                       skip = 10 : skip 10 frames before next failure generation");
  Serial.println("  Timer at specified time        : T <t_ms>");
  Serial.println("                       t_ms = 70 : at each 70 ms");
  Serial.println();
}

void print_param() {

  Serial.print("Failure mode:");

  if ((fg_paddress != 0) || (fg_nmin != 0)){

    if (fg_paddress != 0) {
      Serial.print("  Pattern: ");
      Serial.print(fg_paddress);
      Serial.print(":");
      Serial.print(fg_pdata);
      Serial.print(" N : ");
      Serial.print(fg_nfail);
      Serial.print(" Skip : ");
      Serial.print(fg_skip);
    }
    else {
      // counter based generation
      Serial.print("  Frame range: [");
      Serial.print(fg_nmin);
      Serial.print(":");
      Serial.print(fg_nmax);
      Serial.print("]");
    }

    if (fg_fmode == FM_ARBITRATION){
      Serial.print(" @ Arb ");
    }
    else if (fg_fmode == FM_DATA) {
      Serial.print(" @ Data ");
    }

    Serial.println();

  }

  if (fg_time != 0) {
    Serial.print("  Timed at : ");
    Serial.print(fg_time);
    Serial.println(" us");
  }

  if  ((fg_paddress == 0) && (fg_nmin == 0) && (fg_time == 0)) {
    Serial.print("  None");
  }

  Serial.println();

}

void print_fg_mode(){

  if (fg_fmode == FM_ARBITRATION){
    Serial.print("FG_Arb ");
  }
  else {
    Serial.print("FG_Data ");
  }

}

void print_fgstatus(){
  
  // the total number of each generated failures
  String status = "Total failures so far N/P/T: ";
  status += String(fg_num_n) + "/" + String(fg_num_p) + "/" + String(fg_num_t);

  Serial.println(status);

}