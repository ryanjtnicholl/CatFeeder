int motorAin1 = 2;
int motorAin2 = 3;
int instantfeedbtn = 13;
int timefeedbtn = 12;
int revfeedbtn = 11;
int scheduletog = 10;

void setup() {
  pinMode(motorAin1, OUTPUT);
  pinMode(motorAin2, OUTPUT);
  pinMode(instantfeedbtn, INPUT);
  pinMode(timefeedbtn, INPUT);
  pinMode(revfeedbtn, INPUT);
  pinMode(scheduletog, INPUT);
}

void loop() {

  if (digitalRead(instantfeedbtn) == HIGH)
  {
    digitalWrite(motorAin1, HIGH);
    digitalWrite(motorAin2, LOW); 
  }

  else if (digitalRead(timefeedbtn) == HIGH)
  {
    digitalWrite(motorAin1, HIGH);
    digitalWrite(motorAin2, LOW);
    delay(10000);
     
  }

  else if (digitalRead(revfeedbtn) == HIGH)
  {
    digitalWrite(motorAin1, LOW);
    digitalWrite(motorAin2, HIGH);     
  }
  
  else if (digitalRead(scheduletog) == HIGH) //toggle currently has same function as instant feed
  {
    digitalWrite(motorAin1, HIGH); 
    digitalWrite(motorAin2, LOW);     
  }

  else 
  {
    digitalWrite(motorAin1, LOW);
    digitalWrite(motorAin2, LOW);  
  }

}
