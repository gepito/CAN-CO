/*

Latency (no processing, constant pulse width generation): 6 us

Accuracy of  delayMicroseconds
set   measured [us]
 2     4
 4     6
 8    10
16    18


*/

const byte Pin_interrupt = 3;
const byte Pin_failgen = 7;
int nIT, nITprev = 0;
int nloop = 0;
int dt;

void pulse() {

  digitalWrite(Pin_failgen, HIGH);
  delayMicroseconds(dt);
  digitalWrite(Pin_failgen, LOW);

  nIT++;
}


void setup() {

  pinMode(Pin_failgen, OUTPUT);
  pinMode(Pin_interrupt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Pin_interrupt), pulse, RISING);

  digitalWrite(Pin_failgen, LOW);

  Serial.begin(115200);

  nloop = 0;
  nIT = 0;
  nITprev = 0;
  dt = 8;
  while (!Serial) {}
  Serial.println("nloop dt nIT");

}

void loop() {

  

  if (nIT > nITprev){

    Serial.print(nloop);
    Serial.print("  ");
    Serial.print(dt);
    Serial.print("  ");
    Serial.println(nIT);
    nITprev = nIT;
    dt++;

  }


  nloop++;
  delay(50);

}
