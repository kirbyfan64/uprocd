/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "common.h"
#include "private.h"

#include "uprocd.h"

#include <systemd/sd-daemon.h>

#include <sys/wait.h>
#include <dlfcn.h>
#include <signal.h>
#include <unistd.h>

void _message(int failure, sds error) {
  fprintf(stderr, "%s%.*s\n", failure ? SD_CRIT : SD_INFO, (int)sdslen(error), error);
  if (failure) {
    sd_notifyf(0, "STATUS=\"Failure: %s\"", error);
  }
  sdsfree(error);
}

sds get_xdg_config_home() {
  char *xdg_config_home = getenv("XDG_CONFIG_HOME");
  if (xdg_config_home != NULL) {
    return sdsnew(xdg_config_home);
  }

  char *home = getenv("HOME");
  return sdscat(sdsnew(home), "/.config");
}

config *load_config(const char *module, sds *module_dir) {
  sds xdg_config_home = get_xdg_config_home();

  static const char *search_paths[] = {
    /* "build/modules", */
    "/usr/share/uprocd/modules",
    "/usr/local/share/uprocd/modules",
    "@/uprocd/modules",
  };

  sds module_path = NULL;

  for (int i = 0; i < sizeof(search_paths) / sizeof(search_paths[0]); i++) {
    sds path = sdsnew(search_paths[i]);
    if (path[0] == '@') {
      sdsrange(path, 1, -1);
      sds newpath = sdscat(sdsdup(xdg_config_home), path);
      sdsfree(path);
      path = newpath;
    }

    sds test_path = sdscatfmt(sdsdup(path), "/%s.module", module);
    INFO("Searching %S...", test_path);
    if (access(test_path, F_OK) != -1) {
      module_path = test_path;
      sdsfree(path);
      break;
    }

    sdsfree(test_path);
    test_path = sdscatfmt(sdsdup(path), "/%s/%s.module", module, module);
    INFO("Searching %S...", test_path);
    if (access(test_path, F_OK) != -1) {
      module_path = test_path;
      sdsfree(path);
      break;
    }

    sdsfree(path);
    sdsfree(test_path);
  }

  sdsfree(xdg_config_home);
  if (module_path == NULL) {
    FAIL("Cannot locate module %s config file", module);
    return NULL;
  }

  if (module_dir) {
    *module_dir = sdsdup(module_path);
    sdsrange(*module_dir, 0, strrchr(*module_dir, '/') - *module_dir - 1);
  }

  INFO("Found module at %S.", module_path);
  config *cfg = config_parse(module_path);
  sdsfree(module_path);

  if (cfg == NULL) {
    return NULL;
  }

  return cfg;
}

config *resolve_derived_config(config *cfg) {
  INFO("Resolving derived config...");

  INFO("Locating parent %S.", cfg->derived.base);
  config *base = load_config(cfg->derived.base, NULL);
  if (base == NULL) {
    config_free(cfg);
    return NULL;
  }
  if (base->kind != CONFIG_NATIVE_MODULE) {
    FAIL("A DerivedModule can only have a NativeModule as its base.");
    config_free(cfg);
    return NULL;
  }

  char *key = NULL;
  user_type *type;
  while ((key = table_next(&base->native.props, key, (void**)&type))) {
    sds value = table_get(&cfg->derived.value_strings, key);
    if (value == NULL) {
      if (table_get(&base->native.values, key) == NULL) {
        FAIL("Base module %S requires a value for %s.", cfg->derived.base, key);
        config_free(cfg);
        return NULL;
      }

      continue;
    }

    user_value *usr = user_value_parse(key, value, type);
    if (usr == NULL) {
      config_free(cfg);
      return NULL;
    }

    user_value *prev = table_swap(&base->native.values, key, usr);
    if (prev != NULL) {
      user_value_free(prev);
    }
  }

  base->native.native_lib = sdsdup(cfg->derived.base);

  config_free(cfg);
  return base;
}

typedef struct dl_handle dl_handle;
struct dl_handle {
  void *dl;
  uprocd_module_entry_type entry;
};

int load_dl_handle(const char *module, config *cfg, dl_handle *phandle) {
  char *last_slash = strrchr(cfg->path, '/');
  sds native_lib = cfg->native.native_lib ? sdsdup(cfg->native.native_lib) :
                   sdsnew(module);
  sds path;

  if (last_slash == NULL) {
    path = sdscat(sdsdup(native_lib), ".so");
  } else {
    path = sdsdup(cfg->path);
    sdsrange(path, 0, (last_slash - cfg->path - 1));
    path = sdscatfmt(path, "/%S.so", native_lib);
  }
  sdsfree(native_lib);

  INFO("Loading native library at %S...", path);
  void *dl = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
  sdsfree(path);

  if (dl == NULL) {
    FAIL("Failed to load native library: %s", dlerror());
    return 0;
  }

  void *entry = dlsym(dl, "uprocd_module_entry");
  if (entry == NULL) {
    FAIL("Error loading uprocd_module_entry: %s", dlerror());
    dlclose(dl);
    return 0;
  }

  phandle->dl = dl;
  phandle->entry = entry;
  return 1;
}

void interrupt_main(int sig) {
  longjmp(global_run_data.return_to_main, sig + 128);
}

void clear_child(int sig) {
  waitpid(-1, NULL, WNOHANG);
}

int main(int argc, char **argv) {
  setproctitle_init(argc, argv);

  if (argc != 3 || argv[1][0] != '+' || argv[1][1] != '\0') {
    fprintf(stderr, "uprocd should only be explicitly called by systemd!");
    return 1;
  }

  char *module = argv[2];
  setproctitle("-uprocd:%s", module);

  sds module_dir;
  config *cfg = load_config(module, &module_dir);
  if (cfg == NULL) {
    return 1;
  }

  if (cfg->kind == CONFIG_DERIVED_MODULE) {
    cfg = resolve_derived_config(cfg);
    if (cfg == NULL) {
      return 1;
    }
  }

  dl_handle handle;
  if (!load_dl_handle(module, cfg, &handle)) {
    config_free(cfg);
    return 1;
  }

  global_run_data.module = module;
  global_run_data.module_dir = module_dir;
  global_run_data.process_name = cfg->process_name ? sdsdup(cfg->process_name) : NULL;
  global_run_data.description = cfg->description ? sdsdup(cfg->description) : NULL;
  config_move_out_values(cfg, &global_run_data.config);
  global_run_data.exit_handler = NULL;
  global_run_data.exit_handler_userdata = NULL;
  global_run_data.upcoming_context = NULL;
  config_free(cfg);

  int result;
  if ((result = setjmp(global_run_data.return_to_main)) == 0) {
    INFO("Entering uprocd_run...");
    signal(SIGINT, interrupt_main);
    signal(SIGCHLD, clear_child);
    handle.entry();
  }

  sdsfree(global_run_data.module_dir);
  sdsfree(global_run_data.process_name);
  sdsfree(global_run_data.description);
  return result;
}
