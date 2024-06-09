
////// CODE FOR THE MAIN DISPLAY ///////// updated 6/7/24 3:26pm

#include "TimerISR.h"
#include "periph.h"
#include "helper.h"   
#include "usart_ATmega328p.h" 
#include "spiAVR.h"
#include <stdlib.h>

// PIN DEFINE STATEMNETS 
#define LED_PIN        1 // PD1
#define RX             0 // PD0
#define TX             1 // PD1
#define NUM_TASKS      4 // number of tasks for timerISR to run
#define START_BUTTON   0 //PC0
#define TEST_LED       PORTD = SetBit(PORTD, 2, 1) // turns on an LED (FOR DEBUGGING ONLY)
#define TEST_LED_OFF   PORTD = SetBit(PORTD, 2, 0) // turns on an LED (FOR DEBUGGING ONLY)
#define TOGGLE_BTN     GetBit(PORTC, 0); // port for the on/off button

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
bool is_up   = 0;
bool is_side = 0;
bool is_dwn  = 0;

// flags
bool displayInitialized = 0; // will be set to one once the display has ran through the init prosses
bool gameStartedFlag = 0; // set to true if the startup animation is complete
bool gameOverFlag = 0; // set to true once the game is over;
bool restartGameFlag = 0; // is set to 1 if the player wants to reset the game

// keep track of the game of the game states
unsigned char score = 100; // total score is decreased if the player plays a move late or early
int moveCnt = 0; // keeps track of the number of moves that have been played

// TASK PERIODS
const unsigned int GCD_PERIOD = 1;
const unsigned int DISPLAY_INIT_PERIOD = 200;
const unsigned int STARTUP_PERIOD = 1000;
const unsigned int DISPLAY_TEST_PERIOD = 1;
const unsigned int MGEN_PERIOD = 1500;

// STATES (declare the states for each task here)
enum startUpStates        { STARTUP_INIT, STARTUP_WAIT, STARTUP_3, STARTUP_2, STARTUP_1, STARTUP_GO, GAME_STARTED }; // displays the startup countdown
enum LCD_states           { FIRST_INIT_LCD, LCD_2, LCD_3, LCD_4, LCD_ON }; // state machine to init the display
enum moveGeneratorStates  { MGEN_INIT, MGEN_WAIT, MGEN_GEN }; // MGEN = MOVE GENERATOR
enum displayUpStates      { MGAME_INIT, MGAME_WAIT, MGAME_EARLY, MGAME_DISPLAY_EARLY, MGAME_ONTIME, MGAME_DISPLAY_ONTIME, MGAME_LATE, MGAME_DISPLAY_LATE, MGAME_END_GAME };
enum moveStates           { MOVE_INIT, MOVE_UP, MOVE_DOWN, MOVE_SIDE } move = MOVE_SIDE;

// HELPER FUNCTIONS
bool moveMatch() { // determines if the move from the remote matches that of the current move
  switch(move) {
    case MOVE_UP:
      if(is_up && !is_dwn && !is_side) {
        return true;
      }
      else {
        return false;
      }
      break;
    case MOVE_SIDE:
      if(!is_up && !is_dwn && is_side) {
        return true;
      }
      else {
        return false;
      }
      break;

    case MOVE_DOWN:
      if(!is_up && is_dwn && !is_side) {
        return true;
      }
      else {
        return false;
      }
      break;
    default:
      return false;
      break;
  };

  return false;
}

// TICK FUNCTIONS
// working init
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

// working startup
int StartupTick(int state) { // period: 1000ms 
  char wordX = 64;
  char wordY = 100;
  char word_size = 2;
  switch (state) { // state transitions
  case STARTUP_INIT:
    state = STARTUP_WAIT;
    break;
  case STARTUP_WAIT:
    if(displayInitialized /* && startup pin*/) {
      state = STARTUP_3;
    }
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
    draw_large_char(wordX, wordY, '3', WHITE, BLACK, word_size);
    // TEST_LED;
    break;
  case STARTUP_2:
    // populate_screen(BLACK);
    draw_large_char(wordX, wordY, '2', WHITE, BLACK, word_size);
    break;
  case STARTUP_1:
    draw_large_char(wordX, wordY, '1', WHITE, BLACK, word_size);
    break;
  case STARTUP_GO:
    wordX-=20;
    draw_large_char(wordX, wordY, 'G', WHITE, BLACK, word_size);
    wordX+=20;
    draw_large_char(wordX, wordY, 'O', WHITE, BLACK, word_size);
    break;
  case GAME_STARTED:
    gameStartedFlag = 1;
  default:
    break;
  }
  return state;
}

// move generator (in prog.)
int MGenTick(int state) { // period: 1500ms 
  /* Tick description:
    This tick function generates each move at random every 1.5s (1500ms).
    This will give the display functions enough time to prosses the inputs
    from the bluetooth module.

    Inputs:   gameStartedFlag
    Outputs:  move
  */
  switch(state) { // state transitions
    case MGEN_INIT:
      state = MGEN_WAIT;
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
      int currMove = (rand() % 3); // creates a range of values [0, 2]
        switch(currMove) { // assigns the value to the move state
          case 0:
            move = MOVE_UP;
            break;
          case 1:
            move = MOVE_SIDE;
            break;
          case 2:
            move = MOVE_DOWN;
            break;
          default:
            move = MOVE_INIT;
        }
      break;
    default:
      break;
  } // end state actions
  return state;
}

// display the up character and prosses the data from the bluetooth module
int DisplayTick(int state) { // period: 10ms
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

  static unsigned char i = 0;
  const char STAGE_DURATION = 50; // each stage (early, on time, and late) will last 50 cycles. (50 cycles of 10ms per cycle = 500ms = 0.5s)
  switch(state) { // state transitions
    case MGAME_INIT: // init state that transitions into the wait state instantly
      state = MGAME_WAIT;
      break;
    case MGAME_WAIT: // wait for an input from the move generator task
      switch(move) { // switch statemnt to select which figure to display
        case MOVE_INIT: 
          state = MGAME_WAIT; // game is still in its init state and has not received any input from the move gen task
          break;
        case MOVE_UP: 
          state = MGAME_EARLY;
          displayUpMan(); 
          break;
        case MOVE_SIDE: 
          state = MGAME_EARLY;
          displaySideMan();
          break;
        case MOVE_DOWN: 
          state = MGAME_EARLY;
          displayDownMan();
          break;
        default:
          break;
      }
      break;

    case MGAME_EARLY:
      if (moveMatch()) { // checks if the move is made
        state = MGAME_DISPLAY_EARLY;
      }
      else if(i >= STAGE_DURATION) { // time runs out if a move is made to early
        i = 0; 
        state = MGAME_ONTIME;
      }
      break;

    case MGAME_DISPLAY_EARLY:
      if(i >= 150) { // waits out the remainder of the timer
        i = 0;
        score -= 3;
        state = MGAME_WAIT;
      }
      break;

    case MGAME_ONTIME:
      if (moveMatch()) { // checks if the move is made
        state = MGAME_DISPLAY_ONTIME;
      }
      else if(i >= STAGE_DURATION) { // time runs out to make on time move
        i = 0;
        state = MGAME_LATE;
      }
      break;

    case MGAME_DISPLAY_ONTIME: 
      if(i >= 100) { // waits out the remainder of the timer
        i = 0;
        state = MGAME_WAIT;
      }
      break;

    case MGAME_LATE:
      if (moveMatch()) { // checks if the move is made
        state = MGAME_DISPLAY_LATE;
      }
      else if(i >= STAGE_DURATION) { // time runs out to make late move
        i = 0;
        state = MGAME_WAIT;
      }
      break;

    case MGAME_DISPLAY_LATE:
      if(i >= 50) { // waits out the remainder of the timer
        i = 0; 
        score -=3;
        state = MGAME_WAIT;
      }
      break;

    default:
      break;
    }; // end state transitions

  switch(state) { // state actions
    case MGAME_INIT:
      // no state actions
      break;
    case MGAME_WAIT:
      populate_screen(BLACK);
      break;

    case MGAME_EARLY:
      ++i;
      break;

    case MGAME_DISPLAY_EARLY:
      displayEarly();
      ++i;
      break;

    case MGAME_ONTIME:
      ++i;
      break;

    case MGAME_DISPLAY_ONTIME:
      displayNice();
      ++i;
      break;

    case MGAME_LATE:
      ++i;      
      break;

    case MGAME_DISPLAY_LATE:
      displayLate();
      ++i;
      break;

    default:
      break;
    }; // end state actions

  return state;
}

int GameClockTick(int state) {
  /*
    Keep track of the number of moves a player has made to determine if the game should end or not

    Inputs: moveCnt
    Outputs: gameOverFlag
  */
  return state;
}

int ReadBluetoothTick(int state){

  return state;
}

enum testStates  { TEST_INIT, TEST_RUN }; 
int DisplayTesttTick(int state) {
  switch (state) {
    case TEST_INIT:
    TEST_LED_OFF;
      if(gameStartedFlag) {state = TEST_RUN; TEST_LED;}
      break;
    case TEST_RUN:
      state = TEST_RUN;

      break;  
    default:
      break;
  }
  switch (state) {
    case TEST_INIT:
      break;
    case TEST_RUN:
      switch(move) { // switch statemnt to select which figure to display
        case MOVE_INIT: 
          break;
        case MOVE_UP: 
          displayUpMan(); 
          break;
        case MOVE_SIDE: 

          displaySideMan();
          break;
        case MOVE_DOWN: 
    
          displayDownMan();
          break;
        default:
          break;
      }

      if (move == MOVE_SIDE) {
        displaySideMan();
        TEST_LED;
      }
      else {
        TEST_LED_OFF;
        displayUpManOff();
      }
      break;  
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
  DDRC = 0xFF; PORTC = ~DDRC;  

  // TASK INIT
  unsigned char i = 0;

  // INIT FOR THE SETUP
  tasks[i].period = DISPLAY_INIT_PERIOD;
  tasks[i].state = FIRST_INIT_LCD;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &LCDInitTick;
  i++;

  // INIT FOR THE STARTUP FUNCTION
  tasks[i].period = STARTUP_PERIOD;
  tasks[i].state = STARTUP_INIT;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &StartupTick;
  i++;

  // INIT FOR THE MOVE GENERATOR
  tasks[i].period = MGEN_PERIOD;
  tasks[i].state = MGEN_INIT;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &MGenTick;
  i++;

  // INIT FOR THE MAIN DISPLAY
  tasks[i].period = DISPLAY_TEST_PERIOD;
  tasks[i].state = TEST_INIT;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &DisplayTesttTick;
  i++;

  ADC_init();   // initializes ADC
  initUSART();  // initializes USART communication for bluetooth module
  SPI_INIT();   // initializes SIP communication for display

  // INITIALIZATIONS
  TimerOn();
  TimerSet(GCD_PERIOD);
  while (1) {  } // TIMER ISR FUNCTION WILL INTURUPT AT GCD_PERIOD
  return 0;
}