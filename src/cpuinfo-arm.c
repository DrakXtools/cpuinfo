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
#include <asm/hwcap.h>
#include "cpuinfo.h"
#include "cpuinfo-private.h"

#define DEBUG 1
#include "debug.h"


#ifdef __arm__
cpuinfo_feature_t cpuinfo_feature_architecture = CPUINFO_FEATURE_ARM,
		  cpuinfo_feature_architecture_max = CPUINFO_FEATURE_ARM_MAX;
#else
cpuinfo_feature_t cpuinfo_feature_architecture = CPUINFO_FEATURE_AARCH64,
		  cpuinfo_feature_architecture_max = CPUINFO_FEATURE_AARCH64_MAX;
#endif

// Arch-dependent data
struct arm_cpuinfo {
#ifdef __arm__
  uint32_t features[CPUINFO_FEATURES_SZ_(ARM)];
#else
  uint32_t features[CPUINFO_FEATURES_SZ_(AARCH64)];
#endif
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
    return NULL;
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
  case CPUINFO_FEATURE_AARCH64:
  case CPUINFO_FEATURE_ARM_CRYPTO:
	return ((arm_cpuinfo_t *)(cip->opaque))->features;
  }
  return NULL;
}

#define get_bit(base, i) cpuinfo_feature_get_bit(cip, base+i)
#define set_bit(base, i) cpuinfo_feature_set_bit(cip, base+i)

#define feature_get_32bit(NAME) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_##NAME)
#define feature_set_32bit(NAME) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_##NAME)
#define feature_get_64bit(NAME) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_AARCH64_##NAME)
#define feature_set_64bit(NAME) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_AARCH64_##NAME)


static void cpuinfo_arch_features(struct cpuinfo *cip, cpuinfo_feature_t begin, cpuinfo_feature_t features) {

    if (!cpuinfo_feature_get_bit(cip, begin)) {
	cpuinfo_feature_t arch = (features& CPUINFO_FEATURE_ARCH);
	unsigned long hwaux = getauxval((arch == CPUINFO_FEATURE_ARM_CRYPTO) ? AT_HWCAP2 : AT_HWCAP);
	unsigned long hwcap = (1<<0);

	if (arch != CPUINFO_FEATURE_ARM_CRYPTO)
	    cpuinfo_feature_set_bit(cip, begin);
	for (cpuinfo_feature_t bit = ++begin; bit < features; bit++, hwcap<<=1) {
	    if ((hwaux & hwcap)) {
#if defined(__aarch64__)
		if (bit >= CPUINFO_FEATURE_AARCH64_CRYPTO_AES &&
			bit <= CPUINFO_FEATURE_AARCH64_CRYPTO_CRC32) {
		    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO_BEGIN + (bit-(begin+1))-1);
		} else
#endif
		    cpuinfo_feature_set_bit(cip, bit);
	    }

	}
    }
}

#define arch_features(arch) cpuinfo_arch_features(cip, CPUINFO_FEATURE_##arch##_BEGIN, CPUINFO_FEATURE_##arch##_MAX)

// Returns 1 if CPU supports the specified feature
int cpuinfo_arch_has_feature(struct cpuinfo *cip, unsigned long feature)
{
#if defined(__arm__)
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO)) {
	arch_features(ARM);
	arch_features(ARM_CRYPTO);

    if (feature_get_32bit(NEON))
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_SIMD);

    if (feature_get_32bit(IDIVA) && feature_get_32bit(IDIVT))
	feature_set_32bit(IDIV);
    }
#else
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_AARCH64)) {
	arch_features(AARCH64);
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_64BIT);

	if (feature_get_64bit(ASIMD))
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_SIMD);
    }
#endif

    if (cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_SIMD))
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_POPCOUNT);

    for (cpuinfo_feature_t feat = CPUINFO_FEATURE_ARM_CRYPTO_BEGIN+1; feat < CPUINFO_FEATURE_ARM_CRYPTO_MAX; feat++) {
	if (cpuinfo_feature_get_bit(cip, feat)) {
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO_BEGIN);
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_CRYPTO);
	    break;
	}
    }

    return cpuinfo_feature_get_bit(cip, feature);
}
