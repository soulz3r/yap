#include <stdio.h>

#include "defines.h"

#include "cli/colors.h"
#include "cli/yap_cli.h"
#include "core/logger/logger.h"

i32 main(i32 argc, c **argv) {
  YAP_LOG_CONFIG config = yap_log_config_default();

  if (!yap_log_init(&config)) {
    fprintf(stderr, "Failed to initialize YAP logger.\n");
    return 1;
  }

  const i32 result = yap_cli(argc, argv);

  if (result != 0) {
    fprintf(stderr, "Run " YAP_BLUE "'yap --help'" YAP_RESET YAP_GREY
                    " for usage.\n" YAP_RESET);
  }

  yap_log_shutdown();
  return result;
}
