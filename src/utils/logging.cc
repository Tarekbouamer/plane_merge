
#include "logging.h"

void InitializeGlog(char** argv) {
// #ifndef _MSC_VER  // Broken in MSVC
//   google::InstallFailureSignalHandler();
// #endif
  google::InitGoogleLogging(argv[0]);
}