/* ws_cpuid.h
 * Get the CPU info on x86 processors that support it
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Get CPU info on platforms where the cpuid instruction can be used skip 32 bit versions for GCC
 * Intel has documented the CPUID instruction in the "Intel(r) 64 and IA-32
 * Architectures Developer's Manual" at http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-2a-manual.html
 * the ws_cpuid() routine will return 0 if cpuinfo isn't available.
 */

#if defined(_MSC_VER)     /* MSVC */
static gboolean
ws_cpuid(guint32 *CPUInfo, guint32 selector)
{
	CPUInfo[0] = CPUInfo[1] = CPUInfo[2] = CPUInfo[3] = 0;
	__cpuid((int *) CPUInfo, selector);
	/* XXX, how to check if it's supported on MSVC? just in case clear all flags above */
	return TRUE;
}

#elif defined(__GNUC__)  /* GCC/clang */

#if defined(__x86_64__)
static inline gboolean
ws_cpuid(guint32 *CPUInfo, int selector)
{
	__asm__ __volatile__("cpuid"
						: "=a" (CPUInfo[0]),
							"=b" (CPUInfo[1]),
							"=c" (CPUInfo[2]),
							"=d" (CPUInfo[3])
						: "a"(selector));
	return TRUE;
}
#elif defined(__i386__)
static gboolean
ws_cpuid(guint32 *CPUInfo _U_, int selector _U_)
{
	/* TODO: need a test if older proccesors have the cpuid instruction */
	return FALSE;
}
#elif
static gboolean
ws_cpuid(guint32 *CPUInfo _U_, int selector _U_)
{
	/* Not x86, so no cpuid instruction */
	return FALSE;
}
#endif

#else /* Other compilers */

static gboolean
ws_cpuid(guint32 *CPUInfo _U_, int selector _U_)
{
	return FALSE;
}
#endif

static int
ws_cpuid_sse42(void)
{
	guint32 CPUInfo[4];

	if (!ws_cpuid(CPUInfo, 1))
		return 0;

	/* in ECX bit 20 toggled on */
	return (CPUInfo[2] & (1 << 20));
}
