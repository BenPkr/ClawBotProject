#pragma config(Sensor, in3,    Infra,          sensorReflection)
#pragma config(Sensor, dgtl1,  clawswitch,     sensorTouch)
#pragma config(Sensor, dgtl2,  armswitch,      sensorTouch)
#pragma config(Sensor, dgtl3,  sonar,          sensorSONAR_cm)
#pragma config(Sensor, dgtl5,  ledred,         sensorDigitalOut)
#pragma config(Sensor, dgtl6,  button1,        sensorTouch)  //left button
#pragma config(Sensor, dgtl7,  button2,        sensorTouch) //right button
#pragma config(Sensor, dgtl8,  ledgreen,       sensorDigitalOut)
#pragma config(Motor,  port2,           arm,           tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           claw,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           motor1,        tmotorVex393_MC29, openLoop, reversed, driveLeft)
#pragma config(Motor,  port5,           motor2,        tmotorVex393_MC29, openLoop, driveRight)


bool button1_pushed; //flag to store button1 input
bool button2_pushed; //flag to store button2 input
bool IR; //flag to store IR readings
const int THRESH = 3965;	//threshold to assign HI or LO IR signal
const int click = 30;

//function to update button press variables
void monitorInput()
{
  if(SensorValue(button1) && !button1_pushed)
  {
    button1_pushed = true;
  }

  if(SensorValue(button2) && !button2_pushed)
  {
    button2_pushed = true;
  }
}

//function to update IR signal value
void monitorIR()
{
	if(SensorValue(Infra)<=THRESH){
		wait1Msec(50);
		if(SensorValue(Infra)>THRESH){
			wait1Msec(50);
				if(SensorValue(Infra)<=THRESH){
					//wait1Msec(50);
					//if(SensorValue(Infra)>THRESH){
						IR = true;
					//}else{IR = false;}
				}else{IR = false;}
			}else{IR = false;}
		}else if(SensorValue(Infra)>THRESH){
						wait1Msec(50);
						if(SensorValue(Infra)<=THRESH){
							wait1Msec(50);
								if(SensorValue(Infra)>THRESH){
									//wait1Msec(50);
									//if(SensorValue(Infra)<=THRESH){
										IR = true;
									//}else{IR = false;}
								}else{IR = false;}
							}else{IR = false;}
						}
}

void turnR(){
	motor[motor1] = 30;
	motor[motor2] = -31;
}

void turnL(){
	motor[motor1] = -29;
	motor[motor2] = 32;
}

void forward(){
	motor[motor1] = 24;
	motor[motor2] = 34;
}

void back(){
	motor[motor1] = -20;
	motor[motor2] = -30;
}

void halt(){
	motor[motor1] = 0;
	motor[motor2] = 0;
}

void armdown(){
	motor[arm] = 20;
}

void armup(){
	motor[arm] = -50;
}
void armstop(){
	motor[arm] = 0;
}

void clawopen(){
	motor[claw] = 40;
}

void clawclose(){
	motor[claw] =-40;
}

void clawstop(){
	motor[claw] =0;
}




task main()
{
		//creating a type of enumerated data called state
		enum state {
		START = 0,
		LOCATE,// debugged
		APPROACH,
		CORRECT,//debugged
		AIM1,
		AIM2,
		ARM,
		CLAW,
		ROTATEAFTERPLACE,
		//POINTTOWALL1,
		//POINTTOWALL2,
		BACK, //
		ROTATECLEAR,// to do
		MOVEFORWARD,//
		MOVETOWALL,//
		FINALTURN,
		DONE
	};

	//initializing button variables to zero
	button1_pushed = button2_pushed = false;

	//an instance of our state data type
	state S = START;

	SensorValue(ledgreen) = 0;
	int n = 0; //a counter to exit LOCATE state
	int k = 0; //a counter to exit CORRECT state
	int j = 0; //a counter for the AIM1 and AIM2 sequence
	int f = 0; //a counter to exit the MOVEFORWARD state
	//int t = 0; //a holder to compare sonar values when trying to point toward a wall
	int z = 0; //a counter to exit the APPROACH sequence after IR being false for 3 consecutive loops and to exit the CORRECT looop
	bool INPLACE = false; // a condition that tells the AIM sequence whether to exit to ARM sequence or back to APPROACH

	while(true)
	{
		// every while loop, update the inputs
		monitorInput();
		monitorIR();

		// Switch the states based off of exercise1_state, sends the loop to execute code at indicated state
		switch(S) {

			// at the start, we either BACK up if we are too close to an object, or we LOCATE the target

			case START:
				if(IR == true){
					turnR();
					wait10Msec(50);
					halt();
					S = CORRECT;
					break;
				}else if(button1_pushed || button2_pushed||SensorValue(sonar)<10){
					S = BACK;
					break;
				}
				clawclose();
				wait10Msec(50);
				clawstop();
				S = LOCATE;

			break;
			//////////////////////////////////////////////////////////////////////////
			//each break send us back to the start of the while loop

			case LOCATE:

				turnL();
				n++;
				if(IR == true){
					halt();
					n = 0;
					S = AIM1;
				}else if(n>800){
					halt();
					n = 0;
					S = MOVEFORWARD;
					INPLACE = false;
				}else if(button1_pushed || button2_pushed){
					halt();
					S = BACK;
					INPLACE = false;
				}

			break;
			//////////////////////////////////////////////////////////////////////////
			// this repositions the robot if LOCATE fails
			case MOVEFORWARD:



				if(SensorValue(sonar)<=11){
					halt();
					S = START;
					break;
				}else if(button1_pushed || button2_pushed){
					S = BACK;
					break;
				}else if(f>3){
					f = 0;
					S = START;
					break;
				}else{
				forward();
				wait10Msec(100);
				halt();
				f++;
				}

			break;
			//////////////////////////////////////////////////////////////////////////
			//Back away from an obstacle and start ROTATECLEAR sequence
			case BACK:

				back();
				wait10Msec(30);
				halt();
				button1_pushed = button2_pushed = false;
				S = START;
			break;



			//////////////////////////////////////////////////////////////////////////
			// AIM sequence scans through whole range of angles that register on IR, then goes back to half that range
			//AIM 1 scan through whole range, turning left and incrementing a counter

			case AIM1:


				if(IR == false){
					z++;
					j++;
					if(z>2){//if three consecutive false readings, we know that we have scanned the whole range
						if(z == j){// this means the whole range was IR false and our LOCATE's transition was incorrect
						S = LOCATE;
						j = z = 0;
						INPLACE = false;
						break;
						}else{//otherwise, our scan has worked and we will go to AIM 2
						j = 3+((j-3)/2); // the number of clicks to move back to the start of the IR angle range, then move halfway through the range
						z = 0;
						S = AIM2;
						break;
						}
					}
				break;
				}else if(j>250){//The scan has gone on too long, we should exit the state and re run LOCATE
			 		j = 0;
					z = 0;
					INPLACE = false;
			 		S = LOCATE;
			 		break;
				}else{//In this case, We are still inside the IR range, so reset any z to zero and increment j while continuing to scan
				turnL();
				wait1Msec(click);
				halt();
				j++;
				z = 0;
				}

			break;
			//////////////////////////////////////////////////////////////////////////
			//scan back through half the range turning right

			case AIM2:

				if(j<=0){
					if(INPLACE == true){//INPLACE means we are scanning right before placing the object, so we go to ARM sequence
					j = 0;
					S = ARM;
					INPLACE = false;
					break;
					}else{//On scans before the final scan, we continue to the APPROACH sequence
					j=0;
					S = APPROACH;
					break;
					}
				}else{
				turnR();//Continue turning right
				wait1Msec(click);
				halt();
				j--;
				}

			break;
			//////////////////////////////////////////////////////////////////////////
			//robot approaches target after orienting correctly,
			//CORRECT if we lose the IR signal or ARM if SONAR detects target, or BACK if we hit an object

			case APPROACH:

				if(IR == false){
					if(z>1){//2 consecutive low IR readings, we have lost the signal
						halt();
						z=0; // stop and swing right to prep for Scan in AIM
						turnR();
						wait10Msec(100);
						halt();
						S = CORRECT; // go to correct to prepare AIM
						break;
					}else{
						z++; // if this is the first low IR reading, we just increment z
						break;
					}
				}else if(button1_pushed || button2_pushed){
					halt();
					z=0;
					S = BACK;
				}else if(SensorValue(sonar)<=30){
					halt();
					turnR();
					wait10Msec(50);
					halt();
					z=0;
					INPLACE = true;
					S = CORRECT;
				}else{
				forward();
				z=0;
				break;
				}

			break;

			//////////////////////////////////////////////////////////////////////////
			//Start of correction sequence when approaching robot
			//Turns left until IR detected,then initiates AIM sequence

			case CORRECT:

				if(IR == true){
					z++;
					if(z>1){//to make sure we have actually detected the IR, we will require two consecutive true readings
						k=0;
						z = 0;
						turnR();
						wait1Msec(click);//turn back to the beginning of the IR window
						halt();
						turnR();
						wait1Msec(click);
						halt();
						S = AIM1;//start AIM1 sequence
						break;
					}else{
						turnL();
						wait1Msec(click);
						halt();
						k++;
					}
				}else if(k>150){//if we have been correcting for too long, just restart the locate sequence
					k=0;
					S = LOCATE;
				}else{
				z = 0;
				turnL();
				wait1Msec(click);
				halt();
				k++;
				}

			break;

			//////////////////////////////////////////////////////////////////////////
			//puts arm down

			case ARM:

				while(SensorValue(sonar)>11 && !button1_pushed && !button2_pushed){
					forward();
				}
				halt();
				while(SensorValue(armswitch)!=1){
					armdown();
				}
				armstop();
				S = CLAW;

			break;
			//////////////////////////////////////////////////////////////////////////
			//releases ball then raises ARM

			case CLAW:

				while(SensorValue(clawswitch)!=1){
					clawopen();
				}
				clawstop();
				armup();
				wait10Msec(80);
				armstop();
				back();
				wait1Msec(80);
				halt();

				S = ROTATEAFTERPLACE;

			break;
			//////////////////////////////////////////////////////////////////////////
			//rotates away from the target once the ball has been placed

			case ROTATEAFTERPLACE:

				turnL();
				if(SensorValue(sonar)>20 || SensorValue(sonar)== -1){
					wait10Msec(200);
					halt();
					S = MOVETOWALL;
				}

			break;
			//////////////////////////////////////////////////////////////////////////
			//moves the bot forward until it either detects or touches a wall
			case MOVETOWALL:


				if(SensorValue(sonar)<=10|| button1_pushed || button2_pushed){
					halt();
					S = FINALTURN;
					break;
				}
				forward();

			break;
			//////////////////////////////////////////////////////////////////////////

			case FINALTURN:
				if(button1_pushed || button2_pushed){// If the button triggered this state, back away from the wall
				back();
				wait10Msec(40);
				}
				if(button1_pushed){ //Get the robot to face the wall
					while(SensorValue(sonar)>14){
						turnL();
					}
					halt();
					S = DONE;
				}else if(button2_pushed){
					while(SensorValue(sonar)>14){
						turnR();
					}
					halt();
					S = DONE;
				}else{
					S = DONE;
				}
				//();	// turn to be parallel to the wall
				///wait10Msec(150);
				//halt();

			break;
			//////////////////////////////////////////////////////////////////////////

			case DONE:

				SensorValue(ledgreen) = 1;

			break;
			//////////////////////////////////////////////////////////////////////////

			default:

			}
		}
	}
