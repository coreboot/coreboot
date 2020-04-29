/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This header implements structures to describe the properties defined in the
 * SoundWire Discovery and Configuration (DisCo) Specification Version 1.0.
 *
 * This is available for non-members after providing a name and email address at
 * https://resources.mipi.org/disco_soundwire
 *
 * The structure members mirror the property names defined in the specification,
 * with the exception that '-' is transformed into '_' for compatible naming.
 *
 * See Documentation/drivers/soundwire.md for more information.
 */

#ifndef __DEVICE_SOUNDWIRE_H__
#define __DEVICE_SOUNDWIRE_H__

#include <stdbool.h>
#include <stdint.h>

/**
 * enum soundwire_limits - Limits on number of SoundWire devices in topology.
 * @SOUNDWIRE_MAX: Maximum value for lists of configuration values in SoundWire devices.
 * @SOUNDWIRE_MIN_DPN: Data Port minimum value.  DPn range is 1-14.
 * @SOUNDWIRE_MAX_DPN: Data Port maximum value.  DPn range is 1-14.
 * @SOUNDWIRE_MAX_SLAVE: Maximum number of slave devices that can be attached to one master.
 * @SOUNDWIRE_MAX_LINK: Maximum number of master link devices that can be on one controller.
 * @SOUNDWIRE_MAX_LANE: Maximum number of lanes in a multi-lane slave device.
 * @SOUNDWIRE_MAX_MODE: Maximum number of Audio or Bulk Register Access modes.
 */
enum soundwire_limits {
	SOUNDWIRE_MAX = 32,
	SOUNDWIRE_MIN_DPN = 1,
	SOUNDWIRE_MAX_DPN = 14,
	SOUNDWIRE_MAX_SLAVE = 11,
	SOUNDWIRE_MAX_LINK = 8,
	SOUNDWIRE_MAX_LANE = 8,
	SOUNDWIRE_MAX_MODE = 4
};

/**
 * enum soundwire_sw_version - Versions of SoundWire Discovery and Configuration Specification.
 * @SOUNDWIRE_SW_VERSION_1_0: DisCo Specification Version 1.0 released November 2016.
 */
enum soundwire_sw_version {
	SOUNDWIRE_SW_VERSION_1_0 = 0x00010000
};

/**
 * enum soundwire_version - Versions of SoundWire Specification supported by a device.
 * @SOUNDWIRE_VERSION_1_0: SoundWire Specification Version 1.0 released January 2015.
 * @SOUNDWIRE_VERSION_1_1: SoundWire Specification Version 1.1 released June 2016.
 * @SOUNDWIRE_VERSION_1_2: SoundWire Specification Version 1.2 released April 2019.
 */
enum soundwire_version {
	SOUNDWIRE_VERSION_1_0 = 1,
	SOUNDWIRE_VERSION_1_1,
	SOUNDWIRE_VERSION_1_2
};

/**
 * enum mipi_class - MIPI class encoding.
 * @MIPI_CLASS_NONE: No further class decoding.
 * @MIPI_CLASS_SDCA: Device implements SoundWire Device Class for Audio (SDCA).
 *
 * 0x02-0x7F: Reserved
 * 0x80-0xFF: MIPI Alliance extended device class
 */
enum mipi_class {
	MIPI_CLASS_NONE,
	MIPI_CLASS_SDCA
};

/**
 * struct soundwire_address - SoundWire Device Address Encoding.
 * @version: SoundWire specification version from &enum soundwire_version.
 * @link_id: Zero-based SoundWire master link id.
 * @unique_id: Unique ID for multiple slave devices on the same bus.
 * @manufacturer_id: Manufacturer ID from include/device/mipi_ids.h.
 * @part_id: Vendor defined part ID.
 * @class: MIPI class encoding in &enum mipi_class.
 */
struct soundwire_address {
	enum soundwire_version version;
	uint8_t link_id;
	uint8_t unique_id;
	uint16_t manufacturer_id;
	uint16_t part_id;
	enum mipi_class class;
};

/**
 * struct soundwire_link - SoundWire master device properties.
 * @clock_stop_mode0_supported: %true if clock stop mode0 is supported by this device.
 * @clock_stop_mode1_supported: %true if clock stop mode1 is supported by this device.
 * @max_clock_frequency: Maximum bus clock for this device in Hz.
 * @supported_clock_gears_count: Number of entries in supported_clock_gears.
 * @supported_clock_gears: One entry for each supported clock gear.
 * @clock_frequencies_supported_count: Number of entries in clock_frequencies_supported.
 * @clock_frequencies_supported: One entry for each clock frequency supported in Hz.
 * @default_frame_rate: Controller default frame rate in Hz.
 * @default_frame_row_size: Number of rows between 48-256.
 * @default_frame_col_size: Number of columns: 2, 4, 8, 16.
 * @dynamic_frame_shape: %true if bus driver may change frame shape dynamically.
 * @command_error_threshold: Number of times that software may retry sending a command.
 */
struct soundwire_link {
	bool clock_stop_mode0_supported;
	bool clock_stop_mode1_supported;
	unsigned int max_clock_frequency;
	size_t supported_clock_gears_count;
	unsigned int supported_clock_gears[SOUNDWIRE_MAX];
	size_t clock_frequencies_supported_count;
	uint64_t clock_frequencies_supported[SOUNDWIRE_MAX];
	unsigned int default_frame_rate;
	unsigned int default_frame_row_size;
	unsigned int default_frame_col_size;
	bool dynamic_frame_shape;
	unsigned int command_error_threshold;
};

/**
 * struct soundwire_controller - SoundWire controller properties.
 * @master_count: Number of masters present on this device.
 * @master_list: One entry for each master device.
 */
struct soundwire_controller {
	unsigned int master_list_count;
	struct soundwire_link master_list[SOUNDWIRE_MAX_LINK];
};

/* SoundWire port bitmask, used for slave source/sink port list property. */
#define SOUNDWIRE_PORT(port) BIT(port)

/**
 * struct soundwire_slave - SoundWire slave device properties.
 * @wake_up_unavailable: Wake from this device is not supported or allowed.
 * @test_mode_supported: %true if test mode is supported by this device.
 * @clock_stop_mode1_supported: %true if clock stop mode1 is supported by this device.
 * @simplified_clockstopprepare_sm_supported: %true if slave only supports the simplified
 *                                            clock stop prepare state machine and will
 *                                            always be ready for a stop clock transition.
 * @clockstopprepare_timeout: Slave-specific timeout in milliseconds.
 * @clockstopprepare_hard_reset_behavior: %true when slave keeps the status of the
 *                                        StopClockPrepare state machine after exit from
 *                                        mode1 and must be de-prepared by software.
 * @slave_channelprepare_timeout: Slave-specific timeout in milliseconds.
 * @highPHY_capable: %true if device is HighPHY capable.
 * @paging_supported: %true if device implements paging registers.
 * @bank_delay_supported: %true if device implements bank delay/bridge registers.
 * @port15_read_behavior: %true if device supports read to Port15 alias.
 * @master_count: Number of master links present on this slave.
 * @source_port_list: Bitmap identifying supported source ports, starting at bit 1.
 * @sink_port_list: Bitmap identifying supported sink ports, starting at bit 1.
 */
struct soundwire_slave {
	bool wake_up_unavailable;
	bool test_mode_supported;
	bool clock_stop_mode1_supported;
	bool simplified_clockstopprepare_sm_supported;
	unsigned int clockstopprepare_timeout;
	bool clockstopprepare_hard_reset_behavior;
	unsigned int slave_channelprepare_timeout;
	bool highPHY_capable;
	bool paging_supported;
	bool bank_delay_supported;
	bool port15_read_behavior;
	size_t master_count;
	uint32_t source_port_list;
	uint32_t sink_port_list;
};

/**
 * enum soundwire_multilane_dir - Direction of lane in slave multilane device.
 * @MASTER_LANE: Lane is connected to a master device.
 * @SLAVE_LINK: Lane is connected to a slave device.
 */
enum soundwire_multilane_dir {
	MASTER_LANE,
	SLAVE_LINK,
};

/**
 * struct soundwire_multilane_map - Pair a soundwire lane with direction.
 * @lane: Slave device lane number.
 * @direction: Direction of the slave lane.
 * @connection: The connection that this lane makes.
 */
struct soundwire_multilane_map {
	unsigned int lane;
	enum soundwire_multilane_dir direction;
	union lane_type {
		unsigned int master_lane;
		unsigned int slave_link;
	} connection;
};

/**
 * struct soundwire_multilane - Multi-Lane SoundWire slave device.
 * @lane_mapping_count: Number of entries in lane_mapping.
 * @lane_mapping: One entry for each lane that is connected to lanes on a master device or
 *                slave devices via a slave link.  Lane 0 is always connected to the master
 *                and entry 0 in this array is ignored.
 * @lane_bus_holder_count: Number of entries in lane_bus_holder.
 * @lane_bus_holder: One entry for each lane, %true if the device behaves as a bus holder.
 */
struct soundwire_multilane {
	size_t lane_mapping_count;
	struct soundwire_multilane_map lane_mapping[SOUNDWIRE_MAX_LANE];
	size_t lane_bus_holder_count;
	bool lane_bus_holder[SOUNDWIRE_MAX_LANE];
};

/**
 * enum soundwire_prepare_channel_behavior - Specifies the dependencies between the
 *                                           Channel Prepare sequence and bus clock config.
 * @CHANNEL_PREPARE_ANY_FREQUENCY: Channel Prepare can happen at any bus clock rate.
 * @CHANNEL_PREPARE_SUPPORTED_FREQUENCY: Channel Prepare sequence shall happen only after
 *                                       the bus clock is changed to a supported frequency.
 */
enum soundwire_prepare_channel_behavior {
	CHANNEL_PREPARE_ANY_FREQUENCY,
	CHANNEL_PREPARE_SUPPORTED_FREQUENCY
};

/**
 * struct soundwire_audio_mode - Properties for each supported Audio Mode.
 * @sdw_name: SoundWire device name for this audio mode device instance.
 * @max_bus_frequency: Maximum bus frequency of this mode in Hz.
 * @min_bus_frequency: Minimum bus frequency of this mode in Hz.
 * @bus_frequency_configs_count: Number of entries in bus_frequency_configs.
 * @bus_frequency_configs: One entry for each supported bus frequency,
 *                         if not all values in min to max range are valid.
 * @max_sampling_frequency: Maximum sampling frequency of this mode in Hz.
 * @min_sampling_frequency: Minimum sampling frequency of this mode in Hz.
 * @sampling_frequency_configs_count: Number of entries in sampling_frequency_configs.
 * @sampling_frequency_configs: One entry for each supported sampling frequency,
 *                              if not all values in min to max range are valid.
 * @prepare_channel_behavior: Dependencies between Channel Prepare and bus clock.
 * @glitchless_transitions: Bitmap describing possible glitchless transitions from this audio
 *                          mode to another audio mode.  Not used for only one mode.
 */
struct soundwire_audio_mode {
	unsigned int max_bus_frequency;
	unsigned int min_bus_frequency;
	size_t bus_frequency_configs_count;
	uint64_t bus_frequency_configs[SOUNDWIRE_MAX];
	unsigned int max_sampling_frequency;
	unsigned int min_sampling_frequency;
	size_t sampling_frequency_configs_count;
	uint64_t sampling_frequency_configs[SOUNDWIRE_MAX];
	enum soundwire_prepare_channel_behavior prepare_channel_behavior;
	uint32_t glitchless_transitions;
};

/* Type of SoundWire Data Port supported for this device. */
enum soundwire_data_port_type {
	FULL_DATA_PORT,
	SIMPLIFIED_DATA_PORT,
	REDUCED_DATA_PORT
};

/* Number of samples that can be grouped together (0-based count). */
enum soundwire_block_group_count {
	BLOCK_GROUP_COUNT_1,
	BLOCK_GROUP_COUNT_2,
	BLOCK_GROUP_COUNT_3,
	BLOCK_GROUP_COUNT_4
};

/* Bitmap identifying the types of modes supported. */
enum soundwire_mode_bitmap {
	MODE_ISOCHRONOUS = BIT(0),
	MODE_TX_CONTROLLED = BIT(1),
	MODE_RX_CONTROLLED = BIT(2),
	MODE_FULL_ASYNCHRONOUS = BIT(3)
};

/* Bitmap identifying the encoding schemes supported. */
enum soundwire_port_encoding_bitmap {
	ENCODE_TWOS_COMPLEMENT = BIT(0),
	ENCODE_SIGN_MAGNITUDE = BIT(1),
	ENCODE_IEEE_32BIT_FP = BIT(2)
};

/**
 * struct soundwire_dpn - Configuration properties for SoundWire DPn Data Ports.
 * @port_max_wordlength: Maximum number of bits in a Payload Channel Sample. (1-64)
 * @port_min_wordlength: Minimum number of bits in a Payload Channel Sample. (1-64)
 * @port_wordlength_configs_count: Number of entries in port_wordlength_configs.
 * @port_wordlength_configs: One entry for each supported wordlength.
 *                           Used if only specific wordlength values are allowed.
 * @data_port_type: Type of data port from &enum soundwire_data_port_type.
 * @max_grouping_supported: 0-based maximum number of samples that can be grouped for
 *                          %FULL_DATA_PORT.  The %SIMPLIFIED_DATA_PORT and %REDUCED_DATA_PORT
 *                          require 4 samples in a group.
 * @simplified_channelprepare_sm: %true if the channel prepare sequence is not required,
 *                                and the Port Ready interrupt is not supported.
 * @port_channelprepare_timeout: Port-specific timeout value in milliseconds.
 * @imp_def_dpn_interrupts_supported: Bitmap for support of implementation-defined interrupts.
 * @min_channel_number: Minimum channel number supported.
 * @max_channel_number: Maximum channel number supported.
 * @channel_number_list_count: Number of entries in channel_number_list.
 * @channel_number_list: One entry for each available channel number.
 *                       Used if only specific channels are available.
 * @channel_combination_list_count: Number of entries in channel_combination_list.
 * @channel_combination_list: One bitmap entry for each valid channel combination.
 * @modes_supported: Bitmap identifying the types of modes supported by the device.
 * @max_async_buffer: Number of samples that this port can buffer in asynchronous modes.
 *                    Only required if the slave implements buffer larger than required.
 * @block_packing_mode: %true if BlockPackingMode may be configured as BlockPerPort or
 *                      BlocKPerChannel.  %false if BlockPackingMode must be BlockPerPort.
 * @port_encoding_type: Bitmap describing the types of Payload Channel Sample encoding
 *                      schemes implemented by this port.
 * @port_audio_mode_count: Number of entries in audio_mode_list.
 * @port_audio_mode_list: One entry for each supported audio mode id.
 */
struct soundwire_dpn {
	unsigned int port_max_wordlength;
	unsigned int port_min_wordlength;
	size_t port_wordlength_configs_count;
	uint64_t port_wordlength_configs[SOUNDWIRE_MAX];
	enum soundwire_data_port_type data_port_type;
	enum soundwire_block_group_count max_grouping_supported;
	bool simplified_channelprepare_sm;
	unsigned int port_channelprepare_timeout;
	uint32_t imp_def_dpn_interrupts_supported;
	unsigned int min_channel_number;
	unsigned int max_channel_number;
	size_t channel_number_list_count;
	uint64_t channel_number_list[SOUNDWIRE_MAX];
	size_t channel_combination_list_count;
	uint64_t channel_combination_list[SOUNDWIRE_MAX];
	enum soundwire_mode_bitmap modes_supported;
	unsigned int max_async_buffer;
	bool block_packing_mode;
	uint32_t port_encoding_type;
	size_t port_audio_mode_count;
	unsigned int port_audio_mode_list[SOUNDWIRE_MAX_MODE];
};

/**
 * struct soundwire_bra_mode - Bulk Register Access mode properties.
 * @max_bus_frequency: Maximum bus frequency of this mode in Hz.
 * @min_bus_frequency: Minimum bus frequency of this mode in Hz.
 * @bus_frequency_configs_count: Number of entries in bus_frequency_configs.
 * @bus_frequency_configs: One entry for each supported bus frequency,
 *                         Used if not all values in min to max range are valid.
 * @max_data_per_frame: Maximum data bytes per frame, excluding header, CRC, and footer.
 * @min_us_between_transactions: Amount of delay in uS required between transactions.
 * @max_bandwidth: Maximum bandwidth in bytes per second that can be written/read.
 * @block_alignment: Size of basic block in bytes.
 */
struct soundwire_bra_mode {
	unsigned int max_bus_frequency;
	unsigned int min_bus_frequency;
	size_t bus_frequency_configs_count;
	uint64_t bus_frequency_configs[SOUNDWIRE_MAX];
	unsigned int max_data_per_frame;
	unsigned int min_us_between_transactions;
	unsigned int max_bandwidth;
	unsigned int block_alignment;
};

/**
 * struct soundwire_dp0 - Configuration properties for SoundWire DP0 Data Port.
 * @port_max_wordlength: Maximum number of bits in a Payload Channel Sample. (1-64)
 * @port_min_wordlength: Minimum number of bits in a Payload Channel Sample. (1-64)
 * @port_wordlength_configs_count: Number of entries in port_wordlength_configs.
 * @port_wordlength_configs: One entry for each supported wordlength.
 *                           Used if only specific wordlength values are allowed.
 * @bra_flow_controlled: Used if the slave can result in an OK_NotReady response.
 * @bra_imp_def_response_supported: %true if implementation defined response is supported.
 * @bra_role_supported: %true if the slave supports initiating BRA transactions.
 * @simplified_channel_prepare_sm: %true if the channel prepare sequence is not required,
 *                                 and the Port Ready interrupt is not supported.
 * @imp_def_dp0_interrupts_supported: If set, each bit corresponds to support for
 *                                    implementation-defined interrupts ImpDef.
 * @imp_def_bpt_supported: %true if implementation defined Payload Type is supported.
 * @bra_mode_count: Number of entries in bra_mode_list.
 * @bra_mode_list: One entry for each supported Bulk Register Access mode id.
 */
struct soundwire_dp0 {
	unsigned int port_max_wordlength;
	unsigned int port_min_wordlength;
	size_t port_wordlength_configs_count;
	uint64_t port_wordlength_configs[SOUNDWIRE_MAX];
	bool bra_flow_controlled;
	bool bra_imp_def_response_supported;
	bool bra_role_supported;
	bool simplified_channel_prepare_sm;
	unsigned int imp_def_dp0_interrupts_supported;
	bool imp_def_bpt_supported;
	size_t bra_mode_count;
	unsigned int bra_mode_list[SOUNDWIRE_MAX_MODE];
};

/**
 * struct soundwire_dpn_entry - Full duplex data port properties for DPn 1-14.
 * @port: DPn data port number, starting at 1.
 * @source: Source data port properties. (optional)
 * @sink: Sink data port properties. (optional)
 */
struct soundwire_dpn_entry {
	size_t port;
	struct soundwire_dpn *source;
	struct soundwire_dpn *sink;
};

/**
 * struct soundwire_codec - Contains all configuration for a SoundWire codec slave device.
 * @slave: Properties for slave device.
 * @audio_mode: Properties for audio modes used by DPn data ports 1-14.
 * @dpn: Properties for DPn data ports 1-14.
 * @dp0_bra_mode: Properties for Bulk Register Access mode for data port 0. (optional)
 * @dp0: Properties for data port 0. (optional)
 * @multilane: Properties for slave multilane device. (optional)
 */
struct soundwire_codec {
	struct soundwire_slave *slave;
	struct soundwire_audio_mode *audio_mode[SOUNDWIRE_MAX_MODE];
	struct soundwire_dpn_entry dpn[SOUNDWIRE_MAX_DPN - SOUNDWIRE_MIN_DPN];
	struct soundwire_bra_mode *dp0_bra_mode[SOUNDWIRE_MAX_MODE];
	struct soundwire_dp0 *dp0;
	struct soundwire_multilane *multilane;
};

#endif /* __DEVICE_SOUNDWIRE_H__ */
