/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     trap_c.c
 * @brief    source file for the trap process
 * @version  V1.0
 * @date     12. December 2017
 ******************************************************************************/
#include "typesdef.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <csi_config.h>
#include <csi_core.h>
#include <csi_kernel.h>
#include "osal/string.h"
#include "osal/task.h"
#include <k_api.h>

extern void csi_kernel_task_dump(k_task_handle_t task_handle, void* stack);

void hgprintf(const char *fmt, ...);
#define trap_err(fmt,...) hgprintf(KERN_EMERG fmt, ##__VA_ARGS__)

void trap_c(uint32_t *regs)
{
    int i;
//    uint32_t *stack_top;
    uint32_t vec = (__get_PSR() & PSR_VEC_Msk) >> PSR_VEC_Pos;

    disable_print(0);
    trap_err("---------------------------------------------------------------\r\n");
    trap_err("CPU Exception: NO.%u\r\n", vec);
    for (i = 0; i < 16; i++) {
        trap_err("r%d: %08x\t", i, regs[i]);
        if ((i % 4) == 3) {
            trap_err("\r\n");
        }
    }
    trap_err("r28 : %08x\r\n", regs[16]);
    trap_err("epsr: %08x\r\n", regs[17]);
    trap_err("epc : %08x\r\n", regs[18]);
    csi_kernel_task_dump((k_task_handle_t)g_active_task[0], (void* )regs[14]);
    mcu_reset();
}

