/*
 *  cpuinfo.h - Public interface
 *
 *  cpuinfo (C) 2006-2007 Gwenole Beauchesne
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

#ifndef CPUINFO_H
#define CPUINFO_H

#ifdef __cplusplus
extern "C" {
#endif

// cpuinfo_t is a private data structure
typedef struct cpuinfo cpuinfo_t;

// Returns a new cpuinfo descriptor
extern cpuinfo_t *cpuinfo_new(void);

// Release the cpuinfo descriptor and all allocated data
extern void cpuinfo_destroy(cpuinfo_t *cip);

// Dump all useful information for debugging
extern int cpuinfo_dump(cpuinfo_t *cip, FILE *out);

/* ========================================================================= */
/* == General Processor Information                                       == */
/* ========================================================================= */

#define CPUINFO_CLASS(N) (((unsigned int)(N)) << 8)

// Processor vendor
typedef enum {
  CPUINFO_VENDOR_UNKNOWN,
  CPUINFO_VENDOR_AMD,
  CPUINFO_VENDOR_CENTAUR,
  CPUINFO_VENDOR_CYRIX,
  CPUINFO_VENDOR_IBM,
  CPUINFO_VENDOR_INTEL,
  CPUINFO_VENDOR_MOTOROLA,
  CPUINFO_VENDOR_MIPS,
  CPUINFO_VENDOR_NEXTGEN,
  CPUINFO_VENDOR_NSC,
  CPUINFO_VENDOR_PMC,
  CPUINFO_VENDOR_RISE,
  CPUINFO_VENDOR_SIS,
  CPUINFO_VENDOR_TRANSMETA,
  CPUINFO_VENDOR_UMC,
  CPUINFO_VENDOR_PASEMI
} cpuinfo_vendor_t;

void cpuinfo_get_endian(cpuinfo_t *cip);

// Get processor vendor ID 
extern int cpuinfo_get_vendor(cpuinfo_t *cip);

// Get processor name
extern const char *cpuinfo_get_model(cpuinfo_t *cip);

// Processor socket
typedef enum {
  CPUINFO_SOCKET_UNKNOWN,

  CPUINFO_SOCKET_478 = CPUINFO_CLASS('I'),
  CPUINFO_SOCKET_479,
  CPUINFO_SOCKET_604,
  CPUINFO_SOCKET_771,
  CPUINFO_SOCKET_775,

  CPUINFO_SOCKET_754 = CPUINFO_CLASS('A'),
  CPUINFO_SOCKET_939,
  CPUINFO_SOCKET_940,
  CPUINFO_SOCKET_AM2,
  CPUINFO_SOCKET_F,
  CPUINFO_SOCKET_S1
} cpuinfo_socket_t;

// Get processor frequency in MHz
extern int cpuinfo_get_frequency(cpuinfo_t *cip);

// Get processor socket ID
extern int cpuinfo_get_socket(cpuinfo_t *cip);

// Get number of cores per CPU package
extern int cpuinfo_get_cores(cpuinfo_t *cip);

// Get number of threads per CPU core
extern int cpuinfo_get_threads(cpuinfo_t *cip);

/* ========================================================================= */
/* == Processor Caches Information                                        == */
/* ========================================================================= */

typedef enum {
  CPUINFO_CACHE_TYPE_UNKNOWN,
  CPUINFO_CACHE_TYPE_DATA,
  CPUINFO_CACHE_TYPE_CODE,
  CPUINFO_CACHE_TYPE_UNIFIED,
  CPUINFO_CACHE_TYPE_TRACE
} cpuinfo_cache_type_t;

typedef struct {
  int type;		// cache type (above)
  int level;	// cache level
  int size;		// cache size in KB
} cpuinfo_cache_descriptor_t;

typedef struct {
  int count;	// number of cache descriptors
  const cpuinfo_cache_descriptor_t *descriptors;
} cpuinfo_cache_t;

// Get cache information (returns read-only descriptors)
extern const cpuinfo_cache_t *cpuinfo_get_caches(cpuinfo_t *cip);

/* ========================================================================= */
/* == Processor Features Information                                      == */
/* ========================================================================= */

typedef enum {
  CPUINFO_FEATURE_ARCH	= 0xff00,
  CPUINFO_FEATURE_MASK	= 0x00ff,

  CPUINFO_FEATURE_COMMON = 0,
  CPUINFO_FEATURE_64BIT,
  CPUINFO_FEATURE_SIMD,
  CPUINFO_FEATURE_POPCOUNT,
  CPUINFO_FEATURE_CRYPTO,
  CPUINFO_FEATURE_LITTLE_ENDIAN,
  CPUINFO_FEATURE_BIG_ENDIAN,
  CPUINFO_FEATURE_MIDDLE_ENDIAN,
  CPUINFO_FEATURE_COMMON_MAX,

  CPUINFO_FEATURE_X86	= CPUINFO_CLASS('X'),
  CPUINFO_FEATURE_X86_AC,
  CPUINFO_FEATURE_X86_CPUID,
  CPUINFO_FEATURE_X86_FPU,
  CPUINFO_FEATURE_X86_VME,
  CPUINFO_FEATURE_X86_DE,
  CPUINFO_FEATURE_X86_PSE,
  CPUINFO_FEATURE_X86_TSC,
  CPUINFO_FEATURE_X86_MSR,
  CPUINFO_FEATURE_X86_PAE,
  CPUINFO_FEATURE_X86_MCE,
  CPUINFO_FEATURE_X86_CX8,
  CPUINFO_FEATURE_X86_APIC,
  CPUINFO_FEATURE_X86_SEP,
  CPUINFO_FEATURE_X86_MTRR,
  CPUINFO_FEATURE_X86_PGE,
  CPUINFO_FEATURE_X86_MCA,
  CPUINFO_FEATURE_X86_CMOV,
  CPUINFO_FEATURE_X86_PAT,
  CPUINFO_FEATURE_X86_PSE_36,
  CPUINFO_FEATURE_X86_PSN,
  CPUINFO_FEATURE_X86_CLFLUSH,
  CPUINFO_FEATURE_X86_DS,
  CPUINFO_FEATURE_X86_ACPI,
  CPUINFO_FEATURE_X86_FXSR,
  CPUINFO_FEATURE_X86_SS,
  CPUINFO_FEATURE_X86_HTT,
  CPUINFO_FEATURE_X86_IA64,
  CPUINFO_FEATURE_X86_PBE,
  CPUINFO_FEATURE_X86_MMX,
  CPUINFO_FEATURE_X86_MMX_EXT,
  CPUINFO_FEATURE_X86_3DNOW,
  CPUINFO_FEATURE_X86_3DNOW_EXT,
  CPUINFO_FEATURE_X86_3DNOW_PREFETCH,  
  CPUINFO_FEATURE_X86_SSE,
  CPUINFO_FEATURE_X86_SSE2,
  CPUINFO_FEATURE_X86_SSE3,
  CPUINFO_FEATURE_X86_SSSE3,
  CPUINFO_FEATURE_X86_SSE4_1,
  CPUINFO_FEATURE_X86_SSE4_2,
  CPUINFO_FEATURE_X86_SSE4A,
  CPUINFO_FEATURE_X86_SSE5,
  CPUINFO_FEATURE_X86_MISALIGNSSE,
  CPUINFO_FEATURE_X86_VMX,
  CPUINFO_FEATURE_X86_SVM,
  CPUINFO_FEATURE_X86_LM,
  CPUINFO_FEATURE_X86_LAHF64,
  CPUINFO_FEATURE_X86_POPCNT,
  CPUINFO_FEATURE_X86_TSC_DEADLINE,
  CPUINFO_FEATURE_X86_ABM,
  CPUINFO_FEATURE_X86_BSFCC,
  CPUINFO_FEATURE_X86_TM,
  CPUINFO_FEATURE_X86_TM2,
  CPUINFO_FEATURE_X86_EIST,
  CPUINFO_FEATURE_X86_NX,
  CPUINFO_FEATURE_X86_DTES64,
  CPUINFO_FEATURE_X86_MONITOR,
  CPUINFO_FEATURE_X86_DS_CPL,
  CPUINFO_FEATURE_X86_SMX,
  CPUINFO_FEATURE_X86_CNXT_ID,
  CPUINFO_FEATURE_X86_CX16,
  CPUINFO_FEATURE_X86_XTPR,
  CPUINFO_FEATURE_X86_PDCM,
  CPUINFO_FEATURE_X86_PCID,
  CPUINFO_FEATURE_X86_DCA,
  CPUINFO_FEATURE_X86_X2APIC,
  CPUINFO_FEATURE_X86_MOVBE,
  CPUINFO_FEATURE_X86_XSAVE,
  CPUINFO_FEATURE_X86_OSXSAVE,
  CPUINFO_FEATURE_X86_PCLMULQDQ,
  CPUINFO_FEATURE_X86_FMA,
  CPUINFO_FEATURE_X86_AES,
  CPUINFO_FEATURE_X86_AVX,
  CPUINFO_FEATURE_X86_F16C,
  CPUINFO_FEATURE_X86_HYPERVISOR,
  CPUINFO_FEATURE_X86_CMP_LEGACY,
  CPUINFO_FEATURE_X86_EXTAPIC,
  CPUINFO_FEATURE_X86_CR8_LEGACY,
  CPUINFO_FEATURE_X86_OSVW,
  CPUINFO_FEATURE_X86_IBS,
  CPUINFO_FEATURE_X86_SKINIT,
  CPUINFO_FEATURE_X86_WDT,
  CPUINFO_FEATURE_X86_LWP,
  CPUINFO_FEATURE_X86_FMA4,
  CPUINFO_FEATURE_X86_NODEID_MSR,
  CPUINFO_FEATURE_X86_TBM,
  CPUINFO_FEATURE_X86_TOPOEXT,
  CPUINFO_FEATURE_X86_FFXSR,
  CPUINFO_FEATURE_X86_PAGE1GB,
  CPUINFO_FEATURE_X86_RDTSCP,
  CPUINFO_FEATURE_X86_MAX,
  
  CPUINFO_FEATURE_IA64	= CPUINFO_CLASS('I'),
  CPUINFO_FEATURE_IA64_LB,
  CPUINFO_FEATURE_IA64_SD,
  CPUINFO_FEATURE_IA64_AO,
  CPUINFO_FEATURE_IA64_MAX,

  CPUINFO_FEATURE_PPC	= CPUINFO_CLASS('P'),
  CPUINFO_FEATURE_PPC_VMX,
  CPUINFO_FEATURE_PPC_FSQRT,
  CPUINFO_FEATURE_PPC_FSEL,
  CPUINFO_FEATURE_PPC_MFCRF,
  CPUINFO_FEATURE_PPC_POPCNTB,
  CPUINFO_FEATURE_PPC_FRIZ,
  CPUINFO_FEATURE_PPC_MFPGPR,
  CPUINFO_FEATURE_PPC_MAX,
  CPUINFO_FEATURE_PPC_GPOPT		= CPUINFO_FEATURE_PPC_FSQRT,
  CPUINFO_FEATURE_PPC_GFXOPT	= CPUINFO_FEATURE_PPC_FSEL,
  CPUINFO_FEATURE_PPC_FPRND		= CPUINFO_FEATURE_PPC_FRIZ,

  CPUINFO_FEATURE_MIPS	= CPUINFO_CLASS('M'),
  CPUINFO_FEATURE_MIPS_MAX,

  CPUINFO_FEATURE_ARM	= CPUINFO_CLASS('a'),
  CPUINFO_FEATURE_ARM_BEGIN = CPUINFO_FEATURE_ARM,
  CPUINFO_FEATURE_ARM_SWP,
  CPUINFO_FEATURE_ARM_HALF,
  CPUINFO_FEATURE_ARM_THUMB,
  CPUINFO_FEATURE_ARM_26BIT,
  CPUINFO_FEATURE_ARM_FAST_MULT,
  CPUINFO_FEATURE_ARM_FPA,
  CPUINFO_FEATURE_ARM_VFP,
  CPUINFO_FEATURE_ARM_EDSP,
  CPUINFO_FEATURE_ARM_JAVA,
  CPUINFO_FEATURE_ARM_IWMMXT,
  CPUINFO_FEATURE_ARM_CRUNCH,
  CPUINFO_FEATURE_ARM_THUMBEE,
  CPUINFO_FEATURE_ARM_NEON,
  CPUINFO_FEATURE_ARM_VFPv3,
  CPUINFO_FEATURE_ARM_VFPv3D16,
  CPUINFO_FEATURE_ARM_TLS,
  CPUINFO_FEATURE_ARM_VFPv4,
  CPUINFO_FEATURE_ARM_IDIVA,
  CPUINFO_FEATURE_ARM_IDIVT,
  CPUINFO_FEATURE_ARM_VFPD32,
  CPUINFO_FEATURE_ARM_LPAE,
  CPUINFO_FEATURE_ARM_EVTSTRM,
  CPUINFO_FEATURE_ARM_IDIV,
  CPUINFO_FEATURE_ARM_MAX,
  
#define CPUINFO_FEATURE_CONTINUE(a, b, c) CPUINFO_FEATURE_ ## b ## _BEGIN = (CPUINFO_FEATURE_ ## b|((CPUINFO_FEATURE_ ## a ## _MAX)&CPUINFO_FEATURE_MASK)), \
  CPUINFO_FEATURE_ ## b ## _ ## c = CPUINFO_FEATURE_ ## b ## _BEGIN +1
  CPUINFO_FEATURE_AARCH64 = CPUINFO_CLASS('b'),
  CPUINFO_FEATURE_CONTINUE(ARM, AARCH64, FP),
  CPUINFO_FEATURE_AARCH64_ASIMD,
  CPUINFO_FEATURE_AARCH64_EVTSTRM,
  CPUINFO_FEATURE_AARCH64_CRYPTO_AES,
  CPUINFO_FEATURE_AARCH64_CRYPTO_PMULL,
  CPUINFO_FEATURE_AARCH64_CRYPTO_SHA1,
  CPUINFO_FEATURE_AARCH64_CRYPTO_SHA2,
  CPUINFO_FEATURE_AARCH64_CRYPTO_CRC32,
  CPUINFO_FEATURE_AARCH64_ATOMICS,
  CPUINFO_FEATURE_AARCH64_FPHP,
  CPUINFO_FEATURE_AARCH64_ASIMDHP,
  CPUINFO_FEATURE_AARCH64_CPUID,
  CPUINFO_FEATURE_AARCH64_ASIMDRDM,
  CPUINFO_FEATURE_AARCH64_JSCVT,
  CPUINFO_FEATURE_AARCH64_FCMA,
  CPUINFO_FEATURE_AARCH64_LRCPC,
  CPUINFO_FEATURE_AARCH64_MAX,

  CPUINFO_FEATURE_ARM_CRYPTO = CPUINFO_CLASS('c'),
  CPUINFO_FEATURE_CONTINUE(AARCH64, ARM_CRYPTO, AES),
  CPUINFO_FEATURE_ARM_CRYPTO_PMULL,
  CPUINFO_FEATURE_ARM_CRYPTO_SHA1,
  CPUINFO_FEATURE_ARM_CRYPTO_SHA2,
  CPUINFO_FEATURE_ARM_CRYPTO_CRC32,
  CPUINFO_FEATURE_ARM_CRYPTO_MAX
} cpuinfo_feature_t;

extern cpuinfo_feature_t cpuinfo_feature_common,
			 cpuinfo_feature_common_max,
			 cpuinfo_feature_architecture,
			 cpuinfo_feature_architecture_max;

// Returns 1 if CPU supports the specified feature
extern int cpuinfo_has_feature(cpuinfo_t *cip, int feature);

// Utility functions to convert IDs
extern const char *cpuinfo_string_of_vendor(int vendor);
extern const char *cpuinfo_string_of_socket(int socket);
extern const char *cpuinfo_string_of_cache_type(int cache_type);
extern const char *cpuinfo_string_of_feature(int feature);
extern const char *cpuinfo_string_of_feature_detail(int feature);

#ifdef __cplusplus
}
#endif

#endif /* CPUINFO_H */
