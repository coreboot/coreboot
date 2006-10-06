/*
 * (C) Copyright David Gibson <dwg@au1.ibm.com>, IBM Corporation.  2005.
 *
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *                                                                       
 *  You should have received a copy of the GNU General Public License    
 *  along with this program; if not, write to the Free Software          
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *                                                                   USA 
 */

#include "dtc.h"

/*
 * Tree building functions
 */

struct property *build_property(char *name, struct data val, char *label)
{
	struct property *new = xmalloc(sizeof(*new));

	new->name = name;
	new->val = val;

	new->next = NULL;

	new->label = label;

	return new;
}

struct property *chain_property(struct property *first, struct property *list)
{
	assert(first->next == NULL);
	
	first->next = list;
	return first;      
}

struct node *build_node(struct property *proplist, struct node *children)
{
	struct node *new = xmalloc(sizeof(*new));
	struct node *child;

	memset(new, 0, sizeof(*new));

	new->proplist = proplist;
	new->children = children;

	for_each_child(new, child) {
		child->parent = new;
	}

	return new;
}

struct node *name_node(struct node *node, char *name, char * label)
{
	assert(node->name == NULL);

	node->name = name;

	node->label = label;

	return node;
}

struct node *chain_node(struct node *first, struct node *list)
{
	assert(first->next_sibling == NULL);

	first->next_sibling = list;
	return first;
}

void add_property(struct node *node, struct property *prop)
{
	struct property **p;

	prop->next = NULL;

	p = &node->proplist;
	while (*p)
		p = &((*p)->next);

	*p = prop;
}

void add_child(struct node *parent, struct node *child)
{
	struct node **p;

	child->next_sibling = NULL;

	p = &parent->children;
	while (*p)
		p = &((*p)->next_sibling);

	*p = child;
}

struct reserve_info *build_reserve_entry(u64 address, u64 size, char *label)
{
	struct reserve_info *new = xmalloc(sizeof(*new));

	new->re.address = address;
	new->re.size = size;

	new->next = NULL;

	new->label = label;

	return new;
}

struct reserve_info *chain_reserve_entry(struct reserve_info *first,
					struct reserve_info *list)
{
	assert(first->next == NULL);

	first->next = list;
	return first;
}

struct reserve_info *add_reserve_entry(struct reserve_info *list,
				      struct reserve_info *new)
{
	struct reserve_info *last;

	new->next = NULL;

	if (! list)
		return new;

	for (last = list; last->next; last = last->next)
		;

	last->next = new;

	return list;
}

/*
 * Tree accessor functions
 */

static char *get_unitname(struct node *node)
{
	if (node->name[node->basenamelen] == '\0')
		return "";
	else
		return node->name + node->basenamelen + 1;
}

static struct property *get_property(struct node *node, char *propname)
{
	struct property *prop;

	for_each_property(node, prop)
		if (streq(prop->name, propname))
			return prop;

	return NULL;
}

static cell_t propval_cell(struct property *prop)
{
	assert(prop->val.len == sizeof(cell_t));
	return be32_to_cpu(*((cell_t *)prop->val.val));
}

static struct node *get_subnode(struct node *node, char *nodename)
{
	struct node *child;

	for_each_child(node, child)
		if (streq(child->name, nodename))
			return child;

	return NULL;
}

static struct node *get_node_by_path(struct node *tree, char *path)
{
	char *p;
	struct node *child;

	if (!path || ! (*path))
		return tree;

	while (path[0] == '/')
		path++;

	p = strchr(path, '/');

	for_each_child(tree, child) {
		if (p && strneq(path, child->name, p-path))
			return get_node_by_path(child, p+1);
		else if (!p && streq(path, child->name))
			return child;
	}

	return NULL;
}

static struct node *get_node_by_phandle(struct node *tree, cell_t phandle)
{
	struct node *child, *node;	

	assert((phandle != 0) && (phandle != -1));

	if (tree->phandle == phandle)
		return tree;

	for_each_child(tree, child) {
		node = get_node_by_phandle(child, phandle);
		if (node)
			return node;
	}

	return NULL;
}
/*
 * Tree checking functions
 */

#define ERRMSG(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
#define WARNMSG(...) fprintf(stderr, "Warning: " __VA_ARGS__)

static int must_be_one_cell(struct property *prop, struct node *node)
{
	if (prop->val.len != sizeof(cell_t)) {
		ERRMSG("\"%s\" property in %s has the wrong length (should be 1 cell)\n",
		       prop->name, node->fullpath);
		return 0;
	}

	return 1;
}

static int must_be_cells(struct property *prop, struct node *node)
{
	if ((prop->val.len % sizeof(cell_t)) != 0) {
		ERRMSG("\"%s\" property in %s is not a multiple of cell size\n",
		       prop->name, node->fullpath);
		return 0;
	}

	return 1;
}

static int must_be_string(struct property *prop, struct node *node)
{
	if (! data_is_one_string(prop->val)) {
		ERRMSG("\"%s\" property in %s is not a string\n",
		       prop->name, node->fullpath);
		return 0;
	}

	return 1;
}

static int name_prop_check(struct property *prop, struct node *node)
{
	if ((prop->val.len != node->basenamelen+1)
	    || !strneq(prop->val.val, node->name, node->basenamelen)) {
		ERRMSG("name property \"%s\" does not match node basename in %s\n",
		       prop->val.val,
		       node->fullpath);
		return 0;
	}

	return 1;
}

static struct {
	char *propname;
	int (*check_fn)(struct property *prop, struct node *node);
} prop_checker_table[] = {
	{"name", must_be_string},
	{"name", name_prop_check},
	{"linux,phandle", must_be_one_cell},
	{"#address-cells", must_be_one_cell},
	{"#size-cells", must_be_one_cell},
	{"reg", must_be_cells},
	{"model", must_be_string},
	{"device_type", must_be_string},
};

#define DO_ERR(...) do {ERRMSG(__VA_ARGS__); ok = 0; } while (0)

static int check_properties(struct node *node)
{
	struct property *prop, *prop2;
	int ok = 1;

	for_each_property(node, prop) {
		int i;

		/* check for duplicates */
		/* FIXME: do this more efficiently */
		for (prop2 = prop->next; prop2; prop2 = prop2->next) {
			if (streq(prop->name, prop2->name)) {
				DO_ERR("Duplicate propertyname %s in node %s\n",
					prop->name, node->fullpath);
			}
		}
			

		/* check name length */
		if (strlen(prop->name) > MAX_PROPNAME_LEN)
			WARNMSG("Property name %s is too long in %s\n",
				prop->name, node->fullpath);
			
		/* check this property */
		for (i = 0; i < ARRAY_SIZE(prop_checker_table); i++) {
			if (streq(prop->name, prop_checker_table[i].propname))
				if (! prop_checker_table[i].check_fn(prop, node)) {
					ok = 0;
					break;
				}
		}
	}

	return ok;
}

static int check_node_name(struct node *node)
{
	int ok = 1;
	int len = strlen(node->name);

	if (len == 0 && node->parent)
		DO_ERR("Empty, non-root nodename at %s\n", node->fullpath);

	if (len > MAX_NODENAME_LEN)
		DO_ERR("Overlength nodename at %s\n", node->fullpath);


	return ok;
}

static int check_structure(struct node *tree)
{
	struct node *child, *child2;
	int ok = 1;

	if (! check_node_name(tree))
		ok = 0;

	if (! check_properties(tree))
		ok = 0;

	for_each_child(tree, child) {
		/* Check for duplicates */

		for (child2 = child->next_sibling;
		     child2;
		     child2 = child2->next_sibling) {
			if (streq(child->name, child2->name))
				DO_ERR("Duplicate node name %s\n",
					child->fullpath);
		}
		if (! check_structure(child))
			ok = 0;
	}

	return ok;
}

#define CHECK_HAVE(node, propname) \
	do { \
		if (! (prop = get_property((node), (propname)))) \
			DO_ERR("Missing \"%s\" property in %s\n", (propname), \
				(node)->fullpath); \
	} while (0);

#define CHECK_HAVE_WARN(node, propname) \
	do { \
		if (! (prop  = get_property((node), (propname)))) \
			WARNMSG("%s has no \"%s\" property\n", \
				(node)->fullpath, (propname)); \
	} while (0)

#define CHECK_HAVE_STRING(node, propname) \
	do { \
		CHECK_HAVE((node), (propname)); \
		if (prop && !data_is_one_string(prop->val)) \
			DO_ERR("\"%s\" property in %s is not a string\n", \
				(propname), (node)->fullpath); \
	} while (0)

#define CHECK_HAVE_STREQ(node, propname, value) \
	do { \
		CHECK_HAVE_STRING((node), (propname)); \
		if (prop && !streq(prop->val.val, (value))) \
			DO_ERR("%s has wrong %s, %s (should be %s\n", \
				(node)->fullpath, (propname), \
				prop->val.val, (value)); \
	} while (0)
		 
#define CHECK_HAVE_ONECELL(node, propname) \
	do { \
		CHECK_HAVE((node), (propname)); \
		if (prop && (prop->val.len != sizeof(cell_t))) \
			DO_ERR("\"%s\" property in %s has wrong size %d (should be 1 cell)\n", (propname), (node)->fullpath, prop->val.len); \
	} while (0)

#define CHECK_HAVE_WARN_ONECELL(node, propname) \
	do { \
		CHECK_HAVE_WARN((node), (propname)); \
		if (prop && (prop->val.len != sizeof(cell_t))) \
			DO_ERR("\"%s\" property in %s has wrong size %d (should be 1 cell)\n", (propname), (node)->fullpath, prop->val.len); \
	} while (0)

#define CHECK_HAVE_WARN_PHANDLE(xnode, propname, root) \
	do { \
		struct node *ref; \
		CHECK_HAVE_WARN_ONECELL((xnode), (propname)); \
		if (prop) {\
			ref = get_node_by_phandle((root), propval_cell(prop)); \
			if (! ref) \
				DO_ERR("\"%s\" property in %s refers to non-existant phandle %x\n", (propname), (xnode)->fullpath, propval_cell(prop)); \
		} \
	} while (0)

#define CHECK_HAVE_WARN_STRING(node, propname) \
	do { \
		CHECK_HAVE_WARN((node), (propname)); \
		if (prop && !data_is_one_string(prop->val)) \
			DO_ERR("\"%s\" property in %s is not a string\n", \
				(propname), (node)->fullpath); \
	} while (0)

static int check_root(struct node *root)
{
	struct property *prop;
	int ok = 1;

	CHECK_HAVE_STRING(root, "model");

	CHECK_HAVE(root, "#address-cells");
	CHECK_HAVE(root, "#size-cells");

	CHECK_HAVE_WARN(root, "compatible");

	return ok;
}

static int check_cpus(struct node *root, int outversion, int boot_cpuid_phys)
{
	struct node *cpus, *cpu;
	struct property *prop;
	struct node *bootcpu = NULL;
	int ok = 1;

	cpus = get_subnode(root, "cpus");
	if (! cpus) {
		ERRMSG("Missing /cpus node\n");
		return 0;
	}

	CHECK_HAVE_WARN(cpus, "#address-cells");
	CHECK_HAVE_WARN(cpus, "#size-cells");

	for_each_child(cpus, cpu) {
		CHECK_HAVE_STREQ(cpu, "device_type", "cpu");

		if (cpu->addr_cells != 1)
			DO_ERR("%s has bad #address-cells value %d (should be 1)\n",
			       cpu->fullpath, cpu->addr_cells);
		if (cpu->size_cells != 0)
			DO_ERR("%s has bad #size-cells value %d (should be 0)\n",
			       cpu->fullpath, cpu->size_cells);

		CHECK_HAVE_ONECELL(cpu, "reg");
		if (prop) {
			cell_t unitnum;
			char *eptr;

			unitnum = strtol(get_unitname(cpu), &eptr, 16);
			if (*eptr)
				WARNMSG("%s has bad format unit name %s (should be CPU number\n",
					cpu->fullpath, get_unitname(cpu));
			else if (unitnum != propval_cell(prop))
				WARNMSG("%s unit name \"%s\" does not match \"reg\" property <%x>\n",
				       cpu->fullpath, get_unitname(cpu),
				       propval_cell(prop));
		}

/* 		CHECK_HAVE_ONECELL(cpu, "d-cache-line-size"); */
/* 		CHECK_HAVE_ONECELL(cpu, "i-cache-line-size"); */
		CHECK_HAVE_ONECELL(cpu, "d-cache-size");
		CHECK_HAVE_ONECELL(cpu, "i-cache-size");

		CHECK_HAVE_WARN_ONECELL(cpu, "clock-frequency");
		CHECK_HAVE_WARN_ONECELL(cpu, "timebase-frequency");

		prop = get_property(cpu, "linux,boot-cpu");
		if (prop) {
			if (prop->val.len)
				WARNMSG("\"linux,boot-cpu\" property in %s is non-empty\n",
					cpu->fullpath);
			if (bootcpu)
				DO_ERR("Multiple boot cpus (%s and %s)\n",
				       bootcpu->fullpath, cpu->fullpath);
			else
				bootcpu = cpu;
		}
	}

	if (outversion < 2) {
		if (! bootcpu)
			WARNMSG("No cpu has \"linux,boot-cpu\" property\n");
	} else {
		if (bootcpu)
			WARNMSG("\"linux,boot-cpu\" property is deprecated in blob version 2 or higher\n");
		if (boot_cpuid_phys == 0xfeedbeef)
			WARNMSG("physical boot CPU not set.  Use -b option to set\n");
	}

	return ok;	
}

static int check_memory(struct node *root)
{
	struct node *mem;
	struct property *prop;
	int nnodes = 0;
	int ok = 1;

	for_each_child(root, mem) {
		if (! strneq(mem->name, "memory", mem->basenamelen))
			continue;

		nnodes++;

		CHECK_HAVE_STREQ(mem, "device_type", "memory");
		CHECK_HAVE(mem, "reg");
	}

	if (nnodes == 0) {
		ERRMSG("No memory nodes\n");
		return 0;
	}

	return ok;	
}

static int check_chosen(struct node *root)
{
	struct node *chosen;
	struct property *prop;
	int ok = 1;

	chosen = get_subnode(root, "chosen");
	if (! chosen) {
		ERRMSG("Missing /chosen node\n");
		return 0;
	}

	CHECK_HAVE_ONECELL(chosen, "linux,platform");

	CHECK_HAVE_WARN_STRING(chosen, "bootargs");
	CHECK_HAVE_WARN_STRING(chosen, "linux,stdout-path");
	CHECK_HAVE_WARN_PHANDLE(chosen, "interrupt-controller", root);

	return ok;	
}

static int check_addr_size_reg(struct node *node,
			       int p_addr_cells, int p_size_cells)
{
	int addr_cells = p_addr_cells;
	int size_cells = p_size_cells;
	struct property *prop;
	struct node *child;
	int ok = 1;

	node->addr_cells = addr_cells;
	node->size_cells = size_cells;

	prop = get_property(node, "reg");
	if (prop) {
		int len = prop->val.len / 4;

		if ((len % (addr_cells+size_cells)) != 0)
			DO_ERR("\"reg\" property in %s has invalid length (%d) for given #address-cells (%d) and #size-cells (%d)\n",
			       node->fullpath, prop->val.len,
			       addr_cells, size_cells);
	}

	prop = get_property(node, "#address-cells");
	if (prop)
		addr_cells = propval_cell(prop);

	prop = get_property(node, "#size-cells");
	if (prop)
		size_cells = propval_cell(prop);

	for_each_child(node, child) {
		ok = ok && check_addr_size_reg(child, addr_cells, size_cells);
	}

	return ok;
}

static int check_phandles(struct node *root, struct node *node)
{
	struct property *prop;
	struct node *child, *other;
	cell_t phandle;
	int ok = 1;

	prop = get_property(node, "linux,phandle");
	if (prop) {
		phandle = propval_cell(prop);
		if ((phandle == 0) || (phandle == -1)) {
			DO_ERR("%s has invalid linux,phandle %x\n",
			       node->fullpath, phandle);
		} else {
			other = get_node_by_phandle(root, phandle);
			if (other)
				DO_ERR("%s has duplicated phandle %x (seen before at %s)\n",
				       node->fullpath, phandle, other->fullpath);

			node->phandle = phandle;
		}
	}

	for_each_child(node, child)
		ok = ok && check_phandles(root, child);

	return 1;
}

static cell_t get_node_phandle(struct node *root, struct node *node)
{
	static cell_t phandle = 1; /* FIXME: ick, static local */

	fprintf(stderr, "get_node_phandle(%s)   phandle=%x\n",
		node->fullpath, node->phandle);

	if ((node->phandle != 0) && (node->phandle != -1))
		return node->phandle;

	assert(! get_property(node, "linux,phandle"));

	while (get_node_by_phandle(root, phandle))
		phandle++;

	node->phandle = phandle;
	add_property(node,
		     build_property("linux,phandle",
				    data_append_cell(empty_data, phandle),
				    NULL));

	return node->phandle;
}

static void apply_fixup(struct node *root, struct property *prop,
			struct fixup *f)
{
	struct node *refnode;
	cell_t phandle;

	refnode = get_node_by_path(root, f->ref);
	if (! refnode)
		die("Reference to non-existent node \"%s\"\n", f->ref);

	phandle = get_node_phandle(root, refnode);

	assert(f->offset + sizeof(cell_t) <= prop->val.len);

	*((cell_t *)(prop->val.val + f->offset)) = cpu_to_be32(phandle);
}

static void fixup_phandles(struct node *root, struct node *node)
{
	struct property *prop;
	struct node *child;

	for_each_property(node, prop) {
		struct fixup *f = prop->val.refs;

		while (f) {
			apply_fixup(root, prop, f);
			prop->val.refs = f->next;
			fixup_free(f);
			f = prop->val.refs;
		}
	}

	for_each_child(node, child)
		fixup_phandles(root, child);
}

int check_device_tree(struct node *dt, int outversion, int boot_cpuid_phys)
{
	int ok = 1;

	if (! check_structure(dt))
		return 0;

	ok = ok && check_addr_size_reg(dt, -1, -1);
	ok = ok && check_phandles(dt, dt);

	fixup_phandles(dt, dt);

	if (! ok)
		return 0;

	ok = ok && check_root(dt);
	ok = ok && check_cpus(dt, outversion, boot_cpuid_phys);
	ok = ok && check_memory(dt);
	ok = ok && check_chosen(dt);
	if (! ok)
		return 0;

	return 1;
}

struct boot_info *build_boot_info(struct reserve_info *reservelist,
				  struct node *tree)
{
	struct boot_info *bi;

	bi = xmalloc(sizeof(*bi));
	bi->reservelist = reservelist;
	bi->dt = tree;

	return bi;
}
