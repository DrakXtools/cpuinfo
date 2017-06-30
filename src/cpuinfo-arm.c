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
  }
  return NULL;
}

#if (__arm__)
#define feature_get_bit(NAME) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM_##NAME)
#define feature_set_bit(NAME) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_##NAME)
#else
#define feature_get_bit(NAME) cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_AARCH64_##NAME)
#define feature_set_bit(NAME) cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_AARCH64_##NAME)
#endif

// Returns 1 if CPU supports the specified feature
int cpuinfo_arch_has_feature(struct cpuinfo *cip, unsigned long feature)
{
#if defined(__arm__)
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_ARM)) {
	int end = CPUINFO_FEATURE_ARM_MAX-CPUINFO_FEATURE_ARM;

	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM);

	for (int i = 0; i < end; i++) {
	    unsigned long hwcap = (1<<i);
	    unsigned long type = AT_HWCAP;
	    int j = 0;
	    if (i == CPUINFO_FEATURE_ARM_MAX-CPUINFO_FEATURE_ARM_CRYPTO) {
		type = getauxval(AT_HWCAP2);
		hwcap = (1<<j++);
		if (type & hwcap)
		    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM_CRYPTO + i);

	    } else {
		type = getauxval(type);
		if (type & hwcap)
		    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_ARM + i);
		if (hwcap == HWCAP_IDIVT && (type & HWCAP_IDIV))
		    feature_set_bit(IDIV);
	    }
	}
    }
#endif
#if defined(__aarch64__)
    if (!cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_AARCH64)) {
	unsigned long type = getauxval(AT_HWCAP);

	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_AARCH64);
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_64BIT);

	if (type & HWCAP_FP)
	    feature_set_bit(FP);
	if (type & HWCAP_ASIMD)
	    feature_set_bit(ASIMD);
	if (type & HWCAP_EVTSTRM)
	    feature_set_bit(EVTSTRM);

	if (type & HWCAP_AES)
	    feature_set_bit(CRYPTO_AES);
	if (type & HWCAP_PMULL)
	    feature_set_bit(CRYPTO_PMULL);
	if (type & HWCAP_SHA1)
	    feature_set_bit(CRYPTO_SHA1);
	if (type & HWCAP_SHA2)
	    feature_set_bit(CRYPTO_SHA2);
	if (type & HWCAP_CRC32)
	    feature_set_bit(CRYPTO_CRC32);

	if (type & HWCAP_ATOMICS)
	    feature_set_bit(ATOMICS);
	if (type & HWCAP_FPHP)
	    feature_set_bit(FPHP);
	if (type & HWCAP_ASIMDHP)
	    feature_set_bit(ASIMDHP);

	if (feature_get_bit(ASIMD))
	    cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_SIMD);
    }
#endif


    if (cpuinfo_feature_get_bit(cip, CPUINFO_FEATURE_SIMD))
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_POPCOUNT);

    if (feature_get_bit(CRYPTO_AES) ||
	    feature_get_bit(CRYPTO_PMULL) ||
	    feature_get_bit(CRYPTO_SHA1) ||
	    feature_get_bit(CRYPTO_SHA2) ||
	    feature_get_bit(CRYPTO_CRC32))
	cpuinfo_feature_set_bit(cip, CPUINFO_FEATURE_CRYPTO);

    return cpuinfo_feature_get_bit(cip, feature);
}
