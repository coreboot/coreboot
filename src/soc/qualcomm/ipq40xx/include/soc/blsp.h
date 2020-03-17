/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __BLSP_H_
#define __BLSP_H_

typedef enum {
	BLSP_QUP_ID_0,
	BLSP_QUP_ID_1,
	BLSP_QUP_ID_2,
	BLSP_QUP_ID_3,
} blsp_qup_id_t;

typedef enum {
	BLSP_SUCCESS = 0,
	BLSP_ID_ERROR,
	BLSP_ERROR,
	BLSP_UNSUPPORTED
} blsp_return_t;

typedef enum {
	BLSP_PROTO_I2C_UIM = 1,
	BLSP_PROTO_I2C_ONLY,
	BLSP_PROTO_SPI_ONLY,
	BLSP_PROTO_UART_FLOW_CTL,
	BLSP_PROTO_UIM,
	BLSP_PROTO_I2C_UART,
} blsp_protocol_t;

blsp_return_t blsp_i2c_init(blsp_qup_id_t id);
int blsp_i2c_init_board(blsp_qup_id_t id);

#endif
