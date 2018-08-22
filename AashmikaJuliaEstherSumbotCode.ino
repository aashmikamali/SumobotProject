    //Aashmika Mali, Julia Chae, Esther Lee
    //Mr.Wong
    //2017-10-11
    //This program controls our sumobot through the control of motors, IR sensors and Ultrasonic sensors. The ultrasonic sensors were used to sense the distance of the opponents. Based on the distance
    //the motors were then programmed to go in the direction of the detected bot. The IR sensors were used to sense where the bot was located on the ring. That is, if the bot was on the white ring, the IR
    //sensors would send back a reading different from the black inner of the circle. Therefore it was programmed to go in the opposite direction when on the white ring.
    
    // Digital Pins to enable current
    #define D_MOTOR_LEFT 8
    #define D_MOTOR_RIGHT 9
    
    //PWM pins to enable speed
    #define D_LEFT_PWM 3
    #define D_RIGHT_PWM 5
    
    //Defines pin numbers for ultrasonic sensors
    #define trigPinFront 4// blue wire //blue ultrasonic
    #define echoPinFront 11 //white wire //blue ultrasonic
    #define trigPinBack 2// //red ultrasonic
    #define echoPinBack 10 //red ultrsonics

    //Defines input pins for ir
    #define IR_PIN_FRONT A3
    #define IR_PIN_BACK A5 
    #define IR_PIN_LEFT A2 
    #define IR_PIN_RIGHT A4

    //Speed definitions
    int speedRight = 255;
    int speedLeft = 255;

    //Defines the speed (when going forward)
    #define MAX_PWM_RIGHT 104 
    #define MAX_PWM_LEFT 104 
  
  
    //MainStateMachine states
    #define START 0
    #define SEEKING_OPPONENT_FRONT 1
    #define MAYBE_LOCATED_FRONT 2
    #define TARGET_LOCKED_ON_FRONT 3
    #define CHARGING_FRONT 4
    #define SEEKING_OPPONENT_BACK 5
    #define MAYBE_LOCATED_BACK 6
    #define TARGET_LOCKED_ON_BACK 7
    #define CHARGING_BACK 8
    #define SENSE_WHITE_FRONT 9
    #define SENSE_WHITE_BACK 10
    #define SENSE_WHITE_LEFT 11
    #define SENSE_WHITE_RIGHT 12
    
    //MainStateMachine Distance ranges
    #define START_TARGET_DISTANCE 55//40
    #define SEEKING_OPPONENT_TARGET_DISTANCE 42//32
    #define MAYBE_LOCATED_TARGET_DISTANCE 30//25
    #define TARGET_LOCKED_ON_DISTANCE 20//20
    #define END_CHARGING_TARGET_DISTANCE 30
    #define CHANGE_STATE_DELAY 100

    //Defines the number for the counter when changing state
    #define COUNTER_CHECK 2
    int carState = START;

    //Defines the main Digital output for the direction of the motors
    #define FORWARD HIGH
    #define BACKWARD LOW
    
    //Defines global longs for the readings from the ultrasonic sensors
    long durationFront;
    int distanceFront;
    long durationBack;
    int distanceBack;

    //Defines the counters for the conditions to change state
    int counterFront=0;
    int counterFront2=0;
    int counterBack=0;
    int counterBack2=0; 

    //Defines the global variables for the values given from the 4 different IR sensors
    int frontIR = 0;
    int backIR = 0;
    int leftIR = 0;
    int rightIR = 0;


    //Defines the battery voltage
    #define BATTERY_VOLTAGE A1
    
    //Defines the adjusted delay based on the battery voltage
    int batteryInput = 0;
    int adjustedDelay = 0;
    int adjustedSpeed = 0;
    int adjustedSpeedSpin = 0;

    //This sets up the output/input of all the pins being used
    void setup (void) 
    {
      //Serial.begin (9600);
      pinMode (D_MOTOR_LEFT, OUTPUT);
      pinMode (D_MOTOR_RIGHT, OUTPUT); 
    
      pinMode (D_LEFT_PWM, OUTPUT); 
      pinMode (D_RIGHT_PWM, OUTPUT); 
    
      pinMode(trigPinFront, OUTPUT);
      pinMode(echoPinFront, INPUT); 
      pinMode(trigPinBack, OUTPUT); 
      pinMode(echoPinBack, INPUT); 

      pinMode (IR_PIN_FRONT, INPUT);
      pinMode (IR_PIN_BACK, INPUT);
      pinMode (IR_PIN_LEFT, INPUT);
      pinMode (IR_PIN_RIGHT, INPUT);
      
    }

    //Changes the speed and delay based on the battery input voltage
    void determineSpeedAndDelay (void)
    {
      batteryInput = analogRead (BATTERY_VOLTAGE);
      Serial.print("Batter Input: ");
      Serial.println(batteryInput);
      adjustedSpeed = ((1000 - batteryInput) / 4 );
      adjustedSpeedSpin = ((1000 - batteryInput)/ 6);

      if (adjustedSpeed > 145) //the voltage on the batteries are too low, so the following is indication that it must be replaced.
      {
        adjustedSpeed = 145;
        rightTurn();
        delay (5000);
        stopMethod ();
        delay (1000);
        rightTurn ();
        delay (3000);
      }
    }
    
   //This function reads the analog input for all four IRs.
   void readIRValues ()
   {
     frontIR = analogRead (IR_PIN_FRONT);
     delay(50);
     backIR = analogRead (IR_PIN_BACK); 
     delay(50);
     leftIR = analogRead (IR_PIN_LEFT);
     delay(50);
     rightIR = analogRead (IR_PIN_RIGHT);
     delay(50);
   }

    //This is the MainStateMachine function. Here all the different states are located, and a change of state is dependent on the distance that the Ultrasonic sensor sends back, or the reading that the IR
    //sensors give. 
   void  mainStateMachine (void)
    {    
      analogWrite (D_LEFT_PWM, speedLeft);
      analogWrite (D_RIGHT_PWM, speedRight); 
      determineSpeedAndDelay();
      switch(carState)
      {
        //State of the car at the beginning of the match
        case START:
        {
          speedLeft = 50 + adjustedSpeedSpin;
          speedRight = 50 + adjustedSpeedSpin;
          leftTurn();
          if (distanceFront < START_TARGET_DISTANCE)
          {
            counterFront ++;
            if (counterFront == COUNTER_CHECK)
            {
              carState = SEEKING_OPPONENT_FRONT;
              rightTurn();
              delay (250);
              counterFront = 0;
            }
          }
          else
            counterFront =0;

          if (distanceBack <START_TARGET_DISTANCE)
          {
            counterBack ++;
            if (counterBack == COUNTER_CHECK)
             {
              carState = SEEKING_OPPONENT_BACK;
              rightTurn();
              delay (250);//200
              counterBack = 0;
             }
          }
          else
            counterBack = 0;                    
          break;
        }
        
        //State of the car looking for an opponent on the ring using the front sensor
        case SEEKING_OPPONENT_FRONT:
        {
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          backward();
          if (distanceFront < SEEKING_OPPONENT_TARGET_DISTANCE)
          {
            counterFront ++;
            if (counterFront ==COUNTER_CHECK)
            {
              carState = MAYBE_LOCATED_FRONT;
              delay (CHANGE_STATE_DELAY);
              counterFront =0; 
            }
          }
          else if (distanceFront > START_TARGET_DISTANCE)
          {
            counterFront2 ++;
            if (counterFront2 == COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterFront2 =0;
            }
          }
          else 
          {
            counterFront =0; 
            counterFront2 =0;
          }
          break;
        }
        
        //State of the car looking for an opponent on the ring using the back sensor 
        case SEEKING_OPPONENT_BACK:
        {
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          forward();
          if (distanceBack < SEEKING_OPPONENT_TARGET_DISTANCE)
          {
            counterBack ++;
            if (counterBack ==COUNTER_CHECK)
            {
              carState = MAYBE_LOCATED_BACK;
              delay (CHANGE_STATE_DELAY);
              counterBack = 0; 
            }
          }
          else if (distanceBack > START_TARGET_DISTANCE)
          {
            counterBack2 ++;
            if (counterBack2==COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterBack2 =0; 
            }
          }
          else
          {
            counterBack =0;
            counterBack2=0;
          }
          break;
        }
        
        //State of the car when the front sensor senses a potential opponent 
        case MAYBE_LOCATED_FRONT:
        {
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          backward();
          if (distanceFront < MAYBE_LOCATED_TARGET_DISTANCE)
          {
            counterFront ++;
            if (counterFront ==COUNTER_CHECK)
            {
              carState = TARGET_LOCKED_ON_FRONT;
              delay (CHANGE_STATE_DELAY);
              counterFront =0;
            }
          }
          else if (distanceFront > SEEKING_OPPONENT_TARGET_DISTANCE)
          {
            counterFront2++;
            if (counterFront2==COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterFront2=0;
            }
          }
          else
          {
            counterFront=0;
            counterFront2=0;
          }
          break;
        }
        
        //State of the car when the back sensor senses a potential opponent 
        case MAYBE_LOCATED_BACK:
        {
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          forward();
          if (distanceBack < MAYBE_LOCATED_TARGET_DISTANCE)
          {
            counterBack ++;
            if (counterBack = COUNTER_CHECK)
            {
              carState = TARGET_LOCKED_ON_BACK;
              delay (CHANGE_STATE_DELAY);
              counterBack =0;
            }
          }
          else if (distanceBack > SEEKING_OPPONENT_TARGET_DISTANCE)
          {
            counterBack2=0;
            if (counterBack2==COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterBack2=0;
            }
          }
          else
          {
            counterBack =0;
            counterBack2=0;
          }
          break;
        }
        
        //State of the car when the presence of the target is confirmed in the front of the car
        case TARGET_LOCKED_ON_FRONT:
        {
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          backward();
          if (distanceFront < TARGET_LOCKED_ON_DISTANCE)
          {
            counterFront ++;
            if (counterFront ==COUNTER_CHECK)
            {
              carState = CHARGING_FRONT;
              delay (CHANGE_STATE_DELAY);
              counterFront =0;
             }
          }
          else if (distanceFront > MAYBE_LOCATED_TARGET_DISTANCE)
          {
            counterFront2 ++;
            if (counterFront2 ==COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterFront2=0;
            }
          }
          else
          {
            counterFront =0;
            counterFront2=0;
          }
          break;
        }
        
        //State of the car when the presence of target is confirmed in the back of the car 
        case TARGET_LOCKED_ON_BACK:
        {
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          forward();
          if (distanceBack < TARGET_LOCKED_ON_DISTANCE)
          {
            counterBack ++;
            if (counterBack ==COUNTER_CHECK)
            {
              carState = CHARGING_BACK;
              delay (CHANGE_STATE_DELAY);
              counterBack=0;
            }
          }
          else if (distanceBack > MAYBE_LOCATED_TARGET_DISTANCE)
          {
            counterBack2++;
            if (counterBack2 ==COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterBack2 =0;
            }
          }
          else
          {
            counterBack=0;
            counterBack2=0;
          }
          break;
        }
        
        //State for charging at the opponent in front of the car
        case CHARGING_FRONT:
        {
          backward();
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          if (distanceFront > END_CHARGING_TARGET_DISTANCE && distanceFront < 250)
          {
            counterFront ++;
            if (counterFront ==COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterFront =0;
            }
          }
          else
            counterFront =0;
          break;
        }
        
        //State for charging at the opponent behind the car
        case CHARGING_BACK:
        {
          forward();
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          if (distanceBack > END_CHARGING_TARGET_DISTANCE && distanceFront < 250)
          {
            counterBack++;
            if (counterBack ==COUNTER_CHECK)
            {
              carState = START;
              delay (CHANGE_STATE_DELAY);
              counterBack =0;
            }
          }
          else
            counterBack =0;
          break;
        }
        
        //Determines what to do if the border is sensed through the back IR sensor 
        case SENSE_WHITE_FRONT: //Back sensor 
        {
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          backward();
          delay (1500);
          carState = START;
          break;
        }
        
        //Determines what to do if the border is sensed through the front IR sensor 
        case SENSE_WHITE_BACK: //Front sensor 
        {
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          forward();
          delay (1500);
          carState = START;
          break;
        }
        
        //Determines what to do if the border is sensed through the left IR sensor 
        case SENSE_WHITE_LEFT: //Left hand sensor 
        {
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          rightTurn ();
          if (leftIR > 750)
          {
          backward ();
          delay (1000);
          carState = START;
          }
          break;
        }        
        
        //Determines what to do if the border is sensed through the right IR sensor 
        case SENSE_WHITE_RIGHT: //Right hand sensor 
        {
          speedRight = MAX_PWM_RIGHT + adjustedSpeed;
          speedLeft = MAX_PWM_LEFT + adjustedSpeed;
          leftTurn (); 
          if (rightIR > 750)
          {
          backward ();
          delay (1000);
          carState = START;
          }
          break;
        }
      }
      
    }
    
    //This method is to send a signal to the motors to stop
    void stopMethod ()
    {
      analogWrite (D_LEFT_PWM, 0);
      analogWrite (D_RIGHT_PWM, 0); 
    }

    //This method is to send a signal to the motors to move forward
    void forward ()
    {
      digitalWrite (D_MOTOR_LEFT, FORWARD);
      digitalWrite (D_MOTOR_RIGHT, FORWARD); 
    }
  

    //This method is to send a signal to the motors to move backward
    void backward ()
    {
      digitalWrite (D_MOTOR_LEFT, BACKWARD);
      digitalWrite (D_MOTOR_RIGHT, BACKWARD); 
    }
  

    //This method is to send a signal to the motors to turn left
    void leftTurn ()
    {
      digitalWrite (D_MOTOR_LEFT, BACKWARD);
      digitalWrite (D_MOTOR_RIGHT, FORWARD); 
    }
    

    //This method is to send a signal to the motors to turn right
    void rightTurn ()
    {
      digitalWrite (D_MOTOR_LEFT, FORWARD);
      digitalWrite (D_MOTOR_RIGHT, BACKWARD); 
    }

    //This method finds the reading from the front ultrasonic sensor, and converts it into a distance that is used in the logic
    void testUltrasonicFront (void)
    {
      // Clears the trigPin
      digitalWrite(trigPinFront, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPinFront, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinFront, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      durationFront = pulseIn(echoPinFront, HIGH);
      // Calculating the distance
      distanceFront= durationFront*0.034/2;
      }

    //This method finds the reading from the back ultrasonic sensor, and converts it into a distance that is used in the logic
    void testUltrasonicBack (void)
    {
      // Clears the trigPin
      digitalWrite(trigPinBack, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPinBack, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinBack, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      durationBack = pulseIn(echoPinBack, HIGH);
      // Calculating the distance
      distanceBack= durationBack*0.034/2; //durationBack*0.034/2
    }
      

    //This is the main loop that calls all sensor readings, stateMachine, and logic if the IR detectes the white line. This method is constantly being looped while the sumobot is on.
    void loop (void)
    {
      readIRValues();
      if (rightIR < 800)
      {
        carState = SENSE_WHITE_RIGHT;
      }
      else if (leftIR <800)
      {
        carState = SENSE_WHITE_LEFT;
      }
      else if (frontIR <810)
      {
        carState = SENSE_WHITE_FRONT;
      }
      else if (backIR < 750)
      {
        carState = SENSE_WHITE_BACK;
      }
      
      testUltrasonicFront ();
      delay (20);
      testUltrasonicBack();
      mainStateMachine ();   
    }

