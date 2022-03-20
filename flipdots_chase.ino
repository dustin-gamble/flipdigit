
byte frameHeader[] = {0x80, 0x89, 0xFF};
byte frameClosure[] = {0x8F};

int send_buffer[]={ 0x80, 0x83, 0xFF, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00};

int digit;

void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);



  Serial1.write(0x80);
  Serial1.write(0x83);
  Serial1.write(0xFF);

  for(int i; i <28; i++){
  Serial1.write(0x00);
  //Serial.println(i);
  delay(1);
  }

  Serial1.write(0x8F);
  Serial1.write(0x00);
  Serial1.write(0x00);


}

void loop() {

  

//digit = 0;

//for(int sendx = 0; sendx < 11; sendx++){

for(int y = 0; y<28; y++){
  Serial1.write(0x80);
  Serial1.write(0x83);
  Serial1.write(0xFF);

    for(int i=0; i <28; i++){
      if(i == y){
       Serial1.write(0x01);
      }else{
        Serial1.write(0x00);
      }
      
      //Serial.print("-");
    }

  Serial1.write(0x8F);
  Serial1.write(0x00);
  Serial1.write(0x00);
  Serial.println(y);
delay(100);
}
  //Serial.println(sendx);




//}

}



void hexprint(int input){

//Serial.print("Recieving: ");
//Serial.println(input);
String inputstring = String(input);
if(inputstring == ""){Serial1.write(0x00);}
if(inputstring == "-"){Serial1.write(0x01);}
if(inputstring == "0"){Serial1.write(0x7E);}
if(inputstring == "1"){Serial1.write(0x30);}
if(inputstring == "2"){Serial1.write(0x6D);}
if(inputstring == "3"){Serial1.write(0x79);}
if(inputstring == "4"){Serial1.write(0x33);}
if(inputstring == "5"){Serial1.write(0x5B);}
if(inputstring == "6"){Serial1.write(0x5F);}
if(inputstring == "7"){Serial1.write(0x70);}
if(inputstring == "8"){Serial1.write(0x7F);}
if(inputstring == "9"){Serial1.write(0x73);}
if(inputstring == "10"){Serial1.write(0x01);}
  
}
