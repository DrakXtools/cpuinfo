/*
 *  cpuinfo-arm.c - Processor identification code, arm specific
 *
 *  cpuinfo (C) 2017 Per Øyvind Karlsen
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "sysdeps.h"
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#if defined __linux__
#include <sys/utsname.h>
#endif
#include <sys/auxv.h>
#include "cpuinfo.h"
#include "cpuinfo-private.h"

#define DEBUG 1
#include "debug.h"


cpuinfo_feature_t cpuinfo_feature_architecture = CPUINFO_FEATURE_ARM,
		  cpuinfo_feature_architecture_max = CPUINFO_FEATURE_ARM_MAX;

// Arch-dependent data
struct arm_cpuinfo {
  uint32_t features[CPUINFO_FEATURES_SZ_(ARM)];
};

typedef struct arm_cpuinfo arm_cpuinfo_t;


// Returns a new cpuinfo descriptor
int cpuinfo_arch_new(struct cpuinfo *cip)
{
  arm_cpuinfo_t *p = (arm_cpuinfo_t *)malloc(sizeof(*p));
  if (p == NULL)
	return -1;
  memset(p->features, 0, sizeof(p->features));
  cip->opaque = p;
  return 0;

}


// Release the cpuinfo descriptor and all allocated data
void cpuinfo_arch_destroy(struct cpuinfo *cip)
{
  if (cip->opaque)
	free(cip->opaque);
}

int cpuinfo_dump(struct cpuinfo *cip, FILE *out)
{
    return 0;
}

int cpuinfo_arch_get_vendor(struct cpuinfo *cip)
{
    return 0;
}

// Get processor name
char *cpuinfo_arch_get_model(struct cpuinfo *cip)
{
    return "ARM";
}

// Get processor frequency in MHz
int cpuinfo_arch_get_frequency(struct cpuinfo *cip)
{
    return 0;
}

int cpuinfo_arch_get_socket(struct cpuinfo *cip)
{
    return -1;
}

// Get number of cores per CPU package
int cpuinfo_arch_get_cores(struct cpuinfo *cip)
{
    return 1;
}

// Get number of threads per CPU core
int cpuinfo_arch_get_threads(struct cpuinfo *cip)
{
    return 1;
}

cpuinfo_list_t cpuinfo_arch_get_caches(struct cpuinfo *cip)
{
    return NULL;
}

// Returns features table
uint32_t *cpuinfo_arch_feature_table(struct cpuinfo *cip, int feature)
{
  switch (feature & CPUINFO_FEATURE_ARCH) {
  case CPUINFO_FEATURE_COMMON:
	return cip->features;
  case CPUINFO_FEATURE_ARM:
	return ((arm_cpuinfo_t *)(cip->opaque))->features;
  }
  return NULL;
}

#define feature_get_bit(NAME) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_##NAME)
#define feature_set_bit(NAME) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_##NAME)

// Returns 1 if CPU supports the specified feature
int cpuinfo_arch_has_feature(struct cpuinfo *cip, unsigned long feature)
{
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM)) {
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM);

	for (unsigned long feat = CPUINFO_FEATURE_ARM+1; feat < CPUINFO_FEATURE_ARM_MAX; feat++) {
	    unsigned long type = 0;
	    unsigned long hwcap = 0;
	    unsigned long offset = CPUINFO_FEATURE_ARM;

	    if (feat > CPUINFO_FEATURE_ARM_CRYPTO) {
		hwcap = (1 << (feat - CPUINFO_FEATURE_ARM_CRYPTO-1));
		type = AT_HWCAP2;
	    } else if (feat < CPUINFO_FEATURE_ARM_CRYPTO) {
		if (feat == CPUINFO_FEATURE_ARM_IDIV)
		    hwcap = ((1 << (CPUINFO_FEATURE_ARM_IDIVA - offset)) |
			    (1 << (CPUINFO_FEATURE_ARM_IDIVT - offset)));
		else
		    hwcap = (1 << (feat - offset));
		type = AT_HWCAP;
	    }

	    if ((getauxval(type) & hwcap))
		cpuinfo_feature_set_bit(cip, feat);
	}

	if (feature_get_bit(IWMMXT) ||
		feature_get_bit(NEON))
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_SIMD);

	if (feature_get_bit(NEON))
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_POPCOUNT);
    }

  return cpuinfo_feature_get_bit(cip, feature);
}
