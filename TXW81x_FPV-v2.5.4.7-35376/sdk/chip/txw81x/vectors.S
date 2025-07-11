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
 * @file     vectors.S
 * @brief    define default vector handlers. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     28. Nove 2017
 ******************************************************************************/

#include <csi_config.h>

.import trap_c

.section .trap_stack
    .align  2
    .globl  g_trapstackalloc
    .global g_trapstackbase
    .global g_top_trapstack
g_trapstackalloc:
g_trapstackbase:
    .space 512
g_top_trapstack:

.text

/******************************************************************************
 * Functions:
 *     uint32_t __get_PC(uint32_t);
 * default exception handler
 ******************************************************************************/
    .global __get_PC
    .type   __get_PC, %function
__get_PC:
	grs r0, __get_PC
	jmp r15

/******************************************************************************
 * Functions:
 *     void trap(void);
 * default exception handler
 ******************************************************************************/
    .global trap
    .type   trap, %function
trap:
    psrset  ee
    subi    sp, 4
    stw     r0, (sp)

    lrw     r0, g_top_trapstack
    subi    r0, 76
    stm     r0-r15, (r0)
    stw     r28, (r0, 64)

    ldw     r1, (sp)
    addi    sp, 4
    stw     sp, (r0, 56) /* save r14 */
    mov     sp, r0
    stw     r1, (sp)     /* save r0 */

    mfcr    r1, epsr
    stw     r1, (sp, 68)
    mfcr    r1, epc
    stw     r1, (sp, 72)

    jbsr    trap_c

    .align  2
    .weak   Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    br      trap
    .size   Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro  def_irq_handler handler_name
    .weak   \handler_name
    .globl  \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler trap0_handler
    def_irq_handler tspend_handler
    def_irq_handler USB20DMA_USB11SOF_IRQHandler
    def_irq_handler USB20MC_USB11CTL_IRQHandler
    def_irq_handler UART0_IRQHandler
    def_irq_handler UART1_IRQHandler
    def_irq_handler UART2_IRQHandler
    def_irq_handler QSPI_IRQHandler
    def_irq_handler SPI0_IRQHandler
    def_irq_handler SPI1_IRQHandler
    def_irq_handler SPI2_IRQHandler
    def_irq_handler SPI3_IRQHandler
    def_irq_handler TIM0_IRQHandler
    def_irq_handler TIM1_IRQHandler
    def_irq_handler TIM2_IRQHandler
    def_irq_handler TIM3_IRQHandler
    def_irq_handler TIM4_IRQHandler
    def_irq_handler TIM5_IRQHandler
    def_irq_handler TIM6_IRQHandler
    def_irq_handler TIM7_IRQHandler
    def_irq_handler SDIO_IRQHandler
    def_irq_handler SDIO_RST_IRQHandler
    def_irq_handler SDHOST_IRQHandler
    def_irq_handler LMAC_IRQHandler
    def_irq_handler GMAC_IRQHandler
    def_irq_handler M2M0_IRQHandler
    def_irq_handler M2M1_IRQHandler
    def_irq_handler CORET_IRQHandler
    def_irq_handler SYSAES_IRQHandler
    def_irq_handler CRC_IRQHandler
    def_irq_handler ADKEY_IRQHandler
    def_irq_handler PD_TMR_IRQHandler
    def_irq_handler WKPND_IRQHandler
    def_irq_handler PDWKPND_IRQHandler
    def_irq_handler LVD_IRQHandler
    def_irq_handler WDT_IRQHandler
    def_irq_handler SYS_ERR_IRQHandler
    def_irq_handler IIS0_IRQHandler
    def_irq_handler IIS1_IRQHandler
    def_irq_handler GPIOA_IRQHandler
    def_irq_handler GPIOB_IRQHandler
    def_irq_handler DVP_IRQHandler
    def_irq_handler JPG_IRQHandler
