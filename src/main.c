#include <stdio.h>

#include "cli/colors.h"
#include "cli/yap_cli.h"
#include "core/logger/logger.h"

#if defined(YAP_PLATFORM_WINDOWS)
#include <windows.h>
#endif

int main(int argc, char **argv) {
#if defined(YAP_PLATFORM_WINDOWS)
  SetConsoleOutputCP(CP_UTF8);
#endif

  YAP_LOG_CONFIG config = yap_log_config_default();

  if (!yap_log_init(&config)) {
    fprintf(stderr, "Failed to initialize YAP logger.\n");
    return 1;
  }

  const int result = yap_cli(argc, argv);

  if (result != 0) {
    fprintf(stderr, "Run " YAP_BLUE "'yap --help'" YAP_RESET YAP_GREY
                    " for usage.\n" YAP_RESET);
  }

  yap_log_shutdown();
  return result;
}
