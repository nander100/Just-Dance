
////// CODE FOR THE MAIN DISPLAY ///////// updated 6/7/24 3:26pm

#include "TimerISR.h"
#include "periph.h"
#include "helper.h"   
#include "usart_ATmega328p.h" 
#include "spiAVR.h"
#include "serialATmega.h"
#include <stdlib.h>

// PIN DEFINE STATEMNETS 
#define LED_PIN          1 // PD1
#define RX               0 // PD0
#define TX               1 // PD1
#define NUM_TASKS        5 // number of tasks for timerISR to run
#define START_BUTTON     0 //PC0
#define TEST_LED_R       PORTD = SetBit(PORTD, 2, 1) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_R_OFF   PORTD = SetBit(PORTD, 2, 0) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_G       PORTD = SetBit(PORTD, 3, 1) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_G_OFF   PORTD = SetBit(PORTD, 3, 0) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_Y       PORTD = SetBit(PORTD, 4, 1) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_Y_OFF   PORTD = SetBit(PORTD, 4, 0) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_B       PORTD = SetBit(PORTD, 5, 1) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_B_OFF   PORTD = SetBit(PORTD, 5, 0) // turns on an LED (FOR DEBUGGING ONLY)
#define RESET            GetBit(PORTC, 0) // port for the on/off button

// DEFINE TASK
typedef struct _task{
  signed   char state;    //Task's current state
  unsigned long period;     //Task period
  unsigned long elapsedTime;  //Time elapsed since last task tick
  int (*TickFct)(int);    //Task tick function
} task;
task tasks[NUM_TASKS]; // DECLARE TASK ARRAY

// GLOBAL VARIABLES
// variables to store the force acting in each direction
bool z   = 0;
bool x = 0;
bool y  = 0;

// flags
bool displayInitialized = 0; // will be set to one once the display has ran through the init prosses
bool gameStartedFlag = 0; // set to true if the startup animation is complete
bool gameOverFlag = 0; // set to true once the game is over;
bool restartGameFlag = 0; // is set to 1 if the player wants to reset the game

// keep track of the game of the game states
unsigned char score = 100; // total score is decreased if the player plays a move late or early
int moveCnt = 0; // keeps track of the number of moves that have been played

// TASK PERIODS
const unsigned int GCD_PERIOD = 10;
const unsigned int DISPLAY_INIT_PERIOD = 200;
const unsigned int STARTUP_PERIOD = 1000;
const unsigned int DISPLAY_TEST_PERIOD = 10;
const unsigned int DISPLAY_MAIN_PERIOD = 10;
const unsigned int BT_PERIOD = 10;
const unsigned int MGEN_PERIOD = 50;

// STATES (declare the states for each task here)
enum startUpStates        { STARTUP_INIT, STARTUP_WAIT, STARTUP_3, STARTUP_2, STARTUP_1, STARTUP_GO, GAME_STARTED }; // displays the startup countdown
enum LCD_states           { FIRST_INIT_LCD, LCD_2, LCD_3, LCD_4, LCD_ON }; // state machine to init the display
enum moveGeneratorStates  { MGEN_INIT, MGEN_WAIT, MGEN_GEN }; // MGEN = MOVE GENERATOR
enum displayUpStates      { MGAME_INIT, MGAME_WAIT, MGAME_EARLY, MGAME_DISPLAY_EARLY, MGAME_ONTIME, MGAME_DISPLAY_ONTIME, MGAME_LATE, MGAME_DISPLAY_LATE, MGAME_END_MOVE, MGAME_END_GAME };
enum bluetoothStates      { BT_INIT, BT_RECEIVE };
enum moveStates           { MOVE_INIT, MOVE_UP, MOVE_DOWN, MOVE_SIDE } move;

// HELPER FUNCTIONS
bool moveMatch() { // determines if the move from the remote matches that of the current move
  switch(move) {
    case MOVE_UP:
      if(z && !y && !x) { TEST_LED_R; return true; }
      else { TEST_LED_R_OFF; return false; }
      break;

    case MOVE_SIDE:
      if(!z && !y && x) { TEST_LED_G; return true; }
      else { TEST_LED_G_OFF; return false; }
      break;

    case MOVE_DOWN:
      if(!z && y && !x) { TEST_LED_Y; return true; }
      else { TEST_LED_Y_OFF; return false; }
      break;

    default:
      return false;
      break;

  };

  return false;
}

// TICK FUNCTIONS
// INIT WORKING
int LCDInitTick(int state) { // period: 200ms 
  switch(state) {
    case FIRST_INIT_LCD:
      hardwareRst();
      PORTB = SetBit(PORTB, 1, 0);
      PORTB = SetBit(PORTB, 2, 0);
      state = LCD_2;
      break;
    case LCD_2:
      command(0x11);
      state = LCD_3;
      break;
    case LCD_3:
      command(0x36);
      data(0x00); 
      command(0x3A); 
      data(0x05); 
      command(0x29);
      state = LCD_4;
      break;
    case LCD_4:
      set_column(0, 127);
      set_row(0, 159);
      data(0x2C); 
      for (uint32_t i = 0; i < 128 * 160; i++) { write_pixel(0x001F); }
      state = LCD_ON;
      break;
    case LCD_ON:
      displayInitialized = 1;
    default:
      break;  
  }
  return state;
}

//  startup (WORKING)
int StartupTick(int state) { // period: 1000ms 
  char wordX = 64;
  char wordY = 100;
  char word_size = 2;
  switch (state) { // state transitions
  case STARTUP_INIT:
    if(displayInitialized ) {
  
      state = STARTUP_WAIT;
    }
    break;
  case STARTUP_WAIT:
      populate_screen(BLACK);
      state = STARTUP_3;
    break;
  case STARTUP_3:
    state = STARTUP_2;
    break;
  case STARTUP_2:
    state = STARTUP_1;
    break;
  case STARTUP_1:
    state = STARTUP_GO;
    break;
  case STARTUP_GO:
    state = GAME_STARTED;
    break;
  default:
    break;
  }

  switch (state) { // state actions
  case STARTUP_INIT:
    break;
  case STARTUP_WAIT:
    break;
  case STARTUP_3:
    populate_screen(BLACK);
    // draw_line(129, 0, 128, 128, BLACK);
    characterWrite(wordX, wordY, '3', WHITE, BLACK, word_size);
    // TEST_LED_R;
    break;
  case STARTUP_2:
    // populate_screen(BLACK);
    characterWrite(wordX, wordY, '2', WHITE, BLACK, word_size);
    break;
  case STARTUP_1:
    characterWrite(wordX, wordY, '1', WHITE, BLACK, word_size);
    break;
  case STARTUP_GO:
    wordX-=20;
    characterWrite(wordX, wordY, 'G', WHITE, BLACK, word_size);
    wordX+=20;
    characterWrite(wordX, wordY, 'O', WHITE, BLACK, word_size);
    break;
  case GAME_STARTED:
    gameStartedFlag = 1;
    if(ADC_read(0) < 900) {
      gameStartedFlag = 0;
      // TEST_LED_B;
      state = STARTUP_INIT;
      
    }
    // TEST_LED_Y;
      // TEST_LED_B;
    break;
  default:
    break;
  }
  return state;
}

// move generator (WORKING!!)
int MGenTick(int state) { // period: 1500ms 
  /* Tick description:
    This tick function generates each move at random every 1.5s (1500ms).
    This will give the display functions enough time to prosses the inputs
    from the bluetooth module.

    Inputs:   gameStartedFlag
    Outputs:  move
  */
  static int currMove = -1;
  switch(state) { // state transitions
    case MGEN_INIT:
      state = MGEN_WAIT;
      // TEST_LED_B; 
      break;
    case MGEN_WAIT:
      if(gameStartedFlag) { // waits for the game to start before generating moves
        state = MGEN_GEN;
      }
      break;
    case MGEN_GEN:
      break;
    default:
      break;
  } // end state transitions

  switch(state) { // state actions
    case MGEN_INIT:
      break;
    case MGEN_WAIT:
      move = MOVE_INIT; // move is in the initial state. (ensures no display is on while waiting)
      break;
    case MGEN_GEN:
      currMove = rand() % 2;
      switch(currMove) { // assigns the value to the move state
        case 0:
          move = MOVE_UP;
          break;
        case 1:
          move = MOVE_DOWN;
          break;
        default:
          move = MOVE_INIT;
      }
      break;
    default:
      move = MOVE_INIT;
      break;
  } // end state actions
  return state;
}

// display the up character and prosses the data from the bluetooth module

 // working BT read function
int ReadBluetoothTick(int state){ 
  /* Reads and interprets the bluetooth signal from the remote */
  unsigned char getGyroVals = 0x00;
  switch (state) {
    case BT_INIT:
      if(gameStartedFlag) {
        state = BT_RECEIVE;
      }
      break;
    case BT_RECEIVE:
      if(!USART_HasReceived()) break;
      getGyroVals = USART_Receive();
      // TEST_LED_B;


      // assign the global variables to determine when there is sufficient force to determine the x, y, and z directions
      x = getGyroVals & 0x01;
      y = getGyroVals & 0x02;
      z = getGyroVals & 0x04;
      
      PORTD = SetBit(PORTD, 2, z);
      PORTD = SetBit(PORTD, 3, x);
      PORTD = SetBit(PORTD, 4, y);

      getGyroVals = 0x00;
      TEST_LED_B_OFF;
      break;
    default:
    break;
  }
  return state;
}

unsigned char i = 0;
unsigned char j = 0; // keeps track of how long the remote senses down
unsigned char cnt = 0; // keeps track of how many ticks have gone by to change to the next move
enum testStates  { TEST_INIT, DISPLAY_CHARACTERS, WAIT_FOR_MOVE, CHECK_MOVE, NEW_DISPLAY_WAIT, END_GAME }; 
int DisplayTesttTick(int state) {
    /* Tick Description:
     Here, the stick figure for the all positions is drawn. This tick will 
     also print out "NICE!", "EARLY!", or "LATE" to the LCD depending on 
     the input from the bluetooth sensor.
     
    Move variable representations
     -1. game over
     0.  up 
     1.  side
     2.  down

     Inputs: move, is_up, is_down, is_side
     Outputs: pixels to the display
  */
  static char currMove = 0;

  switch (state) {
    case TEST_INIT:
      if(gameStartedFlag) {
        state = DISPLAY_CHARACTERS; 
        char wordX = 64;
        char wordY = 100;
        char word_size = 2;
        wordX-=20;
        characterWrite(wordX, wordY, 'G', BLACK, BLACK, word_size);
        wordX+=20;
        characterWrite(wordX, wordY, 'O', BLACK, BLACK, word_size);  
        displayRed();
        displayGreen();
      }
      break;
    case DISPLAY_CHARACTERS:
      switch(move) { // switch statemnt to select which figure to display
          case MOVE_INIT: 
            break;
          case MOVE_UP: 
            displayDownManOff();
            displayUpMan();
            break;
          case MOVE_DOWN: 
            displayUpManOff();
            displayDownMan();

            break;
          default:
            break;
        }

        state = WAIT_FOR_MOVE;
        currMove = move;
        break;
    case WAIT_FOR_MOVE:
      if(i >= 30 || j >= 30) {
        state = CHECK_MOVE;

        cnt = 0;
      }
      else if(cnt >= 250) {
        displayRed();
        score -= 3;
        state = DISPLAY_CHARACTERS;
        // TEST_LED_Y;
        cnt = 0;
        moveCnt ++;

      }
      case NEW_DISPLAY_WAIT:
        // TEST_LED_B;
        if(moveCnt >= 10) {
          state = END_GAME; 
          populate_screen(BLACK);     
          characterWrite(30, 60, (score /10) + 48, WHITE,BLACK, 2); 
          characterWrite(50, 60, (score %10) + 48, WHITE,BLACK, 2);
        
        }
        else if(cnt >= 250) {
          TEST_LED_B_OFF;
          state = DISPLAY_CHARACTERS;
          cnt = 0;
        }
        break;
    case END_GAME:
      if(ADC_read(0) < 900 && !gameStartedFlag){
        state = TEST_INIT;
        moveCnt = 0;
        redXlocation = RINIT;
        greenXlocation = GINIT;
        score = 100;

      }
      break; 
    default:
      break;
  }

  switch (state) {
    case TEST_INIT:
      // no state actions
      break;
    case DISPLAY_CHARACTERS:
      break;

    case WAIT_FOR_MOVE: 
      if(x && !y && !z) {
        ++j;  // down
      }
      else if(!z && y && !x) {
        ++i; // up
      }
      ++cnt; // always increase the count after each tick to keep track of the time of each move
      break;

    case CHECK_MOVE:
    // TEST_LED_G;
      state = NEW_DISPLAY_WAIT;
      if(j >=30 && currMove == MOVE_DOWN) {
        displayGreen();
      }
      else if(j >=30 && currMove == MOVE_UP){
        displayRed();
        score-=3;
      }
      else if(i >=30 && currMove == MOVE_DOWN) {
        displayRed();
        score-=3;
      }
      else if(i >=30 && currMove == MOVE_UP) {
        displayGreen();
      }
      else {
        populate_screen(RED);
      }
      i = 0;
      j = 0;
      moveCnt ++;
      break;
    case NEW_DISPLAY_WAIT:
      cnt++;
      break;  
    case END_GAME:


    default:
      break;
  }

  return state;
}

// TIMER INTURUPT
void TimerISR() {
  for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {                   // Iterate through each task in the task array
    if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
      tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
      tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
    }
    tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
  }
}

int main(void) {
  // INITIALIZE PORT AND DDR REGISTERS
  DDRD = 0xFE; PORTD = ~DDRD; //1111 1110
  DDRB = 0xFF; PORTB = ~DDRB;
  DDRC = 0x00; PORTC = ~DDRC;  

  // INIT FOR THE SETUP
  unsigned char i = 0;
  tasks[i].period = DISPLAY_INIT_PERIOD;
  tasks[i].state = FIRST_INIT_LCD;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &LCDInitTick;
  i++;

  // // INIT FOR THE STARTUP FUNCTION
  tasks[i].period = STARTUP_PERIOD;
  tasks[i].state = STARTUP_INIT;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &StartupTick;
  i++;

  // // INIT FOR THE MOVE GENERATOR
  tasks[i].period = MGEN_PERIOD;
  tasks[i].state = MGEN_INIT;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &MGenTick;
  i++;

  // INIT FOR THE TEST DISPLAY
  tasks[i].period = DISPLAY_TEST_PERIOD;
  tasks[i].state = TEST_INIT;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &DisplayTesttTick;
  i++;

  // INIT FOT THE MAIN DISPLAY FUNCTION
  // tasks[i].period = DISPLAY_MAIN_PERIOD;
  // tasks[i].state = MGAME_INIT;
  // tasks[i].elapsedTime = tasks[i].period;
  // tasks[i].TickFct = &DisplayTick;
  // i++;
  
  // INIT FOR THE BLUETOOTH COMMUNICATION
  tasks[i].period = BT_PERIOD;
  tasks[i].state = BT_INIT;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &ReadBluetoothTick;
  i++;
  
  ADC_init();   // initializes ADC
  initUSART();  // initializes USART communication for bluetooth module
  SPI_INIT();   // initializes SIP communication for display
  srand(1738);

  // INITIALIZATIONS
  TimerOn();
  TimerSet(GCD_PERIOD);
  // serial_println(ADC_read(0));
  while (1) {  } // TIMER ISR FUNCTION WILL INTURUPT AT GCD_PERIOD
  return 0;
}