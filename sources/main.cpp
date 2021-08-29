#include <algorithms/spectrogram.h>
#include <config.h>
#include <logger.h>
#include <radio/rtl_sdr_scanner.h>
#include <signal.h>

volatile bool isRunning{true};

void handler(int) {
  Logger::info("main", "received stop signal");
  isRunning = false;
}

int main() {
  Logger::info("main", "start app auto-sdr");
#ifndef NDEBUG
  Logger::info("main", "build type: debug");
#else
  Logger::info("main", "build type: release");
#endif

  try {
    std::vector<std::unique_ptr<RtlSdrScanner>> scanners;
    for (int i = 0; i < RtlSdrScanner::devicesCount(); ++i) {
      scanners.push_back(std::make_unique<RtlSdrScanner>(i, RTL_SDR_GAIN, SCANNER_FREQUENCIES, IGNORED_FREQUENCIES));
    }

    if (scanners.empty()) {
      Logger::warn("main", "not found rtl sdr devices");
    } else {
      signal(SIGINT, handler);
      while (isRunning && !scanners.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        scanners.erase(std::remove_if(scanners.begin(), scanners.end(), [](const std::unique_ptr<RtlSdrScanner>& scanner) { return !scanner->isRunning(); }), scanners.end());
      }
    }
  } catch (const std::exception& exception) {
    Logger::error("main", "main exception: {}", exception.what());
  }
  Logger::info("main", "stop app auto-sdr");
  return 0;
}
