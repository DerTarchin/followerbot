// SETUP
// Call Pixy and Servo
#include <SPI.h>  
#include <Pixy.h>
Pixy pixy;
#include <Servo.h> 

//Initialize Variables and objects
Servo motor; 
Servo servo; 
int width = 319;
int height = 199;
int speed = 90;
int dir = 90;
int sizeMIN = 8000;
int sizeMAX = 11000;
int sizeSetPin = 3;
int stopPin = 2;
boolean STOP = true;
int lastSize;
int refresh = 5;
int refreshSpeed = 50;
int sizes[50];

void setup() 
{
  delay(1000);
  Serial.begin(9600);
  Serial.println("Starting program...\n");
  Serial.print("Initializing Pixy...\t\t");
  pixy.init();
  Serial.print("Successful\n");
  Serial.print("Mounting Servos...\t\t");
  motor.attach(9);
  servo.attach(10);
  Serial.print("Succesful\n");
  Serial.print("Calibrating ESC...\t\t");
  motor.write(speed);
  delay(2000);
  Serial.print("Succesful\n");
  Serial.print("Setting Servo... \t\t");
  servo.write(90);
  Serial.print("Successful\n");
  Serial.println("\n\n Standing By...");
  pinMode(sizeSetPin, INPUT);
  pinMode(stopPin, INPUT);
  resetArray();
}

void loop() 
{
  if (digitalRead(sizeSetPin) == LOW) {
    sizeMIN = lastSize-1500;
    sizeMAX = lastSize+1500;
    Serial.print("New ideal size: ");
    Serial.print(sizeMIN);
    Serial.print(" - ");
    Serial.print(sizeMAX);
    Serial.println();
    delay(1000);
  }
  if (digitalRead(sizeSetPin) == LOW) {
    Serial.println("size");
    delay(1000);
  }
  if (digitalRead(stopPin) == LOW) {
    if (!STOP) {
      STOP = true;
      speed = 90;
      Serial.println("Program Halted.");
    }
    else {
      STOP = false;
      Serial.println("Program Resumed.");
      delay(2000);
      motor.write(102);
      delay(2000);
    }
    delay(1000);
  }

  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32]; 
  int size = 500;
  int largest;
  blocks = pixy.getBlocks();
  if (blocks)
  {
    i++;
    if (i%refresh==0)
    {
      for (j=0; j<blocks; j++)
      {
        int cur_size = pixy.blocks[j].width * pixy.blocks[j].height;
        if (cur_size > size) {
          size = cur_size;
          largest = j;
        }
      }
      if (size> 500) {
        lastSize = size;
        //       Serial.print("Size (");
        //       Serial.print(size);
        //       Serial.print(")\t\t");

        if (i%refreshSpeed==0) {
          int total=0; 
          int sum=0; 
          int aveSize;

          for (int i=0; i<50; i++) {
            if (sizes[i] != 0) {
              sum = sum + sizes[i];
              sum = sum/2;
            }
          }
          aveSize = sum;

          //Change speed
          Serial.print("Speed (");
          Serial.print(speed);
          Serial.print(")");
          Serial.println();
          if (!STOP) {
            Serial.print("* ");
            Serial.print(aveSize);
            Serial.print(" *");
           // checkSpeed(aveSize);
          } 
          speed = 107;
          resetArray();
        }

        //change direction
        //Serial.print("\tDir (");
        //Serial.print(dir);
        //Serial.print(")");
        checkDir(pixy.blocks[largest].x+(pixy.blocks[largest].width/2));

        Serial.println();

        for(int i=0; i<50; i++) {
          if (sizes[i] == 0) {
            sizes[i] = size;
            break;
          }
        }
      }
    }
    if (!STOP) {
      //write servo and motor
      motor.write(speed);
      servo.write(dir);
    }
    else if (STOP) {
      speed = 90;
      dir = 90;
      servo.write(dir);
      motor.write(speed);
    }
  }
  else {
    //motor.write(90); 
  }
}

void resetArray() {
  for (int i=0; i<50; i++) {
    sizes[i] = 0;
  }
}

void checkSpeed(int size) {
  if (size<=sizeMAX && size>=sizeMIN){
    Serial.print("\t-\t");
  }
  if (size<=sizeMIN) {
    increaseSpeed();
  }
  if (size>=sizeMAX) {
    reduceSpeed();
  }
}

void reduceSpeed() {
  Serial.print("\tv\t");
  if (speed > 90){
    speed-=10;
    if (speed <90){
      speed = 90;
    }
  }
  else if (speed>=40) {
    speed -=3;
  }
}

void increaseSpeed(){
  Serial.print("\t^\t");
  if (speed>=90 && speed <= 94) {
    motor.write(102);
    delay(2000);
    speed = 95;
  }
  else if (speed >= 115 && speed <= 120) { // MAX 140
    speed += 1;
  }
  else if (speed < 90) {
    speed = 90;
  }
  else if (speed < 115){ 
    speed +=2;
  }
}

void checkDir(int x) {
  if (x < 165) {
    //Serial.print("\t<\t");

    dir = map(x, 20, 165, 0, 90);
    dir = constrain(dir, 0, 90);
  }
  else if (x>235) {
    // Serial.print("\t>\t");
    dir = map(x, 235, 320, 90, 180);
    dir = constrain(dir, 90, 180);
  }
  else {
    // Serial.print("\t-\t");
    dir = 90; 
  }
}
