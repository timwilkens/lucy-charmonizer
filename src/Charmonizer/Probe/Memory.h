/* Charmonizer/Probe/Memory.h
 */

#ifndef H_CHAZ_MEMORY
#define H_CHAZ_MEMORY 

#ifdef __cplusplus
extern "C" {
#endif

/* The Memory module attempts to detect these symbols or alias them to
 * synonyms:
 * 
 * alloca
 *
 * These following symbols will be defined if the associated headers are
 * available:
 * 
 * HAS_ALLOCA_H            <alloca.h> 
 * HAS_MALLOC_H            <malloc.h>
 */
void chaz_Memory_run(void);

#ifdef CHAZ_USE_SHORT_NAMES
  #define Memory_run    chaz_Memory_run
#endif

#ifdef __cplusplus
}
#endif

#endif /* H_CHAZ_MEMORY */


/**
 * Copyright 2010 The Apache Software Foundation
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

