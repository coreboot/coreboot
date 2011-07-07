#define typeMalloc(type,elts) (type *)malloc((elts)*sizeof(type))
#define typeCalloc(type,elts) (type *)calloc((elts),sizeof(type))
#define typeRealloc(type,elts,ptr) (type *)_nc_doalloc(ptr, (elts)*sizeof(type))

