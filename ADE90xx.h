/**
 **********************************************************************************
 * @file   lib.h
 * @author Ali Moallem (https://github.com/AliMoal)
 * @brief  
 **********************************************************************************
 *
 *! Copyright (c) 2021 Mahda Embedded System (MIT License)
 *!
 *! Permission is hereby granted, free of charge, to any person obtaining a copy
 *! of this software and associated documentation files (the "Software"), to deal
 *! in the Software without restriction, including without limitation the rights
 *! to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *! copies of the Software, and to permit persons to whom the Software is
 *! furnished to do so, subject to the following conditions:
 *!
 *! The above copyright notice and this permission notice shall be included in all
 *! copies or substantial portions of the Software.
 *!
 *! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *! OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *! SOFTWARE.
 *!
 **********************************************************************************
 **/

//* Define to prevent recursive inclusion ---------------------------------------- //
#ifndef ADE900XX_H
#define ADE900XX_H

#ifdef __cplusplus
extern "C" {
#endif

//* Includes ---------------------------------------------------------------------- //
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "ADE90xxReg.h"

//? User Configurations and Notes ------------------------------------------------- //
//! Important Notes:
//  1. SPI Configuration: 8Bits, CPOL=LOW(0), CPHA=1EDGE(0), Max speed: 20MHz(ADE9000) - 10MHz(ADE9078), (referred to Datasheet)
//  2. Tie PM0 and PM1 to LOW(ground) to activate the normal mode OR implement the related functions in ADE90xx_Handler_t
//! 3. Pass ADE90xx_SAVE_DATA_VALUE define as 1 whenever you want Getting/Reading or Setting/Writing functions work FASTER.
//!    BE CAREFUL if you pass this as 1, All the data in register(for Setting/Writing) or variable(for Getting/Reading) with
//!    every Get/Read or Set/Write function lost and get new ones. It means that the deselected bits will be set as ZERO
//*    Although You can use WriteFast functions without passing ADE90xx_SAVE_DATA_VALUE define as 1
//! NOT TESTED // 4. If using Non-blocking mode, User must handle interrupt by own, Otherwise for Blocking mode user just needs to put
//! NOT TESTED //    IRQx Handlers in interrupt callbacks OR define IRQx read functions in ADE90xx_Handler_t. 
//! NOT TESTED //    It is strongly recommended to update IRQx pins with interrupt.
#define ADE90xx_VoltageFactor          991          // Factor of Voltage from user circut structure
#define ADE90xx_CurrentFactor          10           // Factor of Current from user circut structure
#define ADE90xx_ChooseFullScale         0           // 0: ADE9078 | 1 : ADE9000 | Others: The same as 0
#define ADE90xx_USE_FLOAT               0           // 0: use double data type (FPType: double) | 1: use float data type (FPType: float)
#define ADE90xx_SAVE_DATA_VALUE         0           // 0: Saving previous data | 1: Getting new data | Others: The same as 0 | READ NOTE ABOVE NO. 3 
#define ADE90xx_USE_MACRO_DELAY         0           // 0: Use handler delay ,So you have to set ADE90xx_Delay in Handler | 1: use Macro delay, So you have to set ADE90xx_MACRO_DELAY Macros | Others: The same as 0
// #define ADE90xx_MACRO_DELAY_US(x)                   // If you want to use Macro delay, place your delay function in microseconds here
// #define ADE90xx_MACRO_DELAY_MS(x)                   // If you want to use Macro delay, place your delay function in miliseconds here
#define ADE90xx_Debug_Enable            0           // 0: Disable debug | 1: Enable debug (depends on printf in stdio.h) | Others: The same as 0
// #define ADE90xx_BLOCKING_MODE           0           //! Do Not Use It, It's not tested !// 0: Non-blocking Mode | 1: Blocking Mode (Handler) | 2: Blocking Mode (Read Functions) | Others: The same as 0 | READ NOTE ABOVE NO. 4 
// #define ADE90xx_INLINE_ATTRIBUTE      inline        // Replace your inline attribute - It is useful when you want to use force inline (i.e. __forceinline in Keil Software)
// #pragma anon_unions                                 // Uncomment if you are using Keil software
//? ------------------------------------------------------------------------------- //

//* Defines and Macros ------------------------------------------------------------ //
#define ADE90xx_ReadReg(h, r, b, d)   ADE90xx_ReadRegLL(h, ADE_ADD_##r, ADE_SIZE_##r, ADE_SB_##b, ADE_MB_##b, d)
#define ADE90xx_WriteReg(h, r, b, d)  ADE90xx_WriteRegLL(h, ADE_ADD_##r, ADE_SIZE_##r, ADE_SB_##b, ADE_MB_##b, d)


//! DO NOT USE OR EDIT THIS BLOCK ------------------------------------------------- //
#ifdef ADE90xx_USE_FLOAT
#if ADE90xx_USE_FLOAT == 1
#define FPType   float
#else
#define FPType   double
#endif
#else
#define FPType   double
#endif
//! ------------------------------------------------------------------------------- //

/**
 ** ==================================================================================
 **                                 ##### Enums #####                               
 ** ==================================================================================
 **/

// Informations will be completed later...

// Be careful in the boolean data commands, if user selects anything except 0 and 1, it will be seen as 1
enum MainCommands {
  ADE_DISRPLPF = 0,  // 0/1: Enable/Disable the low-pass filter in the total reactive power datapath.
  ADE_DISAPLPF,      // 0/1: Enable/Disable the low-pass filter in the total active power datapath.
  ADE_ININTEN,       // 0/1: Disable/Enable the digital integrator in the Neutral Current channel.
  ADE_VNOMC_EN,      // 0/1: Not use/Use the nominal phase voltage rms, VNOM,
                     //      in the computation of Phase C total apparent power, CVA.
  ADE_VNOMB_EN,      // 0/1: Not use/Use the nominal phase voltage rms, VNOM,
                     //      in the computation of Phase B total apparent power, BVA.
  ADE_VNOMA_EN,      // 0/1: Not use/Use the nominal phase voltage rms, VNOM,
                     //      in the computation of Phase A total apparent power, AVA.
  ADE_ZX_SRC_SEL,    // 0/1: Data going into the zero-crossing detection circuit comes after/before the high-pass filter,
                     //      integrator, and phase compensation.
  ADE_INTEN,         // 0/1: Disable/Enable the integrators in the phase current channels.
                     //      The neutral current channel integrator is managed by the ADE_ININTEN.
  ADE_MTEN,          // 0/1: Disable/Enable multipoint phase and gain compensation.
                     //      If enabled, an additional gain factor, xIGAIN0 through xIGAIN4,
                     //      is applied to the current channel based on the xIRMS current rms amplitude and
                     //      the MTTHR_Lx and MTTHR_Hx register values.
  ADE_HPFDIS,        // 0/1: Enable/Disable high-pass filters in all the voltage and current channels.
  ADE_ISUM_CFG,      // 00:  ISUM = AI_PCF + BI_PCF + CI_PCF (for approximated neutral current rms calculation).
                     // 01:  ISUM = AI_PCF + BI_PCF + CI_PCF + NI_PCF (to determine mismatch between neutral and phase currents).
                     // 10:  ISUM = AI_PCF + BI_PCF + CI_PCF - NI_PCF (to determine mismatch between neutral and phase currents).
                     // 11:  ISUM = AI_PCF + BI_PCF + CI_PCF (for approximated neutral current rms calculation).
  ADE_EXT_REF,       // 0/1: Not use/Use an external voltage reference.
  ADE_IRQ0_ON_IRQ1,  // 0/1: Not combine/Combine all the interrupts onto a single interrupt pin, IRQ1, instead of using two pins, IRQ0 and IRQ1. 
                     //      Note that the IRQ0 pin still indicates the enabled IRQ0 events while in this mode and
                     //      the IRQ1 indicates both IRQ1 and IRQ0 events.
  ADE_BURST_EN,      // 0/1: Disable/Enable burst read functionality on the registers from Address 0x500 to Address 0x6FF. 
                     //      Note that this bit disables the CRC being appended to SPI register reads.
  ADE_PWR_SETTLE,    // 00:  64ms  time for the power and filter based rms measurements to settle before starting the power,
                     //      energy and CF accumulations.
                     // 01:  128ms time for the power and filter based rms measurements to settle before starting the power,
                     //      energy and CF accumulations.  
                     // 10:  256ms time for the power and filter based rms measurements to settle before starting the power,
                     //      energy and CF accumulations.  
                     // 11:  0ms   time for the power and filter based rms measurements to settle before starting the power,
                     //      energy and CF accumulations.
  ADE_CF_ACC_CLR,    // 0/1: Not clear/Clear r the accumulation in the digital to frequency converter and CFDEN counter.
                     //      Note that this bit automatically clears itself.
  ADE_CF4_CFG,       // 00:  Output on the CF4 pin is CF4, from digital to frequency converter.
                     // 01:  Output on the CF4 pin is CF4, from digital to frequency converter.
                     // 10:  Output on the CF4 pin is EVENT.
                     // 11:  Output on the CF4 pin is DREADY.
  ADE_CF3_CFG,       // 0:   Output on the CF3 pin is CF3, from digital to frequency converter.
                     // 1:   Output on the CF3 pin is Zero Crossing output selected by the ADE_ZX_SEL.
  ADE_SWRST,         // 0/1: Not initiate/Initiate a software reset. Note that this is self clearing.
  ADE_UPERIOD_SEL,   // 0/1: Not use/Use a user configured line period, in ADE_USER_PERIOD, for the resampling calculation.
                     //      If not use, the phase voltage line period selected by the ADE_LP_SEL.
  ADE_HPF_CRN,       // 000: 38.695Hz  High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
                     // 001: 19.6375Hz High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
                     // 010: 9.895Hz   High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
                     // 011: 4.9675HZ  High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
                     // 100: 2.49Hz    High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
                     // 101: 1.2475Hz  High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
                     // 110: 0.625Hz   High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
                     // 111: 0.3125HZ  High-pass filter corner (f3dB) enabled when the ADE_HPFDIS is 0.
  ADE_PEAKSEL,       // xx0/1: Disable/Enable phase A detection to monitor peak voltages and currents on. 
                     // x0/1x: Disable/Enable phase B detection to monitor peak voltages and currents on. 
                     // 0/1xx: Disable/Enable phase C detection to monitor peak voltages and currents on. 
};

enum IntEvCommands {
  ADE_IntEv_MISMTCH,       // 0/1: This indicates Not change/Change in the relationship between ISUMRMS and ISUMLVL.
  ADE_IntEv_COH_WFB_FULL,  // 0/1: The waveform buffer Is not/Is full with resampled data, which is selected when WF_CAP_SEL = 0 in the WFB_CFG register.
  ADE_IntEv_WFB_TRIG,      // 0/1: One of the events configured in WFB_TRIG_CFG Does not occur/Occours.
  ADE_IntEv_PF_RDY,        // 0/1: The power factor measurements Have not/Have  been updated, every 1.024 sec
  ADE_IntEv_PWRRDY,        // 0/1: The power values in the xWATT_ACC, xVA_ACC, xVAR_ACC, xFVAR_ACC registers Have not/Have been updated, after PWR_TIME 4 kSPS samples.
  ADE_IntEv_PAGE_FULL,     // 0/1: A page is Not enabled/Enabled in the ADE_WFB_WFB_PG_IRQEN register has been filled with fixed data rate samples, when ADE_WFB_WF_CAP_SEL is 1.
  ADE_IntEv_WFB_TRIG_IRQ,  // 0/1: The waveform buffer Has not/Has stopped filling after an event configured in WFB_TRIG_CFG occurs. 
                           //      This happens with fixed data rate samples only, when ADE_WFB_WF_CAP_SEL = 0.
  ADE_IntEv_DREADY,        // 0/1: New waveform samples Are not/Are ready. The update rate depends on the data selected in the ADE_WFB_WF_SRC.
  ADE_IntEv_CF4,           // 0/1: When a CF4 pulse Is not/Is issued, when the CF4 pin goes from a high to low state.
  ADE_IntEv_CF3,           // 0/1: When a CF3 pulse Is not/Is issued, when the CF3 pin goes from a high to low state.
  ADE_IntEv_CF2,           // 0/1: When a CF2 pulse Is not/Is issued, when the CF2 pin goes from a high to low state.
  ADE_IntEv_CF1,           // 0/1: When a CF1 pulse Is not/Is issued, when the CF1 pin goes from a high to low state.
  ADE_IntEv_REVPSUM4,      // 0/1: When the CF4 polarity Did not change/Changed sign.
                           //      For example, if the last CF4 pulse was positive reactive energy and the next CF4 pulse is negative reactive energy, the REVPSUM4 bit is set. 
                           //      This bit is updated when a CF4 pulse is output, when the CF4 pin goes from high to low.
  ADE_IntEv_REVPSUM3,      // 0/1: When the CF3 polarity Did not change/Changed sign.
                           //      For example, if the last CF3 pulse was positive reactive energy and the next CF3 pulse is negative reactive energy, the REVPSUM3 bit is set. 
                           //      This bit is updated when a CF3 pulse is output, when the CF3 pin goes from high to low.
  ADE_IntEv_REVPSUM2,      // 0/1: When the CF2 polarity Did not change/Changed sign.
                           //      For example, if the last CF2 pulse was positive reactive energy and the next CF2 pulse is negative reactive energy, the REVPSUM2 bit is set. 
                           //      This bit is updated when a CF2 pulse is output, when the CF2 pin goes from high to low.
  ADE_IntEv_REVPSUM1,      // 0/1: When the CF1 polarity Did not change/Changed sign.
                           //      For example, if the last CF1 pulse was positive reactive energy and the next CF1 pulse is negative reactive energy, the REVPSUM1 bit is set. 
                           //      This bit is updated when a CF1 pulse is output, when the CF1 pin goes from high to low.
  ADE_IntEv_REVRPC,        // 0/1: The Phase C total or fundamental reactive power Has not/Has changed sign.
  ADE_IntEv_REVRPB,        // 0/1: The Phase B total or fundamental reactive power Has not/Has changed sign.
  ADE_IntEv_REVRPA,        // 0/1: The Phase A total or fundamental reactive power Has not/Has changed sign.
  ADE_IntEv_REVAPC,        // 0/1: The Phase C total or fundamental active power Has not/Has changed sign.
  ADE_IntEv_REVAPB,        // 0/1: The Phase B total or fundamental active power Has not/Has changed sign.
  ADE_IntEv_REVAPA,        // 0/1: The Phase A total or fundamental active power Has not/Has changed sign.
  ADE_IntEv_EGYRDY,        // 0/1: One of the energies register Has not/Has been updated. The update rate depends on the data selected in the EGY_TMR_MODE.
  ADE_IntEv_ERROR3,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ERROR2,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ERROR1,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ERROR0,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_CRC_DONE,      //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_CRC_CHG,       //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_SEQERR,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_RSTDONE,       //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXIC,          //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXIB,          //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXIA,          //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXCOMB,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXVC,          //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXVB,          //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXVA,          //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXTOVC,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXTOVB,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ZXTOVA,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_RFNOLOAD,      //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_VANLOAD,       //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_RNLOAD,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv_ANLOAD,        //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__DREADY,       //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__RFNOLOAD,     //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__VANLOAD,      //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__RNLOAD,       //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__ANLOAD,       //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__REVPSUM4,     //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__REVPSUM3,     //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__REVPSUM2,     //      * Informations will be added later, Referred to the Datasheet *//
  ADE_IntEv__REVPSUM1      //      * Informations will be added later, Referred to the Datasheet *//
};

enum WFBCommands {
  //* Some WFB commands are in IntEv section to use them user has to use IntEv functions.
  ADE_WFB_WF_IN_EN,       //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_WF_SRC,         //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_WF_MODE,        //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_WF_CAP_SEL,     //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_WF_CAP_EN,      //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_BURST_CHAN,     //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_WFB_PG_IRQEN,   //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_TRIG_FORCE,     //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_ZXCOMB,         //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_ZXVC,           //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_ZXVB,           //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_ZXVA,           //* Informations will be added later, Referred to the Datasheet *// 
  ADE_WFB_ZXIC,           //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_ZXIB,           //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_ZXIA,           //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_WFB_LAST_PAGE,  //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_WFB_TRIG_ADDR,  //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_LP_SEL,         //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_UPERIOD_SEL,    //* Informations will be added later, Referred to the Datasheet *//
  ADE_WFB_USER_PERIOD     //* Informations will be added later, Referred to the Datasheet *//
};

/**
 ** ==================================================================================
 **                                ##### Typedef #####                               
 ** ==================================================================================
 **/

/**
 ** ==================================================================================
 **                                ##### Struct #####                               
 ** ==================================================================================
 **/

/**
 * @brief  Typedef of Resample Data buffer
 */
typedef FPType
ADE90xx_WFBR_Data_t[512];
/**
 * @brief  Typedef of Fixed Data Rate buffer
 */
typedef FPType
ADE90xx_WFBF_Data_t[256];

/**
 * @brief  Handler Structure of Library
 * @note   Must be initialized at first if user want to use Low Level, user is free not to initialize this structure.
 */
typedef struct
ADE90xx_Handler_s
{
  // SPI Functions:
  void (*ADE_SPI_CS_HIGH)(void);            // Must be initialized
  void (*ADE_SPI_CS_LOW)(void);             // Must be initialized
  void (*ADE_SPI_WriteByte)(uint8_t Data);  // Must be initialized
  uint8_t (*ADE_SPI_ReadByte)(void);        // Must be initialized
  // Power Management Functions:
  void (*ADE_PM0_HIGH)(void);               // Can  be initialized
  void (*ADE_PM0_LOW)(void);                // Must be initialized
  void (*ADE_PM1_HIGH)(void);               // Can  be initialized
  void (*ADE_PM1_LOW)(void);                // Must be initialized
  // Control Functions:
  bool (*ADE_CF1_Read)(void);               // Can  be initialized
  bool (*ADE_CF2_Read)(void);               // Can  be initialized
  bool (*ADE_CF3_Read)(void);               // Can  be initialized
  bool (*ADE_CF4_Read)(void);               // Can  be initialized

#ifdef ADE90xx_USE_MACRO_DELAY
#if ADE90xx_USE_MACRO_DELAY == 0
  // Delay Functins:
  void (*ADE_Delay_US)();
  void (*ADE_Delay_MS)();
#endif
#endif

#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 2
  // IRQx Funtions:
  bool (*ADE_IRQ0_Read)(void);              // Must be initialized when using Blocking Mode (Read Functions)
  bool (*ADE_IRQ1_Read)(void);              // Must be initialized when using Blocking Mode (Read Functions)
#elif ADE90xx_BLOCKING_MODE == 1
  bool IsActiveIRQ0;                        // !!! DO NOT USE OR EDIT THESE !!!             
  bool IsActiveIRQ1;                        // !!! DO NOT USE OR EDIT THESE !!!            
#endif
#endif

  uint16_t PWR_TIME_VAL;                    // !!! DO NOT USE OR EDIT THESE !!!                    
  uint16_t EGY_TIME_VAL;                    // !!! DO NOT USE OR EDIT THESE !!!

} ADE90xx_Handler_t;

/**
 ** ==================================================================================
 **                               ##### Variables #####                               
 ** ==================================================================================
 **/

/**
 ** ==================================================================================
 **                                 ##### Union #####                               
 ** ==================================================================================
 **/

/**
 ** ==================================================================================
 **                            ##### Public Functions #####                               
 ** ==================================================================================
 **/
//---------------------------- LOW LEVEL ----------------------------//
/**
 * @brief  Initializes the low level
 * @note   For now This function just works when ADE90xx_Debug_Enable is NOT 0, Otherwise user is free not to use it.
 * @retval None.
 */
void
ADE90xx_InitLL(void);
/**
 * @brief  Reads Register value with masking the bits.
 * @note   To use simply, use ADE90xx_ReadReg macro instead.
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  RegAdd:      Address of the register. Choose data from RegisterAdd enum.
 * @param  RegSize:     Size of the register, 16 or 32 bits. Choose data from RegisterSize enum.
 * @param  StartBit:    Start bit from LSB. 0 to 16 for 16 bits and 0 to 31 for 32 bits. Choose data from RegisterStartBit enum.
 * @param  MaskBit:     Masking from start bit. Choose data from RegisterMaskBit enum.
 * @param  Data:        Pointer of value to read Masked bits.
 * @retval Value to read Masked bits
 */
uint32_t
ADE90xx_ReadRegLL(ADE90xx_Handler_t *ADE_Handler, uint16_t RegAdd, uint8_t RegSize, uint8_t StartBit, uint32_t MaskBit, uint32_t *Data);
/**
 * @brief  Writes Register value with masking the bits.
 * @note   To use simply, use ADE90xx_WriteReg macro instead.
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  RegAdd:      Address of the register. Choose data from RegisterAdd enum. 
 * @param  RegSize:     Size of the register, 16 or 32 bits. Choose data from RegisterSize enum.
 * @param  StartBit:    Start bit from LSB. 0 to 16 for 16 bits and 0 to 31 for 32 bits. Choose data from RegisterStartBit enum.
 * @param  MaskBit:     Masking from start bit. Choose data from RegisterMaskBit enum.
 * @param  Data:        Value to write Masked bits.
 * @retval None
 */
void
ADE90xx_WriteRegLL(ADE90xx_Handler_t *ADE_Handler, uint16_t RegAdd, uint8_t RegSize, uint8_t StartBit, uint32_t MaskBit, uint32_t Data);
//------------------------------ BASIC ------------------------------//
/**
 * @brief  Initializes the ADE90xx IC
 * @note   Must be called at first before calling any functions, except in case of using Low Level functions.
 *         Note that by calling this function the IC will reset and all configurations set in IC will be set as default.
 * @param  ADE_Handler:       Pointer to the handler of the library.
 *         @note              This parameter must be filled before calling this function.
 * @param  UseRogowskiCoil:   Pass 1 when use Rogowski Coil. In case of CT, pass it as 0.
 * @param  UseINChannel:      For enabling IN channel pass it as 1.
 * @param  DisableVAR:        For Disabling VAR calculation. 0: Enable Cal. 1: Disable Cal.
 * @retval None
 */
void
ADE90xx_Init(ADE90xx_Handler_t *ADE_Handler, bool UseRogowskiCoil, bool UseINChannel, bool DisableVAR);
/**
 * @brief  Resets the IC
 * @note   Be careful! It resets all register, So it is highly recommended to use ADE90xx_Init function instead!
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval None
 */
void
ADE90xx_Reset(ADE90xx_Handler_t *ADE_Handler);
//------------------------- CONFIGURATIONS --------------------------//
//------------------------------ Main
/**
 * @brief  Set main configurations to the IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  Command:     Configuration command. Choose it from MainCommands enum.
 * @param  Data:        Data to set.
 * @retval None
 */
void
ADE90xx_SetMainConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t Data); 
/**
 * @brief  Get main configurations to the IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  Command:     Configuration command. Choose it from MainCommands enum.
 * @param  Data:        Pointer of data to get.
 * @retval None
 */
void
ADE90xx_GetMainConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t *Data);
//----------------------- Interrupt & Event
/**
 * @brief  Set Interrupt and Event configurations to the IC
 * @param  ADE_Handler:       Pointer to the handler of the library.
 * @param  SelectStatusMask:  Select 0 for Status and 1 for Mask.
 * @param  Command:           Configuration command. Choose it from IntEvCommands enum.
 * @param  Data:              Data to set.
 * @retval None
 */
void
ADE90xx_SetIntEvConfig(ADE90xx_Handler_t *ADE_Handler, bool SelectStatusMask, uint16_t Command, uint32_t Data);
/**
 * @brief  Get Interrupt and Event configurations to the IC
 * @param  ADE_Handler:       Pointer to the handler of the library.
 * @param  SelectStatusMask:  Select 0 for Status and 1 for Mask.
 * @param  Command:           Configuration command. Choose it from IntEvCommands enum.
 * @param  Data:              Pointer of data to get.
 * @retval None
 */
void
ADE90xx_GetIntEvConfig(ADE90xx_Handler_t *ADE_Handler, bool SelectStatusMask, uint16_t Command, uint32_t *Data);
//-------------------------- IRQx Handlers
/**
 * @brief  IRQ0 pin handler
 * @note   Place this function in the ISR of IRQ0 pin.
 *         This function need to be called when ADE90xx_BLOCKING_MODE is 1.
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval None
 */
void
ADE90xx_IRQ0_Handler(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  IRQ1 pin handler
 * @note   Place this function in the ISR of IRQ1 pin.
 *         This function need to be called when ADE90xx_BLOCKING_MODE is 1.
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval None
 */
void
ADE90xx_IRQ1_Handler(ADE90xx_Handler_t *ADE_Handler);
//----------------------------- Others
/**
 * @brief  Sets Accumulated Power Configuration
 * @param  ADE_Handler:  Pointer to the handler of the library.
 * @param  TimeInSecond: Time in second to accumulate power.
 * @retval None
 */
void
ADE90xx_SetAPWRConfig(ADE90xx_Handler_t *ADE_Handler, FPType TimeInSecond);
/**
 * @brief  Set Accumulated Energy Configuration
 * @param  ADE_Handler:       Pointer to the handler of the library.
 * @param  TimeInSecond:      Time in second to calculate energy.
 * @param  useFreqDSP:        Use Frequency of DSP or Zero Crossing based of ZX_SEL bits in the ZX_LP_SEL register.
 *                            0: Zero cross | 1: DSP Frequency
 * @param  ZeroCrossChannel:  Select Zero Crossing channel in case of useFreqDSP = 0. For DSP Frequency, It does not change anything. 
 *                            0: Voltage A | 1: Voltage B | 2: Voltage C | 3: Combined Signal A, B and C | 5: Previous Config | Others: The same as 0
 * @param  Mode:              0: Accumulate energy and Keep the value of registers after reading
 *                            1: Accumulate energy and Reset the value of registers after reading
 * @retval None
 */
void
ADE90xx_SetAEGYConfig(ADE90xx_Handler_t *ADE_Handler, FPType TimeInSecond, bool useFreqDSP, uint8_t ZeroCrossChannel, bool Mode);
/**
 * @brief  Disables ADC of each channel
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  Mask:        0: Enable ADC | 1: Disable ADC in 0b0xxxxxxx in form to 
 *                      MSB                              LSB
 *                      0 | VC | VB | VA | IN | IC | IB | IA  
 * @retval None
 */
void
ADE90xx_SetDisableADC(ADE90xx_Handler_t *ADE_Handler, uint8_t Mask);
/**
 * @brief  Redirects the output of the ADC to the selected channel
 * @param  ADE_Handler:       Pointer to the handler of the library.
 * @param  Channel:           0: IA | 1: IB | 2: IC | 3: IN | 4: VA | 5: VB | 6: VC | Others: VA
 * @param  RedirectChannel:   0: IA | 1: IB | 2: IC | 3: IN | 4: VA | 5: VB | 6: VC | Others: VA
 * @retval 
 */
void
ADE90xx_ADCRedirect(ADE90xx_Handler_t *ADE_Handler, uint8_t Channel, uint8_t RedirectChannel);
//----------------------------- Gains
// Note that : |Current Channel Gain × Voltage Channel Gain × Power Gain| ≤ 3.75
/**
 * @brief  Adjust Gain for VA
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_AVGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for IA
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_AIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for VB
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_BVGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for IB
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_BIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for VC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_CVGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_CIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for IN
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_NIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for Powers of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_APGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for Powers of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_BPGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjust Gain for Powers of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  GainValue:   Gain value
 *         @note        It has to be between 1 to 32 approximately
 * @retval None
 */
void
ADE90xx_CPGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue);
/**
 * @brief  Adjusts PGA Gain of each channel
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  Channel:     0 to 6 | In order to IA:0 | IB:1 | IC:2 | IN:3 | VA:4 | VB:5 | VC:6 | Other numbers: VA
 * @param  GainValue:   1 or 2 or 4 | Other numbers: 1
 * @retval None
 */
void
ADE90xx_PGAGain(ADE90xx_Handler_t *ADE_Handler, uint8_t Channel, uint8_t GainValue);
//------------------------------ DATA -------------------------------//
// sinc4 data:
/**
 * @brief  Gets the value of VA sinc4
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VA sinc4
 */
FPType
ADE90xx_VSinc4A(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of VB sinc4
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VB sinc4
 */
FPType
ADE90xx_VSinc4B(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of VC sinc4
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VC sinc4
 */
FPType
ADE90xx_VSinc4C(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IA sinc4
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IA sinc4
 */
FPType
ADE90xx_ISinc4A(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IB sinc4
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IB sinc4
 */
FPType
ADE90xx_ISinc4B(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IC sinc4
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IC sinc4
 */
FPType
ADE90xx_ISinc4C(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IN sinc4
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IN sinc4
 */
FPType
ADE90xx_ISinc4N(ADE90xx_Handler_t *ADE_Handler);
// sinc4 + IIR Low-pass filtered data:
/**
 * @brief  Gets the value of VA sinc4 + IIR Low-pass filtered data
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VA sinc4 + IIR Low-pass filtered data
 */
FPType
ADE90xx_VSinc4LPFA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of VB sinc4 + IIR Low-pass filtered data
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VB sinc4 + IIR Low-pass filtered data
 */
FPType
ADE90xx_VSinc4LPFB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of VC sinc4 + IIR Low-pass filtered data
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VC sinc4 + IIR Low-pass filtered data
 */
FPType
ADE90xx_VSinc4LPFC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IA sinc4 + IIR Low-pass filtered data
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IA sinc4 + IIR Low-pass filtered data
 */
FPType
ADE90xx_ISinc4LPFA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IB sinc4 + IIR Low-pass filtered data
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IB sinc4 + IIR Low-pass filtered data
 */
FPType
ADE90xx_ISinc4LPFB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IC sinc4 + IIR Low-pass filtered data
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IC sinc4 + IIR Low-pass filtered data
 */
FPType
ADE90xx_ISinc4LPFC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IN sinc4 + IIR Low-pass filtered data
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IN sinc4 + IIR Low-pass filtered data
 */
FPType
ADE90xx_ISinc4LPFN(ADE90xx_Handler_t *ADE_Handler);
// DSP data:
/**
 * @brief  Gets the value of VA processed in DSP unit 
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VA processed in DSP unit 
 */
FPType
ADE90xx_VDSPA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of VB processed in DSP unit 
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VB processed in DSP unit 
 */
FPType   
ADE90xx_VDSPB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of VC processed in DSP unit 
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of VC processed in DSP unit 
 */
FPType   
ADE90xx_VDSPC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IA processed in DSP unit 
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IA processed in DSP unit 
 */
FPType   
ADE90xx_IDSPA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IB processed in DSP unit 
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IB processed in DSP unit 
 */
FPType   
ADE90xx_IDSPB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IC processed in DSP unit 
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IC processed in DSP unit 
 */
FPType   
ADE90xx_IDSPC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the value of IN processed in DSP unit 
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The value of IN processed in DSP unit 
 */
FPType   
ADE90xx_IDSPN(ADE90xx_Handler_t *ADE_Handler);
// RMS values:
/**
 * @brief  Gets the RMS value of VA
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The RMS value of VA
 */
FPType
ADE90xx_VRMSA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the RMS value of VB
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The RMS value of VB
 */
FPType
ADE90xx_VRMSB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the RMS value of VC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The RMS value of VC
 */
FPType
ADE90xx_VRMSC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the RMS value of IA
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The RMS value of IA
 */
FPType
ADE90xx_IRMSA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the RMS value of IB
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The RMS value of IB
 */
FPType
ADE90xx_IRMSB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the RMS value of IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The RMS value of IC
 */
FPType
ADE90xx_IRMSC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the RMS value of IN
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The RMS value of IN
 */
FPType
ADE90xx_IRMSN(ADE90xx_Handler_t *ADE_Handler);
// Low-pass filtered total active powers:
/**
 * @brief  Gets the low-pass filtered total active power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The low-pass filtered total active power of phase A
 */
FPType
ADE90xx_LPFTAcPA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the low-pass filtered total active power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The low-pass filtered total active power of phase B
 */
FPType
ADE90xx_LPFTAcPB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the low-pass filtered total active power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The low-pass filtered total active power of phase C
 */
FPType
ADE90xx_LPFTAcPC(ADE90xx_Handler_t *ADE_Handler);
// Low-pass filtered total reactive powers:
/**
 * @brief  Gets the low-pass filtered total reactive power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The low-pass filtered total reactive power of phase A
 */
FPType
ADE90xx_LPFTRePA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the low-pass filtered total reactive power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The low-pass filtered total reactive power of phase B
 */
FPType
ADE90xx_LPFTRePB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the low-pass filtered total reactive power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The low-pass filtered total reactive power of phase C
 */
FPType
ADE90xx_LPFTRePC(ADE90xx_Handler_t *ADE_Handler);
// Total aparent powers:
/**
 * @brief  Gets the total apparent power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The total apparent power of phase A
 */
FPType
ADE90xx_TAPA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the total apparent power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The total apparent power of phase B
 */
FPType   
ADE90xx_TAPB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the total apparent power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The total apparent power of phase C
 */
FPType   
ADE90xx_TAPC(ADE90xx_Handler_t *ADE_Handler);
// Fundamental reactive powers:
/**
 * @brief  Gets the fundamental reactive power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The fundamental reactive power of phase A
 */
FPType
ADE90xx_FRePA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the fundamental reactive power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The fundamental reactive power of phase B
 */
FPType   
ADE90xx_FRePB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the fundamental reactive power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The fundamental reactive power of phase C
 */
FPType   
ADE90xx_FRePC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated total active powers:
/**
 * @brief  Gets the accumulated total active power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total active power of phase A
 */
FPType
ADE90xx_ATAcPA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total active power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total active power of phase B
 */
FPType   
ADE90xx_ATAcPB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total active power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total active power of phase C
 */
FPType   
ADE90xx_ATAcPC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated total reactive powers:
/**
 * @brief  Gets the accumulated total reactive power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total reactive power of phase A
 */
FPType
ADE90xx_ATRePA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total reactive power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total reactive power of phase B
 */
FPType   
ADE90xx_ATRePB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total reactive power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total reactive power of phase C
 */
FPType   
ADE90xx_ATRePC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated total apparent powers:
/**
 * @brief  Gets the accumulated total apparent power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total apparent power of phase A
 */
FPType
ADE90xx_ATAPA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total apparent power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total apparent power of phase B
 */
FPType   
ADE90xx_ATAPB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total apparent power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total apparent power of phase C
 */
FPType   
ADE90xx_ATAPC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated total active energies:
/**
 * @brief  Gets the accumulated total active energy of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total active energy of phase A
 */
FPType
ADE90xx_ATAcEA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total active energy of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total active energy of phase B
 */
FPType   
ADE90xx_ATAcEB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total active energy of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total active energy of phase C
 */
FPType   
ADE90xx_ATAcEC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated total reactive energies:
/**
 * @brief  Gets the accumulated total reactive energy of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total reactive energy of phase A
 */
FPType
ADE90xx_ATReEA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total reactive energy of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total reactive energy of phase B
 */
FPType   
ADE90xx_ATReEB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total reactive energy of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total reactive energy of phase C
 */
FPType   
ADE90xx_ATReEC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated total apparent energies:
/**
 * @brief  Gets the accumulated total apparent energy of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total apparent energy of phase A
 */
FPType
ADE90xx_ATAEA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total apparent energy of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total apparent energy of phase B
 */
FPType   
ADE90xx_ATAEB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated total apparent energy of phase c
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated total apparent energy of phase c
 */
FPType   
ADE90xx_ATAEC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated fundamental reactive powers:
/**
 * @brief  Gets the accumulated fundamental reactive power of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated fundamental reactive power of phase A
 */
FPType
ADE90xx_AFRePA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated fundamental reactive power of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated fundamental reactive power of phase B
 */
FPType   
ADE90xx_AFRePB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated fundamental reactive power of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated fundamental reactive power of phase C
 */
FPType   
ADE90xx_AFRePC(ADE90xx_Handler_t *ADE_Handler);
// Accumulated fundamental reactive energies:
/**
 * @brief  Gets the accumulated fundamental reactive energy of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated fundamental reactive energy of phase A
 */
FPType
ADE90xx_AFReEA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated fundamental reactive energy of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated fundamental reactive energy of phase B
 */
FPType   
ADE90xx_AFReEB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the accumulated fundamental reactive energy of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The accumulated fundamental reactive energy of phase C
 */
FPType   
ADE90xx_AFReEC(ADE90xx_Handler_t *ADE_Handler);
// Power factors:
/**
 * @brief  Gets the power factor of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The power factor of phase A
 */
FPType
ADE90xx_PFA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the power factor of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The power factor of phase B
 */
FPType   
ADE90xx_PFB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the power factor of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The power factor of phase C
 */
FPType   
ADE90xx_PFC(ADE90xx_Handler_t *ADE_Handler);
// Frequencies:
/**
 * @brief  Gets the frequency of phase A
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The frequency of phase A
 */
FPType
ADE90xx_FreqA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the frequency of phase B
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The frequency of phase B
 */
FPType
ADE90xx_FreqB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the frequency of phase C
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The frequency of phase C
 */
FPType
ADE90xx_FreqC(ADE90xx_Handler_t *ADE_Handler);
// Angles:
/**
 * @brief  Gets the Angle value betveen phase VA and VB
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase VA and VB
 */
FPType
ADE90xx_AngleVAtoVB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase VB and VC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase VB and VC
 */
FPType
ADE90xx_AngleVBtoVC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase VA and VC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase VA and VC
 */
FPType
ADE90xx_AngleVAtoVC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase VA and IA
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase VA and IA
 */
FPType
ADE90xx_AngleVAtoIA(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase VB and IB
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase VB and IB
 */
FPType
ADE90xx_AngleVBtoIB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase VC and IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase VC and IC
 */
FPType
ADE90xx_AngleVCtoIC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase IA and IB
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase IA and IB
 */
FPType
ADE90xx_AngleIAtoIB(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase IB and IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase IB and IC
 */
FPType
ADE90xx_AngleIBtoIC(ADE90xx_Handler_t *ADE_Handler);
/**
 * @brief  Gets the Angle value betveen phase IA and IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @retval The Angle value betveen phase IA and IC
 */
FPType
ADE90xx_AngleIAtoIC(ADE90xx_Handler_t *ADE_Handler);
//---------------------------- Waveform -----------------------------//
/**
 * @brief  Set Waveform buffer configurations to the IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  Command:     Configuration command. Choose it from WFBCommands enum.
 * @param  Data:        Data to set.
 * @retval None
 */
void
ADE90xx_SetWFBConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t Data);
/**
 * @brief  Get Waveform buffer configurations to the IC
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  Command:     Configuration command. Choose it from WFBCommands enum.
 * @param  Data:        Pointer of data to get.
 * @retval None
 */
void
ADE90xx_GetWFBConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t *Data);
/**
 * @brief  Start catching resample data.
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  EnableInt:   Enable ADE_IntEv_COH_WFB_FULL interrupt. Interrupt occurs when catching data is completed.
 * @retval None
 */
void
ADE90xx_StartWFBConfigResample(ADE90xx_Handler_t *ADE_Handler, bool EnableInt);
/**
 * @brief  Get Waveform buffer resample data.
 * @note   In caes of ADE90xx_BLOCKING_MODE == 0 AND EnableInt = 1, user has to call this function after handling ADE_IntEv_COH_WFB_FULL interrupt.
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  ADE_IA:      IA Data to get, if user does not need it, pass it as NULL.
 * @param  ADE_VA:      VA Data to get, if user does not need it, pass it as NULL.
 * @param  ADE_IB:      IB Data to get, if user does not need it, pass it as NULL.
 * @param  ADE_VB:      VB Data to get, if user does not need it, pass it as NULL.
 * @param  ADE_IC:      IC Data to get, if user does not need it, pass it as NULL.
 * @param  ADE_VC:      VC Data to get, if user does not need it, pass it as NULL.
 * @param  ADE_IN:      IN Data to get, if user does not need it, pass it as NULL.
 * @retval None
 */
void
ADE90xx_GetWFBDataResample(ADE90xx_Handler_t *ADE_Handler, ADE90xx_WFBR_Data_t ADE_IA,
                           ADE90xx_WFBR_Data_t ADE_VA, ADE90xx_WFBR_Data_t ADE_IB, ADE90xx_WFBR_Data_t ADE_VB,
                           ADE90xx_WFBR_Data_t ADE_IC, ADE90xx_WFBR_Data_t ADE_VC, ADE90xx_WFBR_Data_t ADE_IN);
/**
 * @brief  Start catching fixed data-rate data.
 * @note   //! Better not to use, Information will be added later.
 * @param  ADE_Handler:       Pointer to the handler of the library.
 * @param  SelectChannel:     Channel(s) to get data.
 *                            0: All channels | 1: IA and VA | 2: IB and VB | 3: IC and VC | 8: IA | 9: VA | 10: IB | 11: VB | 12: IC | 13: VC | 14: IN | Others: All channels
 * @param  SourceWF:          Source of Signal
 *                            0: Sinc4 | 2:  Sinc4 + IIR LPF | 3: Waveform DSP
 * @param  ModeWF:            Mode Of Operation | Recommended: 2
 *                            0: Stop when waveform buffer is full | 1: Continuous fill — stop only on enabled trigger events.
 *                            2: Continuous filling — center capture around enabled trigger events. | 3: Continuous fill — save event address of enabled trigger events.
 * @param  EnableIntPage:     Enable Interrupt when page is full.
 * @param  IntTrigger:        Enable Interrupt when trigger event occurs. (Other bits are reserved)
 *                            bit 10: TRIG_FORCE : Set this bit to trigger an event to stop the waveform buffer filling.(Not useful for first configuration)
 *                            bit  9: ZXCOMB : Zero-crossing on combined signal from VA, VB, and VC. For more information please refer to ADE90xx datasheet.
 *                            bit  8: ZXVC : Zero-crossing on VC signal. For more information please refer to ADE90xx datasheet.
 *                            bit  7: ZXVB : Zero-crossing on VB signal. For more information please refer to ADE90xx datasheet.
 *                            bit  6: ZXVA : Zero-crossing on VA signal. For more information please refer to ADE90xx datasheet.
 *                            bit  5: ZXIC : Zero-crossing on IC signal. For more information please refer to ADE90xx datasheet.
 *                            bit  4: ZXIB : Zero-crossing on IB signal. For more information please refer to ADE90xx datasheet.
 *                            bit  3: ZXIA : Zero-crossing on IA signal. For more information please refer to ADE90xx datasheet.
 * @retval None
 */
void
ADE90xx_StartWFBConfigFixedDataRate(ADE90xx_Handler_t *ADE_Handler, uint8_t SelectChannel, uint8_t SourceWF, uint8_t ModeWF, bool EnableIntPage, uint16_t IntTrigger);
/**
 * @brief  Get Waveform buffer fixed data-rate data.
 * @note   //! Better not to use, It needs to be optimized.
 * @param  ADE_Handler: Pointer to the handler of the library.
 * @param  SectionWF:   // * Will be added later
 * @param  SourceWF:    // * Will be added later
 * @param  ADE_IA:      // * Will be added later
 * @param  ADE_VA:      // * Will be added later
 * @param  ADE_IB:      // * Will be added later
 * @param  ADE_VB:      // * Will be added later
 * @param  ADE_IC:      // * Will be added later
 * @param  ADE_VC:      // * Will be added later
 * @param  ADE_IN:      // * Will be added later
 * @retval None
 */
void 
ADE90xx_GetWFBDataFixedDataRate(ADE90xx_Handler_t *ADE_Handler, const bool SectionWF, uint8_t SourceWF, ADE90xx_WFBF_Data_t ADE_IA,
                                ADE90xx_WFBF_Data_t ADE_VA, ADE90xx_WFBF_Data_t ADE_IB, ADE90xx_WFBF_Data_t ADE_VB,
                                ADE90xx_WFBF_Data_t ADE_IC, ADE90xx_WFBF_Data_t ADE_VC, ADE90xx_WFBF_Data_t ADE_IN);

//! NOT IMPLEMENTED YET ----------------------------------------------------------- //

#ifdef __cplusplus
}
#endif
#endif
