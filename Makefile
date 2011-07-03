# GENERATED BY gen_charmonizer_makefiles.pl: do not hand-edit!!!

# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CC= cc
DEFS=
CFLAGS= -Isrc $(DEFS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $@

PROGNAME= charmonize

TESTS= TestDirManip TestFuncMacro TestHeaders TestIntegers TestLargeFiles TestUnusedVars TestVariadicMacros

OBJS= charmonize.o src/Charmonizer/Core/Compiler.o src/Charmonizer/Core/ConfWriter.o src/Charmonizer/Core/Dir.o src/Charmonizer/Core/HeaderChecker.o src/Charmonizer/Core/OperatingSystem.o src/Charmonizer/Core/Stat.o src/Charmonizer/Core/Util.o src/Charmonizer/Probe.o src/Charmonizer/Probe/AtomicOps.o src/Charmonizer/Probe/DirManip.o src/Charmonizer/Probe/Floats.o src/Charmonizer/Probe/FuncMacro.o src/Charmonizer/Probe/Headers.o src/Charmonizer/Probe/Integers.o src/Charmonizer/Probe/LargeFiles.o src/Charmonizer/Probe/Memory.o src/Charmonizer/Probe/UnusedVars.o src/Charmonizer/Probe/VariadicMacros.o

TEST_OBJS= src/Charmonizer/Test.o src/Charmonizer/Test/TestDirManip.o src/Charmonizer/Test/TestFuncMacro.o src/Charmonizer/Test/TestHeaders.o src/Charmonizer/Test/TestIntegers.o src/Charmonizer/Test/TestLargeFiles.o src/Charmonizer/Test/TestUnusedVars.o src/Charmonizer/Test/TestVariadicMacros.o

HEADERS= src/Charmonizer/Core/Compiler.h src/Charmonizer/Core/ConfWriter.h src/Charmonizer/Core/Defines.h src/Charmonizer/Core/Dir.h src/Charmonizer/Core/HeaderChecker.h src/Charmonizer/Core/OperatingSystem.h src/Charmonizer/Core/Stat.h src/Charmonizer/Core/Util.h src/Charmonizer/Probe.h src/Charmonizer/Probe/AtomicOps.h src/Charmonizer/Probe/DirManip.h src/Charmonizer/Probe/Floats.h src/Charmonizer/Probe/FuncMacro.h src/Charmonizer/Probe/Headers.h src/Charmonizer/Probe/Integers.h src/Charmonizer/Probe/LargeFiles.h src/Charmonizer/Probe/Memory.h src/Charmonizer/Probe/UnusedVars.h src/Charmonizer/Probe/VariadicMacros.h src/Charmonizer/Test.h

CLEANABLE= $(OBJS) $(PROGNAME) $(TEST_OBJS) $(TESTS) *.pdb

all: $(PROGNAME)

$(PROGNAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGNAME)

$(OBJS) $(TEST_OBJS): $(HEADERS)

tests: $(TESTS)

TestDirManip: src/Charmonizer/Test.o src/Charmonizer/Test/TestDirManip.o
	$(CC) $(CFLAGS) src/Charmonizer/Test/TestDirManip.o src/Charmonizer/Test.o -o $@

TestFuncMacro: src/Charmonizer/Test.o src/Charmonizer/Test/TestFuncMacro.o
	$(CC) $(CFLAGS) src/Charmonizer/Test/TestFuncMacro.o src/Charmonizer/Test.o -o $@

TestHeaders: src/Charmonizer/Test.o src/Charmonizer/Test/TestHeaders.o
	$(CC) $(CFLAGS) src/Charmonizer/Test/TestHeaders.o src/Charmonizer/Test.o -o $@

TestIntegers: src/Charmonizer/Test.o src/Charmonizer/Test/TestIntegers.o
	$(CC) $(CFLAGS) src/Charmonizer/Test/TestIntegers.o src/Charmonizer/Test.o -o $@

TestLargeFiles: src/Charmonizer/Test.o src/Charmonizer/Test/TestLargeFiles.o
	$(CC) $(CFLAGS) src/Charmonizer/Test/TestLargeFiles.o src/Charmonizer/Test.o -o $@

TestUnusedVars: src/Charmonizer/Test.o src/Charmonizer/Test/TestUnusedVars.o
	$(CC) $(CFLAGS) src/Charmonizer/Test/TestUnusedVars.o src/Charmonizer/Test.o -o $@

TestVariadicMacros: src/Charmonizer/Test.o src/Charmonizer/Test/TestVariadicMacros.o
	$(CC) $(CFLAGS) src/Charmonizer/Test/TestVariadicMacros.o src/Charmonizer/Test.o -o $@

clean:
	rm -f $(CLEANABLE)

