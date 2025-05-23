#include <xc.h>

enum pinType {input, output, input_pullup};
enum outType {high = 1, low = 0};
void PinMode(int pin, enum pinType type);
void DigitalWrite(int pin, enum outType value);
void SetPullUp(int pin, int pullUp);
int DigitalRead(int pin);
