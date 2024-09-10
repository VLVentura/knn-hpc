#ifndef CORE_MESSAGE_H
#define CORE_MESSAGE_H

#include <cstdint>

#pragma pack(push, 1)

struct ComputeRange {
  int startLine{0};
  int endLine{0};
  int totalLines{0};

  [[nodiscard]] uint8_t* Serialize() { return reinterpret_cast<uint8_t*>(this); }  // NOLINT

  void ParseFromBuffer(uint8_t* buffer) {                        // NOLINT
    const auto* data = reinterpret_cast<ComputeRange*>(buffer);  // NOLINT
    startLine = data->startLine;
    endLine = data->endLine;
    totalLines = data->totalLines;
  }
};

#pragma pack(pop)

#endif  // CORE_MESSAGE_H
