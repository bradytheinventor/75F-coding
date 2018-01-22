#include "device.h"
using namespace std;

//constructor, stores this device's address and initializes last message timer
Device::Device(string address) {
	this->address = address;
	lastMessageTime = time(0);
}

//send a transmission to a particular address
string Device::send(string msg, string tgt, string step) {
	//reset last message timer
	lastMessageTime = time(0);

	//build transmission string according to protocol
	string transmission = tgt + address + step + msg;
	return transmission;
}

//receive a transmission from a particular address, or "0000" if the transmission is meant for all devices
void Device::receive(string transmission) {
	//parse transmission string
	string tgt = transmission.substr(0, 4);
	string src = transmission.substr(4, 4);
	string step = transmission.substr(8, 2);
	string msg = transmission.substr(10, 8);
	
	//ignore general messages if this device has been paired
	if(isPaired() && tgt == "0000") {
		return;
	}

	//ignore messages that are meant for other devices only
	if(tgt != address && tgt != "0000") {
		return;
	}

	//ignore messages sent from this device
	if(src == address) {
		return;
	}

	//ignore blank messages
	if(msg == "") {
		return;
	}

	//parse message and decide what to do with it
	if(msg == "----pair") {
		//if the message is "----pair", the transmitting device is working to pair with this device
		//	this could be replaced with a binary code, I chose to use alphanumeric characters for easier reading
		pairing = true;

		if(step == "00") {
			//if the step is 00, the transmitting device is initiating
			//store transmitting device address and callback
			pairedAddress = src;

		} else if(step == "01") {
			//if the step is 01, the transmitting device has stored this device's address and wants a response
			//store transmitting device address and respond
			pairedAddress = src;
			paired = true;

		} else if(step == "10") {
			//if the step is 10, the transmitting device has stored this device's address and is ready to begin
			pairing = false;
			paired = true;
		}

	} else {
		//otherwise, the transmitting device is sending useful data
		if(step == "00") {
			//if the step is 00, the transmitting device has sent useful data
			//store data and callback
			data = msg;
			newData = true;

		} else if(msg == data && step == "01") {
			//if the step is 01, the transmitting device has sent the data it received
			//if the message is a copy of this device's data byte, the callback was successful
			callBack = true;

		} else if(step == "10") {
			//if the step is 10, the transmitting device has received the callback
			//there isn't anything in particular we need to do with this
		}
	}

	return;
}

//get whether this device has paired. A device has paired once it has completed a handshake with the other device
bool Device::isPaired() {
	return paired;
}

//get whether this device is pairing. A device is pairing once it receives a pairing message ("----pair")
bool Device::isPairing() {
	return pairing;
}

//get whether this device has heard a callback message
bool Device::hasHeardCallBack() {
	return callBack;
}

//get whether this device has received a new data transmission
bool Device::hasNewData() {
	return newData;
}

//setters to reset pairing, callBack, and newData flags
void Device::resetPairing() {
	pairing = false;
}

void Device::resetCallBack() {
	callBack = false;
}

void Device::resetNewData() {
	newData = false;
}

void Device::setData(string d) {
	data = d;
}

//getters for last message time, device address, paired device address, and current data "byte"
time_t Device::getLastMessageTime() {
	return lastMessageTime;
}

string Device::getAddress() {
	return address;
}

string Device::getPairedAddress() {
	return pairedAddress;
}

string Device::getData() {
	return data;
}