
#include "RPApi.h"
#include <libraries/CRC/CRC16.h>

// Variables and objects
static char MSG_PIN = 10;
static char MSG_TILE = 27;
static bool frameSent = false;
enum command_t : uint8_t {TILE = 0, 
													PIN = 1,
													REGISTER = 2,
													GAME_START = 3,
													GAME_OVER = 4,
													GAME_INIT = 5,
													GAME_STATUS = 6};


namespace JungleSpeed {
	static void setup(void);
}

// Function declarations
static void updateGame(char* eventType, void* event);
static void handleTileMsg(Message_t* msg);
static void handlePinMsg(Message_t* msg);
static void sendPinPack();
static void sendMsgToTile();
static void lightsOff();
static void sendTilePack();
static void getNewAddr();
static void debug(int r, int g, int b);
static void initGame();

Application_t jungleSpeed =  {
	"JungleSpeed", 
	JungleSpeed::setup
};

// A packet for communication between Pins
struct packet_p {
	uint8_t startByte;
	uint8_t msgType;
	uint16_t pinAddr;
} pinPackOut, *pinPackIn;

// A packet for communication between Tile and a Pin
struct packet_t {
	// Tile header data
	uint8_t startByte;
	uint8_t msgType;
	uint8_t escape;
	uint8_t frame_nr;
	uint16_t address_to;
	uint16_t address_from;
	// Tile Package begins
	command_t command;
	uint16_t pinAddr;
	uint8_t color;
	uint8_t score;
	// Tile package ends
	uint16_t crc;
	uint8_t endByte;
} tilePackOut, *tilePackIn;

struct score_t {
	uint8_t prev;
	uint8_t now;
} score;

// Setup function, called only once in the beginning of the runtime of the program.
static void JungleSpeed::setup(void) {
	srand(millis());
	
  tilePackOut.msgType = MSG_TILE;
	tilePackOut.escape = 0x7f;
  tilePackOut.address_to = 0xFFFF;
	tilePackOut.command = REGISTER;
	tilePackOut.color = 0;
	tilePackOut.score = 10;
	tilePackOut.endByte = 0x7F;

	// Subscribe to a message of type packet_p
	// Each time a packet_p is received, the function handlePinMsg() is called.
	comm.subscribe(MSG_PIN, handlePinMsg);
	comm.subscribe(MSG_TILE, handleTileMsg);
	
	// Run the update function in each loop throughout the lifetime of the program
	eventManager.subscribeListener(EVENT_ACT, updateGame);

	// Create a timer, that calls sendPinPack() once after 500 milliseconds, 
	timerManager.createOneShot(500, sendPinPack);
//	timerManager.createPeriodic(5000, 7000, debug);

	score.now = 10;
}

static void updateGame(char* eventType, void* event) {
	
	// If the game is over, do nothing, otherwise, update the movement etc.
	switch (tilePackOut.command) {
		case GAME_OVER:
			break;

		default:
			movement.update();
			if(*movement.R() > 2.1 && !frameSent) {
				light.set(1, 1000, 1000, 1000);
				light.update();
				sendMsgToTile();
				frameSent = true;
				delay(1000);
			}
			else
				frameSent = false;
				light.set(1, 0, 0, 0);
				light.update();
			break;
	}
}

static void debug(int r, int g, int b) {
	light.setAll(r, g, b);
	light.update();
	comm.send((uint8_t*)&pinPackOut, sizeof(struct packet_p));
	delay(1000);
	light.setAll(0,0,0);
	light.update();
}

static void sendMsgToTile() {
	// Random number of frame, and calculate the CRC
  tilePackOut.frame_nr = rand() % 256;
  tilePackOut.crc = calc_crc16((uint8_t*)&tilePackOut, sizeof(struct packet_t));

	// Send package to tile
  comm.send((uint8_t*)&tilePackOut, sizeof(struct packet_t));
  
	// Alert the user that he has sent a package 
	light.set(1, 500, 500, 500);
  light.update();
	timerManager.createOneShot(1000, lightsOff);
	vibrateUntil(200);
}

static void lightsOff() {
	light.setAll(0, 0, 0);
	light.update();
}

static void handleTileMsg(Message_t* msg) {
	tilePackIn = (packet_t*)msg->data;

	switch(tilePackIn->command) {
		case GAME_START:
			tilePackOut.command = PIN;
			light.setAll(1000, 0, 0);
			light.update();
			timerManager.createOneShot(1000, lightsOff);
			break;

		case GAME_OVER:
			tilePackOut.command = GAME_OVER;
			if(tilePackIn->pinAddr == tilePackOut.address_from) { // t->address_from?
				light.setAll(0, 1000, 0);
				light.update();
				sound.setVol(5);
				sound.playSong(2);
			} else {
				light.setAll(1000, 0, 0);
				light.update();
			}
			break;

		case GAME_INIT:
			initGame();
			break;

		case GAME_STATUS:
			if (tilePackIn->pinAddr == tilePackOut.pinAddr) {
				score.prev = score.now;
				score.now = tilePackIn->score;

				if(score.now < score.prev) { //t->address_from?
					light.setAll(0, 1000, 0);
					light.update();
					vibrateUntil(300);
				}
				else if(score.now > score.prev || score.now == 10) {
					light.setAll(1000, 0, 0);
					light.update();
				}
			}

			timerManager.createOneShot(1000, lightsOff);
			break;

		default:
			break;

	}

}

static void handlePinMsg(Message_t* msg) {
	pinPackIn = (packet_p*)msg->data;

	// For debug purposes, lets turn on the lights if we recaive this kind of a package
	//light.setAll(1000, 1000, 0);
	//light.update();

	// If the addresses are the same, lets change ours, and send out a new address.
	if (pinPackIn->pinAddr == pinPackOut.pinAddr) {
		sendPinPack();
	}
}

static void sendPinPack() {
	// Initialize pinPack
	pinPackOut.msgType = MSG_PIN;
	pinPackOut.pinAddr = 20000 + rand()*rand() % 20000;

	getNewAddr();
	
	comm.send((uint8_t*)&pinPackOut, sizeof(struct packet_p));
}

static void getNewAddr()  {
  tilePackOut.address_from = pinPackOut.pinAddr;
	tilePackOut.pinAddr = tilePackOut.address_from;
}

static void initGame() {
			light.setAll(0, 0, 500);
			light.update();
			delay(200);
			light.setAll(0, 0, 0);
			light.update();
			delay(200);
			light.setAll(0, 0, 500);
			light.update();
			delay(200);
			light.setAll(0, 0, 0);
			light.update();
			delay(200);

			sound.stop();

			tilePackOut.command = REGISTER;
			score.now = 10;
}
