/******************************************************************************
  * \attention
  *
  * <h2><center>&copy; COPYRIGHT 2021 STMicroelectronics</center></h2>
  *
  * Licensed under ST MIX MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        www.st.com/mix_myliberty
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
******************************************************************************/

/*! \file
 *
 *  \author SRA
 *
 *  \brief Implementation of NFC-DEP protocol
 *
 *  NFC-DEP is also known as NFCIP - Near Field Communication
 *  Interface and Protocol
 *
 *  This implementation was based on the following specs:
 *    - NFC Forum Digital 1.1
 *    - ECMA 340 3rd Edition 2013
 *
 *
 * \addtogroup RFAL
 * @{
 *
 * \addtogroup RFAL-AL
 * \brief RFAL Abstraction Layer
 * @{
 *
 * \addtogroup NFC-DEP
 * \brief RFAL NFC-DEP Module
 * @{
 */

#ifndef RFAL_NFCDEP_H_
#define RFAL_NFCDEP_H_

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "st_errno.h"
#include "rfal_rf.h"

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */
#define RFAL_NFCDEP_FRAME_SIZE_MAX_LEN  254U             /*!< NFCIP Maximum Frame Size   Digital 1.0 Table 91                */
#define RFAL_NFCDEP_DEPREQ_HEADER_LEN   5U               /*!< DEP_REQ header length: CMD_TYPE + CMD_CMD + PBF + DID + NAD    */

/*! Length NFCIP DEP REQ or RES header (incl LEN)                                                                           */
#define RFAL_NFCDEP_DEP_HEADER          ( RFAL_NFCDEP_LEN_LEN + RFAL_NFCDEP_CMDTYPE_LEN + RFAL_NFCDEP_CMD_LEN + RFAL_NFCDEP_DEP_PFB_LEN )
#define RFAL_NFCDEP_HEADER              ( RFAL_NFCDEP_CMDTYPE_LEN + RFAL_NFCDEP_CMD_LEN ) /*!< NFCIP header length           */
#define RFAL_NFCDEP_SB_LEN              1U               /*!< SB length on NFCIP fram for NFC-A                              */
#define RFAL_NFCDEP_LEN_LEN             1U               /*!< LEN length on NFCIP frame                                      */
#define RFAL_NFCDEP_CMDTYPE_LEN         1U               /*!< Length of the cmd type (REQ | RES) on NFCIP frame              */
#define RFAL_NFCDEP_CMD_LEN             1U               /*!< Length of the cmd on NFCIP frame                               */
#define RFAL_NFCDEP_DID_LEN             1U               /*!< Length of did on NFCIP frame                                   */
#define RFAL_NFCDEP_DEP_PFB_LEN         1U               /*!< Length of the PFB field on NFCIP frame                         */

#define RFAL_NFCDEP_DSL_RLS_LEN_NO_DID  (RFAL_NFCDEP_LEN_LEN + RFAL_NFCDEP_CMDTYPE_LEN + RFAL_NFCDEP_CMD_LEN)  /*!< Length of DSL_REQ and RLS_REQ with no DID */
#define RFAL_NFCDEP_DSL_RLS_LEN_DID     (RFAL_NFCDEP_DSL_RLS_LEN_NO_DID + RFAL_NFCDEP_DID_LEN)                 /*!< Length of DSL_REQ and RLS_REQ with DID    */

#define RFAL_NFCDEP_FS_VAL_MIN           64U             /*!< Minimum LR value                                               */
#define RFAL_NFCDEP_LR_VAL_MASK          0x03U           /*!< Bit mask for a LR value                                        */
#define RFAL_NFCDEP_PP_LR_MASK           0x30U           /*!< Bit mask for LR value in PP byte on a ATR REQ/RES              */
#define RFAL_NFCDEP_PP_LR_SHIFT          4U              /*!< Position of LR value in PP byte on a ATR REQ/RES               */

#define RFAL_NFCDEP_DID_MAX              14U             /*!< Max DID value Digital 14.6.2.3                                 */
#define RFAL_NFCDEP_DID_KEEP             0xFFU           /*!< Keep DID value already configured                              */
#define RFAL_NFCDEP_DID_NO               0x00U           /*!< No DID shall be used                                           */
#define RFAL_NFCDEP_NAD_NO               0x00U           /*!< No NAD shall be used                                           */

#define RFAL_NFCDEP_OPER_RTOX_REQ_DIS    0x01U           /*!< Operation config: RTOX REQ disable                             */
#define RFAL_NFCDEP_OPER_RTOX_REQ_EN     0x00U           /*!< Operation config: RTOX REQ enable                              */

#define RFAL_NFCDEP_OPER_ATN_DIS         0x00U           /*!< Operation config: ATN disable                                  */
#define RFAL_NFCDEP_OPER_ATN_EN          0x02U           /*!< Operation config: ATN enable                                   */

#define RFAL_NFCDEP_OPER_EMPTY_DEP_DIS   0x04U           /*!< Operation config: empty DEPs disable                           */
#define RFAL_NFCDEP_OPER_EMPTY_DEP_EN    0x00U           /*!< Operation config: empty DEPs enable                            */

#define RFAL_NFCDEP_OPER_FULL_MI_DIS     0x00U           /*!< Operation config: full chaining DEPs disable                   */
#define RFAL_NFCDEP_OPER_FULL_MI_EN      0x08U           /*!< Operation config: full chaining DEPs enable                    */


#define RFAL_NFCDEP_BRS_MAINTAIN         0xC0U           /*!< Value signalling that BR is to be maintained (no PSL)          */
#define RFAL_NFCDEP_BRS_Dx_MASK          0x07U           /*!< Value signalling that BR is to be maintained (no PSL)          */
#define RFAL_NFCDEP_BRS_DSI_POS          3U              /*!< Value signalling that BR is to be maintained (no PSL)          */

#define RFAL_NFCDEP_WT_DELTA             (16U - RFAL_NFCDEP_WT_DELTA_ADJUST) /*!< NFC-DEP dWRT (adjusted)  Digital 2.0 B.10  */
#define RFAL_NFCDEP_WT_DELTA_ADJUST      4U              /*!< dWRT value adjustment                                          */


#define RFAL_NFCDEP_ATR_REQ_NFCID3_POS   2U              /*!< NFCID3 offset in ATR_REQ frame                                 */
#define RFAL_NFCDEP_NFCID3_LEN           10U             /*!< NFCID3 Length                                                  */

#define RFAL_NFCDEP_LEN_MIN              3U              /*!< Minimum length byte LEN value                                  */
#define RFAL_NFCDEP_LEN_MAX              255U            /*!< Maximum length byte LEN value                                  */

#define RFAL_NFCDEP_ATRRES_HEADER_LEN    2U              /*!< ATR RES Header Len:  CmdType: 0xD5 + Cod: 0x01                 */
#define RFAL_NFCDEP_ATRRES_MIN_LEN       17U             /*!< Minimum length for an ATR RES                                  */
#define RFAL_NFCDEP_ATRRES_MAX_LEN       64U             /*!< Maximum length for an ATR RES  Digital 1.0 14.6.1              */
#define RFAL_NFCDEP_ATRREQ_MIN_LEN       16U             /*!< Minimum length for an ATR REQ                                  */
#define RFAL_NFCDEP_ATRREQ_MAX_LEN       RFAL_NFCDEP_ATRRES_MAX_LEN /*!< Maximum length for an ATR REQ  Digital 1.0 14.6.1   */

#define RFAL_NFCDEP_GB_MAX_LEN           (RFAL_NFCDEP_ATRREQ_MAX_LEN - RFAL_NFCDEP_ATRREQ_MIN_LEN) /*!< Maximum length the General Bytes on ATR  Digital 1.1  16.6.3 */

#define RFAL_NFCDEP_WT_INI_DEFAULT       RFAL_NFCDEP_WT_INI_MAX  /*!< WT Initiator default value Digital 1.0 14.6.3.8        */
#define RFAL_NFCDEP_WT_INI_MIN           0U                      /*!< WT Initiator minimum value Digital 1.0 14.6.3.8        */
#define RFAL_NFCDEP_WT_INI_MAX           14U                     /*!< WT Initiator maximum value Digital 1.0 14.6.3.8 A.10   */
#define RFAL_NFCDEP_RWT_INI_MAX          rfalNfcDepWT2RWT( RFAL_NFCDEP_WT_INI_MAX ) /*!< RWT Initiator maximum value   */

#define RFAL_NFCDEP_WT_TRG_MAX_D10       8U                                     /*!< WT target max Digital 1.0 14.6.3.8 A.10 */
#define RFAL_NFCDEP_WT_TRG_MAX_D11       14U                                    /*!< WT target max Digital 1.1 16.6.3.9 A.9  */
#define RFAL_NFCDEP_WT_TRG_MAX           RFAL_NFCDEP_WT_TRG_MAX_D11             /*!< WT target max Digital x.x               */
#define RFAL_NFCDEP_RWT_TRG_MAX          rfalNfcDepWT2RWT( RFAL_NFCDEP_WT_TRG_MAX ) /*!< RWT Initiator maximum value         */

/*! Maximum Frame Waiting Time = ((256 * 16/fc)*2^FWImax) = ((256*16/fc)*2^14) = (1048576 / 64)/fc = (100000h*64)/fc         */
#define RFAL_NFCDEP_MAX_FWT              ((uint32_t)1U<<20)

#define RFAL_NFCDEP_WT_MASK              0x0FU           /*!< Bit mask for the Wait Time value                               */

#define RFAL_NFCDEP_BR_MASK_106          0x01U              /*!< Enable mask bit rate 106                                    */
#define RFAL_NFCDEP_BR_MASK_212          0x02U              /*!< Enable mask bit rate 242                                    */
#define RFAL_NFCDEP_BR_MASK_424          0x04U              /*!< Enable mask bit rate 424                                    */

/*
 ******************************************************************************
 * GLOBAL MACROS
 ******************************************************************************
 */

#define rfalNfcDepWT2RWT( wt )         ( (uint32_t)1U << (((uint32_t)(wt) & RFAL_NFCDEP_WT_MASK) + 12U) )                 /*!< Converts WT value to RWT (1/fc)               */

/*! Returns the BRS value from the given bit rate */
#define rfalNfcDepDx2BRS( br )         ( (((uint8_t)(br) & RFAL_NFCDEP_BRS_Dx_MASK) << RFAL_NFCDEP_BRS_DSI_POS) | ((uint8_t)(br) & RFAL_NFCDEP_BRS_Dx_MASK) )

#define rfalNfcDepBRS2DRI( brs )       (uint8_t)( (uint8_t)(brs) & RFAL_NFCDEP_BRS_Dx_MASK )                              /*!< Returns the DRI value from the given BRS byte */
#define rfalNfcDepBRS2DSI( brs )       (uint8_t)( ((uint8_t)(brs) >> RFAL_NFCDEP_BRS_DSI_POS) & RFAL_NFCDEP_BRS_Dx_MASK ) /*!< Returns the DSI value from the given BRS byte */

#define rfalNfcDepPP2LR( PPx )         ( ((uint8_t)(PPx) & RFAL_NFCDEP_PP_LR_MASK ) >> RFAL_NFCDEP_PP_LR_SHIFT)  /*!< Returns the LR value from the given PPx byte  */
#define rfalNfcDepLR2PP( LRx )         ( ((uint8_t)(LRx) << RFAL_NFCDEP_PP_LR_SHIFT) & RFAL_NFCDEP_PP_LR_MASK)   /*!< Returns the PP byte with the given LRx value  */

/*! Returns the Frame size value from the given LRx value  */
#define rfalNfcDepLR2FS( LRx )         (uint16_t)(MIN( (RFAL_NFCDEP_FS_VAL_MIN * ((uint16_t)(LRx) + 1U) ), RFAL_NFCDEP_FRAME_SIZE_MAX_LEN ))

/*!
 *  Despite DIGITAL 1.0 14.6.2.1 stating that the last two bytes may filled with
 *  any value, some devices (Samsung Google Nexus) only accept when these are 0 */
#define rfalNfcDepSetNFCID( dst, src, len )   ST_MEMSET( (dst), 0x00, RFAL_NFCDEP_NFCID3_LEN ); \
                                              if( (len) > 0U ) {ST_MEMCPY( (dst), (src), (len) );}

/*
 ******************************************************************************
 * GLOBAL ENUMERATIONS
 ******************************************************************************
 */

/*! Enumeration of NFC-DEP bit rate in ATR    Digital 1.0 Table 93 and 94   */
enum {
  RFAL_NFCDEP_Bx_NO_HIGH_BR = 0x00,       /*!< Peer supports no high bit rates      */
  RFAL_NFCDEP_Bx_08_848     = 0x01,       /*!< Peer also supports 848               */
  RFAL_NFCDEP_Bx_16_1695    = 0x02,       /*!< Peer also supports 1695              */
  RFAL_NFCDEP_Bx_32_3390    = 0x04,       /*!< Peer also supports 3390              */
  RFAL_NFCDEP_Bx_64_6780    = 0x08        /*!< Peer also supports 6780              */
};

/*! Enumeration of NFC-DEP bit rate Divisor in PSL   Digital 1.0 Table 100  */
enum {
  RFAL_NFCDEP_Dx_01_106  = RFAL_BR_106,   /*!< Divisor D =  1 : bit rate = 106      */
  RFAL_NFCDEP_Dx_02_212  = RFAL_BR_212,   /*!< Divisor D =  2 : bit rate = 212      */
  RFAL_NFCDEP_Dx_04_424  = RFAL_BR_424,   /*!< Divisor D =  4 : bit rate = 424      */
  RFAL_NFCDEP_Dx_08_848  = RFAL_BR_848,   /*!< Divisor D =  8 : bit rate = 848      */
  RFAL_NFCDEP_Dx_16_1695 = RFAL_BR_1695,  /*!< Divisor D = 16 : bit rate = 1695     */
  RFAL_NFCDEP_Dx_32_3390 = RFAL_BR_3390,  /*!< Divisor D = 32 : bit rate = 3390     */
  RFAL_NFCDEP_Dx_64_6780 = RFAL_BR_6780   /*!< Divisor D = 64 : bit rate = 6780     */
};

/*! Enumeration of  NFC-DEP Length Reduction (LR)   Digital 1.0 Table 91    */
enum {
  RFAL_NFCDEP_LR_64  = 0x00,              /*!< Maximum payload size is  64 bytes    */
  RFAL_NFCDEP_LR_128 = 0x01,              /*!< Maximum payload size is 128 bytes    */
  RFAL_NFCDEP_LR_192 = 0x02,              /*!< Maximum payload size is 192 bytes    */
  RFAL_NFCDEP_LR_254 = 0x03               /*!< Maximum payload size is 254 bytes    */
};

/*
 ******************************************************************************
 * GLOBAL DATA TYPES
 ******************************************************************************
 */

/*! NFC-DEP callback to check if upper layer has deactivation pending   */
typedef bool (* rfalNfcDepDeactCallback)(void);


/*! Enumeration of the nfcip communication modes */
typedef enum {
  RFAL_NFCDEP_COMM_PASSIVE,      /*!< Passive communication mode    */
  RFAL_NFCDEP_COMM_ACTIVE        /*!< Active communication mode     */
} rfalNfcDepCommMode;


/*! Enumeration of the nfcip roles */
typedef enum {
  RFAL_NFCDEP_ROLE_INITIATOR,    /*!< Perform as Initiator          */
  RFAL_NFCDEP_ROLE_TARGET        /*!< Perform as Target             */
} rfalNfcDepRole;


/*! Struct that holds all NFCIP configs                                                      */
typedef struct {

  rfalNfcDepRole      role;     /*!< Current NFCIP role                                      */
  rfalNfcDepCommMode  commMode; /*!< Current NFCIP communication mode                        */
  uint8_t             oper;     /*!< Operation config similar to NCI 1.0 Table 81            */

  uint8_t             did;      /*!< Current Device ID (DID)                                 */
  uint8_t             nad;      /*!< Current Node Addressing (NAD)                           */
  uint8_t             bs;       /*!< Bit rate in Sending Direction                           */
  uint8_t             br;       /*!< Bit rate in Receiving Direction                         */
  uint8_t             nfcid[RFAL_NFCDEP_NFCID3_LEN]; /*!< Pointer to the NFCID to be used    */
  uint8_t             nfcidLen; /*!< Length of the given NFCID in nfcid                      */
  uint8_t             gb[RFAL_NFCDEP_GB_MAX_LEN]; /*!< Pointer General Bytes (GB) to be used */
  uint8_t             gbLen;    /*!< Length of the given GB in gb                            */
  uint8_t             lr;       /*!< Length Reduction (LR) to be used                        */
  uint8_t             to;       /*!< Timeout (TO)  to be used                                */
  uint32_t            fwt;      /*!< Frame Waiting Time (FWT) to be used                     */
  uint32_t            dFwt;     /*!< Delta Frame Waiting Time (dFWT) to be used              */
} rfalNfcDepConfigs;


/*! ATR_REQ command    Digital 1.1 16.6.2   */
typedef struct {
  uint8_t      CMD1;                           /*!< Command format 0xD4                    */
  uint8_t      CMD2;                           /*!< Command Value                          */
  uint8_t      NFCID3[RFAL_NFCDEP_NFCID3_LEN]; /*!< NFCID3 value                           */
  uint8_t      DID;                            /*!< DID                                    */
  uint8_t      BSi;                            /*!< Sending Bitrate for Initiator          */
  uint8_t      BRi;                            /*!< Receiving Bitrate for Initiator        */
  uint8_t      PPi;                            /*!< Optional Parameters presence indicator */
  uint8_t      GBi[RFAL_NFCDEP_GB_MAX_LEN];    /*!< General Bytes                          */
} rfalNfcDepAtrReq;


/*! ATR_RES response    Digital 1.1 16.6.3  */
typedef struct {
  uint8_t      CMD1;                           /*!< Response Byte 0xD5                     */
  uint8_t      CMD2;                           /*!< Command Value                          */
  uint8_t      NFCID3[RFAL_NFCDEP_NFCID3_LEN]; /*!< NFCID3 value                           */
  uint8_t      DID;                            /*!< DID                                    */
  uint8_t      BSt;                            /*!< Sending Bitrate for Initiator          */
  uint8_t      BRt;                            /*!< Receiving Bitrate for Initiator        */
  uint8_t      TO;                             /*!< Timeout                                */
  uint8_t      PPt;                            /*!< Optional Parameters presence indicator */
  uint8_t      GBt[RFAL_NFCDEP_GB_MAX_LEN];    /*!< General Bytes                          */
} rfalNfcDepAtrRes;


/*! Structure of transmit I-PDU Buffer format from caller                                    */
typedef struct {
  uint8_t  prologue[RFAL_NFCDEP_DEPREQ_HEADER_LEN];  /*!< Prologue space for NFC-DEP header*/
  uint8_t  inf[RFAL_NFCDEP_FRAME_SIZE_MAX_LEN];      /*!< INF | Data area of the buffer    */
} rfalNfcDepBufFormat;


/*! Activation info as Initiator and Target                                       */
typedef union { /*  PRQA S 0750 # MISRA 19.2 - Both members of the union will not be used concurrently , device is only initiatior or target a time. No problem can occur. */
  struct {
    rfalNfcDepAtrRes  ATR_RES;      /*!< ATR RES            (Initiator mode)  */
    uint8_t           ATR_RESLen;   /*!< ATR RES length     (Initiator mode)  */
  } Target;                           /*!< Target                               */
  struct {
    rfalNfcDepAtrReq  ATR_REQ;      /*!< ATR REQ            (Target mode)     */
    uint8_t           ATR_REQLen;   /*!< ATR REQ length     (Target mode)     */
  } Initiator;                        /*!< Initiator                            */
} rfalNfcDepActivation;


/*! NFC-DEP device Info */
typedef struct {
  uint8_t           GBLen;      /*!< General Bytes length                       */
  uint8_t           WT;         /*!< WT to be used (ignored in Listen Mode)     */
  uint32_t          FWT;        /*!< FWT to be used (1/fc)(ignored Listen Mode) */
  uint32_t          dFWT;       /*!< Delta FWT to be used (1/fc)                */
  uint8_t           LR;         /*!< Length Reduction coding the max payload    */
  uint16_t          FS;         /*!< Frame Size                                 */
  rfalBitRate       DSI;        /*!< Bit Rate coding from Initiator  to Target  */
  rfalBitRate       DRI;        /*!< Bit Rate coding from Target to Initiator   */
  uint8_t           DID;        /*!< Device ID (RFAL_NFCDEP_DID_NO if no DID)   */
  uint8_t           NAD;        /*!< Node ADdress (RFAL_NFCDEP_NAD_NO if no NAD)*/
} rfalNfcDepInfo;


/*! NFC-DEP Device structure */
typedef struct {
  rfalNfcDepActivation    activation;        /*!< Activation Info               */
  rfalNfcDepInfo          info;              /*!< NFC-DEP device Info           */
} rfalNfcDepDevice;


/*! NFCIP Protocol structure for P2P Target
 *
 *   operParam : derives from NFC-Forum NCI NFC-DEP Operation Parameter
 *               NCI 1.1 Table 86: NFC-DEP Operation Parameter
 *               and it's a bit mask composed as:
 *                  [ 0000b
 *                    | Chain SHALL use max. Transport Data Byte[1b]
 *                    | I-PDU with no Transport Data SHALL NOT be sent [1b]
 *                    | NFC-DEP Target SHALL NOT send RTOX request [1b]
 *                  ]
 *
 */
typedef struct {
  rfalNfcDepCommMode commMode;       /*!< Initiator in Active P2P or Passive P2P*/
  uint8_t            operParam;      /*!< NFC-DEP Operation Parameter           */
  uint8_t           *nfcid;          /*!< Initiator's NFCID2 or NFCID3          */
  uint8_t            nfcidLen;       /*!< Initiator's NFCID length (NFCID2/3)   */
  uint8_t            DID;            /*!< Initiator's Device ID DID             */
  uint8_t            NAD;            /*!< Initiator's Node ID NAD               */
  uint8_t            BS;             /*!< Initiator's Bit Rates supported in Tx */
  uint8_t            BitRate;        /*!< Initiator's Bit Rates supported in Rx */
  uint8_t            LR;             /*!< Initiator's Length reduction          */
  uint8_t           *GB;             /*!< Initiator's General Bytes (Gi)        */
  uint8_t            GBLen;          /*!< Initiator's General Bytes length      */
} rfalNfcDepAtrParam;


/*! Structure of parameters to be passed in for nfcDepListenStartActivation       */
typedef struct {
  rfalNfcDepBufFormat  *rxBuf;        /*!< Receive Buffer struct reference      */
  uint16_t             *rxLen;        /*!< Receive INF data length in bytes     */
  bool                 *isRxChaining; /*!< Received data is not complete        */
  rfalNfcDepDevice     *nfcDepDev;    /*!< NFC-DEP device info                  */
} rfalNfcDepListenActvParam;


/*! NFCIP Protocol structure for P2P Target
 *
 *   operParam : derives from NFC-Forum NCI NFC-DEP Operation Parameter
 *               NCI 1.1 Table 86: NFC-DEP Operation Parameter
 *               and it's a bit mask composed as:
 *                  [ 0000b
 *                    | Chain SHALL use max. Transport Data Byte[1b]
 *                    | I-PDU with no Transport Data SHALL NOT be sent [1b]
 *                    | NFC-DEP Target SHALL NOT send RTOX request [1b]
 *                  ]
 *
 */
typedef struct {
  rfalNfcDepCommMode commMode;                       /*!< Target in Active P2P or Passive P2P   */
  uint8_t            nfcid3[RFAL_NFCDEP_NFCID3_LEN]; /*!< Target's NFCID3                       */
  uint8_t            bst;                            /*!< Target's Bit Rates supported in Tx    */
  uint8_t            brt;                            /*!< Target's Bit Rates supported in Rx    */
  uint8_t            to;                             /*!< Target's timeout (TO) value           */
  uint8_t            ppt;                            /*!< Target's Presence optional Params(PPt)*/
  uint8_t            GBt[RFAL_NFCDEP_GB_MAX_LEN];    /*!< Target's General Bytes (Gt)           */
  uint8_t            GBtLen;                         /*!< Target's General Bytes length         */
  uint8_t            operParam;                      /*!< NFC-DEP Operation Parameter           */
} rfalNfcDepTargetParam;


/*! Structure of parameters to be passed in for nfcDepStartIpduTransceive              */
typedef struct {
  rfalNfcDepBufFormat *txBuf;         /*!< Transmit Buffer struct reference          */
  uint16_t            txBufLen;       /*!< Transmit Buffer INF field length in bytes */
  bool                isTxChaining;   /*!< Transmit data is not complete             */
  rfalNfcDepBufFormat *rxBuf;         /*!< Receive Buffer struct reference           */
  uint16_t            *rxLen;         /*!< Receive INF data length                   */
  bool                *isRxChaining;  /*!< Received data is not complete             */
  uint32_t            FWT;            /*!< FWT to be used (ignored in Listen Mode)   */
  uint32_t            dFWT;           /*!< Delta FWT to be used                      */
  uint16_t            FSx;            /*!< Other device Frame Size (FSD or FSC)      */
  uint8_t             DID;            /*!< Device ID (RFAL_ISODEP_NO_DID if no DID)  */
} rfalNfcDepTxRxParam;


/*! Struct that holds all DEP parameters/configs for the following communications */
typedef struct {
  uint8_t   did;           /*!< Device ID (DID) to be used                      */

  uint8_t  *txBuf;         /*!< Pointer to the Tx buffer to be sent             */
  uint16_t  txBufLen;      /*!< Length of the data in the txBuf                 */
  uint8_t   txBufPaylPos;  /*!< Position inside txBuf where data starts         */
  bool      txChaining;    /*!< Flag indicating chaining on transmission        */

  uint8_t  *rxBuf;         /*!< Pointer to the Rx buffer for incoming data      */
  uint16_t  rxBufLen;      /*!< Length of the data in the rxBuf                 */
  uint8_t   rxBufPaylPos;  /*!< Position inside rxBuf where data is to be placed*/

  uint32_t  fwt;           /*!< Frame Waiting Time (FWT) to be used             */
  uint32_t  dFwt;          /*!< Delta Frame Waiting Time (dFWT) to be used      */
  uint16_t  fsc;           /*!< Frame Size (FSC) to be used                     */

} rfalNfcDepDEPParams;

/*! NFCIP commands (Request, Response) */
typedef enum {
  NFCIP_CMD_ATR_REQ = 0x00,
  NFCIP_CMD_ATR_RES = 0x01,
  NFCIP_CMD_WUP_REQ = 0x02,
  NFCIP_CMD_WUP_RES = 0x03,
  NFCIP_CMD_PSL_REQ = 0x04,
  NFCIP_CMD_PSL_RES = 0x05,
  NFCIP_CMD_DEP_REQ = 0x06,
  NFCIP_CMD_DEP_RES = 0x07,
  NFCIP_CMD_DSL_REQ = 0x08,
  NFCIP_CMD_DSL_RES = 0x09,
  NFCIP_CMD_RLS_REQ = 0x0A,
  NFCIP_CMD_RLS_RES = 0x0B
} rfalNfcDepCmd;


/*! NFCIP module states */
typedef enum {
  NFCIP_ST_IDLE,
  NFCIP_ST_INIT_IDLE,
  NFCIP_ST_INIT_ATR,
  NFCIP_ST_INIT_PSL,
  NFCIP_ST_INIT_DEP_IDLE,
  NFCIP_ST_INIT_DEP_TX,
  NFCIP_ST_INIT_DEP_RX,
  NFCIP_ST_INIT_DEP_ATN,
  NFCIP_ST_INIT_DSL,
  NFCIP_ST_INIT_RLS,

  NFCIP_ST_TARG_WAIT_ATR,
  NFCIP_ST_TARG_WAIT_ACTV,
  NFCIP_ST_TARG_DEP_IDLE,
  NFCIP_ST_TARG_DEP_RX,
  NFCIP_ST_TARG_DEP_RTOX,
  NFCIP_ST_TARG_DEP_TX,
  NFCIP_ST_TARG_DEP_SLEEP
} rfalNfcDepState;


/*! Struct that holds all NFCIP data */
typedef struct {
  rfalNfcDepConfigs       cfg;               /*!< Holds the current configuration to be used    */

  rfalNfcDepState         state;             /*!< Current state of the NFCIP module             */
  uint8_t                 pni;               /*!< Packet Number Information (PNI) counter       */

  uint8_t                 lastCmd;           /*!< Last command sent                             */
  uint8_t                 lastPFB;           /*!< Last PFB sent                                 */
  uint8_t                 lastPFBnATN;       /*!< Last PFB sent (excluding  ATN)                */
  uint8_t                 lastRTOX;          /*!< Last RTOX value sent                          */

  uint8_t                 cntTxRetrys;       /*!< Retransmissions counter                       */
  uint8_t                 cntTORetrys;       /*!< Timeouts counter                              */
  uint8_t                 cntRTOXRetrys;     /*!< RTOX counter                                  */
  uint8_t                 cntNACKRetrys;     /*!< NACK counter                                  */
  uint8_t                 cntATNRetrys;      /*!< Attention (ATN) counter                       */

  uint16_t                fsc;               /*!< Current Frame Size (FSC) to be used           */
  bool                    isTxChaining;      /*!< Flag for chaining on Transmission             */
  bool                    isRxChaining;      /*!< Flag for chaining on Reception                */
  uint8_t                *txBuf;             /*!< Pointer to the Tx buffer to be sent           */
  uint8_t                *rxBuf;             /*!< Pointer to the Rx buffer for incoming data    */
  uint16_t                txBufLen;          /*!< Length of the data in the txBuf               */
  uint16_t                rxBufLen;          /*!< Length of rxBuf buffer                        */
  uint16_t               *rxRcvdLen;         /*!< Length of the data in the rxBuf               */
  uint8_t                 txBufPaylPos;      /*!< Position in txBuf where data starts           */
  uint8_t                 rxBufPaylPos;      /*!< Position in rxBuf where data is to be placed  */
  bool                    *isChaining;       /*!< Flag for chaining on Reception                */

  rfalNfcDepDevice        *nfcDepDev;        /*!< Pointer to NFC-DEP device info                */

  uint32_t                RTOXTimer;         /*!< Timer used for RTOX                           */
  rfalNfcDepDeactCallback isDeactivating;    /*!< Deactivating flag check callback              */

  bool                    isReqPending;      /*!< Flag pending REQ from Target activation       */
  bool                    isTxPending;       /*!< Flag pending DEP Block while waiting RTOX Ack */
  bool                    isWait4RTOX;       /*!< Flag for waiting RTOX Ack                     */
} rfalNfcDep;


/*
 * *****************************************************************************
 * GLOBAL VARIABLE DECLARATIONS
 ******************************************************************************
 */


/*
 ******************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 ******************************************************************************
 */


#endif /* RFAL_NFCDEP_H_ */

/**
  * @}
  *
  * @}
  *
  * @}
  */
