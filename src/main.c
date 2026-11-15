#include <stdio.h>

#include "cli/yap_cli.h"
#include "core/logger/logger.h"

int main(int argc, char **argv) {
  yap_log_config config = yap_log_config_default();

  if (!yap_log_init(&config)) {
    fprintf(stderr, "Failed to initialize YAP logger.\n");
    return 1;
  }

  const int result = yap_cli(argc, argv);

  yap_log_shutdown();
  return result;
}
