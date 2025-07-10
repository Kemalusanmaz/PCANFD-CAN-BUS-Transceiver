#ifndef CANCHANNELSETTINGS_HPP
#define CANCHANNELSETTINGS_HPP

class CanChannelSettings {
public:
  int getClock() const { return clock; }
  void setClock(int clock_) { clock = clock_; }

  int getDataBitRate() const { return dataBitRate; }
  void setDataBitRate(int dataBitRate_) { dataBitRate = dataBitRate_; }

  int getNominalBitRate() const { return nominalBitRate; }
  void setNominalBitRate(int nominalBitRate_) {
    nominalBitRate = nominalBitRate_;
  }

private:
  int clock;
  int dataBitRate;
  int nominalBitRate;
};

#endif // CANCHANNELSETTINGS_HPP
