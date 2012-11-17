/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Charmonizer/Core/Util.h"
#include "Charmonizer/Core/ConfWriter.h"
#include "Charmonizer/Core/ConfWriterPerl.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Static vars. */
static struct {
    FILE *fh;
} chaz_CWPerl = { NULL };
static chaz_ConfWriter CWPerl_conf_writer;

/* Open the Charmony.pm file handle.
 */
static void
chaz_ConfWriterPerl_open_config_pm(void);

static void
chaz_ConfWriterPerl_clean_up(void);
static void
chaz_ConfWriterPerl_vappend_conf(const char *fmt, va_list args);
static void
chaz_ConfWriterPerl_add_def(const char *sym, const char *value);
static void
chaz_ConfWriterPerl_add_global_def(const char *sym, const char *value);
static void
chaz_ConfWriterPerl_add_typedef(const char *type, const char *alias);
static void
chaz_ConfWriterPerl_add_global_typedef(const char *type, const char *alias);
static void
chaz_ConfWriterPerl_add_sys_include(const char *header);
static void
chaz_ConfWriterPerl_add_local_include(const char *header);
static void
chaz_ConfWriterPerl_start_module(const char *module_name);
static void
chaz_ConfWriterPerl_end_module(void);

void
chaz_ConfWriterPerl_enable(void) {
    CWPerl_conf_writer.clean_up           = chaz_ConfWriterPerl_clean_up;
    CWPerl_conf_writer.vappend_conf       = chaz_ConfWriterPerl_vappend_conf;
    CWPerl_conf_writer.add_def            = chaz_ConfWriterPerl_add_def;
    CWPerl_conf_writer.add_global_def     = chaz_ConfWriterPerl_add_global_def;
    CWPerl_conf_writer.add_typedef        = chaz_ConfWriterPerl_add_typedef;
    CWPerl_conf_writer.add_global_typedef = chaz_ConfWriterPerl_add_global_typedef;
    CWPerl_conf_writer.add_sys_include    = chaz_ConfWriterPerl_add_sys_include;
    CWPerl_conf_writer.add_local_include  = chaz_ConfWriterPerl_add_local_include;
    CWPerl_conf_writer.start_module       = chaz_ConfWriterPerl_start_module;
    CWPerl_conf_writer.end_module         = chaz_ConfWriterPerl_end_module;
    chaz_ConfWriterPerl_open_config_pm();
    chaz_ConfWriter_add_writer(&CWPerl_conf_writer);
    return;
}

static void
chaz_ConfWriterPerl_open_config_pm(void) {
    /* Open the filehandle. */
    chaz_CWPerl.fh = fopen("Charmony.pm", "w+");
    if (chaz_CWPerl.fh == NULL) {
        chaz_Util_die("Can't open 'Charmony.pm': %s", strerror(errno));
    }

    /* Start the module. */
    fprintf(chaz_CWPerl.fh,
            "# Auto-generated by Charmonizer. \n"
            "# DO NOT EDIT THIS FILE!!\n"
            "\n"
            "package Charmony;\n"
            "use strict;\n"
            "use warnings;\n"
            "\n"
            "my %%defs;\n"
            "\n"
            "sub config { \\%%defs }\n"
            "\n"
           );
}

static void
chaz_ConfWriterPerl_clean_up(void) {
    /* Write the last bit of Charmony.pm and close. */
    fprintf(chaz_CWPerl.fh, "\n1;\n\n");
    if (fclose(chaz_CWPerl.fh)) {
        chaz_Util_die("Couldn't close 'Charmony.pm': %s", strerror(errno));
    }
}

static void
chaz_ConfWriterPerl_vappend_conf(const char *fmt, va_list args) {
    (void)fmt;
    (void)args;
}

static char*
chaz_ConfWriterPerl_quotify(const char *string, char *buf, size_t buf_size) {
    char *quoted = buf;

    /* Don't bother with undef values here. */
    if (!string) {
        return NULL;
    }

    /* Allocate memory if necessary. */
    {
        const char *ptr;
        size_t space = 3; /* Quotes plus NUL termination. */
        for (ptr = string; *ptr; ptr++) {
            if (*ptr == '\'' || *ptr == '\\') {
                space += 2;
            }
            else {
                space += 1;
            }
        }
        if (space > buf_size) {
            quoted = (char*)malloc(space);
        }
    }

    /* Perform copying and escaping */
    {
        const char *ptr;
        size_t pos = 0;
        quoted[pos++] = '\'';
        for (ptr = string; *ptr; ptr++) {
            if (*ptr == '\'' || *ptr == '\\') {
                quoted[pos++] = '\\';
            }
            quoted[pos++] = *ptr;
        }
        quoted[pos++] = '\'';
        quoted[pos++] = '\0';
    }

    return quoted;
}

#define CFPERL_MAX_BUF 100
static void
chaz_ConfWriterPerl_add_def(const char *sym, const char *value) {
    char sym_buf[CFPERL_MAX_BUF + 1];
    char value_buf[CFPERL_MAX_BUF + 1];
    char *quoted_sym;
    char *quoted_value;

    /* Quote key. */
    if (!sym) {
        chaz_Util_die("Can't handle NULL key");
    }
    quoted_sym = chaz_ConfWriterPerl_quotify(sym, sym_buf, CFPERL_MAX_BUF);

    /* Quote value or use "undef". */
    if (!value) {
        strcpy(value_buf, "undef");
        quoted_value = value_buf;
    }
    else {
        quoted_value = chaz_ConfWriterPerl_quotify(value, value_buf,
                                                CFPERL_MAX_BUF);
    }

    fprintf(chaz_CWPerl.fh, "$defs{%s} = %s;\n", quoted_sym, quoted_value);

    if (quoted_sym   != sym_buf)   { free(quoted_sym);   }
    if (quoted_value != value_buf) { free(quoted_value); }
}

static void
chaz_ConfWriterPerl_add_global_def(const char *sym, const char *value) {
    (void)sym;
    (void)value;
}

static void
chaz_ConfWriterPerl_add_typedef(const char *type, const char *alias) {
    (void)type;
    (void)alias;
}

static void
chaz_ConfWriterPerl_add_global_typedef(const char *type, const char *alias) {
    (void)type;
    (void)alias;
}

static void
chaz_ConfWriterPerl_add_sys_include(const char *header) {
    (void)header;
}

static void
chaz_ConfWriterPerl_add_local_include(const char *header) {
    (void)header;
}

static void
chaz_ConfWriterPerl_start_module(const char *module_name) {
    fprintf(chaz_CWPerl.fh, "# %s\n", module_name);
}

static void
chaz_ConfWriterPerl_end_module(void) {
    fprintf(chaz_CWPerl.fh, "\n");
}

