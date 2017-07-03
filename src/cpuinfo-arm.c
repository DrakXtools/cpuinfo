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
  uint32_t crypto[CPUINFO_FEATURES_SZ_(ARM_CRYPTO)];

};

typedef struct arm_cpuinfo arm_cpuinfo_t;


// Returns a new cpuinfo descriptor
int cpuinfo_arch_new(struct cpuinfo *cip)
{
  arm_cpuinfo_t *p = (arm_cpuinfo_t *)malloc(sizeof(*p));
  if (p == NULL)
	return -1;
  memset(p->features, 0, sizeof(p->features));
  memset(p->crypto, 0, sizeof(p->crypto));
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
  case CPUINFO_FEATURE_AARCH64:
	return ((arm_cpuinfo_t *)(cip->opaque))->features;
  case CPUINFO_FEATURE_ARM_CRYPTO:
	return ((arm_cpuinfo_t *)(cip->opaque))->crypto;
  }
  return NULL;
}

#if (__arm__)
#define feature_get_bit(NAME) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_##NAME)
#define feature_set_bit(NAME) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_##NAME)
#define get_bit(i) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_BEGIN+1+i)
#define set_bit(i) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_BEGIN+1+i)

#else
#define feature_get_bit(NAME) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_AARCH64_##NAME)
#define feature_set_bit(NAME) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_AARCH64_##NAME)
#define get_bit(i) cpuinfo_feature_get_bit(cip, (CPUINFO_FEATURE_AARCH64_BEGIN)+i)
#define set_bit(i) cpuinfo_feature_set_bit(cip, (CPUINFO_FEATURE_AARCH64_BEGIN)+i)
#endif

#define crypto_get_bit(i) (cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO_BEGIN+i))
#define crypto_set_bit(i) (cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO_BEGIN+i))

// Returns 1 if CPU supports the specified feature
int cpuinfo_arch_has_feature(struct cpuinfo *cip, unsigned long feature)
{
#if defined(__arm__)
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM)) {
	unsigned long hwaux = getauxval(AT_HWCAP);
	unsigned long hwcap = (1<<0);
	int end = CPUINFO_FEATURE_ARM_MAX-(CPUINFO_FEATURE_ARM_BEGIN);
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM);

	for (int i = 0; i < end; i++, hwcap<<=1) {
	    if (hwaux & hwcap)
		set_bit(i);
	    if (hwcap == HWCAP_IDIVT && (hwaux & HWCAP_IDIV))
		feature_set_bit(IDIV);
	}

	if (feature_get_bit(NEON))
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_SIMD);
    }
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO)) {
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO);
	unsigned long hwaux = getauxval(AT_HWCAP2);
	unsigned long hwcap = (1<<0);
	int end = CPUINFO_FEATURE_ARM_CRYPTO_MAX-(CPUINFO_FEATURE_ARM_CRYPTO_BEGIN);

	for (int i = 0; i < end; i++, hwcap<<=1) {
	    if (hwaux & hwcap) {
		crypto_set_bit(i);
		cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_CRYPTO);
	    }
	}
    }
#endif
#if defined(__aarch64__)
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_AARCH64)) {
	unsigned long hwcap = (1<<0);
	unsigned long hwaux = getauxval(AT_HWCAP);
	int end = CPUINFO_FEATURE_AARCH64_MAX-CPUINFO_FEATURE_AARCH64_BEGIN;

	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_AARCH64);
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_64BIT);

#if 1
	for (int i = 0; i < end; i++, hwcap<<=1) {
	    if (hwaux & hwcap) {
		if (i >= 3 && i <= 7) {
		    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_CRYPTO);
		    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO_BEGIN+(i-3));
		} else
		    set_bit(i);
	    }
	}
#endif
	if (feature_get_bit(ASIMD))
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_SIMD);
    }
#endif
    if (cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_SIMD))
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_POPCOUNT);


    return cpuinfo_feature_get_bit(cip, feature);
}
