# ClawBotProject
Documentation of my design project for ENGR 120

I worked in a group of 3 people over the spring semester of 2022 on this project. The other two team members were responsible for 
documenting various stages of the project, while I took responsiblity for designing, building and coding the bot. We 
got an A+ grade for our robot!

DESIGN GOAL

The function of the robot is to find a blinking infrared target and place a ping pong ball on top of it. The search area will be 2.29 m by 2.29 m with 5cm walls
enclosing. The target will be no less than 15cm from all walls. The design should minimize wheel dragging, signal
completion of the job with a sound, minimize the time required to complete the task, and minimize the associated cost. 

BOT DESIGN

The final bot design uses an arm with an extendable outer claw to hold and drop the ping pong ball. It features a two motor gear linked drive mechanism, an IR sensor
fordetecting the target and Ultrasonic sensor for measuring distances. It also has a bumper to detect walls at an angle.

The rest of this README will be brief desccriptions of the files in this repository.

CONCEPT SKETCH

A hand drawn conecpt sketch made at the beginning of the design process. Surprisingly, the rack claw extension mechanism as well as the scanning algorithm
both were used in the final design. Using a rack gear mechanism reduced the number of extra parts we needed for a claw to hold the pingpong ball

EARLY PROTOTYPE

This picture shows off one of the earlier wall detection designs that we phased out in favour of the bumper mechanism. The extendable wings increased
the range of angles at which the ultrasonic sensor could detect the wall. I thought that the wings would bounce sound waves back to the wall and then back to 
the sensor, and they did! It still didnt work for near parallel hits though, so the bumper system had to be used instead.

FINAL TEST

FSM

A finite state machine written in pseudocode to illustrate the FSM run by the robots searching algorithm. The pseudocode is fairly vaugue due to the number
of states and operations I had to describe

MP4

A video of the robot executing the finaltest code. The robot scans until it detects the 10 hz IR signal, then it jitters left until it loses it, incrementing a counter
each jitter. It then jitters back right for half the count and approaches the target while the IR signal is still detected. Once the ultrasonic sensor
gets close to the target, one more scanning sequence is completed to ensure proper orientation, and the placement sequence is executed. Feel free to download and 
watch the video!

FINALTEST.C

Well commented code for the finaltest. Have a look if you want!

MOTIONTEST FSM

An earlier FSM that describes the states for the motion test

TEST - 

Three videos displaying earlier tests of the robot. These where to make sure our base design worked before moving on to using sensors to automate the robot
