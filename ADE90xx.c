/**
 **********************************************************************************
 * @file   lib.c
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

//* Private Includes -------------------------------------------------------------- //
#include "ADE90xx.h"

// #include "driver/uart.h"

//* Private Defines and Macros ---------------------------------------------------- //
#define CHECK_REG_ADD_VALIDATION    1     //  0: Disable | 1: Enable
#define MAX_MASK                0xFFFFFFFF


//#define MAX_MASK_32      0xFFFFFFFF
//#define MAX_MASK_16      0xFFFF
//#define ReadReg(r, m, d)  ((r >= 0x480 && r <= 0x4FE) ? ADE90xx_ReadReg16(ADE_Handler, r, m, d) : ADE90xx_ReadReg32(ADE_Handler, r, m, d))
//#define WriteReg(r, m, d) ((r >= 0x480 && r <= 0x4FE) ? ADE90xx_WriteReg16(ADE_Handler, r, m, d) : ADE90xx_WriteReg32(ADE_Handler, r, m, d))
//#define ReadReg16Log(r) ADE90xx_ReadReg16(ADE_Handler, r, MAX_MASK_16, NULL)
//#define ReadReg32Log(r) ADE90xx_ReadReg32(ADE_Handler, r, MAX_MASK_32, NULL)

//* Others ------------------------------------------------------------------------ //

#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
#define PROGRAMLOG(arg...) printf(arg)
#define WRITE_REG_LOG(n, m, cv, i) if (ADE_ADD_##n >= 0x480 && ADE_ADD_##n <= 0x4FE) \
                                   {PROGRAMLOG("%-12s | 0x%03X   | 0x%04X     | 0x%04X     | 0x%04X        | %-50s \r\n", #n, ADE_ADD_##n, m & 0xFFFF, ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL), cv, i ? i : (""));} else \
                                   {PROGRAMLOG("%-12s | 0x%03X   | 0x%08X | 0x%08X | 0x%08X    | %-50s \r\n"               , #n, ADE_ADD_##n, m, ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL), cv, i ? i : (""));}
#define READ_REG_LOG(n, m , i)     if (ADE_ADD_##n >= 0x480 && ADE_ADD_##n <= 0x4FE) \
                                   {PROGRAMLOG("%-12s | 0x%03X   | 0x%04X     | 0x%04X     | ------       | %-50s \r\n", #n, ADE_ADD_##n, m & 0xFFFF, ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL), i ? i : (""));} else \
                                   {PROGRAMLOG("%-12s | 0x%03X   | 0x%08X | 0x%08X | ----------    | %-50s \r\n"       , #n, ADE_ADD_##n, m, ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL), i ? i : ("")i);}                      
#define VALUE_LOG(s, n, func, i)    PROGRAMLOG("%-12s | 0x%03X   | ---------- | %-10f | ----------    | %-50s \r\n", s, ADE_ADD_##n, func, i);
#define WRITE_VAL_LOG(s, n, d, cv, i) PROGRAMLOG("%-12s | 0x%03X   | ---------- | %-10f | %-10f    | %-50s \r\n", s ? s : "------------", ADE_ADD_##n, d, cv, i ? i : (""));
#define REAL_VAL_LOG(s, n, d, i)      PROGRAMLOG("%-12s | 0x%03X   | ---------- | %-10f | ----------    | %-50s \r\n", s ? s : "------------", ADE_ADD_##n, d, i ? i : (""));
#else
#define PROGRAMLOG(arg...)
#define WRITE_REG_LOG(n, m, cv, i)
#define READ_REG_LOG(n, m, i)
#define VALUE_LOG(s, n, func, i)
#define WRITE_VAL_LOG(s, n, d, cv, i) 
#define REAL_VAL_LOG(s, n, d, i)      
#endif
#else 
#define PROGRAMLOG(arg...)
#define WRITE_REG_LOG(n, m, cv, i)
#define READ_REG_LOG(n, m, i)
#define VALUE_LOG(s, n, func, i)
#define WRITE_VAL_LOG(s, n, d, cv, i) 
#define REAL_VAL_LOG(s, n, d, i)      
#endif

#ifdef ADE90xx_USE_MACRO_DELAY
#if ADE90xx_USE_MACRO_DELAY == 1
#define Delay_US(x)   ADE90xx_MACRO_DELAY_US(x)
#ifndef ADE90xx_MACRO_DELAY_US
#error "ADE90xx_MACRO_DELAY_US is not defined. Please Use handler delay or config ADE90xx_MACRO_DELAY_US macro, You can choose it on ADE90xx_USE_MACRO_DELAY define"
#endif
#define Delay_MS(x)   ADE90xx_MACRO_DELAY_MS(x)
#ifndef ADE90xx_MACRO_DELAY_MS
#error "ADE90xx_MACRO_DELAY_MS is not defined. Please Use handler delay or config ADE90xx_MACRO_DELAY_MS macro, You can choose it on ADE90xx_USE_MACRO_DELAY define"
#endif
#else
#define Delay_US(x)   ADE_Handler->ADE_Delay_US(x)
#define Delay_MS(x)   ADE_Handler->ADE_Delay_MS(x)
#endif
#else
#define Delay_US(x)
#define Delay_MS(x)
#endif

#ifdef ADE90xx_ChooseFullScale
#if ADE90xx_ChooseFullScale == 1
#define FSSinc4         67107786.0f
#define FSDec           74518668.0f
#define FSxPCF          74532013.0f
#define FSResample      18196.0f
#define FSPower         20694066.0f
#else
#define FSSinc4         67110000.0f
#define FSDec           74520000.0f
#define FSxPCF          74770000.0f
#define FSResample      18100.0f
#define FSPower         20823646.0f
#endif
#else
#define FSSinc4         67110000.0f
#define FSDec           74520000.0f
#define FSxPCF          74770000.0f
#define FSResample      18100.0f
#define FSPower         20823646.0f
#endif

/**
 ** ==================================================================================
 **                            ##### Private Enums #####                               
 ** ==================================================================================
 **/

/**
 ** ==================================================================================
 **                           ##### Private Typedef #####                               
 ** ==================================================================================
 **/

/**
 ** ==================================================================================
 **                           ##### Private Struct #####                               
 ** ==================================================================================
 **/

/**
 ** ==================================================================================
 **                          ##### Private Variables #####                               
 ** ==================================================================================
 **/

/**
 ** ==================================================================================
 **                            ##### Private Union #####                               
 ** ==================================================================================
 **/

/**
 *! ==================================================================================
 *!                          ##### Private Functions #####                               
 *! ==================================================================================
 **/

//static uint8_t
//ADE90xx_ReadRegUINT8(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg)
//{
//  ADE_Handler->ADE_SPI_CS_LOW();
//  ADE_Handler->ADE_SPI_WriteByte((Reg >> 4) & 0xFF);
//  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF8);
//  uint8_t retval = ADE_Handler->ADE_SPI_ReadByte();
//          retval = ADE_Handler->ADE_SPI_ReadByte();
//  ADE_Handler->ADE_SPI_CS_HIGH();
//  return retval;
//}

//static void
//ADE90xx_WriteRegUINT8(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg, uint8_t Data)
//{
//  ADE_Handler->ADE_SPI_CS_LOW();
//  ADE_Handler->ADE_SPI_WriteByte((Reg >> 4) & 0xFF);
//  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF0);
//  ADE_Handler->ADE_SPI_WriteByte(0x00);
//  ADE_Handler->ADE_SPI_WriteByte(Data);
//  ADE_Handler->ADE_SPI_CS_HIGH();
//}

static uint16_t
ADE90xx_ReadReg16(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg, uint16_t MASK, uint16_t *Data)
{
#ifdef CHECK_REG_ADD_VALIDATION
#if  CHECK_REG_ADD_VALIDATION == 1
  if (Reg < 0x480 || Reg > 0x4FE) {PROGRAMLOG("!!! Register 0x%X is 32bit, Use ReadReg32 function to read it.\r\n", Reg); return 0;}
#else
#endif
#else
#endif
  
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte( (Reg >> 4) & 0xFF);
  ADE_Handler->ADE_SPI_WriteByte(((Reg << 4) | 0x08) & 0xF8);
  uint16_t retval = ADE_Handler->ADE_SPI_ReadByte() << 8;
  retval |= ADE_Handler->ADE_SPI_ReadByte();
  ADE_Handler->ADE_SPI_CS_HIGH();
  
#ifdef ADE90xx_SAVE_DATA_VALUE
#if ADE90xx_SAVE_DATA_VALUE == 1
  if (Data) *Data = retval & MASK; else return retval & MASK;
#else
  if (Data) *Data = (retval & MASK) | ((~MASK) & (*Data)); else return (retval & MASK);
#endif
#else
  if (Data) *Data = (retval & MASK) | ((~MASK) & (*Data)); else return (retval & MASK);
#endif
  return *Data;
}
static void
ADE90xx_WriteReg16(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg, uint16_t MASK, uint16_t Data)
{
#ifdef CHECK_REG_ADD_VALIDATION
#if  CHECK_REG_ADD_VALIDATION == 1
  if (Reg < 0x480 || Reg > 0x4FE) {PROGRAMLOG("!!! Register 0x%X is 32bit, Use WriteReg32 function to write it.\r\n", Reg); return;}
#else
#endif
#else
#endif
  
//  PROGRAMLOG("O");
//  ADE_Handler->ADE_Delay_MS(1000);
//  PROGRAMLOG("K\r\n");
  
#ifdef ADE90xx_SAVE_DATA_VALUE
#if ADE90xx_SAVE_DATA_VALUE == 1
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(Reg >> 4);
  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF0);
  ADE_Handler->ADE_SPI_WriteByte((MASK & Data) >> 8);
  ADE_Handler->ADE_SPI_WriteByte(MASK & Data);
  ADE_Handler->ADE_SPI_CS_HIGH();
#else
  uint16_t ReadData = (ADE90xx_ReadReg16(ADE_Handler, Reg, 0xFFFF, NULL) & (~MASK)) | (MASK & Data);
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(Reg >> 4);
  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF0);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 8);
  ADE_Handler->ADE_SPI_WriteByte(ReadData);
  ADE_Handler->ADE_SPI_CS_HIGH();
#endif
#else
  uint16_t ReadData = (ADE90xx_ReadReg16(ADE_Handler, Reg, MASK, NULL) & (~MASK)) | (MASK & Data);
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(Reg >> 4);
  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF0);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 8);
  ADE_Handler->ADE_SPI_WriteByte(ReadData);
  ADE_Handler->ADE_SPI_CS_HIGH();
#endif
}
static uint32_t
ADE90xx_ReadReg32(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg, uint32_t MASK, uint32_t *Data)
{
#ifdef CHECK_REG_ADD_VALIDATION
#if  CHECK_REG_ADD_VALIDATION == 1
  if (Reg >= 0x480 && Reg <= 0x4FE) {PROGRAMLOG("!!! Register 0x%X is 16bit, Use ReadReg16 function to Read it.\r\n", Reg); return 0;}
#else
#endif
#else
#endif
  
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(Reg >> 4);
  ADE_Handler->ADE_SPI_WriteByte(((Reg << 4) | 0x08) & 0xF8);
  uint32_t retval = ADE_Handler->ADE_SPI_ReadByte() << 24;
  retval |= ADE_Handler->ADE_SPI_ReadByte() << 16;
  retval |= ADE_Handler->ADE_SPI_ReadByte() << 8;
  retval |= ADE_Handler->ADE_SPI_ReadByte();
  ADE_Handler->ADE_SPI_CS_HIGH();
  
#ifdef ADE90xx_SAVE_DATA_VALUE
#if ADE90xx_SAVE_DATA_VALUE == 1
  if (Data) *Data = retval & MASK; else return retval & MASK;
#else
  if (Data) *Data = (retval & MASK) | ((~MASK) & (*Data)); else return (retval & MASK);
#endif
#else
  if (Data) *Data = (retval & MASK) | ((~MASK) & (*Data)); else return (retval & MASK);
#endif
  return *Data;
}
static void
ADE90xx_WriteReg32(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg, uint32_t MASK, uint32_t Data)
{
#ifdef CHECK_REG_ADD_VALIDATION
#if  CHECK_REG_ADD_VALIDATION == 1
  if (Reg >= 0x480 && Reg <= 0x4FE) {PROGRAMLOG("!!! Register 0x%X is 16bit, Use WriteReg16 function to write it.\r\n", Reg); return;}
#else
#endif
#else
#endif
  
#ifdef ADE90xx_SAVE_DATA_VALUE
#if ADE90xx_SAVE_DATA_VALUE == 1
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(Reg >> 4);
  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF0);
  ADE_Handler->ADE_SPI_WriteByte((MASK & Data) >> 24);
  ADE_Handler->ADE_SPI_WriteByte((MASK & Data) >> 16);
  ADE_Handler->ADE_SPI_WriteByte((MASK & Data) >> 8);
  ADE_Handler->ADE_SPI_WriteByte(MASK & Data);
  ADE_Handler->ADE_SPI_CS_HIGH();
#else
  uint32_t ReadData = (ADE90xx_ReadReg32(ADE_Handler, Reg, 0xFFFFFFFF, NULL) & (~MASK)) | (MASK & Data);
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(Reg >> 4);
  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF0);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 24);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 16);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 8);
  ADE_Handler->ADE_SPI_WriteByte(ReadData);
  ADE_Handler->ADE_SPI_CS_HIGH();
#endif
#else
  uint32_t ReadData = (ADE90xx_ReadReg32(ADE_Handler, Reg, MASK, NULL) & (~MASK)) | (MASK & Data);
  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(Reg >> 4);
  ADE_Handler->ADE_SPI_WriteByte((Reg << 4) & 0xF0);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 24);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 16);
  ADE_Handler->ADE_SPI_WriteByte(ReadData >> 8);
  ADE_Handler->ADE_SPI_WriteByte(ReadData);
  ADE_Handler->ADE_SPI_CS_HIGH();
#endif
}

static uint32_t
ADE90xx_ReadRegS(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg, uint32_t MASK, uint32_t *Data)
{
  if (Reg >= 0x480 && Reg <= 0x4FE) // 16bit Register
    return ADE90xx_ReadReg16(ADE_Handler, Reg, MASK, (uint16_t *)Data);
  else // 32bit Register
    return ADE90xx_ReadReg32(ADE_Handler, Reg, MASK, Data);
}
static void
ADE90xx_WriteRegS(ADE90xx_Handler_t *ADE_Handler, uint16_t Reg, uint32_t MASK, uint32_t Data)
{
  if (Reg >= 0x480 && Reg <= 0x4FE) // 16bit Register
    ADE90xx_WriteReg16(ADE_Handler, Reg, MASK, Data);
  else // 32bit Register
    ADE90xx_WriteReg32(ADE_Handler, Reg, MASK, Data);
}

#define ADE90xx_WriteRegLog(n, m, d, i)           ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_##n, m, d);    WRITE_REG_LOG(n, m, d, i ? i : (""))
#define ADE90xx_ReadRegLog(n, m, i)               ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL);  READ_REG_LOG(n, m, i ? i : (""))

#define ADE90xx_ValLogFunc(n, m)                  ((FPType)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL))
#define ADE90xx_WriteValLog(s, n, m, d, func, i)  ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_##n, m, d);    VALUE_LOG(s ? s : "------------", n, func, i ? i : (""))
#define ADE90xx_ReadValLog(s, n, m, func, i)      ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL);  VALUE_LOG(s ? s : "------------", n, func, i ? i : (""))

#define ADE90xx_Write(n, m, d)                    ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_##n, m, d)
#define ADE90xx_WriteRLog(n, m, d, i)             WRITE_REG_LOG(n, m, d, i)
#define ADE90xx_WriteVLog(s, n, d, cv, i)         WRITE_VAL_LOG(s, n, d, cv, i)
#define ADE90xx_Read(n, m)                        ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_##n, m, NULL)
#define ADE90xx_ReadRLog(n, m, i)                 READ_REG_LOG(n, m, i)
#define ADE90xx_ReadVLog(s, n, d, i)              REAL_VAL_LOG(s, n, d, i)

#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 2
static ADE90xx_INLINE_ATTRIBUTE bool
ADE90xx_CheckIRQ0(ADE90xx_Handler_t *ADE_Handler)
{
  if (ADE_Handler->ADE_IRQ0_Read) 
    return ADE_Handler->ADE_IRQ0_Read();
  return 1;
}
static ADE90xx_INLINE_ATTRIBUTE bool
ADE90xx_CheckIRQ1(ADE90xx_Handler_t *ADE_Handler)
{
  if (ADE_Handler->ADE_IRQ1_Read) 
    return ADE_Handler->ADE_IRQ1_Read();
  return 1;
}
#elif ADE90xx_BLOCKING_MODE == 1
static ADE90xx_INLINE_ATTRIBUTE bool
ADE90xx_CheckIRQ0(ADE90xx_Handler_t *ADE_Handler)
{
  if (ADE_Handler->IsActiveIRQ0) 
  {
    ADE_Handler->IsActiveIRQ0 = false;
    return 0; // It means IRQ0 pin is low
  }
  return 1;
}
static ADE90xx_INLINE_ATTRIBUTE bool
ADE90xx_CheckIRQ1(ADE90xx_Handler_t *ADE_Handler)
{
  if (ADE_Handler->IsActiveIRQ1) 
  {
    ADE_Handler->IsActiveIRQ1 = false;
    return 0; // It means IRQ1 pin is low
  }
  return 1;
}
#endif
#if ADE90xx_BLOCKING_MODE == 1 || ADE90xx_BLOCKING_MODE == 2
static bool
ADE90xx_CheckInt(ADE90xx_Handler_t *ADE_Handler, bool Statusx, uint8_t Bitx)
{
  bool flag = false;
  if (Statusx)
  {
// #ifdef ADE90xx_BLOCKING_MODE
// #if ADE90xx_BLOCKING_MODE == 1 || ADE90xx_BLOCKING_MODE == 2
//   while(ADE90xx_CheckIRQ1(ADE_Handler)){}
// #endif
// #endif
    flag = ADE90xx_ReadRegLL(ADE_Handler, ADE_ADD_STATUS1, ADE_SIZE_STATUS1, Bitx, 1, NULL);
    if (flag && (Bitx != ADE_MB_ERROR1 && Bitx != ADE_MB_ERROR0))
      ADE90xx_WriteRegLL(ADE_Handler, ADE_ADD_STATUS1, ADE_SIZE_STATUS1, Bitx, 1, 1);
  }
  else
  {
// #ifdef ADE90xx_BLOCKING_MODE
// #if ADE90xx_BLOCKING_MODE == 1 || ADE90xx_BLOCKING_MODE == 2
//   while(ADE90xx_CheckIRQ0(ADE_Handler)){}
// #endif
// #endif
    flag = ADE90xx_ReadRegLL(ADE_Handler, ADE_ADD_STATUS1, ADE_SIZE_STATUS0, Bitx, 1, NULL);
    if (flag)
      ADE90xx_WriteRegLL(ADE_Handler, ADE_ADD_STATUS1, ADE_SIZE_STATUS0, Bitx, 1, 1);
  }
  return flag;
}
static void
ADE90xx_IntHandler(ADE90xx_Handler_t *ADE_Handler, bool Statusx, uint8_t Bitx)
{
  bool NotPassInt = true; 
  bool IntPresence = false;
  uint16_t CounterSafe = 0; 
  while(NotPassInt) 
  {
    IntPresence = false;
    if(Statusx)
    {
#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 1
      if (ADE_Handler->IsActiveIRQ1) 
      {
        ADE_Handler->IsActiveIRQ1 = false;
        IntPresence = true; // It means IRQ1 pin is low
      }
#elif ADE90xx_BLOCKING_MODE == 2
      if (ADE_Handler->ADE_IRQ1_Read) 
        IntPresence = !ADE_Handler->ADE_IRQ1_Read();
#endif
#endif
    }
    else
    {
#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 1
      if (ADE_Handler->IsActiveIRQ0) 
      {
        ADE_Handler->IsActiveIRQ0 = false;
        IntPresence = true; // It means IRQ0 pin is low
      }
#elif ADE90xx_BLOCKING_MODE == 2
      if (ADE_Handler->ADE_IRQ0_Read) 
        IntPresence = !ADE_Handler->ADE_IRQ0_Read();
#endif
#endif
    }
    if(IntPresence)
    {
      if (Statusx)
      {
        NotPassInt = ADE90xx_ReadRegLL(ADE_Handler, ADE_ADD_STATUS1, ADE_SIZE_STATUS1, Bitx, 1, NULL);
        if (NotPassInt && (Bitx != ADE_MB_ERROR1 && Bitx != ADE_MB_ERROR0))
          ADE90xx_WriteRegLL(ADE_Handler, ADE_ADD_STATUS1, ADE_SIZE_STATUS1, Bitx, 1, 1);
      }
      else
      {
        NotPassInt = ADE90xx_ReadRegLL(ADE_Handler, ADE_ADD_STATUS0, ADE_SIZE_STATUS0, Bitx, 1, NULL);
        if (NotPassInt)
          ADE90xx_WriteRegLL(ADE_Handler, ADE_ADD_STATUS0, ADE_SIZE_STATUS0, Bitx, 1, 1);
      }
      NotPassInt = NotPassInt ? false : true;
    }
    if (CounterSafe == 0xFFFF)
    {
      PROGRAMLOG("Error To Find Interrupt, Status:%d Bit:%d\r\n", Statusx, Bitx);
      return;
    }
    else
      CounterSafe++;
  }
}
#endif
#endif


// //uint16_t DataCo[512] = {0};
// static void 
// ADE90xx_ReadWFBBurst(ADE90xx_Handler_t *ADE_Handler, uint32_t MASK, uint16_t *Data, uint16_t NumOfData)
// {
//   ADE_Handler->ADE_SPI_CS_LOW();
//   ADE_Handler->ADE_SPI_WriteByte(0x80);
//   ADE_Handler->ADE_SPI_WriteByte(0x08);
  
//  for(uint16_t DateBrustCounter = 0; DateBrustCounter < NumOfData; DateBrustCounter++)
//     Data[DateBrustCounter]  = (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte(); 
  
//   ADE_Handler->ADE_SPI_CS_HIGH();
// #ifdef ADE90xx_SAVE_DATA_VALUE
// #if ADE90xx_SAVE_DATA_VALUE == 1
// #else
//   for(uint16_t MASKCounter = 0; MASKCounter < NumOfData; MASKCounter++)
//      Data[MASKCounter] &= MASK;
// #endif
// #else
//   for(uint16_t MASKCounter = 0; MASKCounter < NumOfData; MASKCounter++)
//      Data[MASKCounter] &= MASK;
// #endif
// }
/**
 ** ==================================================================================
 **                           ##### Public Functions #####                               
 ** ==================================================================================
 **/
//---------------------------- LOW LEVEL ----------------------------//
void
ADE90xx_InitLL(void)
{
  PROGRAMLOG("\r\n");
  PROGRAMLOG("    Name     | Reg Add |    Mask    |   Value    | Correct Value | Information \r\n");
  PROGRAMLOG("-------------|---------|------------|------------|---------------|-------------\r\n");
}
uint32_t
ADE90xx_ReadRegLL(ADE90xx_Handler_t *ADE_Handler, uint16_t RegAdd, uint8_t RegSize, uint8_t StartBit, uint32_t MaskBit, uint32_t *Data)
{
#ifdef CHECK_REG_ADD_VALIDATION
#if  CHECK_REG_ADD_VALIDATION == 1
  if ((RegAdd < 0x480 || RegAdd > 0x4FE ) && (RegSize == 16)) {PROGRAMLOG("!!! Register 0x%X is 32bit, RegSize must be 32. make it correct and then recall the function\r\n", RegAdd); return 0;}
  if ((RegAdd >= 0x480 && RegAdd <= 0x4FE) && (RegSize == 32)) {PROGRAMLOG("!!! Register 0x%X is 16bit, RegSize must be 16. make it correct and then recall the function\r\n", RegAdd); return 0;}
#else
#endif
#else
#endif

  uint32_t retval = 0;

  ADE_Handler->ADE_SPI_CS_LOW();
  ADE_Handler->ADE_SPI_WriteByte(  RegAdd >> 4);
  ADE_Handler->ADE_SPI_WriteByte(((RegAdd << 4) | 0x08) & 0xF8);

  if (RegSize == 16) // 16bit Register
  {
    retval  = ADE_Handler->ADE_SPI_ReadByte() << 8;
    retval |= ADE_Handler->ADE_SPI_ReadByte();
  }
  else // 32bit Register
  {
    retval  = ADE_Handler->ADE_SPI_ReadByte() << 24;
    retval |= ADE_Handler->ADE_SPI_ReadByte() << 16;
    retval |= ADE_Handler->ADE_SPI_ReadByte() << 8;
    retval |= ADE_Handler->ADE_SPI_ReadByte();
  }

  ADE_Handler->ADE_SPI_CS_HIGH();

// #ifdef ADE90xx_SAVE_DATA_VALUE
// #if ADE90xx_SAVE_DATA_VALUE == 1
//   if (Data) *Data = ((retval >> StartBit) & MaskBit); else return ((retval >> StartBit) & MaskBit);
// #else
//   if (Data) *Data = ((retval >> StartBit) & MaskBit) | ((~(MaskBit << StartBit)) & (*Data)); else return ((retval >> StartBit) & MaskBit);
// #endif
// #else
//   if (Data) *Data = ((retval >> StartBit) & MaskBit) | ((~(MaskBit << StartBit)) & (*Data)); else return ((retval >> StartBit) & MaskBit);
// #endif

#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
if (RegSize == 16)
  PROGRAMLOG("------------ | 0x%03X   | 0x%04X     | 0x%04X     | ------        | ----------- \r\n", RegAdd, (MaskBit << StartBit) & 0xFFFF, retval & (MaskBit << StartBit));
else
  PROGRAMLOG("------------ | 0x%03X   | 0x%08X | 0x%08X | ----------    | ----------- \r\n"        , RegAdd, (MaskBit << StartBit), retval & (MaskBit << StartBit));               
#else
#endif
#else 
#endif

  if (Data) 
  { 
    *Data = ((retval >> StartBit) & MaskBit); 
    return *Data;
  }
  else 
    return ((retval >> StartBit) & MaskBit);

}
void
ADE90xx_WriteRegLL(ADE90xx_Handler_t *ADE_Handler, uint16_t RegAdd, uint8_t RegSize, uint8_t StartBit, uint32_t MaskBit, uint32_t Data)
{
#ifdef CHECK_REG_ADD_VALIDATION
#if  CHECK_REG_ADD_VALIDATION == 1
  if ((RegAdd < 0x480 || RegAdd > 0x4FE ) && (RegSize == 16)) {PROGRAMLOG("!!! Register 0x%X is 32bit, RegSize must be 32. make it correct and then recall the function\r\n", RegAdd); return;}
  if ((RegAdd >= 0x480 && RegAdd <= 0x4FE) && (RegSize == 32)) {PROGRAMLOG("!!! Register 0x%X is 16bit, RegSize must be 16. make it correct and then recall the function\r\n", RegAdd); return;}
#else
#endif
#else
#endif

  if (RegSize == 16)
  {
#ifdef ADE90xx_SAVE_DATA_VALUE
#if ADE90xx_SAVE_DATA_VALUE == 1
    uint16_t ReadData = (MaskBit & Data) << StartBit;
#else
    uint16_t ReadData = 0;
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte(  RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte(((RegAdd << 4) | 0x08) & 0xF8);  
    ReadData  = ADE_Handler->ADE_SPI_ReadByte() << 8;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte();
    ADE_Handler->ADE_SPI_CS_HIGH();
    ReadData = (ReadData & (~(MaskBit << StartBit))) | ((MaskBit & Data) << StartBit);
#endif
#else
    uint16_t ReadData = 0;
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte(  RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte(((RegAdd << 4) | 0x08) & 0xF8);  
    ReadData  = ADE_Handler->ADE_SPI_ReadByte() << 8;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte();
    ADE_Handler->ADE_SPI_CS_HIGH();
    ReadData = (ReadData & (~(MaskBit << StartBit))) | ((MaskBit & Data) << StartBit);
#endif
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte( RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte((RegAdd << 4) & 0xF0);
    ADE_Handler->ADE_SPI_WriteByte(ReadData >> 8);
    ADE_Handler->ADE_SPI_WriteByte(ReadData);
    ADE_Handler->ADE_SPI_CS_HIGH();

#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
    uint16_t ReadDebug = 0;
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte(  RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte(((RegAdd << 4) | 0x08) & 0xF8);
    ReadDebug  = ADE_Handler->ADE_SPI_ReadByte() << 8;
    ReadDebug |= ADE_Handler->ADE_SPI_ReadByte();
    ADE_Handler->ADE_SPI_CS_HIGH();
    PROGRAMLOG("------------ | 0x%03X   | 0x%04X     | 0x%04X     | 0x%04X     | ------        | ----------- \r\n", RegAdd, (MaskBit << StartBit) & 0xFFFF, ReadDebug & (MaskBit << StartBit), (MaskBit & Data) << StartBit);             
#else
#endif
#else 
#endif
  }
  else 
  {
#ifdef ADE90xx_SAVE_DATA_VALUE
#if ADE90xx_SAVE_DATA_VALUE == 1
    uint32_t ReadData = (MaskBit & Data) << StartBit;
#else
    uint32_t ReadData = 0;
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte(  RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte(((RegAdd << 4) | 0x08) & 0xF8);
    ReadData  = ADE_Handler->ADE_SPI_ReadByte() << 24;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte() << 16;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte() << 8;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte();
    ADE_Handler->ADE_SPI_CS_HIGH();  
    ReadData = (ReadData & (~(MaskBit << StartBit))) | ((MaskBit & Data) << StartBit);
#endif
#else
    uint32_t ReadData = 0;
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte(  RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte(((RegAdd << 4) | 0x08) & 0xF8);
    ReadData  = ADE_Handler->ADE_SPI_ReadByte() << 24;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte() << 16;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte() << 8;
    ReadData |= ADE_Handler->ADE_SPI_ReadByte();
    ADE_Handler->ADE_SPI_CS_HIGH(); 
    ReadData = (ReadData & (~(MaskBit << StartBit))) | ((MaskBit & Data) << StartBit);
#endif
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte( RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte((RegAdd << 4) & 0xF0);
    ADE_Handler->ADE_SPI_WriteByte(ReadData >> 24);
    ADE_Handler->ADE_SPI_WriteByte(ReadData >> 16);
    ADE_Handler->ADE_SPI_WriteByte(ReadData >> 8);
    ADE_Handler->ADE_SPI_WriteByte(ReadData);
    ADE_Handler->ADE_SPI_CS_HIGH();

#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
    uint32_t ReadDebug = 0;
    ADE_Handler->ADE_SPI_CS_LOW();
    ADE_Handler->ADE_SPI_WriteByte(  RegAdd >> 4);
    ADE_Handler->ADE_SPI_WriteByte(((RegAdd << 4) | 0x08) & 0xF8);
    ReadDebug  = ADE_Handler->ADE_SPI_ReadByte() << 24;
    ReadDebug |= ADE_Handler->ADE_SPI_ReadByte() << 16;
    ReadDebug |= ADE_Handler->ADE_SPI_ReadByte() << 8;
    ReadDebug |= ADE_Handler->ADE_SPI_ReadByte();
    ADE_Handler->ADE_SPI_CS_HIGH();
    PROGRAMLOG("------------ | 0x%03X   | 0x%08X | 0x%08X | 0x%08X | ----------    | ----------- \r\n", RegAdd, (MaskBit << StartBit), ReadDebug & (MaskBit << StartBit), (MaskBit & Data) << StartBit);             
#else
#endif
#else 
#endif
  }
}
//------------------------------ BASIC ------------------------------//
void
ADE90xx_Init(ADE90xx_Handler_t *ADE_Handler, bool UseRogowskiCoil, bool UseINChannel, bool DisableVAR)
{
  if (!ADE_Handler->ADE_SPI_CS_HIGH) return;
  if (!ADE_Handler->ADE_SPI_CS_LOW) return;
  if (!ADE_Handler->ADE_SPI_WriteByte) return;
  if (!ADE_Handler->ADE_SPI_ReadByte) return;

#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 2
  if (!ADE_Handler->ADE_IRQ0_Read) return;
  if (!ADE_Handler->ADE_IRQ1_Read) return;
#endif
#endif
  
  PROGRAMLOG("---------------\r\n");
  PROGRAMLOG("ADE90xx Init...\r\n");

#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 1
  ADE_Handler->IsActiveIRQ0 = false;
  ADE_Handler->IsActiveIRQ1 = false;
#endif
#endif
  
  if (ADE_Handler->ADE_PM0_LOW) ADE_Handler->ADE_PM0_LOW();   // For normal operation
  if (ADE_Handler->ADE_PM1_LOW) ADE_Handler->ADE_PM1_LOW();   // For normal operation

/*  
  // Quick Start:
  // 1.  Wait for the RSTDONE interrupt, indicated by the IRQ pin going low
  // 2.  Configure PSM0 normal power mode by setting the PM1 pin and the PM0 pin to low
  // 3.  Configure PGA gain on current and voltage channels using the PGA_GAIN gain register. The default gain on all channels is 1
  // 4.  Configure the HPFDIS bits in the CONFIG0 register to enable/disable the high-pass filter. The high-pass filter is
  //     enabled by default. It is recommended to keep the highpass filter enabled. Set the desired corner frequency for
  //     HPF using the HPF_CRN bits in the CONFIG2 register. The default value for HPF_CRN is 6 (1.25 Hz).
  // 5.  If Rogowski coils are used as current sensors, enable the digital integrator using the INTEN and INITEN bits in the
  //     CONFIG0 register. Disable the integrator when using current transformers. The digital integrators are disabled by default
  //     a  If an integrator is enabled, set the DICOEFF register to 0xFFFFE000. 
  // 6.  Configure the expected fundamental frequency using the SELFREQ bit (50 Hz: SELFREQ = 0, 60 Hz: SELFREQ = 1)
  //     in the ACCMODE register, and program the nominal voltage in the VLEVEL register for fundamental calculations.
  //     VLEVEL = X * 1,144,084, where X is the dynamic range that the nominal signal is at with respect to full scale.
  // 7.  Configure the zero-crossing source for ZX detection. If ZX_SRC_SEL = 1 in the CONFIG0 register, data before the
  //     HPF, integrator, and phase compensation is used. If ZX_SRC_SEL = 0, data after the HPF, integrator, and phase
  //     compensation is used. It is recommended to have ZX_SRC_SEL = 0
  // 8.  Set VCONSEL = 000 in the ACCMODE register for a 3-phase, 4-wire wye configuration.
  // 9.  If energy is monitored using the CF outputs, configure the following registers. Skip this section if the CF outputs are not used.
  //     a  Configure the CFxSEL bits in the CFMODE register to select the energy type to monitor.
  //     b  Configure the TERMSELx bits in the COMPMODE register to select the phases to include in the CF calculation.
  //     c  Program xTHR to 0x00100000.
  //     d  Compute and program the corresponding CFxDEN register based on the desired impulses per kilowatt-hour.
  //     e  Configure the CF pulse width using the CF_LCFG register.
  // 10. If energy is monitored using energy registers, configure the following registers:
  //     a  Configure the WATTACC and VARACC bits in the ACCMODE register to select amongst available accumulation modes (for example: signed, absolute,
  //        positive, or negative accumulation mode). The default accumulation mode is signed
  //     b  Configure the NOLOAD_TMR bits in the EP_CFG register and set the ACT_NL_LVL, REACT_NL_LVL,
  //        and APP_NL_LVL level registers to detect no load and prevent energy accumulation of noise.
  //     c  Configure the EGY_TMR_MODE bit in the EP_CFG register to select sample (EGY_TMR_MODE = 0) or
  //        line cycle (EGY_TMR_MODE = 1) accumulation. Set the desired samples or half line cycles in the EGY_TIME register.
  //     d  Configure the EGY_LD_ACCUM bit in the EP_CFG register to add the internal energy register to user energy
  //        register on EGYRDY (EGY_LD_ACCUM = 0), or to overwrite the user energy register with the internal energy register value (EGY_LD_ACCUM = 1).
  //     e  Configure the RD_RST_EN bit in the EP_CFG register to enable reset of user energy registers on read
  //        (RD_RST_EN = 1), or to disable reset of user energy registers on read (RD_RST_EN = 0).
  // 11. The ADE90xx can provide interrupts for a variety of events on the IRQ0 and IRQ1 pins. The MASK0 or MASK1 and
  //     STATUS0 or STATUS1 registers manage the respective interrupt pins.
  // 12. See the Power Quality Measurements section to configure the power quality parameters.
  // 13. See the Waveform Buffer section to configure and use the waveform buffer.
  // 14. Enable the DSP by setting the RUN register = 1, and enable energy accumulation by setting the EGY_PWR_EN bit in the EP_CFG register = 1.
//  WriteReg(ADE_ADD_RUN, 1, 1);
  // 15. Note that calibration is performed once at typical operating conditions. When the calibration values are computed,
  //     write the constants to registers before enabling the DSP
  // 16. To prevent any changes to the ADE90xx configuration, enable write protection by writing 0x3C64 to the WR_LOCK register
*/
  
  PROGRAMLOG("ADE90xx Software Reset...\r\n");
  ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_CONFIG1, 1, 1); // Software Reset

  // Quick Start: (ADE9078)
#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 1 || ADE90xx_BLOCKING_MODE == 2
  // 1.  Wait for the RSTDONE interrupt, indicated by the IRQ1 pin going low
  while(ADE90xx_CheckIRQ1(ADE_Handler));
  PROGRAMLOG("RSTDONE interrupt detected...\r\n");
#else
  Delay_MS(100); // To wait for interrupt
#endif
#else
  Delay_MS(100); // To wait for interrupt
#endif

  PROGRAMLOG("\r\n");
  PROGRAMLOG("    Name     | Reg Add |    Mask    |   Value    | Correct Value | Information \r\n");
  PROGRAMLOG("-------------|---------|------------|------------|---------------|-------------\r\n");
  
  // 1.1. Tie IRQ1 to High
  // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 16, 1 << 16); // Turn Off RSTDONE status
  ADE90xx_Write(STATUS1, 1 << 16 , 1 << 16); // Turn Off RSTDONE status
  // // ADE90xx_WriteRegLog(STATUS1, 1 << 16, 1 << 16, NULL);
  
  // 2.  Configure the xIGAIN, xVGAIN, and xPGAIN registers via the SPI to calibrate the measurements
  // USING DEFAULT: xIGAIN, xVGAIN, xPGAIN

  // 3.  If other calibration values are required, for example, to improve rms performance at low input signal levels, write these registers.
  // USING DEFAULT

  // 4.  If the CFx pulse output is used, configure the CFxDEN and xTHR registers.
  // USING DEFAULT: CFxDEN, xTHR
  
  // 5.  Configure the expected fundamental frequency (50 Hz or 60 Hz network) in the SELFREQ bit and write VLEVEL = 0x117514.
  // USING DEFAULT: SELFREQ
  ADE90xx_WriteRegLog(VLEVEL, MAX_MASK, 0x117514, NULL);
  // ADE90xx_Write(VLEVEL, MAX_MASK, 0x117514); 
  // ADE90xx_WriteRLog(VLEVEL, MAX_MASK, 0x117514, NULL);
  
  // 6.  If a Rogowski coil sensor is used, write the INTEN bit in the CONFIG0 register to enable the digital integrator on
  //     the IA, IB, and IC channels. To enable the digital integrator on the neutral current, IN, channel, set the ININTEN bit.
  //     Additionally, write DICOEF = 0xFFFFE000 to configure the digital integrator. If current transformers are used, 
  //     INTEN and ININTEN in the CONFIG0 register must = 0.
  if (UseRogowskiCoil) 
  {
    if (UseINChannel) 
    {
      ADE90xx_WriteRegLog(CONFIG0, MAX_MASK, (1 << 5 /*INTEN*/) | (1 << 11 /*ININTEN*/) /*| (1 << 3 HPFDIS)*/, "Enable INTEN & ININTEN");
      // ADE90xx_Write(CONFIG0, MAX_MASK, (1 << 5 /*INTEN*/) | (1 << 11 /*ININTEN*/) /*| (1 << 3 HPFDIS)*/); 
      // ADE90xx_WriteRLog(CONFIG0, MAX_MASK, (1 << 5 /*INTEN*/) | (1 << 11 /*ININTEN*/) /*| (1 << 3 HPFDIS)*/, "Enable INTEN & ININTEN");
    }
    else
    {
      ADE90xx_WriteRegLog(CONFIG0, MAX_MASK, 1 << 5 /*INTEN*/, "Enable INTEN"); 
      // ADE90xx_Write(CONFIG0, MAX_MASK, 1 << 5 /*INTEN*/);
      // ADE90xx_WriteRLog(CONFIG0, MAX_MASK, 1 << 5 /*INTEN*/, "Enable INTEN");
    }
    
    ADE90xx_WriteRegLog(DICOEFF, MAX_MASK, 0xFFFFE000, NULL);
    // ADE90xx_Write(DICOEFF, MAX_MASK, 0xFFFFE000);
    // ADE90xx_WriteRLog(DICOEFF, MAX_MASK, 0xFFFFE000, NULL);
  }
  
  // 7.   If the service bring measured is something other than 4-wire wye, see Table 24 to determine how to configure
  //      ICONSEL and VCONSEL in the ACCMODE register.
  //TODO: NEED TO IMPLEMENT
  // USING DEFAULT: VCONSEL, ICONSEL

  // 7.1. Disable calculating VAR
  if (DisableVAR)
  {
    ADE90xx_WriteRegLog(VAR_DIS, MAX_MASK, 1, "Disable VAR calculation");
    // ADE90xx_Write(VAR_DIS, MAX_MASK, 1);
    // ADE90xx_WriteRLog(VAR_DIS, MAX_MASK, 1, "Disable VAR calculation");
  }
  
  // 8.  Write a 1 to the run register.
  ADE90xx_WriteRegLog(RUN, MAX_MASK, 1, "Run Program");
  // ADE90xx_Write(RUN, MAX_MASK, 1);
  // ADE90xx_WriteRLog(RUN, MAX_MASK, 1, "Run Program");
  
  // 9.  Write a 1 to the EP_CFG register
  ADE90xx_WriteRegLog(EP_CFG, MAX_MASK, 1, "Enable Accumulated Power & Energy");
  // ADE90xx_Write(EP_CFG, MAX_MASK, 1);
  // ADE90xx_WriteRLog(EP_CFG, MAX_MASK, 1, "Enable Accumulated Power & Energy");

  ADE_Handler->PWR_TIME_VAL = 0xFF;
  ADE_Handler->EGY_TIME_VAL = 0xFF;
// //  ADE90xx_ReadRegLog(PART_ID, MAX_MASK);
// //  ADE90xx_ReadRegLog(VERSION, MAX_MASK);

// //  ADE90xx_ReadRegLog(LAST_DATA_32, MAX_MASK);
// //  ADE90xx_ReadRegLog(LAST_DATA_16, MAX_MASK);
// //  ADE90xx_ReadRegLog(LAST_CMD, MAX_MASK);

}
void
ADE90xx_Reset(ADE90xx_Handler_t *ADE_Handler)
{
  PROGRAMLOG("ADE90xx Software Reset...\r\n\r\n");
  // ADE90xx_WriteReg16(ADE_Handler, ADE_ADD_CONFIG1, 1, 1);
  ADE90xx_Write(CONFIG1, 1, 1);
}
//------------------------- CONFIGURATIONS --------------------------//
void
ADE90xx_SetMainConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t Data)
{
  switch(Command)
  {
    // CONFIG0:
    case ADE_DISRPLPF:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 13, Data << 13, "ADE_DISRPLPF");
      break;
    }
    case ADE_DISAPLPF:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 12, Data << 12, "ADE_DISAPLPF");
      break;
    }
    case ADE_ININTEN:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 11, Data << 11, "ADE_ININTEN");
      break;
    }
    case ADE_VNOMC_EN:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 10, Data << 10, "ADE_VNOMC_EN");
      break;
    }
    case ADE_VNOMB_EN:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 9, Data << 9, "ADE_VNOMB_EN");
      break;
    }
    case ADE_VNOMA_EN:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 8, Data << 8, "ADE_VNOMA_EN");
      break;
    }
    case ADE_ZX_SRC_SEL:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 6, Data << 6, "ADE_ZX_SRC_SEL");
      break;
    }
    case ADE_INTEN:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 5, Data << 5, "ADE_INTEN");
      break;
    }
    case ADE_MTEN:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 4, Data << 4, "ADE_MTEN");
      break;
    }
    case ADE_HPFDIS:
    {
      ADE90xx_WriteRegLog(CONFIG0, 1 << 3, Data << 3, "ADE_HPFDIS");
      break;
    }
    case ADE_ISUM_CFG:
    {
      ADE90xx_WriteRegLog(CONFIG0, 3, Data, "ADE_ISUM_CFG");
      break;
    }
    // CONFIG1:
    case ADE_EXT_REF:
    {
      ADE90xx_WriteRegLog(CONFIG1, 1 << 15, Data << 15, "ADE_EXT_REF");
      break;
    }
    case ADE_IRQ0_ON_IRQ1:
    {
      ADE90xx_WriteRegLog(CONFIG1, 1 << 12, Data << 12, "ADE_IRQ0_ON_IRQ1");
      break;
    }
    case ADE_BURST_EN:
    {
      ADE90xx_WriteRegLog(CONFIG1, 1 << 11, Data << 11, "ADE_BURST_EN");
      break;
    }
    case ADE_PWR_SETTLE:
    {
      ADE90xx_WriteRegLog(CONFIG1, 3 << 8, Data << 8, "ADE_PWR_SETTLE");
      break;
    }
    case ADE_CF_ACC_CLR:
    {
      ADE90xx_WriteRegLog(CONFIG1, 1 << 5, Data << 5, "ADE_CF_ACC_CLR");
      break;
    }
    case ADE_CF4_CFG:
    {
      ADE90xx_WriteRegLog(CONFIG1, 3 << 2, Data << 2, "ADE_CF4_CFG");
      break;
    }
    case ADE_CF3_CFG:
    {
      ADE90xx_WriteRegLog(CONFIG1, 1 << 1, Data << 1, "ADE_CF3_CFG");
      break;
    }
    case ADE_SWRST:
    {
      ADE90xx_WriteRegLog(CONFIG1, 1, Data, "ADE_SWRST");
      break;
    }
    // CONFIG2:
    case ADE_UPERIOD_SEL:
    {
      ADE90xx_WriteRegLog(CONFIG2, 1 << 12, Data << 12, "ADE_UPERIOD_SEL");
      break;
    }
    case ADE_HPF_CRN:
    {
      ADE90xx_WriteRegLog(CONFIG2, 7 << 9, Data << 9, "ADE_UPERIOD_SEL");
      break;
    }
    // CONFIG3:
    case ADE_PEAKSEL:
    {
      ADE90xx_WriteRegLog(CONFIG3, 7 << 2, Data << 2, "ADE_PEAKSEL");
      break;
    }
    default:
    {
      PROGRAMLOG("    -----   COMMAND NOT FOUND   -----    \r\n");
      break;
    }
  }
}
void
ADE90xx_GetMainConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t *Data)
{
  switch(Command)
  {
    // CONFIG0:
    case ADE_DISRPLPF:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 13, "ADE_DISRPLPF");
      break;
    }
    case ADE_DISAPLPF:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 12, "ADE_DISAPLPF");
      break;
    }
    case ADE_ININTEN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 11, "ADE_ININTEN");
      break;
    }
    case ADE_VNOMC_EN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 10, "ADE_VNOMC_EN");
      break;
    }
    case ADE_VNOMB_EN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 9, "ADE_VNOMB_EN");
      break;
    }
    case ADE_VNOMA_EN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 8, "ADE_VNOMA_EN");
      break;
    }
    case ADE_ZX_SRC_SEL:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 6, "ADE_ZX_SRC_SEL");
      break;
    }
    case ADE_INTEN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 5, "ADE_INTEN");
      break;
    }
    case ADE_MTEN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 4, "ADE_MTEN");
      break;
    }
    case ADE_HPFDIS:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 1 << 3, "ADE_HPFDIS");
      break;
    }
    case ADE_ISUM_CFG:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG0, 3, "ADE_ISUM_CFG");
      break;
    }
    // CONFIG1:
    case ADE_EXT_REF:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 1 << 15, "ADE_EXT_REF");
      break;
    }
    case ADE_IRQ0_ON_IRQ1:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 1 << 12, "ADE_IRQ0_ON_IRQ1");
      break;
    }
    case ADE_BURST_EN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 1 << 11, "ADE_BURST_EN");
      break;
    }
    case ADE_PWR_SETTLE:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 3 << 8, "ADE_PWR_SETTLE");
      break;
    }
    case ADE_CF_ACC_CLR:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 1 << 5, "ADE_CF_ACC_CLR");
      break;
    }
    case ADE_CF4_CFG:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 3 << 2, "ADE_CF4_CFG");
      break;
    }
    case ADE_CF3_CFG:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 1 << 1, "ADE_CF3_CFG");
      break;
    }
    case ADE_SWRST:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG1, 1, "ADE_SWRST");
      break;
    }
    // CONFIG2:
    case ADE_UPERIOD_SEL:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG2, 1 << 12, "ADE_UPERIOD_SEL");
      break;
    }
    case ADE_HPF_CRN:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG2, 7 << 9, "ADE_HPF_CRN");
      break;
    }
    // CONFIG3:
    case ADE_PEAKSEL:
    {
      *Data = ADE90xx_ReadRegLog(CONFIG3, 7 << 2, "ADE_PEAKSEL");
      break;
    }
    default:
    {
      PROGRAMLOG("    -----   COMMAND NOT FOUND   -----    \r\n");
      break;
    }
  }
}
//----------------------- INTERRUPTS & EVENTS -----------------------//
void
ADE90xx_SetIntEvConfig(ADE90xx_Handler_t *ADE_Handler, bool SelectStatusMask, uint16_t Command, uint32_t Data)
{
  if (SelectStatusMask)
  {
    switch(Command)
    {
      // MASK0:
      case ADE_IntEv_MISMTCH:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 24, Data << 24, "ADE_IntEv_MISMTCH");
        break;
      }
      case ADE_IntEv_COH_WFB_FULL:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 23, Data << 23, "ADE_IntEv_COH_WFB_FULL");
        break;
      }
      case ADE_IntEv_WFB_TRIG:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 22, Data << 22, "ADE_IntEv_WFB_TRIG");
        break;
      }
      case ADE_IntEv_PF_RDY:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 21, Data << 21, "ADE_IntEv_PF_RDY");
        break;
      }
      case ADE_IntEv_PWRRDY:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 18, Data << 18, "ADE_IntEv_PWRRDY");
        break;
      }
      case ADE_IntEv_PAGE_FULL:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 17, Data << 17, "ADE_IntEv_PAGE_FULL");
        break;
      }
      case ADE_IntEv_WFB_TRIG_IRQ:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 16, Data << 16, "ADE_IntEv_WFB_TRIG_IRQ");
        break;
      }
      case ADE_IntEv_DREADY:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 15, Data << 15, "ADE_IntEv_DREADY");
        break;
      }
      case ADE_IntEv_CF4:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 14, Data << 14, "ADE_IntEv_CF4");
        break;
      }
      case ADE_IntEv_CF3:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 13, Data << 13, "ADE_IntEv_CF3");
        break;
      }
      case ADE_IntEv_CF2:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 12, Data << 12, "ADE_IntEv_CF2");
        break;
      }
      case ADE_IntEv_CF1:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 11, Data << 11, "ADE_IntEv_CF1");
        break;
      }
      case ADE_IntEv_REVPSUM4:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 10, Data << 10, "ADE_IntEv_REVPSUM4");
        break;
      }
      case ADE_IntEv_REVPSUM3:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 9, Data << 9, "ADE_IntEv_REVPSUM3");
        break;
      }
      case ADE_IntEv_REVPSUM2:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 8, Data << 8, "ADE_IntEv_REVPSUM2");
        break;
      }
      case ADE_IntEv_REVPSUM1:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 7, Data << 7, "ADE_IntEv_REVPSUM1");
        break;
      }
      case ADE_IntEv_REVRPC:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 6, Data << 6, "ADE_IntEv_REVRPC");
        break;
      }
      case ADE_IntEv_REVRPB:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 5, Data << 5, "ADE_IntEv_REVRPB");
        break;
      }
      case ADE_IntEv_REVRPA:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 4, Data << 4, "ADE_IntEv_REVRPA");
        break;
      }
      case ADE_IntEv_REVAPC:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 3, Data << 3, "ADE_IntEv_REVAPC");
        break;
      }
      case ADE_IntEv_REVAPB:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 2, Data << 2, "ADE_IntEv_REVAPB");
        break;
      }
      case ADE_IntEv_REVAPA:
      {
        ADE90xx_WriteRegLog(MASK0, 1 << 1, Data << 1, "ADE_IntEv_REVAPA");
        break;
      }
      case ADE_IntEv_EGYRDY:
      {
        ADE90xx_WriteRegLog(MASK0, 1 , Data, "ADE_IntEv_EGYRDY");
        break;
      }
      // MASK1:
      case ADE_IntEv_ERROR3:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 31, Data << 31, "ADE_IntEv_ERROR3");
        break;
      }
      case ADE_IntEv_ERROR2:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 30, Data << 30, "ADE_IntEv_ERROR2");
        break;
      }
      case ADE_IntEv_ERROR1:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 29, Data << 29, "ADE_IntEv_ERROR1");
        break;
      }
      case ADE_IntEv_ERROR0:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 28, Data << 28, "ADE_IntEv_ERROR0");
        break;
      }
      case ADE_IntEv_CRC_DONE:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 27, Data << 27, "ADE_IntEv_CRC_DONE");
        break;
      }
      case ADE_IntEv_CRC_CHG:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 26, Data << 26, "ADE_IntEv_CRC_CHG");
        break;
      }
      case ADE_IntEv_SEQERR:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 18, Data << 18, "ADE_IntEv_SEQERR");
        break;
      }
      case ADE_IntEv_RSTDONE:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 16, Data << 16, "ADE_IntEv_RSTDONE");
        break;
      }
      case ADE_IntEv_ZXIC:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 15, Data << 15, "ADE_IntEv_ZXIC");
        break;
      }
      case ADE_IntEv_ZXIB:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 14, Data << 14, "ADE_IntEv_ZXIB");
        break;
      }
      case ADE_IntEv_ZXIA:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 13, Data << 13, "ADE_IntEv_ZXIA");
        break;
      }
      case ADE_IntEv_ZXCOMB:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 12, Data << 12, "ADE_IntEv_ZXCOMB");
        break;
      }
      case ADE_IntEv_ZXVC:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 11, Data << 11, "ADE_IntEv_ZXVC");
        break;
      }
      case ADE_IntEv_ZXVB:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 10, Data << 10, "ADE_IntEv_ZXVB");
        break;
      }
      case ADE_IntEv_ZXVA:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 9, Data << 9, "ADE_IntEv_ZXVA");
        break;
      }
      case ADE_IntEv_ZXTOVC:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 8, Data << 8, "ADE_IntEv_ZXTOVC");
        break;
      }
      case ADE_IntEv_ZXTOVB:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 7, Data << 7, "ADE_IntEv_ZXTOVB");
        break;
      }
      case ADE_IntEv_ZXTOVA:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 6, Data << 6, "ADE_IntEv_ZXTOVA");
        break;
      }
      case ADE_IntEv_RFNOLOAD:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 4, Data << 4, "ADE_IntEv_RFNOLOAD");
        break;
      }
      case ADE_IntEv_VANLOAD:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 2, Data << 2, "ADE_IntEv_VANLOAD");
        break;
      }
      case ADE_IntEv_RNLOAD:
      {
        ADE90xx_WriteRegLog(MASK1, 1 << 1, Data << 1, "ADE_IntEv_RNLOAD");
        break;
      }
      case ADE_IntEv_ANLOAD:
      {
        ADE90xx_WriteRegLog(MASK1, 1, Data, "ADE_IntEv_ANLOAD");
        break;
      }
      // EVENT_MASK:
      case ADE_IntEv__DREADY:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 1 << 16, Data << 16, "ADE_IntEv__DREADY");
        break;
      }
      case ADE_IntEv__RFNOLOAD:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 14, Data << 14, "ADE_IntEv__RFNOLOAD");
        break;
      }
      case ADE_IntEv__VANLOAD:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 12, Data << 12, "ADE_IntEv__VANLOAD");
        break;
      }
      case ADE_IntEv__RNLOAD:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 11, Data << 11, "ADE_IntEv__RNLOAD");
        break;
      }
      case ADE_IntEv__ANLOAD:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 10, Data << 10, "ADE_IntEv__ANLOAD");
        break;
      }
      case ADE_IntEv__REVPSUM4:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 9, Data << 9, "ADE_IntEv__REVPSUM4");
        break;
      }
      case ADE_IntEv__REVPSUM3:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 8, Data << 8, "ADE_IntEv__REVPSUM3");
        break;
      }
      case ADE_IntEv__REVPSUM2:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 7, Data << 7, "ADE_IntEv__REVPSUM2");
        break;
      }
      case ADE_IntEv__REVPSUM1:
      {
        ADE90xx_WriteRegLog(EVENT_MASK, 7 << 6, Data << 6, "ADE_IntEv__REVPSUM1");
        break;
      }
      default:
      {
        PROGRAMLOG("    -----   COMMAND NOT FOUND   -----    \r\n");
        break;
      }
    }
  }
  else
  {
    switch(Command)
    {
      // STATUS0:
      case ADE_IntEv_MISMTCH:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 24, Data << 24);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 24, Data << 24);
        break;
      }
      case ADE_IntEv_COH_WFB_FULL:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 23, Data << 23);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 23, Data << 23);
        break;
      }
      case ADE_IntEv_WFB_TRIG:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 22, Data << 22);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 22, Data << 22);
        break;
      }
      case ADE_IntEv_PF_RDY:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 21, Data << 21);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 21, Data << 21);
        break;
      }
      case ADE_IntEv_PWRRDY:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 18, Data << 18);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 18, Data << 18);
        break;
      }
      case ADE_IntEv_PAGE_FULL:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 17, Data << 17);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 17, Data << 17);
        break;
      }
      case ADE_IntEv_WFB_TRIG_IRQ:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 16, Data << 16);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 16, Data << 16);
        break;
      }
      case ADE_IntEv_DREADY:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 15, Data << 15);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 15, Data << 15);
        break;
      }
      case ADE_IntEv_CF4:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 14, Data << 14);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 14, Data << 14);
        break;
      }
      case ADE_IntEv_CF3:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 13, Data << 13);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 13, Data << 13);
        break;
      }
      case ADE_IntEv_CF2:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 12, Data << 12);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 12, Data << 12);
        break;
      }
      case ADE_IntEv_CF1:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 11, Data << 11);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 11, Data << 11);
        break;
      }
      case ADE_IntEv_REVPSUM4:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 10, Data << 10);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 10, Data << 10);
        break;
      }
      case ADE_IntEv_REVPSUM3:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 9, Data << 9);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 9, Data << 9);
        break;
      }
      case ADE_IntEv_REVPSUM2:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 8, Data << 8);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 8, Data << 8);
        break;
      }
      case ADE_IntEv_REVPSUM1:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 7, Data << 7);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 7, Data << 7);
        break;
      }
      case ADE_IntEv_REVRPC:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 6, Data << 6);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 6, Data << 6);
        break;
      }
      case ADE_IntEv_REVRPB:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 5, Data << 5);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 5, Data << 5);
        break;
      }
      case ADE_IntEv_REVRPA:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 4, Data << 4);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 4, Data << 4);
        break;
      }
      case ADE_IntEv_REVAPC:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 3, Data << 3);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 3, Data << 3);
        break;
      }
      case ADE_IntEv_REVAPB:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 2, Data << 2);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 2, Data << 2);
        break;
      }
      case ADE_IntEv_REVAPA:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 1, Data << 1);
        // ADE90xx_WriteRegLog(STATUS0, 1 << 1, Data << 1);
        break;
      }
      case ADE_IntEv_EGYRDY:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS0, 1, Data);
        // ADE90xx_WriteRegLog(STATUS0, 1 , Data);
        break;
      }
      // STATUS1:
      case ADE_IntEv_ERROR3:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 31, Data << 31);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 31, Data << 31);
        break;
      }
      case ADE_IntEv_ERROR2:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 30, Data << 30);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 30, Data << 30);
        break;
      }
      case ADE_IntEv_ERROR1:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 29, Data << 29);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 29, Data << 29);
        break;
      }
      case ADE_IntEv_ERROR0:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 28, Data << 28);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 28, Data << 28);
        break;
      }
      case ADE_IntEv_CRC_DONE:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 27, Data << 27);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 27, Data << 27);
        break;
      }
      case ADE_IntEv_CRC_CHG:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 26, Data << 26);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 26, Data << 26);
        break;
      }
      case ADE_IntEv_SEQERR:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 18, Data << 18);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 18, Data << 18);
        break;
      }
      case ADE_IntEv_RSTDONE:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 16, Data << 16);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 16, Data << 16);
        break;
      }
      case ADE_IntEv_ZXIC:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 15, Data << 15);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 15, Data << 15);
        break;
      }
      case ADE_IntEv_ZXIB:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 14, Data << 14);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 14, Data << 14);
        break;
      }
      case ADE_IntEv_ZXIA:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 13, Data << 13);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 13, Data << 13);
        break;
      }
      case ADE_IntEv_ZXCOMB:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 12, Data << 12);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 12, Data << 12);
        break;
      }
      case ADE_IntEv_ZXVC:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 11, Data << 11);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 11, Data << 11);
        break;
      }
      case ADE_IntEv_ZXVB:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 10, Data << 10);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 10, Data << 10);
        break;
      }
      case ADE_IntEv_ZXVA:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 9, Data << 9);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 9, Data << 9);
        break;
      }
      case ADE_IntEv_ZXTOVC:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 8, Data << 8);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 8, Data << 8);
        break;
      }
      case ADE_IntEv_ZXTOVB:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 7, Data << 7);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 7, Data << 7);
        break;
      }
      case ADE_IntEv_ZXTOVA:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 6, Data << 6);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 6, Data << 6);
        break;
      }
      case ADE_IntEv_RFNOLOAD:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 4, Data << 4);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 4, Data << 4);
        break;
      }
      case ADE_IntEv_VANLOAD:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 2, Data << 2);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 2, Data << 2);
        break;
      }
      case ADE_IntEv_RNLOAD:
      {

        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1 << 1, Data << 1);
        // ADE90xx_WriteRegLog(STATUS1, 1 << 1, Data << 1);
        break;
      }
      case ADE_IntEv_ANLOAD:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_STATUS1, 1, Data);
        // ADE90xx_WriteRegLog(STATUS1, 1, Data);
        break;
      }
      // EVENT_STATUS:
      case ADE_IntEv__DREADY:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 1 << 16, Data << 16);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 1 << 16, Data << 16);
        break;
      }
      case ADE_IntEv__RFNOLOAD:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 14, Data << 14);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 14, Data << 14);
        break;
      }
      case ADE_IntEv__VANLOAD:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 12, Data << 12);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 12, Data << 12);
        break;
      }
      case ADE_IntEv__RNLOAD:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 11, Data << 11);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 11, Data << 11);
        break;
      }
      case ADE_IntEv__ANLOAD:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 10, Data << 10);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 10, Data << 10);
        break;
      }
      case ADE_IntEv__REVPSUM4:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 9, Data << 9);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 9, Data << 9);
        break;
      }
      case ADE_IntEv__REVPSUM3:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 8, Data << 8);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 8, Data << 8);
        break;
      }
      case ADE_IntEv__REVPSUM2:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 7, Data << 7);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 7, Data << 7);
        break;
      }
      case ADE_IntEv__REVPSUM1:
      {
        ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_EVENT_STATUS, 7 << 6, Data << 6);
        // ADE90xx_WriteRegLog(EVENT_STATUS, 7 << 6, Data << 6);
        break;
      }
      default:
      {
        break;
      }
    }
  }
}
void
ADE90xx_GetIntEvConfig(ADE90xx_Handler_t *ADE_Handler, bool SelectStatusMask, uint16_t Command, uint32_t *Data)
{
  ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_STATUS0, 1 << 23, Data); // ???
  if (SelectStatusMask)
  {
    switch(Command)
    {
      // MASK0:
      case ADE_IntEv_MISMTCH:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 24, NULL);
        break;
      }
      case ADE_IntEv_COH_WFB_FULL:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 23, NULL);
        break;
      }
      case ADE_IntEv_WFB_TRIG:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 22, NULL);
        break;
      }
      case ADE_IntEv_PF_RDY:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 21, NULL);
        break;
      }
      case ADE_IntEv_PWRRDY:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 18, NULL);
        break;
      }
      case ADE_IntEv_PAGE_FULL:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 17, NULL);
        break;
      }
      case ADE_IntEv_WFB_TRIG_IRQ:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 16, NULL);
        break;
      }
      case ADE_IntEv_DREADY:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 15, NULL);
        break;
      }
      case ADE_IntEv_CF4:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 14, NULL);
        break;
      }
      case ADE_IntEv_CF3:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 13, NULL);
        break;
      }
      case ADE_IntEv_CF2:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 12, NULL);
        break;
      }
      case ADE_IntEv_CF1:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 11, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM4:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 10, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM3:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 9, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM2:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 8, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM1:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 7, NULL);
        break;
      }
      case ADE_IntEv_REVRPC:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 6, NULL);
        break;
      }
      case ADE_IntEv_REVRPB:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 5, NULL);
        break;
      }
      case ADE_IntEv_REVRPA:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 4, NULL);
        break;
      }
      case ADE_IntEv_REVAPC:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 3, NULL);
        break;
      }
      case ADE_IntEv_REVAPB:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 2, NULL);
        break;
      }
      case ADE_IntEv_REVAPA:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1 << 1, NULL);
        break;
      }
      case ADE_IntEv_EGYRDY:
      {
        *Data = ADE90xx_ReadRegLog(MASK0, 1, NULL);
        break;
      }
      // MASK1:
      case ADE_IntEv_ERROR3:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 31, NULL);
        break;
      }
      case ADE_IntEv_ERROR2:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 30, NULL);
        break;
      }
      case ADE_IntEv_ERROR1:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 29, NULL);
        break;
      }
      case ADE_IntEv_ERROR0:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 28, NULL);
        break;
      }
      case ADE_IntEv_CRC_DONE:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 27, NULL);
        break;
      }
      case ADE_IntEv_CRC_CHG:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 26, NULL);
        break;
      }
      case ADE_IntEv_SEQERR:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 18, NULL);
        break;
      }
      case ADE_IntEv_RSTDONE:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 16, NULL);
        break;
      }
      case ADE_IntEv_ZXIC:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 15, NULL);
        break;
      }
      case ADE_IntEv_ZXIB:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 14, NULL);
        break;
      }
      case ADE_IntEv_ZXIA:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 13, NULL);
        break;
      }
      case ADE_IntEv_ZXCOMB:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 12, NULL);
        break;
      }
      case ADE_IntEv_ZXVC:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 11, NULL);
        break;
      }
      case ADE_IntEv_ZXVB:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 10, NULL);
        break;
      }
      case ADE_IntEv_ZXVA:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 9, NULL);
        break;
      }
      case ADE_IntEv_ZXTOVC:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 8, NULL);
        break;
      }
      case ADE_IntEv_ZXTOVB:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 7, NULL);
        break;
      }
      case ADE_IntEv_ZXTOVA:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 6, NULL);
        break;
      }
      case ADE_IntEv_RFNOLOAD:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 4, NULL);
        break;
      }
      case ADE_IntEv_VANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 2, NULL);
        break;
      }
      case ADE_IntEv_RNLOAD:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1 << 1, NULL);
        break;
      }
      case ADE_IntEv_ANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(MASK1, 1, NULL);
        break;
      }
      // EVENT_MASK:
      case ADE_IntEv__DREADY:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 1 << 16, NULL);
        break;
      }
      case ADE_IntEv__RFNOLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 14, NULL);
        break;
      }
      case ADE_IntEv__VANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 12, NULL);
        break;
      }
      case ADE_IntEv__RNLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 11, NULL);
        break;
      }
      case ADE_IntEv__ANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 10, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM4:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 9, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM3:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 8, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM2:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 7, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM1:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_MASK, 7 << 6, NULL);
        break;
      }
      default:
      {
        PROGRAMLOG("    -----   COMMAND NOT FOUND   -----    \r\n");
        break;
      }
    }
  }
  else
  {
    switch(Command)
    {
      // STATUS0:
      case ADE_IntEv_MISMTCH:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 24, NULL);
        break;
      }
      case ADE_IntEv_COH_WFB_FULL:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 23, NULL);
        break;
      }
      case ADE_IntEv_WFB_TRIG:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 22, NULL);
        break;
      }
      case ADE_IntEv_PF_RDY:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 21, NULL);
        break;
      }
      case ADE_IntEv_PWRRDY:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 18, NULL);
        break;
      }
      case ADE_IntEv_PAGE_FULL:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 17, NULL);
        break;
      }
      case ADE_IntEv_WFB_TRIG_IRQ:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 16, NULL);
        break;
      }
      case ADE_IntEv_DREADY:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 15, NULL);
        break;
      }
      case ADE_IntEv_CF4:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 14, NULL);
        break;
      }
      case ADE_IntEv_CF3:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 13, NULL);
        break;
      }
      case ADE_IntEv_CF2:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 12, NULL);
        break;
      }
      case ADE_IntEv_CF1:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 11, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM4:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 10, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM3:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 9, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM2:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 8, NULL);
        break;
      }
      case ADE_IntEv_REVPSUM1:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 7, NULL);
        break;
      }
      case ADE_IntEv_REVRPC:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 6, NULL);
        break;
      }
      case ADE_IntEv_REVRPB:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 5, NULL);
        break;
      }
      case ADE_IntEv_REVRPA:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 4, NULL);
        break;
      }
      case ADE_IntEv_REVAPC:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 3, NULL);
        break;
      }
      case ADE_IntEv_REVAPB:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 2, NULL);
        break;
      }
      case ADE_IntEv_REVAPA:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1 << 1, NULL);
        break;
      }
      case ADE_IntEv_EGYRDY:
      {
        *Data = ADE90xx_ReadRegLog(STATUS0, 1, NULL);
        break;
      }
      // STATUS1:
      case ADE_IntEv_ERROR3:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 31, NULL);
        break;
      }
      case ADE_IntEv_ERROR2:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 30, NULL);
        break;
      }
      case ADE_IntEv_ERROR1:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 29, NULL);
        break;
      }
      case ADE_IntEv_ERROR0:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 28, NULL);
        break;
      }
      case ADE_IntEv_CRC_DONE:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 27, NULL);
        break;
      }
      case ADE_IntEv_CRC_CHG:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 26, NULL);
        break;
      }
      case ADE_IntEv_SEQERR:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 18, NULL);
        break;
      }
      case ADE_IntEv_RSTDONE:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 16, NULL);
        break;
      }
      case ADE_IntEv_ZXIC:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 15, NULL);
        break;
      }
      case ADE_IntEv_ZXIB:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 14, NULL);
        break;
      }
      case ADE_IntEv_ZXIA:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 13, NULL);
        break;
      }
      case ADE_IntEv_ZXCOMB:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 12, NULL);
        break;
      }
      case ADE_IntEv_ZXVC:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 11, NULL);
        break;
      }
      case ADE_IntEv_ZXVB:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 10, NULL);
        break;
      }
      case ADE_IntEv_ZXVA:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 9, NULL);
        break;
      }
      case ADE_IntEv_ZXTOVC:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 8, NULL);
        break;
      }
      case ADE_IntEv_ZXTOVB:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 7, NULL);
        break;
      }
      case ADE_IntEv_ZXTOVA:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 6, NULL);
        break;
      }
      case ADE_IntEv_RFNOLOAD:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 4, NULL);
        break;
      }
      case ADE_IntEv_VANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 2, NULL);
        break;
      }
      case ADE_IntEv_RNLOAD:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1 << 1, NULL);
        break;
      }
      case ADE_IntEv_ANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(STATUS1, 1, NULL);
        break;
      }
      // EVENT_STATUS:
      case ADE_IntEv__DREADY:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 1 << 16, NULL);
        break;
      }
      case ADE_IntEv__RFNOLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 14, NULL);
        break;
      }
      case ADE_IntEv__VANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 12, NULL);
        break;
      }
      case ADE_IntEv__RNLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 11, NULL);
        break;
      }
      case ADE_IntEv__ANLOAD:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 10, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM4:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 9, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM3:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 8, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM2:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 7, NULL);
        break;
      }
      case ADE_IntEv__REVPSUM1:
      {
        *Data = ADE90xx_ReadRegLog(EVENT_STATUS, 7 << 6, NULL);
        break;
      }
      default:
      {
        PROGRAMLOG("    -----   COMMAND NOT FOUND   -----    \r\n");
        break;
      }
    }
  }
}
#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 1
//-------------------------- IRQ Handlers
void
ADE90xx_IRQ0_Handler(ADE90xx_Handler_t *ADE_Handler)
{
  ADE_Handler->IsActiveIRQ0 = true;
}
void
ADE90xx_IRQ1_Handler(ADE90xx_Handler_t *ADE_Handler)
{
  ADE_Handler->IsActiveIRQ1 = true;
}
#endif
#endif
//----------------------------- OTHERS ------------------------------//
// Accumulated Power Configuration
void
ADE90xx_SetAPWRConfig(ADE90xx_Handler_t *ADE_Handler, FPType TimeInSecond)
{
  if(TimeInSecond > 2.048) TimeInSecond = 2.48;
  else if(TimeInSecond < 0.00025) TimeInSecond = 0.00025;

  ADE90xx_WriteRegLog(EP_CFG, 1, 0, "Disable EGY_PWR_EN"); // Is Enabled from ADE90xx_Init and has to be disabled before writing new values
  ADE_Handler->PWR_TIME_VAL = round((TimeInSecond * 4000) - 1);
  ADE90xx_WriteValLog("APWRT", PWR_TIME, 0x1FFF, ADE_Handler->PWR_TIME_VAL, (ADE90xx_ValLogFunc(PWR_TIME, 0x1FFF) + 1) / 4000, NULL);
  ADE90xx_WriteRegLog(EP_CFG, 1, 1, "Enable EGY_PWR_EN"); // Enable
}
// Accumulated Energy Configuration
void
ADE90xx_SetAEGYConfig(ADE90xx_Handler_t *ADE_Handler, FPType TimeInSecond, bool useFreqDSP, uint8_t ZeroCrossChannel, bool Mode)
{
  if(TimeInSecond > 2.048) TimeInSecond = 2.48;
  else if(TimeInSecond < 0.00025) TimeInSecond = 0.00025;

  ADE90xx_WriteRegLog(EP_CFG, 1, 0, "Disable EGY_PWR_EN"); // Is Enabled from ADE90xx_Init and has to be disabled before writing new values
  // ADE90xx_WriteRegLog(EP_CFG, 1 << 4, Mode << 4, (Mode) ? ("Overwite Accumulating Energy") : ("Adding Accumulating Energy")); // EGY_LD_ACCUM: Enable:0 Disable:1
  ADE90xx_WriteRegLog(EP_CFG, 1 << 4, 0 << 4, (0) ? ("Overwite Energy") : ("Accumulate Energy")); // EGY_LD_ACCUM: Enable:0 Disable:1
  ADE90xx_WriteRegLog(EP_CFG, 2, (useFreqDSP ? 0 : 1) << 1, (useFreqDSP) ? ("EGY_TMR_MODE: 4k SPS") : ("EGY_TMR_MODE: Zero Cross"));
  if (useFreqDSP == 0/*Zero Cross*/ && ZeroCrossChannel != 5)
  {
    ADE90xx_WriteRegLog(ZX_LP_SEL, 6, ZeroCrossChannel << 1, (ZeroCrossChannel == 3) ? ("ZC Ch: Combined A, B and C") : ((ZeroCrossChannel == 2) ? ("ZC Ch: VoltageC") : ((ZeroCrossChannel == 1) ? ("ZC Ch: VoltageB") : ("ZC Ch: VoltageA"))));
  }
  ADE90xx_WriteRegLog(EP_CFG, 1 << 5, Mode << 5, (Mode) ? ("Read Reset value") : ("Read Maintain value")); // Enable:1 Disable:0
  ADE90xx_WriteRegLog(EP_CFG, 1, 1, "Enable EGY_PWR_EN"); // Enable
  ADE_Handler->EGY_TIME_VAL = round((TimeInSecond * 4000) - 1);
  ADE90xx_WriteValLog("AEGYT", EGY_TIME, 0x1FFF, ADE_Handler->EGY_TIME_VAL, (ADE90xx_ValLogFunc(EGY_TIME, 0x1FFF) + 1) / 4000, NULL);
}
void
ADE90xx_DisableADC(ADE90xx_Handler_t *ADE_Handler, uint8_t Mask)
{
  ADE90xx_WriteRegLog(CHNL_DIS, 0x7F, Mask, "Disable ADC");
}
void
ADE90xx_ADCRedirect(ADE90xx_Handler_t *ADE_Handler, uint8_t Channel, uint8_t RedirectChannel)
{
  ADE90xx_WriteRegLog(ADC_REDIRECT, 7 << (3 * (((Channel <= 6) ? (Channel) : (4/*VA*/)))), (((RedirectChannel <= 6) ? (RedirectChannel) : (4/*VA*/))), NULL);
}
//----------------------------- Gains
void
ADE90xx_AVGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("AVGAIN", AVGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), (ADE90xx_ValLogFunc(AVGAIN, MAX_MASK) / 134217728) + 1, NULL);
}
void
ADE90xx_AIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("AIGAIN", AIGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), (ADE90xx_ValLogFunc(AIGAIN, MAX_MASK) / 134217728) + 1, NULL);
}
void
ADE90xx_BVGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("BVGAIN", BVGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), (ADE90xx_ValLogFunc(BVGAIN, MAX_MASK) / 134217728) + 1, NULL);
}
void
ADE90xx_BIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("BIGAIN", BIGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), (ADE90xx_ValLogFunc(BIGAIN, MAX_MASK) / 134217728) + 1, NULL);
}
void
ADE90xx_CVGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("CVGAIN", CVGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), (ADE90xx_ValLogFunc(CVGAIN, MAX_MASK) / 134217728) + 1, NULL);
}
void
ADE90xx_CIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("CIGAIN", CIGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), (ADE90xx_ValLogFunc(CIGAIN, MAX_MASK) / 134217728) + 1, NULL);
}
void
ADE90xx_NIGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("NIGAIN", NIGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), (ADE90xx_ValLogFunc(NIGAIN, MAX_MASK) / 134217728) + 1, NULL);
}
void
ADE90xx_APGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("APGAIN", APGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), ((FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APGAIN, MAX_MASK, NULL) / 134217728) + 1, NULL);
}
void
ADE90xx_BPGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("BPGAIN", BPGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), ((FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPGAIN, MAX_MASK, NULL) / 134217728) + 1, NULL);
}
void
ADE90xx_CPGAIN(ADE90xx_Handler_t *ADE_Handler, FPType GainValue)
{
  ADE90xx_WriteValLog("CPGAIN", CPGAIN, MAX_MASK, (uint32_t)round((GainValue - 1) * 134217728), ((FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPGAIN, MAX_MASK, NULL) / 134217728) + 1, NULL);
}
void
ADE90xx_PGAGain(ADE90xx_Handler_t *ADE_Handler, uint8_t Channel, uint8_t GainValue)
{
  switch (Channel)
  {
    case 0:
      ADE90xx_WriteRegLog(PGA_GAIN, 0x3      , (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("IA: 4") : ((GainValue == 2) ? ("IA: 2") : (/*GainValue == 1*/"IA: 1")));
    break;
    case 1:
      ADE90xx_WriteRegLog(PGA_GAIN, 0x3 << 2 , (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("IB: 4") : ((GainValue == 2) ? ("IB: 2") : (/*GainValue == 1*/"IB: 1")));
    break;
    case 2:
      ADE90xx_WriteRegLog(PGA_GAIN, 0x3 << 4 , (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("IC: 4") : ((GainValue == 2) ? ("IC: 2") : (/*GainValue == 1*/"IC: 1")));
    break;
    case 3:
      ADE90xx_WriteRegLog(PGA_GAIN, 0x3 << 6 , (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("IN: 4") : ((GainValue == 2) ? ("IN: 2") : (/*GainValue == 1*/"IN: 1")));
    break;
    // case 4:
    //   ADE90xx_WriteRegLog(PGA_GAIN, 0x3 << 8 , (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("VA: 4") : ((GainValue == 2) ? ("VA: 2") : (/*GainValue == 1*/"VA: 1")));
    // break;
    case 5:
      ADE90xx_WriteRegLog(PGA_GAIN, 0x3 << 10, (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("VB: 4") : ((GainValue == 2) ? ("VB: 2") : (/*GainValue == 1*/"VB: 1")));
    break;
    case 6:
      ADE90xx_WriteRegLog(PGA_GAIN, 0x3 << 12, (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("VC: 4") : ((GainValue == 2) ? ("VC: 2") : (/*GainValue == 1*/"VC: 1")));
    break;
    default: // Includes case 4
      ADE90xx_WriteRegLog(PGA_GAIN, 0x3 << 8 , (GainValue == 4) ? (2) : ((GainValue == 2) ? (1) : (/*GainValue == 1*/0)), (GainValue == 4) ? ("VA: 4") : ((GainValue == 2) ? ("VA: 2") : (/*GainValue == 1*/"VA: 1")));
    break;
  }  
}
//------------------------------ DATA -------------------------------//
// sinc4 data:
FPType
ADE90xx_VSinc4A(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  // FPType VSinc4A = (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AV_SINC_DAT, m, NULL) / FSSinc4 * ADE90xx_VoltageFactor;
  FPType VSinc4A = (FPType)(int32_t)ADE90xx_ReadValLog("VSinc4A", AV_SINC_DAT, MAX_MASK, VSinc4A / FSSinc4 * ADE90xx_VoltageFactor , NULL);
  // VALUE_LOG(s ? s : "------------", n, func, i ? i : (""));
  return VSinc4A / FSSinc4 * ADE90xx_VoltageFactor ;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AV_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AV_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_VoltageFactor;
#endif
}
FPType
ADE90xx_VSinc4B(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VSinc4B = (FPType)(int32_t)ADE90xx_ReadValLog("VSinc4B", BV_SINC_DAT, MAX_MASK, VSinc4B / FSSinc4 * ADE90xx_VoltageFactor , NULL);
  return VSinc4B / FSSinc4 * ADE90xx_VoltageFactor ;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BV_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BV_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_VoltageFactor;
#endif
}
FPType
ADE90xx_VSinc4C(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VSinc4C = (FPType)(int32_t)ADE90xx_ReadValLog("VSinc4C", CV_SINC_DAT, MAX_MASK, VSinc4C / FSSinc4 * ADE90xx_VoltageFactor , NULL);
  return VSinc4C / FSSinc4 * ADE90xx_VoltageFactor ;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CV_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CV_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_VoltageFactor;
#endif
}
FPType
ADE90xx_ISinc4A(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4A = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4A", AI_SINC_DAT, MAX_MASK, ISinc4A / FSSinc4 * ADE90xx_CurrentFactor , NULL);
  return ISinc4A / FSSinc4 * ADE90xx_CurrentFactor ;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
}
FPType
ADE90xx_ISinc4B(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4B = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4B", BI_SINC_DAT, MAX_MASK, ISinc4B / FSSinc4 * ADE90xx_CurrentFactor , NULL);
  return ISinc4B / FSSinc4 * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
}
FPType
ADE90xx_ISinc4C(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4C = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4C", CI_SINC_DAT, MAX_MASK, ISinc4C / FSSinc4 * ADE90xx_CurrentFactor , NULL);
  return ISinc4C / FSSinc4 * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
}
FPType
ADE90xx_ISinc4N(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4N = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4N", NI_SINC_DAT, MAX_MASK, ISinc4N / FSSinc4 * ADE90xx_CurrentFactor , NULL);
  return ISinc4N / FSSinc4 * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NI_SINC_DAT, MAX_MASK, NULL) / FSSinc4 * ADE90xx_CurrentFactor;
#endif
}
// sinc4 + IIR Low-pass filtered data:
FPType
ADE90xx_VSinc4LPFA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VSinc4LPFA = (FPType)(int32_t)ADE90xx_ReadValLog("VSinc4LPFA", AV_LPF_DAT, MAX_MASK, VSinc4LPFA / FSDec * ADE90xx_VoltageFactor , NULL);
  return VSinc4LPFA / FSDec * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AV_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AV_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_VoltageFactor;
#endif
}
FPType
ADE90xx_VSinc4LPFB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VSinc4LPFB = (FPType)(int32_t)ADE90xx_ReadValLog("VSinc4LPFB", BV_LPF_DAT, MAX_MASK, VSinc4LPFB / FSDec * ADE90xx_VoltageFactor , NULL);
  return VSinc4LPFB / FSDec * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BV_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BV_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_VoltageFactor;
#endif
}
FPType
ADE90xx_VSinc4LPFC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VSinc4LPFC = (FPType)(int32_t)ADE90xx_ReadValLog("VSinc4LPFC", CV_LPF_DAT, MAX_MASK, VSinc4LPFC / FSDec * ADE90xx_VoltageFactor , NULL);
  return VSinc4LPFC / FSDec * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CV_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_cV_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_VoltageFactor;
#endif
}
FPType
ADE90xx_ISinc4LPFA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4LPFA = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4LPFA", AI_LPF_DAT, MAX_MASK, ISinc4LPFA / FSDec * ADE90xx_CurrentFactor , NULL);
  return ISinc4LPFA / FSDec * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
}
FPType
ADE90xx_ISinc4LPFB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4LPFB = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4LPFB", BI_LPF_DAT, MAX_MASK, ISinc4LPFB / FSDec * ADE90xx_CurrentFactor , NULL);
  return ISinc4LPFB / FSDec * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
}
FPType
ADE90xx_ISinc4LPFC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4LPFC = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4LPFC", CI_LPF_DAT, MAX_MASK, ISinc4LPFC / FSDec * ADE90xx_CurrentFactor , NULL);
  return ISinc4LPFC / FSDec * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
}
FPType
ADE90xx_ISinc4LPFN(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ISinc4LPFN = (FPType)(int32_t)ADE90xx_ReadValLog("ISinc4LPFN", NI_LPF_DAT, MAX_MASK, ISinc4LPFN / FSDec * ADE90xx_CurrentFactor , NULL);
  return ISinc4LPFN / FSDec * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NI_LPF_DAT, MAX_MASK, NULL) / FSDec * ADE90xx_CurrentFactor;
#endif
}
// DSP data:
FPType
ADE90xx_VDSPA(ADE90xx_Handler_t *ADE_Handler)
{ 
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VDSPA = (FPType)(int32_t)ADE90xx_ReadValLog("VDSPA", AV_PCF, MAX_MASK, VDSPA / FSxPCF * ADE90xx_VoltageFactor , NULL);
  return VDSPA / FSxPCF * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AV_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AV_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;
#endif
}
FPType   
ADE90xx_VDSPB(ADE90xx_Handler_t *ADE_Handler)
{ 
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VDSPB = (FPType)(int32_t)ADE90xx_ReadValLog("VDSPB", BV_PCF, MAX_MASK, VDSPB / FSxPCF * ADE90xx_VoltageFactor , NULL);
  return VDSPB / FSxPCF * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BV_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BV_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;
#endif
}
FPType   
ADE90xx_VDSPC(ADE90xx_Handler_t *ADE_Handler)
{ 
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VDSPC = (FPType)(int32_t)ADE90xx_ReadValLog("VDSPC", CV_PCF, MAX_MASK, VDSPC / FSxPCF * ADE90xx_VoltageFactor , NULL);
  return VDSPC / FSxPCF * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CV_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CV_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;
#endif
}
FPType   
ADE90xx_IDSPA(ADE90xx_Handler_t *ADE_Handler)
{ 
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IDSPA = (FPType)(int32_t)ADE90xx_ReadValLog("IDSPA", AI_PCF, MAX_MASK, IDSPA / FSxPCF * ADE90xx_CurrentFactor , NULL);
  return IDSPA / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
}
FPType   
ADE90xx_IDSPB(ADE90xx_Handler_t *ADE_Handler)
{ 
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IDSPB = (FPType)(int32_t)ADE90xx_ReadValLog("IDSPB", BI_PCF, MAX_MASK, IDSPB / FSxPCF * ADE90xx_CurrentFactor , NULL);
  return IDSPB / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
}
FPType   
ADE90xx_IDSPC(ADE90xx_Handler_t *ADE_Handler)
{ 
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IDSPC = (FPType)(int32_t)ADE90xx_ReadValLog("IDSPC", CI_PCF, MAX_MASK, IDSPC / FSxPCF * ADE90xx_CurrentFactor , NULL);
  return IDSPC / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
}
FPType   
ADE90xx_IDSPN(ADE90xx_Handler_t *ADE_Handler)
{ 
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IDSPN = (FPType)(int32_t)ADE90xx_ReadValLog("IDSPN", NI_PCF, MAX_MASK, IDSPN / FSxPCF * ADE90xx_CurrentFactor , NULL);
  return IDSPN / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NI_PCF, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;
#endif
}
// RMS values:
FPType
ADE90xx_VRMSA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VRMSA = (FPType)(int32_t)ADE90xx_ReadValLog("VRMSA", AVRMS, MAX_MASK, VRMSA / FSxPCF * ADE90xx_VoltageFactor , NULL); // 52866837.0;
  return VRMSA / FSxPCF * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;// 52866837.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;// 52866837.0;
#endif
}
FPType
ADE90xx_VRMSB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VRMSB = (FPType)(int32_t)ADE90xx_ReadValLog("VRMSB", BVRMS, MAX_MASK, VRMSB / FSxPCF * ADE90xx_VoltageFactor , NULL); // 52866837.0;
  return VRMSB / FSxPCF * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;// 52866837.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;// 52866837.0;
#endif
}
FPType
ADE90xx_VRMSC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType VRMSC = (FPType)(int32_t)ADE90xx_ReadValLog("VRMSC", CVRMS, MAX_MASK, VRMSC / FSxPCF * ADE90xx_VoltageFactor , NULL); // 52866837.0;
  return VRMSC / FSxPCF * ADE90xx_VoltageFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;// 52866837.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_VoltageFactor;// 52866837.0;
#endif
}
FPType
ADE90xx_IRMSA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IRMSA = (FPType)(int32_t)ADE90xx_ReadValLog("IRMSA", AIRMS, MAX_MASK, IRMSA / FSxPCF * ADE90xx_CurrentFactor , NULL); // 52866837.0;
  return IRMSA / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
}
FPType
ADE90xx_IRMSB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IRMSB = (FPType)(int32_t)ADE90xx_ReadValLog("IRMSB", BIRMS, MAX_MASK, IRMSB / FSxPCF * ADE90xx_CurrentFactor , NULL); // 52866837.0;
  return IRMSB / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
}
FPType
ADE90xx_IRMSC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IRMSC = (FPType)(int32_t)ADE90xx_ReadValLog("IRMSC", CIRMS, MAX_MASK, IRMSC / FSxPCF * ADE90xx_CurrentFactor , NULL); // 52866837.0;
  return IRMSC / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
}
FPType
ADE90xx_IRMSN(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType IRMSN = (FPType)(int32_t)ADE90xx_ReadValLog("IRMSN", NIRMS, MAX_MASK, IRMSN / FSxPCF * ADE90xx_CurrentFactor , NULL); // 52866837.0;
  return IRMSN / FSxPCF * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_NIRMS, MAX_MASK, NULL) / FSxPCF * ADE90xx_CurrentFactor;// 52866837.0;
#endif
}
// Low-pass filtered total active powers:
FPType
ADE90xx_LPFTAcPA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType LPFTAcPA = (FPType)(int32_t)ADE90xx_ReadValLog("LPFTAcPA", AWATT, MAX_MASK, LPFTAcPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return LPFTAcPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATT, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATT, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
FPType
ADE90xx_LPFTAcPB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType LPFTAcPB = (FPType)(int32_t)ADE90xx_ReadValLog("LPFTAcPB", BWATT, MAX_MASK, LPFTAcPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return LPFTAcPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATT, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATT, MAX_MASK, NULL) / 20823646.0;
#endif
}
FPType
ADE90xx_LPFTAcPC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType LPFTAcPC = (FPType)(int32_t)ADE90xx_ReadValLog("LPFTAcPC", CWATT, MAX_MASK, LPFTAcPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return LPFTAcPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATT, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATT, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
// Low-pass filtered total reactive powers:
FPType
ADE90xx_LPFTRePA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType LPFTRePA = (FPType)(int32_t)ADE90xx_ReadValLog("LPFTRePA", AVAR, MAX_MASK, LPFTRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return LPFTRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
FPType
ADE90xx_LPFTRePB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType LPFTRePB = (FPType)(int32_t)ADE90xx_ReadValLog("LPFTRePB", BVAR, MAX_MASK, LPFTRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return LPFTRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
FPType
ADE90xx_LPFTRePC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType LPFTRePC = (FPType)(int32_t)ADE90xx_ReadValLog("LPFTRePC", CVAR, MAX_MASK, LPFTRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return LPFTRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
// Total aparent powers:
FPType
ADE90xx_TAPA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType TAPA = (FPType)(int32_t)ADE90xx_ReadValLog("TAPA", AVA, MAX_MASK, TAPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return TAPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVA, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVA, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
FPType   
ADE90xx_TAPB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType TAPB = (FPType)(int32_t)ADE90xx_ReadValLog("TAPB", BVA, MAX_MASK, TAPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return TAPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVA, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVA, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
FPType   
ADE90xx_TAPC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType TAPC = (FPType)(int32_t)ADE90xx_ReadValLog("TAPC", CVA, MAX_MASK, TAPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return TAPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVA, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVA, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
// Fundamental reactive powers:
FPType
ADE90xx_FRePA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType FRePA = (FPType)(int32_t)ADE90xx_ReadValLog("FRePA", AFVAR, MAX_MASK, FRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return FRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
FPType   
ADE90xx_FRePB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType FRePB = (FPType)(int32_t)ADE90xx_ReadValLog("FRePB", BFVAR, MAX_MASK, FRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return FRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
FPType   
ADE90xx_FRePC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType FRePC = (FPType)(int32_t)ADE90xx_ReadValLog("FRePC", CFVAR, MAX_MASK, FRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor , NULL);
  return FRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVAR, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ;
#endif
}
// Accumulated total active powers:
FPType
ADE90xx_ATAcPA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAcPA = (FPType)(int32_t)ADE90xx_ReadValLog("ATAcPA", AWATT_ACC, MAX_MASK, ATAcPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATAcPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATT_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATT_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_ATAcPB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAcPB = (FPType)(int32_t)ADE90xx_ReadValLog("ATAcPB", BWATT_ACC, MAX_MASK, ATAcPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATAcPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATT_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATT_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_ATAcPC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAcPC = (FPType)(int32_t)ADE90xx_ReadValLog("ATAcPC", CWATT_ACC, MAX_MASK, ATAcPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATAcPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATT_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATT_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
// Accumulated total reactive powers:
FPType
ADE90xx_ATRePA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATRePA = (FPType)(int32_t)ADE90xx_ReadValLog("ATRePA", AVAR_ACC, MAX_MASK, ATRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_ATRePB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATRePB = (FPType)(int32_t)ADE90xx_ReadValLog("ATRePB", BVAR_ACC, MAX_MASK, ATRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_ATRePC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATRePC = (FPType)(int32_t)ADE90xx_ReadValLog("ATRePC", CVAR_ACC, MAX_MASK, ATRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
// Accumulated total apparent powers:
FPType
ADE90xx_ATAPA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAPA = (FPType)(int32_t)ADE90xx_ReadValLog("ATAPA", AVA_ACC, MAX_MASK, ATAPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATAPA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVA_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVA_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_ATAPB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAPB = (FPType)(int32_t)ADE90xx_ReadValLog("ATAPB", BVA_ACC, MAX_MASK, ATAPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);
  return ATAPB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVA_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVA_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_ATAPC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAPC = (FPType)(int32_t)ADE90xx_ReadValLog("ATAPC", CVA_ACC, MAX_MASK, ATAPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL); 
  return ATAPC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVA_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVA_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
// Accumulated total active energies:
FPType
ADE90xx_ATAcEA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  // FPType ATAcEA = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  FPType ATAcEA = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  PROGRAMLOG("ATAcEA       | 0x%03X H | ---------- | %-10f | ----------    | AWATTHR_HI = AWATTHR_LO + 1\r\n", ADE_ADD_AWATTHR_LO, ATAcEA);  
  return ATAcEA;
#else
  // return (FPType)((((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_LO, MAX_MASK, NULL)) / 524288) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
#endif
#else
  // return (FPType)((((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_LO, MAX_MASK, NULL)) / 524288) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
#endif
}
FPType   
ADE90xx_ATAcEB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  // FPType ATAcEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor  ; // / 20823646.0;
  FPType ATAcEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  PROGRAMLOG("ATAcEB       | 0x%03X H | ---------- | %-10f | ----------    | BWATTHR_HI = BWATTHR_LO + 1\r\n", ADE_ADD_BWATTHR_LO, ATAcEB);  
  return ATAcEB;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BWATTHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
FPType   
ADE90xx_ATAcEC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAcEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType ATAcEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("ATAcEC       | 0x%03X H | ---------- | %-10f | ----------    | CWATTHR_HI = CWATTHR_LO + 1\r\n", ADE_ADD_CWATTHR_LO, ATAcEC);  
  return ATAcEC;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CWATTHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
// Accumulated total reactive energies:
FPType
ADE90xx_ATReEA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATReEA =  (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType ATReEA = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("ATReEA       | 0x%03X H | ---------- | %-10f | ----------    | AVARHR_HI = AVARHR_LO + 1\r\n", ADE_ADD_AVARHR_LO, ATReEA);  
  return ATReEA;
#else
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
#endif
#else
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
#endif
}
FPType   
ADE90xx_ATReEB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATReEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType ATReEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("ATReEB       | 0x%03X H | ---------- | %-10f | ----------    | BVARHR_HI = BVARHR_LO + 1\r\n", ADE_ADD_BVARHR_LO, ATReEB);  
  return ATReEB;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
FPType   
ADE90xx_ATReEC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATReEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType ATReEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("ATReEC       | 0x%03X H | ---------- | %-10f | ----------    | CVARHR_HI = CVARHR_LO + 1\r\n", ADE_ADD_CVARHR_LO, ATReEC);  
  return ATReEC;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
// Accumulated total apparent energies:
FPType
ADE90xx_ATAEA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAEA = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType ATAEA = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("ATAEA        | 0x%03X H | ---------- | %-10f | ----------    | AVAHR_HI = AVAHR_LO + 1\r\n", ADE_ADD_AVAHR_LO, ATAEA);  
  return ATAEA;
#else
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
FPType   
ADE90xx_ATAEB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType ATAEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("ATAEB        | 0x%03X H | ---------- | %-10f | ----------    | BVAHR_HI = BVAHR_LO + 1\r\n", ADE_ADD_BVAHR_LO, ATAEB);  
  return ATAEB;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
FPType   
ADE90xx_ATAEC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType ATAEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType ATAEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("ATAEC        | 0x%03X H | ---------- | %-10f | ----------    | CVAHR_HI = CVAHR_LO + 1\r\n", ADE_ADD_CVAHR_LO, ATAEC);  
  return ATAEC;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CVAHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
// Accumulated fundamental reactive powers:
FPType
ADE90xx_AFRePA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AFRePA = (FPType)(int32_t)ADE90xx_ReadValLog("AFRePA", AFVAR_ACC, MAX_MASK, AFRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);  
  return AFRePA / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_AFRePB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AFRePB = (FPType)(int32_t)ADE90xx_ReadValLog("AFRePB", BFVAR_ACC, MAX_MASK, AFRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);  
  return AFRePB / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
FPType   
ADE90xx_AFRePC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AFRePC = (FPType)(int32_t)ADE90xx_ReadValLog("AFRePC", CFVAR_ACC, MAX_MASK, AFRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL , NULL);   
  return AFRePC / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVAR_ACC, MAX_MASK, NULL) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor * 8192 / ADE_Handler->PWR_TIME_VAL ;
#endif
}
// Accumulated fundamental reactive energies:
FPType
ADE90xx_AFReEA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AFReEA = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType AFReEA = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("AFReEA       | 0x%03X H | ---------- | %-10f | ----------    | AFVARHR_HI = AFVARHR_LO + 1\r\n", ADE_ADD_AFVARHR_LO, AFReEA);  
  return AFReEA;
#else
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_AFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
FPType   
ADE90xx_AFReEB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AFReEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType AFReEB = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("AFReEB       | 0x%03X H | ---------- | %-10f | ----------    | BFVARHR_HI = BFVARHR_LO + 1\r\n", ADE_ADD_BFVARHR_LO, AFReEB);  
  return AFReEB;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
FPType   
ADE90xx_AFReEC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AFReEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // FPType AFReEC = (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
  PROGRAMLOG("AFReEC       | 0x%03X H | ---------- | %-10f | ----------    | CFVARHR_HI = CFVARHR_LO + 1\r\n", ADE_ADD_CFVARHR_LO, AFReEC);  
  return AFReEC;
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
#else
  return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_LO, MAX_MASK, NULL) >> 19) | (int64_t)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_HI, MAX_MASK, NULL) * 8192) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor / 3600 / ADE_Handler->EGY_TIME_VAL;
  // return (FPType)(((int64_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_HI, MAX_MASK, NULL) << 32) | ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CFVARHR_LO, MAX_MASK, NULL)) / FSPower * ADE90xx_VoltageFactor * ADE90xx_CurrentFactor ; // / 20823646.0;
#endif
}
// Power factors:
FPType
ADE90xx_PFA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType PFA = (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APF, MAX_MASK, NULL) / 134217728.0;
  VALUE_LOG("PFA", APF, PFA, "");    
  return PFA;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APF, MAX_MASK, NULL) / 134217728.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APF, MAX_MASK, NULL) / 134217728.0;
#endif
}
FPType   
ADE90xx_PFB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType PFB = (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPF, MAX_MASK, NULL) / 134217728.0;
  VALUE_LOG("PFB", BPF, PFB, "");   
  return PFB;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPF, MAX_MASK, NULL) / 134217728.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPF, MAX_MASK, NULL) / 134217728.0;
#endif
}
FPType   
ADE90xx_PFC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType PFC = (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPF, MAX_MASK, NULL) / 134217728.0;
  VALUE_LOG("PFC", CPF, PFC, "");   
  return PFC;
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPF, MAX_MASK, NULL) / 134217728.0;
#endif
#else
  return (FPType)(int32_t)ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPF, MAX_MASK, NULL) / 134217728.0;
#endif
}
// Frequencies:
FPType
ADE90xx_FreqA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType FreqA = 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0);
  VALUE_LOG("FreqA", APERIOD, FreqA, "");  
  return FreqA;
#else
  return 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0);
#endif
#else
  return 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0);
#endif
}
FPType
ADE90xx_FreqB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType FreqB = 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0);
  VALUE_LOG("FreqB", BPERIOD, FreqB, "");
  return FreqB;
#else
  return 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0);
#endif
#else
  return 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0);
#endif
}
FPType
ADE90xx_FreqC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType FreqC = 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0);
  VALUE_LOG("FreqC", CPERIOD, FreqC, "");
  return FreqC;
#else
  return 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0);
#endif
#else
  return 262144000.0 / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0);
#endif
}
// Angles:
FPType
ADE90xx_AngleVAtoVB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleVAtoVB = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_VB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleVAtoVB", ANGL_VA_VB, AngleVAtoVB, "Uses APERIOD Reg");
  return AngleVAtoVB;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_VB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_VB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleVBtoVC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleVBtoVC = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VB_VC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleVBtoVC", ANGL_VB_VC, AngleVBtoVC, "Uses BPERIOD Reg");  
  return AngleVBtoVC;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VB_VC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VB_VC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleVAtoVC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleVAtoVC = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_VC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleVAtoVC", ANGL_VA_VC, AngleVAtoVC, "Uses CPERIOD Reg");  
  return AngleVAtoVC;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_VC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_VC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleVAtoIA(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleVAtoIA = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_IA, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleVAtoIA", ANGL_VA_IA, AngleVAtoIA, "Uses APERIOD Reg"); 
  return AngleVAtoIA;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_IA, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VA_IA, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleVBtoIB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleVBtoIB = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VB_IB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleVBtoIB", ANGL_VB_IB, AngleVBtoIB, "Uses BPERIOD Reg"); 
  return AngleVBtoIB;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VB_IB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VB_IB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleVCtoIC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleVCtoIC = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VC_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleVCtoIC", ANGL_VC_IC, AngleVCtoIC, "Uses CPERIOD Reg"); 
  return AngleVCtoIC;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VC_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_VC_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_CPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleIAtoIB(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleIAtoIB = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IA_IB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleIAtoIB", ANGL_IA_IB, AngleIAtoIB, "Uses APERIOD Reg");  
  return AngleIAtoIB;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IA_IB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IA_IB, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleIBtoIC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleIBtoIC = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IB_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleIBtoIC", ANGL_IB_IC, AngleIBtoIC, "Uses BPERIOD Reg");    
  return AngleIBtoIC;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IB_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IB_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_BPERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
FPType
ADE90xx_AngleIAtoIC(ADE90xx_Handler_t *ADE_Handler)
{
#ifdef ADE90xx_Debug_Enable
#if ADE90xx_Debug_Enable == 1
  FPType AngleIAtoIC = 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IA_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
  VALUE_LOG("AngleIAtoIC", ANGL_IA_IC, AngleIAtoIC, "Uses APERIOD Reg");
  return AngleIAtoIC;
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IA_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
#else
  return 184320.0 * ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_ANGL_IA_IC, MAX_MASK, NULL) / (ADE90xx_ReadRegS(ADE_Handler, ADE_ADD_APERIOD, MAX_MASK, NULL) + 1.0); // 262144000.0 * 360/*degree*/ / 512000/*CLKIN/24*/
#endif
}
// Peaks:
FPType
ADE90xx_VPeak(ADE90xx_Handler_t *ADE_Handler, uint8_t *PhaseABC)
{
  uint32_t VPEAK = ADE90xx_ReadValLog("VPEAK", VPEAK, MAX_MASK, (FPType)(int32_t)((VPEAK & 0xFFFFFF) * 32) / FSxPCF * ADE90xx_VoltageFactor, ((VPEAK >> 24) & 1) ? ("Phase A") : (((VPEAK >> 25) & 1) ? ("Phase B") : (((VPEAK >> 26) & 1) ? ("Phase C") : (0))));
  *PhaseABC  = (VPEAK >> 26) & 1;
  *PhaseABC |= (VPEAK >> 24) & 2;
  *PhaseABC |= (VPEAK >> 22) & 4;
  return (FPType)(int32_t)((VPEAK & 0xFFFFFF) * 32) / FSxPCF * ADE90xx_VoltageFactor;
}
FPType
ADE90xx_IPeak(ADE90xx_Handler_t *ADE_Handler, bool *PhaseABC)
{
  uint32_t IPEAK = ADE90xx_ReadValLog("IPEAK", IPEAK, MAX_MASK, (FPType)(int32_t)((IPEAK & 0xFFFFFF) * 32) / FSxPCF * ADE90xx_CurrentFactor,
                   ((IPEAK >> 24) & 1) ? ("Phase A") : (((IPEAK >> 25) & 1) ? ("Phase B") : (((IPEAK >> 26) & 1) ? ("Phase C") : ("NO PHASE!"))));
  *PhaseABC  = (IPEAK >> 26) & 1;
  *PhaseABC |= (IPEAK >> 24) & 2;
  *PhaseABC |= (IPEAK >> 22) & 4;
  return (FPType)(int32_t)((IPEAK & 0xFFFFFF) * 32) / FSxPCF * ADE90xx_CurrentFactor;
}
//---------------------------- Waveform -----------------------------//
void
ADE90xx_SetWFBConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t Data)
{
  switch(Command)
  {
      // WFB_CFG:
      case ADE_WFB_WF_IN_EN:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_CFG, 1 << 12, Data << 12);
        ADE90xx_WriteRegLog(WFB_CFG, 1 << 12, Data << 12, NULL);
        break;
      }
      case ADE_WFB_WF_SRC:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_CFG, 3 << 8, Data << 8);
        ADE90xx_WriteRegLog(WFB_CFG, 3 << 8, Data << 8, NULL);
        break;
      }
      case ADE_WFB_WF_MODE:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_CFG, 3 << 6, Data << 6);
        ADE90xx_WriteRegLog(WFB_CFG, 3 << 6, Data << 6, NULL);
        break;
      }
      case ADE_WFB_WF_CAP_SEL:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_CFG, 1 << 5, Data << 5);
        ADE90xx_WriteRegLog(WFB_CFG, 1 << 5, Data << 5, NULL);
        break;
      }
      case ADE_WFB_WF_CAP_EN:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_CFG, 1 << 4, Data << 4);
        ADE90xx_WriteRegLog(WFB_CFG, 1 << 4, Data << 4, NULL);
        break;
      }
      case ADE_WFB_BURST_CHAN:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_CFG, 15, Data);
        ADE90xx_WriteRegLog(WFB_CFG, 15, Data, NULL);
        break;
      }
      // WFB_PG_IRQEN
      case ADE_WFB_WFB_PG_IRQEN:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_CFG, 15, Data);
        ADE90xx_WriteRegLog(WFB_PG_IRQEN, 0xFFFF, Data, NULL);
        break;
      }
      // WFB_TRG_CFG
      case ADE_WFB_TRIG_FORCE:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 10, Data << 10);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 10, Data << 10, NULL);
        break;
      }
      case ADE_WFB_ZXCOMB:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 9, Data << 9);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 9, Data << 9, NULL);
        break;
      }
      case ADE_WFB_ZXVC:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 8, Data << 8);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 8, Data << 8, NULL);
        break;
      }
      case ADE_WFB_ZXVB:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 7, Data << 7);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 7, Data << 7, NULL);
        break;
      }
      case ADE_WFB_ZXVA:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 6, Data << 6);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 6, Data << 6, NULL);
        break;
      }
      case ADE_WFB_ZXIC:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 5, Data << 5);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 5, Data << 5, NULL);
        break;
      }
      case ADE_WFB_ZXIB:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 4, Data << 4);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 4, Data << 4, NULL);
        break;
      }
      case ADE_WFB_ZXIA:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_CFG, 1 << 3, Data << 3);
        ADE90xx_WriteRegLog(WFB_TRG_CFG, 1 << 3, Data << 3, NULL);
        break;
      }
      // WFB_TRG_STAT
      case ADE_WFB_WFB_LAST_PAGE:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_STAT, 7 << 12, Data << 12);
        ADE90xx_WriteRegLog(WFB_TRG_STAT, 7 << 12, Data << 12, NULL);
        break;
      }
      case ADE_WFB_WFB_TRIG_ADDR:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_WFB_TRG_STAT, 0x7FF, Data);
        ADE90xx_WriteRegLog(WFB_TRG_STAT, 0x7FF, Data, NULL);
        break;
      }
      // ZX_LP_SEL
      case ADE_WFB_LP_SEL:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_ZX_LP_SEL, 3 << 3, Data << 3);
        ADE90xx_WriteRegLog(ZX_LP_SEL, 3 << 3, Data << 3, NULL);
        break;
      }
      // CONFIG2
      case ADE_WFB_UPERIOD_SEL:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_CONFIG2, 1 << 12, Data << 12);
        ADE90xx_WriteRegLog(CONFIG2, 1 << 12, Data << 12, NULL);
        break;
      }
      // USER_PERIOD
      case ADE_WFB_USER_PERIOD:
      {
        // ADE90xx_WriteRegS(ADE_Handler, ADE_ADD_USER_PERIOD, MAX_MASK, Data);
        ADE90xx_WriteRegLog(USER_PERIOD, MAX_MASK, Data, NULL);
        break;
      }
      default:
      {
        PROGRAMLOG("    -----   COMMAND NOT FOUND   -----    \r\n");
        break;
      }
    };

}
void
ADE90xx_GetWFBConfig(ADE90xx_Handler_t *ADE_Handler, uint16_t Command, uint16_t *Data)
{
  switch(Command)
  {
      // WFB_CFG:
      case ADE_WFB_WF_IN_EN:
      {
        *Data = ADE90xx_ReadRegLog(WFB_CFG, 1 << 12, NULL);
        break;
      }
      case ADE_WFB_WF_SRC:
      {
        *Data = ADE90xx_ReadRegLog(WFB_CFG, 3 << 8, NULL);
        break;
      }
      case ADE_WFB_WF_MODE:
      {
        *Data = ADE90xx_ReadRegLog(WFB_CFG, 3 << 6, NULL);
        break;
      }
      case ADE_WFB_WF_CAP_SEL:
      {
        *Data = ADE90xx_ReadRegLog(WFB_CFG, 1 << 5, NULL);
        break;
      }
      case ADE_WFB_WF_CAP_EN:
      {
        *Data = ADE90xx_ReadRegLog(WFB_CFG, 1 << 4, NULL);
        break;
      }
      case ADE_WFB_BURST_CHAN:
      {
        *Data = ADE90xx_ReadRegLog(WFB_CFG, 15, NULL);
        break;
      }
      // WFB_PG_IRQEN
      case ADE_WFB_WFB_PG_IRQEN:
      {
        *Data = ADE90xx_ReadRegLog(WFB_PG_IRQEN, 0xFFFF, NULL);
        break;
      }
      // WFB_TRG_CFG
      case ADE_WFB_TRIG_FORCE:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 10, NULL);
        break;
      }
      case ADE_WFB_ZXCOMB:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 9, NULL);
        break;
      }
      case ADE_WFB_ZXVC:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 8, NULL);
        break;
      }
      case ADE_WFB_ZXVB:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 7, NULL);
        break;
      }
      case ADE_WFB_ZXVA:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 6, NULL);
        break;
      }
      case ADE_WFB_ZXIC:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 5, NULL);
        break;
      }
      case ADE_WFB_ZXIB:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 4, NULL);
        break;
      }
      case ADE_WFB_ZXIA:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_CFG, 1 << 3, NULL);
        break;
      }
      // WFB_TRG_STAT
      case ADE_WFB_WFB_LAST_PAGE:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_STAT, 7 << 12, NULL);
        break;
      }
      case ADE_WFB_WFB_TRIG_ADDR:
      {
        *Data = ADE90xx_ReadRegLog(WFB_TRG_STAT, 0x7FF << 12, NULL);
        break;
      }
      // ZX_LP_SEL
      case ADE_WFB_LP_SEL:
      {
        *Data = ADE90xx_ReadRegLog(ZX_LP_SEL, 3 << 3, NULL);
        break;
      }
      // CONFIG2
      case ADE_WFB_UPERIOD_SEL:
      {
        *Data = ADE90xx_ReadRegLog(CONFIG2, 1 << 12, NULL);
        break;
      }
      // USER_PERIOD
      case ADE_WFB_USER_PERIOD:
      {
        *Data = ADE90xx_ReadRegLog(USER_PERIOD, MAX_MASK, NULL);
        break;
      }
      default:
      {
        PROGRAMLOG("    -----   COMMAND NOT FOUND   -----    \r\n");
        break;
      }
    };
}
void
ADE90xx_StartWFBConfigResample(ADE90xx_Handler_t *ADE_Handler, bool EnableInt)
{
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_CAP_EN, 0);                 // Disable waveform
  if (EnableInt) 
    ADE90xx_SetIntEvConfig(ADE_Handler, 1, ADE_IntEv_COH_WFB_FULL, 1);   // Enable interrupt after filling resample data is done
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_BURST_CHAN, 0);                // Choose all channel
//  ADE90xx_SetWFBConfig(&ADE_Handler, ADE_WFB_WF_SRC, 3);                    // Choose output type (This is for fixed data rate)
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_CAP_SEL, 0);                // Choose resample waveform
//  ADE90xx_SetWFBConfig(&ADE_Handler, ADE_WFB_WF_MODE, 0);                   // Choose waveform filling mode (This is for fixed data rate)
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_IN_EN, 1);                  // Enable IN channel to catch
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_CAP_EN, 1);                 // Enable waveform
}
void
ADE90xx_GetWFBDataResample(ADE90xx_Handler_t *ADE_Handler, ADE90xx_WFBR_Data_t ADE_IA,
                           ADE90xx_WFBR_Data_t ADE_VA, ADE90xx_WFBR_Data_t ADE_IB, ADE90xx_WFBR_Data_t ADE_VB,
                           ADE90xx_WFBR_Data_t ADE_IC, ADE90xx_WFBR_Data_t ADE_VC, ADE90xx_WFBR_Data_t ADE_IN)
{
#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 1 || ADE90xx_BLOCKING_MODE == 2
  ADE90xx_IntHandler(ADE_Handler, 0, ADE_IntEv_COH_WFB_FULL);
#endif
#endif

  ADE_Handler->ADE_SPI_CS_LOW();
  
  ADE_Handler->ADE_SPI_WriteByte(0x80);
  ADE_Handler->ADE_SPI_WriteByte(0x08);

  for (uint16_t CounterNum = 0; CounterNum < 512; CounterNum++)
  {
    if (ADE_IA) ADE_IA[CounterNum] = (FPType)(int16_t)((ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FSResample * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_VA) ADE_VA[CounterNum] = (FPType)(int16_t)((ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FSResample * ADE90xx_VoltageFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_IB) ADE_IB[CounterNum] = (FPType)(int16_t)((ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FSResample * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_VB) ADE_VB[CounterNum] = (FPType)(int16_t)((ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FSResample * ADE90xx_VoltageFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_IC) ADE_IC[CounterNum] = (FPType)(int16_t)((ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FSResample * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_VC) ADE_VC[CounterNum] = (FPType)(int16_t)((ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FSResample * ADE90xx_VoltageFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_IN) ADE_IN[CounterNum] = (FPType)(int16_t)((ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FSResample * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
  }
  ADE_Handler->ADE_SPI_CS_HIGH();

}
void
ADE90xx_StartWFBConfigFixedDataRate(ADE90xx_Handler_t *ADE_Handler, uint8_t SelectChannel, uint8_t SourceWF, uint8_t ModeWF, bool EnableIntPage, uint16_t IntTrigger)
{
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_CAP_EN, 0);                 // Disable waveform
  if (EnableIntPage) 
  {
    ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WFB_PG_IRQEN, 0x8080);       // Select Page 7 and 15 for interrupt
    ADE90xx_SetIntEvConfig(ADE_Handler, 1, ADE_IntEv_PAGE_FULL, 1);        // Enable interrupt after filling a page is done
  }
  if (SelectChannel == 14)
    ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_IN_EN, 1);                // Enable IN channel to catch
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_BURST_CHAN,                    // Choose Channel | 0: All channels | 9: VA
  ((SelectChannel >= 0 && SelectChannel <= 3) || (SelectChannel >= 8 && SelectChannel <= 14)) ? (SelectChannel) : (0));
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_SRC, SourceWF);             // Choose input signal type            
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_CAP_SEL, 1);                // Choose fixed data rate waveform
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_MODE, ModeWF & 0x3);        // Choose waveform filling mode
  if (IntTrigger & 0x7F8)
  {
    if ((IntTrigger >> 10) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_TRIG_FORCE, 1);            // Enable trigger for TRIG_FORCE                  
    if ((IntTrigger >> 9) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_ZXCOMB, 1);                // Enable trigger for ZXCOMB         
    if ((IntTrigger >> 8) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_ZXVC, 1);                  // Enable trigger for ZXVC
    if ((IntTrigger >> 7) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_ZXVB, 1);                  // Enable trigger for ZXVB
    if ((IntTrigger >> 6) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_ZXVA, 1);                  // Enable trigger for ZXVA
    if ((IntTrigger >> 5) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_ZXIC, 1);                  // Enable trigger for ZXIC
    if ((IntTrigger >> 4) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_ZXIB, 1);                  // Enable trigger for ZXIB
    if ((IntTrigger >> 3) & 1)
      ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_ZXIA, 1);                  // Enable trigger for ZXIA
    ADE90xx_SetIntEvConfig(ADE_Handler, 1, ADE_IntEv_WFB_TRIG, 1);         // Enable interrupt for trigger
  }
  else
    ADE90xx_SetIntEvConfig(ADE_Handler, 1, ADE_IntEv_WFB_TRIG, 0);         // Disable interrupt for trigger
  ADE90xx_SetWFBConfig(ADE_Handler, ADE_WFB_WF_CAP_EN, 1);                 // Enable waveform
}
void
ADE90xx_GetWFBDataFixedDataRate(ADE90xx_Handler_t *ADE_Handler, const bool SectionWF, uint8_t SourceWF, ADE90xx_WFBF_Data_t ADE_IA,
                                ADE90xx_WFBF_Data_t ADE_VA, ADE90xx_WFBF_Data_t ADE_IB, ADE90xx_WFBF_Data_t ADE_VB,
                                ADE90xx_WFBF_Data_t ADE_IC, ADE90xx_WFBF_Data_t ADE_VC, ADE90xx_WFBF_Data_t ADE_IN)
{
#ifdef ADE90xx_BLOCKING_MODE
#if ADE90xx_BLOCKING_MODE == 1 || ADE90xx_BLOCKING_MODE == 2
  ADE90xx_IntHandler(ADE_Handler, 0, ADE_IntEv_PAGE_FULL);
#endif
#endif
  ADE_Handler->ADE_SPI_CS_LOW();
  
  ADE_Handler->ADE_SPI_WriteByte(0x80 + (SectionWF ? 0x40 : 0));
  ADE_Handler->ADE_SPI_WriteByte(0x08);

  FPType FactorValue;

  if (SourceWF == 0)
    FactorValue = FSSinc4;
  else if (SourceWF == 2)
    FactorValue = FSDec;
  else if (SourceWF == 3)
    FactorValue = FSxPCF;
  else 
    FactorValue = 1;

  for (uint16_t CounterNum = (SectionWF ? 128 : 0); CounterNum < (SectionWF ? 256 : 128); CounterNum++)
  {
    if (ADE_IA) ADE_IA[CounterNum] = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FactorValue * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
        
    // FPType DataFPType = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte())/* / FactorValue * ADE90xx_VoltageFactor*/;
    // uart_write_bytes(0, /*(const char*)*/&DataFPType, sizeof(DataFPType));
    // uart_write_bytes(0, "\n", 1);

    if (ADE_VA) ADE_VA[CounterNum] = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FactorValue * ADE90xx_VoltageFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_IB) ADE_IB[CounterNum] = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FactorValue * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_VB) ADE_VB[CounterNum] = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FactorValue * ADE90xx_VoltageFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_IC) ADE_IC[CounterNum] = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FactorValue * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_VC) ADE_VC[CounterNum] = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FactorValue * ADE90xx_VoltageFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
    if (ADE_IN) ADE_IN[CounterNum] = (FPType)(int32_t)((ADE_Handler->ADE_SPI_ReadByte() << 24) | (ADE_Handler->ADE_SPI_ReadByte() << 16) | (ADE_Handler->ADE_SPI_ReadByte() << 8) | ADE_Handler->ADE_SPI_ReadByte()) / FactorValue * ADE90xx_CurrentFactor;
    else {ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte(); ADE_Handler->ADE_SPI_ReadByte();}
  }

  ADE_Handler->ADE_SPI_CS_HIGH();
}
