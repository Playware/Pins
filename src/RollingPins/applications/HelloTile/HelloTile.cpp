
#include "RPApi.h"
#include <libraries/CRC/CRC16.h>

// Variables and objects
static char MSG_TILE = 27;
static bool frameSent = false;
enum command_t : uint8_t {TILE, PIN };

static void updateGame(char* eventType, void* event);
static void sendMsgToTile();
static void handleTileMsg(Message_t* msg);
static uint8_t getNewColor();
static void printColor(uint8_t c);
static void lightsOff();

namespace JungleSpeed {
	static void setup(void);
}

// Function declarations
static void updateGame(char* eventType, void* event);

Application_t helloTile =  {
	"HelloTile", 
	JungleSpeed::setup
};

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
	uint8_t color;
	// Tile package ends
	uint16_t crc;
	uint8_t endByte;
} packOut, *packIn;

// Setup function, called only once in the beginning of the runtime of the program.
static void JungleSpeed::setup(void) {
	srand(millis());
	
  packOut.msgType = MSG_TILE;
	packOut.escape = 0x7F;
  packOut.address_to = 0xFFFF;
	packOut.address_from = rand() % 30000;
	packOut.command = PIN;
	packOut.endByte = 0x7F;

	// Subscribe to a message of type packet_p
	// Each time a packet_p is received, the function handlePinMsg() is called.
	comm.subscribe(MSG_TILE, handleTileMsg);
	
	// Run the update function in each loop throughout the lifetime of the program
	eventManager.subscribeListener(EVENT_ACT, updateGame);
}

static void updateGame(char* eventType, void* event) {
	
	movement.update();
	if(*movement.R() > 2.5 && !frameSent) {
		light.setAll(0, 1000, 0);
		light.update();
		sendMsgToTile();
		frameSent = true;
		timerManager.createOneShot(1000, lightsOff);
	}
}

static void sendMsgToTile() {
	packOut.color = getNewColor();
	packOut.frame_nr = rand() % 256;
	packOut.crc = calc_crc16((uint8_t*)&packOut, sizeof(struct packet_t));

	comm.send((uint8_t*)&packOut, sizeof(struct packet_t));
}

static void handleTileMsg(Message_t* msg) {
	packIn = (packet_t*)msg->data;

	// If the addresses are the same, lets change ours, and send out a new address.
	if (packIn->command == TILE) {
		printColor(packIn->color);
	}
}

static uint8_t getNewColor() {
	return rand() % 4;
}

static void printColor(uint8_t c) {
	switch (c) {
		case 0:
			light.setAll(1000, 0, 0);
			light.update();
			break;
		case 1:
			light.setAll(0, 0, 1000);
			light.update();
			break;
		case 2:
			light.setAll(1000, 1000, 0);
			light.update();
			break;
		case 3:
			light.setAll(1000, 0, 1000);
			light.update();
			break;
		default:
			break;
	}
	
	timerManager.createOneShot(1000, lightsOff);
}

static void lightsOff() {
	frameSent = false;
	light.setAll(0, 0, 0);
	light.update();
}
