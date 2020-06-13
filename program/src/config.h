#ifndef CONFIG_H
#define CONFIG_H

#define DFLT_OUTPUT_ADDON "_blured"
#define DFLT_JSON_ADDON "_info"
#define DFLT_BACKUP_ADDON "_backup"
#define DFLT_BLUR 70
#define DFLT_COUNTRY "eu"
#ifndef DFLT_CONFIG_FILE
#define DFLT_CONFIG_FILE "/usr/local/share/openalpr/config/openalpr.defaults.con"
#endif
#ifndef DFLT_RUNTIME_DIR
#define DFLT_RUNTIME_DIR "/usr/local/share/openalpr/runtime_data/"
#endif
#define DFLT_FAILURE_PIC_FILE "blur_failures.txt"
#define DFLT_SUCCESS_PIC_FILE "blur_success.txt"
#ifndef DFLT_MANUAL_BLUR_BIN
#define DFLT_MANUAL_BLUR_BIN "./build/blur_ui"
#endif


#endif