#include <iostream>
#include <algorithm>
#include <string>
#include <ctime>
using namespace std;

class Device {
	private:
		bool paired = false;
		bool pairing = false;
		bool newData = false;
		bool callBack = false;

		time_t lastMessageTime;
		string address = "";
		string pairedAddress = "";
		string data = "";

	public:
		Device(string address);

		string send(string msg, string tgt, string step);
		void receive(string transmission);

		bool isPaired();
		bool isPairing();

		bool hasHeardCallBack();
		bool hasNewData();

		void resetPairing();
		void resetCallBack();
		void resetNewData();

		void setData(string d);

		time_t getLastMessageTime();
		string getAddress();
		string getPairedAddress();
		string getData();
};