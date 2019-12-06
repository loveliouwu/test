#include "ini_parse.h"




/**********************************CONFIG_MODE*******************************************/

/* process a line of the INI file, storing valid values into config struct */
int config_handler_mode(void *user, const char *section, const char *name,
            const char *value)
{
    config_mode *cfg = (config_mode *)user;

    if (0) ;
    #define CFG(s, n, default) else if (strcmp(section, #s)==0 && \
        strcmp(name, #n)==0) cfg->s##_##n = strdup(value);
    #include "config.def"

    return 1;
}

/* print all the variables in the config, one per line */
void dump_config(config_mode *cfg)
{
    #define CFG(s, n, default) printf("%s_%s = %s\n", #s, #n, cfg->s##_##n);
    #include "config.def"
}
