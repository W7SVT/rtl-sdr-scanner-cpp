#include "help_structures.h"

#include <string.h>
#include <utils.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

std::string frequencyToString(const Frequency &frequency, const std::string &label) {
  char buf[1024];
  const auto f1 = frequency / 1000000;
  const auto f2 = (frequency / 1000) % 1000;
  const auto f3 = frequency % 1000;

  u_int32_t offset = 0;
  if (!label.empty()) {
    offset += sprintf(buf + offset, "%s: ", label.c_str());
  }
  if (f1 == 0 && f2 == 0) {
    offset += sprintf(buf + offset, "%03d Hz", f3);
  } else if (f1 == 0) {
    offset += sprintf(buf + offset, "%03d.%03d Hz", f2, f3);
  } else if (f3 == 0) {
    offset += sprintf(buf + offset, "%03d.%03d kHz", f1, f2);
  } else {
    offset += sprintf(buf + offset, "%03d.%03d.%03d Hz", f1, f2, f3);
  }
  return std::string(buf);
}

std::string powerToString(const Power &power) {
  constexpr auto MIN_POWER = -70.0f;
  constexpr auto MAX_POWER = -10.0f;
  constexpr auto BAR_SIZE = 30;

  const auto p = std::lround(std::min(std::max((power - MIN_POWER) / (MAX_POWER - MIN_POWER), 0.0f), 1.0f) * BAR_SIZE);
  char buf[1024];
  sprintf(buf, "power: %6.2f dB ", power);
  return std::string(buf) + std::string(p, '#') + std::string(BAR_SIZE - p, '_');
}

std::string Signal::toString() const { return frequencyToString(frequency) + ", " + powerToString(power); }

FrequencyRange::FrequencyRange(const Frequency _start, const Frequency _stop, const Frequency _sampleRate, const uint32_t _fft)
    : start(_start), stop(_stop), sampleRate(_sampleRate), fft(_fft == 0 ? countFft(sampleRate) : _fft) {}

std::string FrequencyRange::toString() const {
  char buf[1024];
  sprintf(buf, "%s, %s, fft: %d", frequencyToString(start, "start").c_str(), frequencyToString(stop, "stop").c_str(), fft);
  return std::string(buf);
}

Frequency FrequencyRange::center() const { return (start + stop) / 2; }

uint32_t FrequencyRange::step() const { return sampleRate / fft; }

bool FrequencyRange::operator==(const FrequencyRange &rhs) const { return start == rhs.start && stop == rhs.stop && sampleRate == rhs.sampleRate && fft == rhs.fft; }

bool FrequencyRange::operator<(const FrequencyRange &rhs) const {
  return start < rhs.start || (start == rhs.start && stop < rhs.stop) || (stop == rhs.stop && sampleRate < rhs.sampleRate) || (sampleRate == rhs.sampleRate && fft < rhs.fft);
}
