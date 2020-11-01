#ifndef CONFIG_H
#define CONFIG_H

#ifndef DFLT_OUTPUT_ADDON
#define DFLT_OUTPUT_ADDON "_blurred"
#endif
#ifndef DFLT_JSON_ADDON
#define DFLT_JSON_ADDON "_info"
#endif
#ifndef DFLT_BACKUP_ADDON
#define DFLT_BACKUP_ADDON "_backup"
#endif
#ifndef DFLT_BLUR
#define DFLT_BLUR 70
#endif
#ifndef DFLT_COUNTRY
#define DFLT_COUNTRY "eu"
#endif
#ifndef DFLT_CONFIG_FILE
#define DFLT_CONFIG_FILE "/usr/local/share/openalpr/config/openalpr.defaults.con"
#endif
#ifndef DFLT_RUNTIME_DIR
#define DFLT_RUNTIME_DIR "/usr/local/share/openalpr/runtime_data/"
#endif
#ifndef DFLT_FAILURE_PIC_FILE
#define DFLT_FAILURE_PIC_FILE "blur_failures.txt"
#endif
#ifndef DFLT_SUCCESS_PIC_FILE
#define DFLT_SUCCESS_PIC_FILE "blur_success.txt"
#endif
#ifndef DFLT_MANUAL_BLUR_BIN
#define DFLT_MANUAL_BLUR_BIN "./build/blur_ui"
#endif


#endif