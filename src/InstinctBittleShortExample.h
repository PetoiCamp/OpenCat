#define BITTLE
//#define NUM_SKILLS 5

const int8_t vt[] PROGMEM = {
21, 0, 0, 1,
  57,  43,  60,  47, -18,   7, -17,   7,
  50,  43,  53,  47,  -5,   7,  -5,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  47,  47,  50,   7,   0,   7,   0,
  43,  54,  47,  58,   7, -13,   7, -13,
  43,  60,  47,  65,   7, -25,   7, -25,
  43,  66,  47,  71,   7, -35,   7, -35,
  43,  63,  47,  67,   7, -30,   7, -29,
  43,  57,  47,  60,   7, -18,   7, -17,
  43,  50,  47,  53,   7,  -5,   7,  -5,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  47,  43,  50,  47,   0,   7,   0,   7,
  54,  43,  58,  47, -13,   7, -13,   7,
  60,  43,  65,  47, -25,   7, -25,   7,
  66,  43,  71,  47, -35,   7, -35,   7,
  63,  43,  67,  47, -30,   7, -29,   7,
};

const int8_t balance[] PROGMEM = { 
1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30,};

const int8_t rest[] PROGMEM = { 
1, 0, 0, 1,
  -30, -80, -45,   0,  -3,  -3,   3,   3,  75,  75,  75,  75, -55, -55, -55, -55,};

const int8_t zero[] PROGMEM = { 
1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,};

const int8_t hi[] PROGMEM = { 
-3, 0, -30, 1,
 1, 2, 3, 
    0, -20, -60,   0,   0,   0,   0,   0,  30,  30,  90,  80,  60,  60, -40, -30,	 4, 1, 0, 0,
   35,  -5, -60,   0,   0,   0,   0,   0, -75,  30,  75,  60,  40,  75, -30,   0,	10, 0, 0, 0,
   40,   0, -35,   0,   0,   0,   0,   0, -60,  30,  75,  60,  60,  75, -30,   0,	10, 0, 0, 0,
};

  const char* skillNameWithType[]={"vtI","balanceI","restI","zeroN","hiI"};
#if !defined(MAIN_SKETCH) || !defined(I2C_EEPROM)
		//if it's not the main sketch to save data or there's no external EEPROM, 
		//the list should always contain all information.
  const int8_t* progmemPointer[] = {vt, balance, rest, zero, hi};
#else	//only need to know the pointers to newbilities, because the intuitions have been saved onto external EEPROM,
	//while the newbilities on progmem are assigned to new addresses
  const int8_t* progmemPointer[] = {zero, };
#endif

//Make sure to leave enough memory for SRAM to work properly. Any single skill should be smaller than 400 bytes for safety.
