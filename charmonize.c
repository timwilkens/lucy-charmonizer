/* charmonize.c -- write lucyconf.h config file.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "Charmonizer.h"
#include "Charmonizer/Probe/FuncMacro.h"
#include "Charmonizer/Probe/Headers.h"
#include "Charmonizer/Probe/Integers.h"
#include "Charmonizer/Probe/LargeFiles.h"
#include "Charmonizer/Probe/UnusedVars.h"
#include "Charmonizer/Probe/VariadicMacros.h"

/* Process command line args, set up Charmonizer, etc. Returns the outpath
 * (where the config file should be written to).
 */
FILE*
init(int argc, char **argv);

/* Find <tag_name> and </tag_name> within a string and return the text between
 * them as a newly allocated substring.
 */
static char*
extract_delim(char *source, size_t source_len, const char *tag_name);

/* Version of extract delim which dies rather than returns NULL upon failure.
 */
static char*
extract_delim_and_verify(char *source, size_t source_len, 
                         const char *tag_name);

/* Start the config file.
 */
static void 
start_conf_file();

/* Write the last bit of the config file.
 */
static void 
finish_conf_file(FILE *fh);

/* Print a message to stderr and exit.
 */
void
die(char *format, ...);

int main(int argc, char **argv) 
{
    FILE *config_fh = init(argc, argv);

    /* modules section */
    chaz_Headers_run(config_fh);
    chaz_FuncMacro_run(config_fh);
    chaz_Integers_run(config_fh);
    chaz_LargeFiles_run(config_fh);
    chaz_UnusedVars_run(config_fh);
    chaz_VariadicMacros_run(config_fh);

    /* write tail of config and clean up */
    finish_conf_file(config_fh);
    if (fclose(config_fh))
        die("Error closing config file: %s", strerror(errno));
    chaz_clean_up();

    return 0;
}

FILE* 
init(int argc, char **argv) 
{
    char *outpath, *cc_command, *cc_flags, *os_name, *verbosity_str;
    char *infile_str;
    size_t infile_len;
    FILE *conf_fh;

    /* parse the infile */
    if (argc != 2)
        die("Usage: ./charmonize INFILE");
    infile_str = chaz_slurp_file(argv[1], &infile_len);
    cc_command = extract_delim_and_verify(infile_str, infile_len, 
        "charm_cc_command");
    cc_flags = extract_delim_and_verify(infile_str, infile_len, 
        "charm_cc_flags");
    outpath = extract_delim_and_verify(infile_str, infile_len, 
        "charm_outpath");
    os_name = extract_delim_and_verify(infile_str, infile_len, 
        "charm_os_name");
    verbosity_str = extract_delim(infile_str, infile_len, "charm_verbosity");

    /* open outfile */
    conf_fh = fopen(outpath, "w");
    if (conf_fh == NULL)
        die("Couldn't open '%s': %s", strerror(errno));
    start_conf_file(conf_fh);

    /* set up Charmonizer */
    if (verbosity_str != NULL) {
        const long verbosity = strtol(verbosity_str, NULL, 10);
        chaz_set_verbosity(verbosity);
    }
    chaz_init(conf_fh, os_name, cc_command, cc_flags);
    chaz_set_prefixes("LUCY_", "Lucy_", "lucy_", "lucy_");
    chaz_write_charm_test_h();

    /* clean up */
    free(infile_str);
    free(cc_command);
    free(cc_flags);
    free(os_name);
    free(outpath);
    free(verbosity_str);

    return conf_fh;
}

static char*
extract_delim(char *source, size_t source_len, const char *tag_name)
{
    const size_t tag_name_len = strlen(tag_name);
    const size_t opening_delim_len = tag_name_len + 2;
    const size_t closing_delim_len = tag_name_len + 3;
    char opening_delim[100];
    char closing_delim[100];
    const char *limit = source + source_len - closing_delim_len;
    char *start, *end;
    char *retval = NULL;

    /* sanity check, then create delimiter strings to match against */
    if (tag_name_len > 95)
        die("tag name too long: '%s'");
    sprintf(opening_delim, "<%s>", tag_name);
    sprintf(closing_delim, "</%s>", tag_name);
    
    /* find opening <delimiter> */
    for (start = source; start < limit; start++) {
        if (strncmp(start, opening_delim, opening_delim_len) == 0) {
            start += opening_delim_len;
            break;
        }
    }

    /* find closing </delimiter> */
    for (end = start; end < limit; end++) {
        if (strncmp(end, closing_delim, closing_delim_len) == 0) {
            const size_t retval_len = end - start;
            retval = (char*)malloc((retval_len + 1) * sizeof(char));
            retval[retval_len] = '\0';
            strncpy(retval, start, retval_len);
            break;
        }
    }
    
    return retval;
}

static char*
extract_delim_and_verify(char *source, size_t source_len, const char *tag_name)
{
    char *retval = extract_delim(source, source_len, tag_name);
    if (retval == NULL)
        die("Couldn't extract value for '%s'", tag_name);
    return retval;
}

static void 
start_conf_file(FILE *conf_fh) 
{
    fprintf(conf_fh,
        "/* Header file auto-generated by Charmonizer. \n"
        " * DO NOT EDIT THIS FILE!!\n"
        " */\n\n"
        "#ifndef H_LUCY_CONF\n"
        "#define H_LUCY_CONF 1\n\n"
    );
}

static void
finish_conf_file(FILE *conf_fh) 
{
    fprintf(conf_fh, "\n\n#endif /* H_LUCY_CONF */\n\n");
}

void 
die(char* format, ...) 
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

/**
 * Copyright 2006 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

