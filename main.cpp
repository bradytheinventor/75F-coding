#include <iostream>
#include <algorithm>
#include <string>
#include <ctime>

#include "device.h"
using namespace std;

//assumptions:
//	*	master-slave device relationship, with one-way data flow and two-way transmission; however, the code is written so that
//			any device could be a master or a slave, but not both at the same time
//
//	*	both devices have some way of obtaining a unique address before beginning communication
//			this is possible to code; on power up each device listens for other devices transmitting and picks the lowest
//			available address
//
//	*	the communication channel introduces no error into the signal
//			this assignment is about writing a communication protocol, not an error-free encoding method
//
//	*	the strings I use when passing messages back and forth could be easily replaced with bit sequences in a real implementation
//			I chose to use strings to make it easier to see what happens


//declare variable to store current transmission (simulating a radio connection between the two devices)
//message structure:
//	 ssss				 RRRR					 hh			 MMMMMMMM
//	|----|				|----|					|--|		|--------|
//	sender address		receiver address		state		message
//
//	"hh" is the handshake state (e.g. initial message = "00", callback = "01", final response = "10")
//	messages sent to address "0000" can be heard by all devices, if they choose to do so
string transmission = "ssssRRRRhhMMMMMMMM";

int main() {
	cout << "Use CONTROL + C to exit the program." << endl << endl;
	
	//simulate master power on
	Device master("0001");
	cout << "master power on, using address " << master.getAddress() << endl;
	
	//simulate sensor power on and give sensor a piece of arbitrary data
	Device sensor("0010");
	sensor.setData("11010010");
	cout << "sensor power on, using address " << sensor.getAddress() << endl;

	//enter main loop to simulate a continuous process
	while(1) {
		
		//sensor initiates transmission
		if(!sensor.isPaired() && !sensor.isPairing()) {
			//if the sensor is not paired, send a pairing message to all devices every 1 second
			if(time(0) - sensor.getLastMessageTime() >= 1) {
				transmission = sensor.send("----pair", "0000", "00");

				cout << transmission << endl;
			}

		} else if(sensor.isPairing()) {
			//if the sensor is in the process of pairing, finish handshake
			transmission = sensor.send("----pair", sensor.getPairedAddress(), "10");
			sensor.resetPairing();

			cout << transmission << endl << endl;
		
		} else if(sensor.isPaired()) {
			//otherwise, if the sensor is paired, send a piece of arbitrary data every 10 seconds
			if(time(0) - sensor.getLastMessageTime() >= 10) {
				transmission = sensor.send(sensor.getData(), sensor.getPairedAddress(), "00");

				cout << transmission << endl;
			}

			//if the master has correctly received the sent data, send response
			if(sensor.hasHeardCallBack()) {
				transmission = sensor.send(sensor.getData(), sensor.getPairedAddress(), "10");
				sensor.resetCallBack();

				cout << transmission << endl << endl;
			}
		}

		//once a transmission is sent by the sensor, assume the master receives it instantaneously
		master.receive(transmission);
		
		//master receives transmission and reciprocates
		if(master.isPairing()) {
			//if the master has discovered a new sensor to pair with, try to initiate handshake every 1 second
			if(time(0) - master.getLastMessageTime() >= 1) {
				transmission = master.send("----pair", master.getPairedAddress(), "01");

				cout << transmission << endl;
			}

		} else if(master.isPaired() && master.hasNewData()) {
			//if the master is already paired and has received a new data element, send callback to sensor alerting
			//	that the information has been received
			transmission = master.send(master.getData(), master.getPairedAddress(), "01");
			master.resetNewData();

			cout << transmission << endl;
		}

		//once a transmission is sent by the master, assume the sensor receives it instantaneously
		sensor.receive(transmission);
	}

	return 0;
}