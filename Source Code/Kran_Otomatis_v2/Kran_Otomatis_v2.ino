#include <VarSpeedServo.h> 
VarSpeedServo myservo;

/*  Servo's library from
 * https://github.com/netlabtoolkit/VarSpeedServo
 * 
 * 
 * This program writen by Arif
 * 12 May 2020 Bandung, Indonesia.
 * https://ini.space/asyarif
 * 
 */

/* pin definitions */
#define BAUDRATE     115200
#define ECHO_PIN     12
#define TRIG_PIN     11
#define LED_PIN      9
#define POT_DISTANCE_PIN  A0  //  Adjust Distance         | TOP POT
#define POT_DELAY_PIN     A1  //  Adjust Delay            | MIDDLE POT
#define POT_SERVO_PIN     A2  //  Adjust SERVO Rotation   | BOTTOM POT
#define BUTTON_PIN   2
#define SERVO_PIN    3

#define BUTTON_DEBOUNCE_DELAY   50
#define SERVO_DEFAULT_POSITION  0
#define SERVO_SPEED             180 // 1 - 255 
//#define SERVO_MAX_ROTATION    150

bool  isDetected            = false;
bool  lastState             = true;
int   ledState              = LOW;
int   lastSteadyState       = LOW;
int   lastFlickerableState  = LOW;
int   currentState;

unsigned long lastDebounceTime, prevMillis = 0;
unsigned long next_time, NOW_MILLIS;
unsigned long distance;

#define INTERVAL    30

/* Timeout for distance sensing rather than 1 second */
#define MAX_ECHO    30000

/* Scale factor round trip micro seconds per cm */
#define SCALE_CM    58


void doBlink( int repeats, int time ){
  for (int i = 0; i < repeats; i++){
    digitalWrite(LED_PIN, HIGH);
    delay(time);
    digitalWrite(LED_PIN, LOW);
    delay(time);
  }
}

void setup(){
  
  next_time = INTERVAL;
  Serial.begin( BAUDRATE );

  pinMode( ECHO_PIN, INPUT );
  pinMode( BUTTON_PIN, INPUT_PULLUP );
  pinMode( POT_DISTANCE_PIN, INPUT );
  pinMode( POT_DELAY_PIN, INPUT );
  pinMode( POT_SERVO_PIN, INPUT );
  pinMode( LED_PIN, OUTPUT);
  pinMode( TRIG_PIN, OUTPUT );
  digitalWrite( TRIG_PIN, LOW ); // set trig pin LOW here NOT later
  doBlink(5, 100); // blinking for start indication
  myservo.write(SERVO_DEFAULT_POSITION);
  myservo.attach(SERVO_PIN);

}


void loop(){
/* check if to run this time */
    NOW_MILLIS = millis( );
    if( NOW_MILLIS >= next_time ){
      digitalWrite( TRIG_PIN, HIGH );
      delayMicroseconds( 10 );
      digitalWrite( TRIG_PIN, LOW );
      /* echo time in microseconds
            Maximum  MAX_ECHO
            Minimum  0      (NO ECHO)
    
        Timeout for measurements set by MAX_ECHO define (constant)
      */
      distance = pulseIn( ECHO_PIN, HIGH, MAX_ECHO );
      /* Calculate distance only for VALID readings 0 is no echo or timeout */
      if( distance > 0 ){
        distance /= SCALE_CM;
        Serial.println( int( distance ) );    // Output distance in whole cm
        next_time = NOW_MILLIS + INTERVAL;   // save next time to run this part of loop
      }
    }

    /* POT SERVO READING & SET */
    int valPotDistance = analogRead( POT_DISTANCE_PIN );
    int resultDistance = map( valPotDistance, 0, 1023, 0, 50 );

    /* POT SERVO ROTAION & SET */
    int valPotServo = analogRead( POT_SERVO_PIN );
    int SERVO_MAX_ROTATION = map( valPotServo, 0, 1023, 0, 180 ); 

    /* POT DELAY READING & SET */
    int valPotDelay = analogRead( POT_DELAY_PIN );
    int DELAY_READING = map( valPotDelay, 0, 1023, 0, 1000 );
    
   
    if( distance < resultDistance ){
      isDetected = true;
//      digitalWrite( LED_PIN, HIGH );
    }
  
     if( isDetected != lastState ){
        prevMillis = NOW_MILLIS;
        lastState = isDetected;
        digitalWrite(LED_PIN, LOW);
        myservo.write(SERVO_DEFAULT_POSITION ,SERVO_SPEED, true);
     }
     
     if( isDetected ){
        // IN
        if( NOW_MILLIS - prevMillis > DELAY_READING ) {
          prevMillis = NOW_MILLIS;
          Serial.println("Nyala");
          isDetected = !isDetected;
          ledState = !ledState;
          digitalWrite(LED_PIN, HIGH);
          myservo.write(SERVO_MAX_ROTATION ,SERVO_SPEED, false);
        } 
     }


      /* BUTTON TOGGLE */
      currentState = digitalRead(BUTTON_PIN);
      if ( currentState != lastFlickerableState ) {
        lastDebounceTime = NOW_MILLIS;
        lastFlickerableState = currentState;
      }

      /* TOGGLE SERVO WITH BUTTON */
      if (( NOW_MILLIS - lastDebounceTime ) > BUTTON_DEBOUNCE_DELAY ) {
        if( lastSteadyState == HIGH && currentState == LOW ){
          Serial.println( "The button is pressed" );
          ledState = !ledState;
          if( ledState == LOW ){
            myservo.write( SERVO_MAX_ROTATION ,SERVO_SPEED,false );
          } else {
            myservo.write( SERVO_DEFAULT_POSITION ,SERVO_SPEED,false );  
          }
          digitalWrite( LED_PIN, ledState );
        }else if( lastSteadyState == LOW && currentState == HIGH ){ // move to here if you want after realese 
          Serial.println( "The button is released" );
        }
        lastSteadyState = currentState;
      }
  
}
