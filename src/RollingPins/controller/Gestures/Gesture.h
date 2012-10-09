
#ifndef Gesture_h
#define Gesture_h

class Gesture
{
  static const float THRESHOLD_ACC = 1.3; // Threshold for movement
  
  bool isDetectingGesture;


  public:
    Gesture();
    void subscribeForEvents(void (*gestureStart)(void), void (*gestureEnd)(void));
    void update();

  private:
    //void *gestureStart_();
    //void *gestureEnd_();
};

extern Gesture gesture;

#endif
