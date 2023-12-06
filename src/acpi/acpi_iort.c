/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_iort.h>
#include <assert.h>

unsigned long acpi_iort_its_entry(unsigned long current, acpi_iort_t *iort,
				  acpi_iort_node_t **its_out, u32 its_count, u32 *identifiers)
{
	acpi_iort_its_group_t *its_node_data;
	acpi_iort_node_t *its = (acpi_iort_node_t *)current;
	const unsigned long its_start = current;

	ASSERT(its_out);
	current += sizeof(*its) + sizeof(*its_node_data);
	memset(its, 0, current - its_start);

	its->type = ACPI_IORT_NODE_ITS_GROUP;
	its->revision = 1;
	its->identifier = iort->node_count++;

	its_node_data = (acpi_iort_its_group_t *)its->node_data;
	its_node_data->its_count = its_count;

	for (int i = 0; i < its_count; i++)
		its_node_data->identifiers[i] = identifiers[i];

	current += its_node_data->its_count * sizeof(its_node_data->identifiers[0]);
	its->length = current - its_start;

	*its_out = its;

	return current;
}

unsigned long acpi_iort_smmuv3_entry(unsigned long current, acpi_iort_t *iort,
				     acpi_iort_node_t **smmu_v3_out, u64 base, u32 flags)
{
	acpi_iort_smmu_v3_t *smmu_v3_node_data;
	acpi_iort_node_t *smmu_v3 = (acpi_iort_node_t *)current;
	const unsigned long smmu_v3_start = current;

	ASSERT(smmu_v3_out);
	current += sizeof(*smmu_v3) + sizeof(*smmu_v3_node_data);
	memset(smmu_v3, 0, current - smmu_v3_start);

	smmu_v3->type = ACPI_IORT_NODE_SMMU_V3;
	smmu_v3->revision = 0x5;
	smmu_v3->identifier = iort->node_count++; // Unique identifier.
	smmu_v3->mapping_offset = current - smmu_v3_start;

	/* length is further updated by id map entries */
	smmu_v3->length = smmu_v3->mapping_offset;

	smmu_v3_node_data = (acpi_iort_smmu_v3_t *)smmu_v3->node_data;
	smmu_v3_node_data->base_address = base;
	smmu_v3_node_data->flags = flags;

	*smmu_v3_out = smmu_v3;

	return current;
}

unsigned long acpi_iort_nc_entry(unsigned long current, acpi_iort_t *iort,
				 acpi_iort_node_t **nc_out, u32 node_flags, u64 memory_properties,
				 u32 memory_address_limit, char *device_name)
{
	acpi_iort_named_component_t *nc_node_data;
	acpi_iort_node_t *nc = (acpi_iort_node_t *)current;
	const unsigned long nc_start = current;
	const u16 device_name_len = strlen(device_name);

	ASSERT(nc_out);
	current += sizeof(*nc) + sizeof(*nc_node_data);
	memset(nc, 0, current - nc_start);

	nc->type = ACPI_IORT_NODE_NAMED_COMPONENT;
	nc->revision = 0x4;
	nc->identifier = iort->node_count++; // Unique identifier.

	nc_node_data = (acpi_iort_named_component_t *)nc->node_data;
	nc_node_data->node_flags = node_flags;
	nc_node_data->memory_properties = memory_properties;
	nc_node_data->memory_address_limit = memory_address_limit;

	/* Path of namespace object */
	memset(nc_node_data->device_name, 0, device_name_len + 5);
	strncpy(nc_node_data->device_name, device_name, device_name_len + 1);

	current += device_name_len + 2;
	current = ALIGN_UP(current, 4);

	nc->mapping_offset = current - nc_start;

	/* length is further updated by id map entries */
	nc->length = nc->mapping_offset;

	*nc_out = nc;

	return current;
}

unsigned long acpi_iort_rc_entry(unsigned long current, acpi_iort_t *node,
				 acpi_iort_node_t **rc_out, u64 memory_properties,
				 u32 ats_attribute, u32 pci_segment_number,
				 u8 memory_address_limit, u16 pasid_capabilities)
{
	acpi_iort_root_complex_t *rc_node_data;
	acpi_iort_node_t *rc = (acpi_iort_node_t *)current;
	const unsigned long rc_start = current;

	ASSERT(rc_out);
	current += sizeof(*rc) + sizeof(*rc_node_data);
	memset(rc, 0, current - rc_start);

	rc->type = ACPI_IORT_NODE_PCI_ROOT_COMPLEX;
	rc->revision = 0x4;
	rc->identifier = node->node_count++; // Unique identifier.
	rc->mapping_offset = current - rc_start;

	/* length is further updated by id map entries */
	rc->length = rc->mapping_offset;

	rc_node_data = (acpi_iort_root_complex_t *)rc->node_data;
	rc_node_data->memory_properties = memory_properties;	/* Memory access properties */
	rc_node_data->ats_attribute = ats_attribute;
	rc_node_data->pci_segment_number = pci_segment_number;
	rc_node_data->memory_address_limit = memory_address_limit; /* Memory address size limit */
	rc_node_data->pasid_capabilities = pasid_capabilities;	   /* PASID Capabilities */

	*rc_out = rc;

	return current;
}

unsigned long acpi_iort_id_map_entry(unsigned long current, acpi_iort_node_t *node, u32 input_base,
				     u32 id_count, u32 output_base, u32 output_reference, u32 flags)
{
	acpi_iort_id_mapping_t *id_map = (acpi_iort_id_mapping_t *)current;

	ASSERT(node);
	memset(id_map, 0, sizeof(acpi_iort_id_mapping_t));

	current += sizeof(acpi_iort_id_mapping_t);

	node->mapping_count++;
	node->length += current - (unsigned long)id_map;

	id_map->input_base = input_base;
	id_map->id_count = id_count - 1;
	id_map->output_base = output_base;
	id_map->output_reference = output_reference;
	id_map->flags = flags;

	return current;
}
