#ifndef Display_h
#define Display_h

#include "Arduino.h"

class Display {
  public:
    void WriteDigit(unsigned char num, int sifr, bool dot = false);
    void WriteChar(unsigned char num, int sifr, bool dot = false);
    void WriteWaitDigit(int num, int sifr);
	void Latching(void);
	void Sending(unsigned char byte);
	void Sending(int sifr, unsigned char byte);
    unsigned int m_size;
    unsigned int m_latch;
    unsigned int m_clock;
    unsigned int m_data;
	String m_displaystring;
	String m_convertedstring;
  protected:
    unsigned int m_base;
	String m_empty;
  public:
    Display (unsigned int data, unsigned int clock, unsigned int latch, unsigned int size);
    ~Display ();
    void ShowNum(float val, unsigned int numDigit);
    void ShowNum2(float val, unsigned int numDigit);
    void ShowText(String str);
    void ShowScrollText(String str, unsigned int wait);
    void Wait(int num);
    void Clear() ;
};

#endif // Display_h