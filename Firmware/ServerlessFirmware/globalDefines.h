#define FIRMWARE_VERSION 1

#define FACTORY_RESET_PIN 4

#define MAX_MEMORY 255
#define NUM_VERTICES 100
#define NUM_CONTROLS 100
#define OUTPUT_BUFFER_SIZE 200
#define INPUT_BUFFER_SIZE 200

#define SYSTEM_TASK_INTERVAL 1000 // Time in ms
#define NUMBER_OF_TASKS 4

//#define ON_ARDUINO
//#define ON_PC

#ifndef ON_ARDUINO
#include <string.h>
#endif

//#define USE_INDEXED_IDS

#define ADMIN_ID_SIZE 8
#define ACCESS_CODE_SIZE 8

#define USE_MASTER_ACCESS_CODE
#define GET_MASTER_ACCESS_CODE heepByte masterAccessCode[ACCESS_CODE_SIZE]; for(int i = 0; i < ACCESS_CODE_SIZE; i++) masterAccessCode[i] = i*15;

#define STANDARD_ID_SIZE 4

#ifdef USE_INDEXED_IDS
#define ID_SIZE 1
#else 
#define ID_SIZE STANDARD_ID_SIZE
#endif

typedef unsigned char heepByte;
