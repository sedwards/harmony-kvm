
#include <stdio.h>
#include <stdlib.h>
#include <uuid/uuid.h>

/* SERVICE_UUID 3C8DC645-7BB0-4F18-8A2A-1413E75D63F6 */
/* This is a wrapper, we do this for... reasons... */

int my_uuid_parse(char *SERVICE_UUID, uuid_t my_uuid) {
   	return uuid_parse(SERVICE_UUID, my_uuid);
}

