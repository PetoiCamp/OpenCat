char* p;

bool readS() {
  if (Serial.available() > 0) {
    char stringHeader[2];
    Serial.println("_________");
    {
      String a;
      a = Serial.readStringUntil('\n');
      Serial.println(a);
      Serial.println(a.length());
      strncpy(stringHeader, a.c_str(), 2);
      p = (char*)a.c_str();
      Serial.println(p);
      p[2] = '@';
      p[3] = '%';
    }
    Serial.println(p);
    strncpy(p, stringHeader, 2);
    Serial.println(p);
    return true;

  }
  return false;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (readS()) {
    Serial.println(p);

    Serial.println(p);
    Serial.println(p + 1);
    Serial.println(p + 2);
  }
}
