/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MIPI_PANEL_H__
#define __MIPI_PANEL_H__

#include <commonlib/mipi/cmd.h>
#include <edid.h>
#include <types.h>

/* Definitions for flags in panel_serializable_data */
enum panel_flag {
	PANEL_FLAG_CPHY = BIT(0),
};

/* VESA Display Stream Compression DSC 1.2 constants */
#define DSC_NUM_BUF_RANGES			15

/*
 * struct dsc_rc_range_parameters - DSC Rate Control range parameters
 *
 * This defines different rate control parameters used by the DSC engine
 * to compress the frame.
 */
struct dsc_rc_range_parameters {
	/*
	 * @range_min_qp: Min Quantization Parameters allowed for this range
	 */
	u8 range_min_qp;
	/*
	 * @range_max_qp: Max Quantization Parameters allowed for this range
	 */
	u8 range_max_qp;
	/*
	 * @range_bpg_offset:
	 * Bits/group offset to apply to target for this group
	 */
	u8 range_bpg_offset;
};

struct dsc_config {
	/*
	 * @convert_rgb:
	 * Flag to indicate if RGB - YCoCg conversion is needed
	 * True if RGB input, False if YCoCg input
	 */
	u8 convert_rgb : 1;
		/*
	 * @simple_422: True if simple 4_2_2 mode is enabled else False
	 */
	u8 simple_422 : 1;
	/*
	 * @block_pred_enable:
	 * True if block prediction is used to code any groups within the
	 * picture. False if BP not used
	 */
	u8 block_pred_enable : 1;
	/*
	 * @line_buf_depth:
	 * Bits per component for previous reconstructed line buffer
	 */
	u8 line_buf_depth;
	/*
	 * @bits_per_component: Bits per component to code (8/10/12)
	 */
	u8 bits_per_component;
	/*
	 *  @slice_width: Width of each slice in pixels
	 */
	u16 slice_width;
	/*
	 * @slice_height: Slice height in pixels
	 */
	u16 slice_height;
	/*
	 * @rc_tgt_offset_high:
	 * Offset to bits/group used by RC to determine QP adjustment
	 */
	u8 rc_tgt_offset_high;
	/*
	 * @rc_tgt_offset_low:
	 * Offset to bits/group used by RC to determine QP adjustment
	 */
	u8 rc_tgt_offset_low;
	/*
	 * @bits_per_pixel:
	 * Target bits per pixel with 4 fractional bits, bits_per_pixel << 4
	 */
	u16 bits_per_pixel;
	/*
	 * @rc_edge_factor:
	 * Factor to determine if an edge is present based on the bits produced
	 */
	u8 rc_edge_factor;
	/*
	 * @rc_quant_incr_limit1:
	 * Slow down incrementing once the range reaches this value
	 */
	u8 rc_quant_incr_limit1;
	/*
	 * @rc_quant_incr_limit0:
	 * Slow down incrementing once the range reaches this value
	 */
	u8 rc_quant_incr_limit0;
	/*
	 * @initial_xmit_delay:
	 * Number of pixels to delay the initial transmission
	 */
	u16 initial_xmit_delay;
	/*
	 * @initial_dec_delay:
	 * Initial decoder delay, number of pixel times that the decoder
	 * accumulates data in its rate buffer before starting to decode
	 * and output pixels.
	 */
	u16 initial_dec_delay;
	/*
	 * @first_line_bpg_offset:
	 * Number of additional bits allocated for each group on the first
	 * line of slice.
	 */
	u8 first_line_bpg_offset;
	/*
	 * @initial_offset: Value to use for RC model offset at slice start
	 */
	u16 initial_offset;
	/*
	 * @rc_buf_thresh: Thresholds defining each of the buffer ranges
	 */
	u16 rc_buf_thresh[DSC_NUM_BUF_RANGES - 1];
	/*
	 * @rc_range_params:
	 * Parameters for each of the RC ranges defined in
	 * &struct dsc_rc_range_parameters
	 */
	struct dsc_rc_range_parameters rc_range_params[DSC_NUM_BUF_RANGES];
	/*
	 * @rc_model_size: Total size of RC model
	 */
	u16 rc_model_size;
	/*
	 * @flatness_min_qp: Minimum QP where flatness information is sent
	 */
	u8 flatness_min_qp;
	/*
	 * @flatness_max_qp: Maximum QP where flatness information is sent
	 */
	u8 flatness_max_qp;
	/*
	 * @initial_scale_value: Initial value for the scale factor
	 */
	u8 initial_scale_value;
	/*
	 * @scale_decrement_interval:
	 * Specifies number of group times between decrementing the scale factor
	 * at beginning of a slice.
	 */
	u16 scale_decrement_interval;
	/*
	 * @scale_increment_interval:
	 * Number of group times between incrementing the scale factor value
	 * used at the beginning of a slice.
	 */
	u16 scale_increment_interval;
	/*
	 * @nfl_bpg_offset: Non first line BPG offset to be used
	 */
	u16 nfl_bpg_offset;
	/*
	 * @slice_bpg_offset: BPG offset used to enforce slice bit
	 */
	u16 slice_bpg_offset;
	/*
	 * @final_offset: Final RC linear transformation offset value
	 */
	u16 final_offset;
	/*
	 * @slice_chunk_size:
	 * The (max) size in bytes of the "chunks" that are used in slice
	 * multiplexing.
	 */
	u16 slice_chunk_size;
	/*
	 * @dsc_version_minor: DSC minor version
	 */
	u8 dsc_version_minor;
	/*
	 * @dsc_version_major: DSC major version
	 */
	u8 dsc_version_major;
};

/*
 * The data to be serialized and put into CBFS.
 * Note some fields, for example edid.mode.name, were actually pointers and
 * cannot be really serialized.
 */
struct panel_serializable_data {
	u32 flags; /* flags of panel_flag */
	struct edid edid;  /* edid info of this panel */
	struct dsc_config dsc_config; /* dsc config of this panel */
	u8 init[]; /* A packed array of panel_init_command */
};

#endif /* __MIPI_PANEL_H__ */
