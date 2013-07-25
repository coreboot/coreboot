/* $NoKeywords:$ */
/**
 * @file
 *
 * AGESA gnb file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************
 */
#ifndef _GNBURATOKEN_H_
#define _GNBURATOKEN_H_

/// Template structure for register/field table
typedef struct {
  URA_REGISTER_64B_ENTRY                                        RxSmuIntReq;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuIntToggle;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuServiceIndex;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuIntSts;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuIntAck;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuIntDone;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuAuthSts;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuAuthDone;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuAuthPass;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuFwAuth;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuProtectedMode;///<
  URA_REGISTER_64B_ENTRY                                        REG_FIELD_TABLE_STRUCT_fld11;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuBootSeqDone;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuFwFlags;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuInterruptsEnabled;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuResetCntl;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuRstReg;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuClkCntl;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuCkDisable;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuAuthVector;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuRamStartAddr;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuRomStartAddr;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuIntArgument;///<

} REG_FIELD_TABLE_STRUCT;

#define  IDX_CALC(VarName) (offsetof (REG_FIELD_TABLE_STRUCT, VarName) / 2) /* TODO: why ## */
#define  SEL_CALC(VarName) VarName
#define  FIELD_OFFSET(RegName, FieldName)  ((((IDX_CALC(FieldName) - IDX_CALC(RegName) - 1) / 2) <= 15) ? ((IDX_CALC(FieldName) - IDX_CALC(RegName) - 1) / 2) : 0)

#define  TRxSmuIntReq                                           TOKEN_DEF (IDX_CALC (RxSmuIntReq), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuIntToggle                                       TOKEN_DEF (IDX_CALC (BfxSmuIntToggle), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuServiceIndex                                    TOKEN_DEF (IDX_CALC (BfxSmuServiceIndex), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuIntSts                                           TOKEN_DEF (IDX_CALC (RxSmuIntSts), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuIntAck                                          TOKEN_DEF (IDX_CALC (BfxSmuIntAck), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuIntDone                                         TOKEN_DEF (IDX_CALC (BfxSmuIntDone), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuAuthSts                                          TOKEN_DEF (IDX_CALC (RxSmuAuthSts), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuAuthDone                                        TOKEN_DEF (IDX_CALC (BfxSmuAuthDone), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuAuthPass                                        TOKEN_DEF (IDX_CALC (BfxSmuAuthPass), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuFwAuth                                           TOKEN_DEF (IDX_CALC (RxSmuFwAuth), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuProtectedMode                                   TOKEN_DEF (IDX_CALC (BfxSmuProtectedMode), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuRcuUcEvents                                      TOKEN_DEF (IDX_CALC (REG_FIELD_TABLE_STRUCT_fld11), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuBootSeqDone                                     TOKEN_DEF (IDX_CALC (BfxSmuBootSeqDone), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuFwFlags                                          TOKEN_DEF (IDX_CALC (RxSmuFwFlags), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_PROTOCOL_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuInterruptsEnabled                               TOKEN_DEF (IDX_CALC (BfxSmuInterruptsEnabled), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_PROTOCOL_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuResetCntl                                        TOKEN_DEF (IDX_CALC (RxSmuResetCntl), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuRstReg                                          TOKEN_DEF (IDX_CALC (BfxSmuRstReg), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuClkCntl                                          TOKEN_DEF (IDX_CALC (RxSmuClkCntl), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuCkDisable                                       TOKEN_DEF (IDX_CALC (BfxSmuCkDisable), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuAuthVector                                       TOKEN_DEF (IDX_CALC (RxSmuAuthVector), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuRamStartAddr                                     TOKEN_DEF (IDX_CALC (RxSmuRamStartAddr), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuRomStartAddr                                     TOKEN_DEF (IDX_CALC (RxSmuRomStartAddr), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuIntArgument                                      TOKEN_DEF (IDX_CALC (RxSmuIntArgument), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)


#endif

