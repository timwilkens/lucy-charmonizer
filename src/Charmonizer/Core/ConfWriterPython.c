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
#include "Charmonizer/Core/ConfWriterPython.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Static vars. */
static struct {
    FILE *fh;
} chaz_CWPython = { NULL };
static chaz_ConfWriter CWPython_conf_writer;

/* Open the charmony.py file handle.
 */
static void
chaz_ConfWriterPython_open_config_py(void);

static void
chaz_ConfWriterPython_clean_up(void);
static void
chaz_ConfWriterPython_vappend_conf(const char *fmt, va_list args);
static void
chaz_ConfWriterPython_add_def(const char *sym, const char *value);
static void
chaz_ConfWriterPython_add_global_def(const char *sym, const char *value);
static void
chaz_ConfWriterPython_add_typedef(const char *type, const char *alias);
static void
chaz_ConfWriterPython_add_global_typedef(const char *type, const char *alias);
static void
chaz_ConfWriterPython_add_sys_include(const char *header);
static void
chaz_ConfWriterPython_add_local_include(const char *header);
static void
chaz_ConfWriterPython_start_module(const char *module_name);
static void
chaz_ConfWriterPython_end_module(void);

void
chaz_ConfWriterPython_enable(void) {
    CWPython_conf_writer.clean_up           = chaz_ConfWriterPython_clean_up;
    CWPython_conf_writer.vappend_conf       = chaz_ConfWriterPython_vappend_conf;
    CWPython_conf_writer.add_def            = chaz_ConfWriterPython_add_def;
    CWPython_conf_writer.add_global_def     = chaz_ConfWriterPython_add_global_def;
    CWPython_conf_writer.add_typedef        = chaz_ConfWriterPython_add_typedef;
    CWPython_conf_writer.add_global_typedef = chaz_ConfWriterPython_add_global_typedef;
    CWPython_conf_writer.add_sys_include    = chaz_ConfWriterPython_add_sys_include;
    CWPython_conf_writer.add_local_include  = chaz_ConfWriterPython_add_local_include;
    CWPython_conf_writer.start_module       = chaz_ConfWriterPython_start_module;
    CWPython_conf_writer.end_module         = chaz_ConfWriterPython_end_module;
    chaz_ConfWriterPython_open_config_py();
    chaz_ConfWriter_add_writer(&CWPython_conf_writer);
    return;
}

static void
chaz_ConfWriterPython_open_config_py(void) {
    /* Open the filehandle. */
    chaz_CWPython.fh = fopen("charmony.py", "w+");
    if (chaz_CWPython.fh == NULL) {
        chaz_Util_die("Can't open 'charmony.py': %s", strerror(errno));
    }

    /* Start the module. */
    fprintf(chaz_CWPython.fh,
            "# Auto-generated by Charmonizer. \n"
            "# DO NOT EDIT THIS FILE!!\n"
            "\n"
            "class Charmony(object):\n"
            "    @classmethod\n"
            "    def config(cls):\n"
            "        return cls.defs\n"
            "\n"
            "    defs = {}\n"
            "\n"
           );
}

static void
chaz_ConfWriterPython_clean_up(void) {
    /* No more code necessary to finish charmony.py, so just close. */
    if (fclose(chaz_CWPython.fh)) {
        chaz_Util_die("Couldn't close 'charmony.py': %s", strerror(errno));
    }
}

static void
chaz_ConfWriterPython_vappend_conf(const char *fmt, va_list args) {
    (void)fmt;
    (void)args;
}

static char*
chaz_ConfWriterPython_quotify(const char *string, char *buf, size_t buf_size) {
    char *quoted = buf;

    /* Don't bother with NULL values here. */
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

#define CFPYTHON_MAX_BUF 100
static void
chaz_ConfWriterPython_add_def(const char *sym, const char *value) {
    char sym_buf[CFPYTHON_MAX_BUF + 1];
    char value_buf[CFPYTHON_MAX_BUF + 1];
    char *quoted_sym;
    char *quoted_value;

    /* Quote key. */
    if (!sym) {
        chaz_Util_die("Can't handle NULL key");
    }
    quoted_sym = chaz_ConfWriterPython_quotify(sym, sym_buf, CFPYTHON_MAX_BUF);

    /* Quote value or use "None". */
    if (!value) {
        strcpy(value_buf, "None");
        quoted_value = value_buf;
    }
    else {
        quoted_value = chaz_ConfWriterPython_quotify(value, value_buf,
                                                     CFPYTHON_MAX_BUF);
    }

    fprintf(chaz_CWPython.fh, "    defs[%s] = %s\n", quoted_sym, quoted_value);

    if (quoted_sym   != sym_buf)   { free(quoted_sym);   }
    if (quoted_value != value_buf) { free(quoted_value); }
}

static void
chaz_ConfWriterPython_add_global_def(const char *sym, const char *value) {
    (void)sym;
    (void)value;
}

static void
chaz_ConfWriterPython_add_typedef(const char *type, const char *alias) {
    (void)type;
    (void)alias;
}

static void
chaz_ConfWriterPython_add_global_typedef(const char *type, const char *alias) {
    (void)type;
    (void)alias;
}

static void
chaz_ConfWriterPython_add_sys_include(const char *header) {
    (void)header;
}

static void
chaz_ConfWriterPython_add_local_include(const char *header) {
    (void)header;
}

static void
chaz_ConfWriterPython_start_module(const char *module_name) {
    fprintf(chaz_CWPython.fh, "    # %s\n", module_name);
}

static void
chaz_ConfWriterPython_end_module(void) {
    fprintf(chaz_CWPython.fh, "\n");
}
