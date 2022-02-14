
#include "../maximilian.h"

class maxiClock  {
public:
  maxiClock();
  void ticker();
  void setTempo(maxi_float_t bpm);
  void setTicksPerBeat(int ticksPerBeat);
  maxiOsc timer;
  int currentCount;
  int lastCount;
  int playHead;
  maxi_float_t bps;
  maxi_float_t bpm;
  int ticks;
  bool tick;

  // SETTERS
  void setCurrentCount(int n) { this->currentCount = n; }
  void setLastCount(int n) { this->lastCount = n; }
  void setPlayHead(int n) { this->playHead = n; }
  void setBps(int bps_) { this->bps = bps_; }
  void setBpm(int bpm_) { this->bpm = bpm_; }
  void setTick(int tick_) { this->tick = tick_; }
  void setTicks(int ticks_) { this->ticks = ticks_; }

  // GETTERS
  int getCurrentCount() const { return currentCount; }
  int getLastCount() const { return lastCount; }
  int getPlayHead() const { return playHead; }
  maxi_float_t getBps() const { return bps; }
  maxi_float_t getBpm() const { return bpm; }
  bool getTick() const { return tick; }
  int getTicks() const { return ticks; }
  int isTick() const { return tick; }
};
