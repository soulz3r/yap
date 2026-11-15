#include <stdio.h>

#include "cli/colors.h"
#include "cli/yap_cli.h"
#include "core/logger/logger.h"

#if defined(YAP_PLATFORM_WINDOWS)
#include <windows.h>
#endif

int main(int argc, char **argv) {
#if defined(YAP_PLATFORM_WINDOWS)
  const UINT previous_code_page = GetConsoleOutputCP();
  const BOOL restore_code_page =
      previous_code_page != 0 && SetConsoleOutputCP(CP_UTF8);
#endif

  YAP_LOG_CONFIG config = yap_log_config_default();

  if (!yap_log_init(&config)) {
    fprintf(stderr, "Failed to initialize YAP logger.\n");

#if defined(YAP_PLATFORM_WINDOWS)
    if (restore_code_page) {
      SetConsoleOutputCP(previous_code_page);
    }
#endif

    return 1;
  }

  const int result = yap_cli(argc, argv);

  if (result != 0) {
    fprintf(stderr, "Run " YAP_BLUE "'yap --help'" YAP_RESET YAP_GREY
                    " for usage.\n" YAP_RESET);
  }

  yap_log_shutdown();

#if defined(YAP_PLATFORM_WINDOWS)
  if (restore_code_page) {
    SetConsoleOutputCP(previous_code_page);
  }
#endif

  return result;
}
