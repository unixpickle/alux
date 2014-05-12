/*
 * Copyright (c) 2014, Alex Nichol and Alux contributors.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "pic-x64.h"

namespace OS {

namespace x64 {

const uint16_t PIC1_COMMAND = 0x20;
const uint16_t PIC1_DATA = 0x21;
const uint16_t PIC2_COMMAND = 0xa0;
const uint16_t PIC2_DATA = 0xa1;

const uint8_t ICW1_ICW4 = 0x01; /* ICW4 (not) needed */
const uint8_t ICW1_INIT = 0x10; /* Initialization - required! */
const uint8_t ICW4_8086 = 0x01; /* 8086/88 (MCS-80/85) mode */

static void IOWait() {
  OutB(0x80, 0);
}

void RemapPIC(uint8_t masterBase,
              uint8_t slaveBase,
              uint8_t masterMask,
              uint8_t slaveMask) {
  OutB(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
  IOWait();
  OutB(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
  IOWait();
  OutB(PIC1_DATA, masterBase); // ICW2: Master PIC vector offset
  IOWait();
  OutB(PIC2_DATA, slaveBase); // ICW2: Slave PIC vector offset
  IOWait();
  OutB(PIC1_DATA, 4); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  IOWait();
  OutB(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
  IOWait();
 
  OutB(PIC1_DATA, ICW4_8086);
  IOWait();
  OutB(PIC2_DATA, ICW4_8086);
  IOWait();

  OutB(PIC1_DATA, masterMask);
  OutB(PIC2_DATA, slaveMask);
}

void MaskPIC(uint8_t masterMask, uint8_t slaveMask) {
  OutB(PIC1_DATA, masterMask);
  OutB(PIC2_DATA, slaveMask);
}

}

}
