
#ifndef Worker2_h
#define Worker2_h

extern char EVENT_ACT[];
extern char EVENT_SETUP_DONE[];

namespace Worker
{
  void setup(void (*ctrl_setup)());
  void act();
};

#endif
