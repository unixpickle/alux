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

#include <platform/multiprocessing.h>
#include <platform/interrupts.h>
#include "interrupts/int-handlers-x64.h"
#include "interrupts/pic-x64.h"
#include "interrupts/ioapic-x64.h"
#include "interrupts/lapic-x64.h"

namespace OS {

void InitializeProcessors() {
  x64::InitializeIDT();
  x64::ConfigureDummyIDT();
  x64::GetGlobalIDT().Load();
  cout << "OS::InitializeProcessors() - disabling PIC..." << endl;
  x64::RemapPIC(0xf0, 0xf8, 0xff, 0xff);
  SetInterruptsEnabled(true);
  // if there are a few interrupts masked, we can trigger our handlers for all
  // of them and send appropriate EOIs to the PIC.
  __asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
  SetInterruptsEnabled(false);
  x64::ConfigureRealIDT();
  
  x64::InitializeIOAPIC();
  x64::IOAPIC & apic = x64::GetBaseIOAPIC();
  for (uint32_t i = 0; i < apic.GetPinCount(); i++) {
    apic.MaskPin(i);
  }
  apic.MapIRQ(0, 0x20);
  apic.MapIRQ(1, 0x21);
  apic.MapIRQ(6, 0x26);
  x64::IOAPIC::StartUsing();

  x64::InitializeLocalAPIC();
  x64::LAPIC & lapic = x64::GetLocalAPIC();

  cout << "OS::InitializeProcessors() - enabling first LAPIC..." << endl;
  lapic.SetDefaults();
  lapic.Enable();
  
  Panic("TODO: send CPU IPIs here");
}

}
