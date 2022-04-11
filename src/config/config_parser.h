#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

#include "ConfigParams.h"

/**
 * Parse all needed params from the config file located next to the binary. 
 */
ConfigParams parseConfig(int argc, const char *argv[]);

#endif // __CONFIG_PARSER_H__