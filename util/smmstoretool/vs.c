/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "vs.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "udk2017.h"
#include "utils.h"

static size_t get_var_hdr_size(bool is_auth_var_store)
{
	if (is_auth_var_store)
		return sizeof(AUTHENTICATED_VARIABLE_HEADER);
	return sizeof(VARIABLE_HEADER);
}

struct var_store_t vs_load(struct mem_range_t vs_data, bool is_auth_var_store)
{
	uint8_t *var_hdr = vs_data.start;

	struct var_store_t vs = {
		.is_auth_var_store = is_auth_var_store,
		.vars = NULL,
	};

	struct var_t *last_var = NULL;

	const size_t var_hdr_size = get_var_hdr_size(is_auth_var_store);
	while (var_hdr + var_hdr_size < vs_data.start + vs_data.length) {
		uint16_t start_id;
		uint8_t state;
		struct var_t var = {0};
		uint8_t *var_data = var_hdr;

		if (is_auth_var_store) {
			const AUTHENTICATED_VARIABLE_HEADER *auth_hdr =
				(void *)var_data;

			start_id = auth_hdr->StartId;
			state = auth_hdr->State;

			var.reserved = auth_hdr->Reserved;
			var.attrs = auth_hdr->Attributes;
			var.name_size = auth_hdr->NameSize;
			var.data_size = auth_hdr->DataSize;
			var.guid = auth_hdr->VendorGuid;
		} else {
			const VARIABLE_HEADER *no_auth_hdr = (void *)var_data;

			start_id = no_auth_hdr->StartId;
			state = no_auth_hdr->State;

			var.reserved = no_auth_hdr->Reserved;
			var.attrs = no_auth_hdr->Attributes;
			var.name_size = no_auth_hdr->NameSize;
			var.data_size = no_auth_hdr->DataSize;
			var.guid = no_auth_hdr->VendorGuid;
		}

		var_hdr += HEADER_ALIGN(var_hdr_size +
					var.name_size +
					var.data_size);

		if (start_id != VARIABLE_DATA)
			break;

		if (state != VAR_ADDED)
			continue;

		if (var.data_size == UINT32_MAX ||
		    var.name_size == UINT32_MAX ||
		    var.attrs == UINT32_MAX)
			continue;

		CHAR16 *name = (void *)(var_data + var_hdr_size);
		var.name = xmalloc(var.name_size);
		memcpy(var.name, name, var.name_size);

		uint8_t *data =
			(void *)(var_data + var_hdr_size + var.name_size);
		var.data = xmalloc(var.data_size);
		memcpy(var.data, data, var.data_size);

		struct var_t *var_node = xmalloc(sizeof(*var_node));
		*var_node = var;
		if (last_var != NULL)
			last_var->next = var_node;
		else if (vs.vars == NULL)
			vs.vars = var_node;
		last_var = var_node;
	}

	return vs;
}

static void store_var(const struct var_t *var, bool is_auth_var_store, uint8_t *data)
{
	if (is_auth_var_store) {
		AUTHENTICATED_VARIABLE_HEADER hdr;
		memset(&hdr, 0xff, sizeof(hdr));

		hdr.StartId = VARIABLE_DATA;
		hdr.State = VAR_ADDED;
		hdr.Reserved = var->reserved;
		hdr.Attributes = var->attrs;
		hdr.VendorGuid = var->guid;
		hdr.NameSize = var->name_size;
		hdr.DataSize = var->data_size;

		hdr.MonotonicCount = 0;
		memset(&hdr.TimeStamp, 0, sizeof(hdr.TimeStamp));
		//hdr.PubKeyIndex = 0;

		memcpy(data, &hdr, sizeof(hdr));
		data += sizeof(hdr);
	} else {
		VARIABLE_HEADER hdr;
		memset(&hdr, 0xff, sizeof(hdr));

		hdr.StartId = VARIABLE_DATA;
		hdr.State = VAR_ADDED;
		hdr.Reserved = var->reserved;
		hdr.Attributes = var->attrs;
		hdr.VendorGuid = var->guid;
		hdr.NameSize = var->name_size;
		hdr.DataSize = var->data_size;

		memcpy(data, &hdr, sizeof(hdr));
		data += sizeof(hdr);
	}

	memcpy(data, var->name, var->name_size);
	memcpy(data + var->name_size, var->data, var->data_size);
}

bool vs_store(struct var_store_t *vs, struct mem_range_t vs_data)
{
	uint8_t *out_data = vs_data.start;

	const size_t var_hdr_size = get_var_hdr_size(vs->is_auth_var_store);
	for (struct var_t *var = vs->vars; var != NULL; var = var->next) {
		const size_t var_size =
			var_hdr_size + var->name_size + var->data_size;
		if (out_data + var_size > vs_data.start + vs_data.length) {
			fprintf(stderr,
				"Not enough space to serialize Variable Store.\n");
			return false;
		}

		store_var(var, vs->is_auth_var_store, out_data);
		out_data += HEADER_ALIGN(var_size);
	}

	// The rest is "uninitialized".
	memset(out_data, 0xff, vs_data.length - (out_data - vs_data.start));

	return true;
}

struct var_t *vs_new_var(struct var_store_t *vs)
{
	struct var_t *new_var = xmalloc(sizeof(*new_var));

	memset(new_var, 0, sizeof(*new_var));
	new_var->attrs = EFI_VARIABLE_NON_VOLATILE
				   | EFI_VARIABLE_BOOTSERVICE_ACCESS
				   | EFI_VARIABLE_RUNTIME_ACCESS;

	struct var_t *var = vs->vars;
	if (var == NULL) {
		vs->vars = new_var;
	} else {
		while (var->next != NULL)
			var = var->next;
		var->next = new_var;
	}

	return new_var;
}

struct var_t *vs_find(struct var_store_t *vs,
		      const char name[],
		      const EFI_GUID *guid)
{
	size_t name_size;
	CHAR16 *uchar_name = to_uchars(name, &name_size);

	struct var_t *var;
	for (var = vs->vars; var != NULL; var = var->next) {
		if (var->name_size != name_size)
			continue;
		if (memcmp(var->name, uchar_name, name_size) != 0)
			continue;
		if (memcmp(&var->guid, guid, sizeof(*guid)) != 0)
			continue;
		break;
	}

	free(uchar_name);
	return var;
}

static void free_var(struct var_t *var)
{
	free(var->name);
	free(var->data);
	free(var);
}

void vs_delete(struct var_store_t *vs, struct var_t *var)
{
	if (vs->vars == var) {
		vs->vars = var->next;
		free_var(var);
		return;
	}

	for (struct var_t *v = vs->vars; v != NULL; v = v->next) {
		if (v->next == var) {
			v->next = var->next;
			free_var(var);
			return;
		}
	}
}

void vs_free(struct var_store_t *vs)
{
	for (struct var_t *next, *var = vs->vars; var != NULL; var = next) {
		next = var->next;
		free_var(var);
	}
}
