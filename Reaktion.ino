// (c) SGS Erlangen 2021-2023
// Autor: Robert Risack

#include <Display.h>	// eigene Library zur Ansteuerung des Display
#include <Bounce2.h>	// Library Bounce2
// Debouncing library for Arduino and Wiring by Thomas Ouellet Fredericks with many contributions from the community : 
// https://github.com/thomasfredericks/Bounce2/graphs/contributors
// Version 2.6

int analogReadLevel(int pin, int levels)
{
  long value = analogRead(pin);
  return (levels - 1) - value * 10 / 1024;
}

const int modepin1 = 5; // mode switch pin 1
const int modepin2 = 6; // mode switch pin 2

const int targetPin = 7; // target of pin

const int dataPin = 8; //74HC595  pin 8 DS
const int latchPin = 9; //74HC595  pin 9 STCP
const int clockPin = 10; //74HC595  pin 10 SHCP

const int startPin = 11; // start button
const int reactPin = 12; // fencing weapon button

const int waittimePin = A1;

Bounce2::Button  startButton = Bounce2::Button ();
Bounce reactButton = Bounce();
unsigned long starttime;


const int ledPin = LED_BUILTIN;
enum modeswitch { MODESWITCH_NORMAL=1, MODESWITCH_MULTI=2, MODESWITCH_AUTO=3};

Display disp(dataPin, clockPin, latchPin, 8);

class Mode {
  protected:
    int modeswitch ( ) {
      return digitalRead(modepin2) * 2 + digitalRead(modepin1);
    }
  public:
    enum mode { NOCHANGE = -1, DEBUG, OFFLINE, WAITING, RUNNING, SHOWING };
    virtual mode loop() = 0;
    virtual void setup() = 0;
    virtual mode start_falling() {
      return NOCHANGE;
    }
    virtual mode start_rising() {
      return NOCHANGE;
    }
    virtual mode tip_falling() {
      return NOCHANGE;
    }
    virtual mode tip_rising() {
      return NOCHANGE;
    }
};

class OfflineMode : public Mode
{
  public:
    OfflineMode() {}
    virtual mode loop();
    virtual mode start_falling();
    virtual mode tip_falling();
    virtual mode tip_rising();
    void setup() {}
};
Mode::mode OfflineMode::loop() {
  disp.ShowScrollText("SGS Fechten", 500);
  delay(5);
  return NOCHANGE;
}
Mode::mode OfflineMode::start_falling() {
  digitalWrite(ledPin, LOW);
  return WAITING;
}
Mode::mode OfflineMode::tip_falling() {
  digitalWrite(ledPin, HIGH);
  return NOCHANGE;
}
Mode::mode OfflineMode::tip_rising() {
  digitalWrite(ledPin, LOW);
  return NOCHANGE;
}

class WaitMode : public Mode
{
  private:
    long m_starttime;
    long m_randtime;
    int m_ctr;
  public:
    WaitMode() : m_starttime(0), m_randtime(0), m_ctr(0) {
      setup();
    }
    virtual mode loop();
    virtual void setup();
    virtual mode start_falling();
    virtual mode tip_falling();
};

void WaitMode::setup() {
  m_randtime = randomtime();
  m_starttime = millis();
}

Mode::mode WaitMode::loop() {
  // display wait pattern _ _ _ _ / - - - - / upper line
  disp.Wait(m_ctr % 3);
  if (millis() > m_starttime + m_randtime)
    return RUNNING;
  else
    return NOCHANGE;
}
Mode::mode WaitMode::start_falling() {
  digitalWrite(ledPin, LOW);
  Serial.println("Abort");
  return OFFLINE;
}
Mode::mode WaitMode::tip_falling()
{
  // restart if early reaction
  setup();
  m_ctr++;
  return NOCHANGE;
}

class RunMode : public Mode
{
  private:
    long m_starttime;
    float m_reactiontime;
  public:
    RunMode()
      : m_starttime(0), m_reactiontime(0) {}
    virtual mode loop();
    virtual void setup();
    virtual mode start_falling();
    virtual mode tip_falling();
    float GetReactionTime() {
      return m_reactiontime;
    }
};
void RunMode::setup() {
  digitalWrite(ledPin, HIGH);
  m_starttime = millis();
  m_reactiontime = 0;
}
Mode::mode RunMode::loop() {
  long stoptime = millis() - m_starttime ;
  float sek = (stoptime % 6000000) / 1000.0;
  m_reactiontime = sek;
  
  disp.ShowNum2(m_reactiontime, 2);
  return NOCHANGE;
}
Mode::mode RunMode::start_falling() {
  // abort action
  digitalWrite(ledPin, LOW);
  Serial.println("Abort");
  return OFFLINE;
}
Mode::mode RunMode::tip_falling() {
  // end reaction and print reaction time
  delay(100);
  if (digitalRead(targetPin) == 0)
  {
    digitalWrite(ledPin, LOW);
    Serial.println(m_reactiontime);
    return SHOWING;
  } else {
    return NOCHANGE;
  }
}

class ShowMode : public Mode
{
  private:
    long m_starttime;
    /*    long m_showtime; */
    float m_showvalue;
  public:
    ShowMode()
      : m_showvalue(0) {}
    virtual mode loop();
    virtual void setup() {
      m_starttime = millis();
    }
    virtual mode start_falling();
    void SetValue(float showvalue) {
      m_showvalue = showvalue;
    }
};

Mode::mode ShowMode::loop()
{
  disp.ShowNum2(m_showvalue, 2);
  switch (modeswitch())
  {
  // show time for 10 seconds
    case MODESWITCH_NORMAL:
      if (m_starttime + 10000 < millis())
        return OFFLINE;
      else
        return NOCHANGE;
      break;
    case MODESWITCH_AUTO:
    case MODESWITCH_MULTI:
      if (m_starttime + 5000 < millis())
        return WAITING;
      else
        return NOCHANGE;
      break;
  }
}
Mode::mode ShowMode::start_falling() {
  // start again
  return WAITING;
}

class DebugMode : public Mode
{
  public:
    DebugMode() {
      setup();
    }
    virtual mode loop();
    virtual void setup();
    virtual mode start_falling();
    virtual mode tip_falling();
  private:
    int level;
    int modeswitch;
    long starttime;
    String textToShow;
};

void DebugMode::setup()
{
  level = analogReadLevel(waittimePin, 10);
  modeswitch  = digitalRead(modepin2) * 2 + digitalRead(modepin1);
  starttime = 0;
  textToShow = "Dbug";
  // Serial.println("Debug");

  for (int i=1;i<8;++i)
    pinMode(i, INPUT_PULLUP);
}

Mode::mode DebugMode::start_falling()
{
  disp.Clear();
  textToShow = ("S OK");
  digitalWrite(ledPin, HIGH);
  starttime = millis();
}
Mode::mode DebugMode::tip_falling()
{
  disp.Clear();
  textToShow = ("F OK");
  digitalWrite(ledPin, HIGH);
  starttime = millis();
}

Mode::mode DebugMode::loop()
{
  /*
  for (int i=1;i<8;++i)
    {
      Serial.print(i); Serial.print(" - ");
      Serial.println(digitalRead(i));
    }
      Serial.println(" ");
    delay(1000);
    return NOCHANGE;*/
  int currmodeswitch = digitalRead(modepin2) * 2 + digitalRead(modepin1);

  if (currmodeswitch != modeswitch )
  {
    disp.Clear();
    textToShow = ("M  " + String(currmodeswitch));
    Serial.println(textToShow);
    modeswitch = currmodeswitch;
    starttime = millis();
  }

  int currlevel = analogReadLevel(waittimePin, 10);
  if (currlevel != level)
  {
    disp.Clear();
    textToShow = ("L  " + String(currlevel));
    Serial.println(textToShow);
    level = currlevel;
    starttime = millis();
  }
  if (millis() - starttime > 1000)
  {
    digitalWrite(ledPin, LOW);
    textToShow = "Dbug";
  }
  disp.ShowText(textToShow);
  return NOCHANGE;
}

class MyModes {
    OfflineMode m_offline;
    DebugMode m_debug;
    WaitMode m_wait;
    RunMode m_run;
    ShowMode m_show;
    Mode* m_modeptr;
  public:
    void loop() {
      if ( m_modeptr )
        m_modeptr->loop();
    };
    Mode* operator->() {
      return m_modeptr;
    }
    void switchMode ( Mode::mode val )
    {
      if (val >= 0)
        Serial.println(val);
      Mode* newmode = 0;
      switch (val)
      {
        case Mode::DEBUG: newmode = &m_debug; break;
        case Mode::OFFLINE: newmode = &m_offline; break;
        case Mode::WAITING: newmode = &m_wait; break;
        case Mode::RUNNING: newmode = &m_run; break;
        case Mode::SHOWING:
          newmode = &m_show;
          m_show.SetValue(m_run.GetReactionTime());
          break;
        case Mode::NOCHANGE: newmode = m_modeptr; break;
        default:
          newmode = 0;
      }
      if ( newmode != m_modeptr )
      {
        m_modeptr = newmode;
        m_modeptr->setup();
      }
    }
};

MyModes mymodes;

void setup() {
  pinMode(startPin, INPUT_PULLUP);
  pinMode(reactPin, INPUT_PULLUP);
  pinMode(modepin1, INPUT_PULLUP);
  pinMode(modepin2, INPUT_PULLUP);
  pinMode(targetPin, INPUT_PULLUP);
  startButton.attach(startPin);
  startButton.interval(5);
  reactButton.attach(reactPin);
  reactButton.interval(5);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  starttime = millis();

  Serial.begin(9600);
  Serial.println("setup");

  startButton.update();

  if (digitalRead(startPin) == LOW)
  {
    Serial.println("debug mode");
    mymodes.switchMode(Mode::DEBUG);
  } else {
    Serial.println("normal mode");
    mymodes.switchMode(Mode::OFFLINE);
  }
}

long randomtime()
{
  long poti = analogReadLevel(waittimePin, 10);
  long basetime = poti * 1000  + 1000;
  long randtime = random(basetime, basetime + 500);
  Serial.print("Poti val: ");
  Serial.println(poti);
  Serial.print("Random time: ");
  Serial.println(randtime);
  return randtime;
}

void loop() {

  startButton.update();
  reactButton.update();

  if (startButton.fallingEdge())
  {
    Serial.println("start falling edge");
    Mode::mode newmode = mymodes->start_falling();
    mymodes.switchMode(newmode);
  }

  if (reactButton.fallingEdge())
  {
    Serial.println("tip falling edge");
    Mode::mode newmode = mymodes->tip_falling();
    mymodes.switchMode(newmode);
  }

  if (reactButton.risingEdge()) {
    Serial.println("tip rising edge");
    Mode::mode newmode = mymodes->tip_rising();
    mymodes.switchMode(newmode);
  }

  Mode::mode newmode = mymodes->loop();
  mymodes.switchMode(newmode);


  delay(2);
}
