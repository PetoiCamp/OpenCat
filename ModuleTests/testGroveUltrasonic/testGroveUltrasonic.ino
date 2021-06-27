//
// For the Grove Ultrasonic Sensor provided with the Sensor Pack for Petoi Bittle.
//

// Pins
#define RANGER 6 // Ranger pin is used to trigger (OUT) and detect echo (IN)

// Physical constants
#define LONGEST_DISTANCE_CM (200l) // 200 cm = 2 meters
#define LONGEST_DISTANCE_M (LONGEST_DISTANCE_CM / 100l)
#define SPEED_OF_SOUND_MPS (343l) // meters/second
#define USEC_PER_SEC (1000000l)
#define MAX_ECHO_TIME_US ((2l * USEC_PER_SEC * LONGEST_DISTANCE_M) / SPEED_OF_SOUND_MPS)
#define SPEED_OF_SOUND_CMPS (SPEED_OF_SOUND_MPS * 100l)

long measure_distance_cm()
{
  // Clears the trigPin
  pinMode(RANGER, OUTPUT);
  digitalWrite(RANGER, LOW);
  delayMicroseconds(2);

  // Trigger the ranger
  digitalWrite(RANGER, HIGH);
  delayMicroseconds(5);
  digitalWrite(RANGER, LOW);

  // Set ranger pin to input - the same pin is used for trigger and echo
  pinMode(RANGER, INPUT);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(RANGER, HIGH, MAX_ECHO_TIME_US);

  // Calculating the distance (divide by two because sound is echoed)
  return (SPEED_OF_SOUND_CMPS * duration) / (2 * USEC_PER_SEC);
}

void setup()
{
  pinMode(RANGER, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200);   // Starts the serial communication
}

long counter = 0;
void loop()
{
  long distance = measure_distance_cm();

  Serial.print("[");
  Serial.print(counter++);
  Serial.print("]:\t");
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(100);
}
