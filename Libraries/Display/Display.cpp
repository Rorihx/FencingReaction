#include <Display.h>

// 7 segment codes for numbers 0-9 and A-F (for hex numbers)
unsigned char digitTable[] =
{ 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c
  , 0x39, 0x5e, 0x79, 0x71, 0x00
};
// wait values: 3 lines
unsigned char waitValues[] = { 0x08, 0x40, 0x01 };

// siekoo ASCII codes to simulate all characters a-z and A-Z
// siekoo makes no difference between lower or upper case
// some punctuation symbols included, rest is left empty (0)
unsigned char siekooASCII[] = 
{ 
  0, 0, 0, 0, 0, 0, 0, 0, // unprintable
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0x82, 0, 0, 0, 0, 0, 0, // <space> !"#$%&'
  0x39, 0x0f, 0, 0, 0, 0x40, 0x80, 0x52, // ()*+,-./
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
  0x7f, 0x6f, 0, 0, 0, 0, 0, 0x83, 				  // 8-9 
  0, 0x5f, 124, 88, 94, 121, 113, 61, 			  // @ a-g
  116,  0x11, 0x0d, 117, 0x38, 0x55, 0x54, 92, 	  // h-o
  115, 103, 80, 0x2d, 120,  0x1c, 0x2a, 0x6a, 	  // p-w
  0x14, 0x6e, 0x1b, 0, 0, 0, 0, 0x80, 			  // x-z , [\]^_
  0, 0x5f, 124, 88, 94, 121, 113, 61, 			  // @ A-G
  116,  0x11, 0x0d, 117, 0x38, 0x55, 0x54, 92, 	  // H-O
  115, 103, 80, 0x2d, 120, 0x1c, 0x2a, 0x6a, 	  // P-W
  0x14, 0x6e, 0x1b, 0, 0, 0, 0, 0 				  // X-Z , 
};

String convertString ( String str )
{
	String retStr = str;
	int j = -1;
	for (int i; i<str.length(); ++i) {
		if (str.c_str()[i] == '.') {
			if (j==-1 || retStr[j] & 0x80 == 1)
			{
				j+=1;
				retStr += 0x80;
			} else {
				retStr[j] &= 0x80;
			}
			
		} else {
			// bug: contains \0 in middle of string
			retStr += (char) siekooASCII[str.c_str()[i] & 0x7f];
			j++;
		}
	}
	return retStr;
}
String convertStringSimple ( String str )
{
	String retStr = str;
	int j = -1;
	for (int i=0; i<str.length(); ++i) {
		unsigned char val = siekooASCII[str.c_str()[i] & 0x7f];
		// Serial.println(val);
		retStr[i] = val ? val : 0xFF;  // don't put \0 into middle of string!
		j++;	
	}
	return retStr;
}
/*
unsigned char letters[] = 
{ 
  0, 0, 0, 0, 0, 0, 0, 0, // unprintable
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, // <space> !"#$%&'
  0, 0, 0, 0, 0, 0, 0, 0, // ()*+,-./
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
  0x7f, 0x6f, 0, 0, 0, 0, 0, 0, 				  // 8-9 
  0, 119, 124, 88, 94, 121, 113, 61, 			  // @ A-G
  116,  4, 30, 117, 6, 55, 84, 92, 				  // H-O
  115, 103, 80, 109, 120, 28, 28, 28, 			  // P-W
  118, 110, 91, 0, 0, 0, 0, 0, 					  // X-Z , [\]^_
  0, 119, 124, 88, 94, 121, 113, 61, 			  // @ A-G
  116,  4, 30, 117, 6, 55, 84, 92, 				  // H-O
  115, 103, 80, 109, 120, 28, 28, 28, 			  // P-W
  118, 110, 91, 0, 0, 0, 0, 0 					  // X-Z , [\]^_
};;*/

Display::Display (unsigned int _data,
                  unsigned int _clock,
                  unsigned int _latch,
				  unsigned int _size)
  : m_size(_size), m_latch(_latch), m_data(_data), m_clock(_clock), m_base(10), m_displaystring(""), m_empty("")
{
	pinMode(m_latch, OUTPUT);
	pinMode(m_clock, OUTPUT);
	pinMode(m_data, OUTPUT);
	
	for (int i=0; i<m_size; i++)
	{
		m_empty += " ";
	}
}
Display::~Display() {}

void Display::Sending(unsigned char byte)
{
  shiftOut(m_data, m_clock, MSBFIRST, byte);
}

void Display::Sending(int sifr, unsigned char byte)
{
  unsigned char val = 1 << sifr;
  Sending(byte);
  Sending(val);
}

void Display::Latching(void)
{
  digitalWrite(m_latch, LOW);
  delay(1);
  digitalWrite(m_latch, HIGH);
}

void Display::WriteDigit(unsigned char num, int sifr, bool dot)
{
  Sending(sifr, digitTable[num] + dot * 0x80);
  Latching();
}

void Display::WriteChar(unsigned char c, int sifr, bool dot)
{
  Sending(sifr, c);
  Latching();
}

void Display::WriteWaitDigit(int num, int sifr)
{
  Sending(sifr, waitValues[num]);
  Latching();
}

void Display::ShowNum2(float val, unsigned int dec)
{
	char buff[256];
	dtostrf ( val, m_size, dec, buff );
	ShowText ( buff );
}

void Display::ShowNum(float val, unsigned int dec)
{
  unsigned int ex;
  if (dec < 0 || dec >= m_size)
    ex = 0;
  else
    ex = dec;
   int valint =  int(val * pow(m_base, ex)+0.0005);
  /**/
  int	exp = 1;
  int i=0;
  unsigned char value;
  
  for (; i<m_size; i++)
  {
	unsigned char	value = (valint/exp) % m_base;
	if (valint/exp>0 || dec==i)
		WriteDigit(value, i, dec == i);
	exp *= m_base;
  }
}

void Display::ShowScrollText(String str, unsigned int wait)
{
	if (m_displaystring != str) {
		m_convertedstring = convertStringSimple(m_empty + str + m_empty);
		m_displaystring = str;
	}
	
	int start = (millis() / wait) % (m_displaystring.length()+m_size-1);
	for (int i=start; i<(start + m_size); i++)
	{
		unsigned char c = m_convertedstring[i];
		if (c==0xFF)
			WriteChar(0 , m_size - i - 1,  false );
		else
			WriteChar(c, m_size - i + start - 1,  false );
	}
}

void Display::ShowText(String str)
{
	if (m_displaystring != str) {
		m_convertedstring = convertStringSimple(str);
		m_displaystring = str;
	}

	int i;	
	for (i=0; i<min(m_convertedstring.length(),m_size); i++)
	{
		unsigned char c = m_convertedstring[i];
		if (c==0xFF)
			WriteChar(0 , m_size - i - 1,  false );
		else
			WriteChar(c , m_size - i - 1,  false );
	}
	for (; i<m_size; i++)
			WriteChar(0 , m_size - i - 1,  false );
}

void Display::Wait(int num)
{
  for (int i=0; i<m_size; i++)
    WriteWaitDigit(num, i);
}

void Display::Clear()
{
  for (int i=0; i<m_size; i++)
    WriteDigit(16, i, false);
}
