/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * @file x86/timer.c
 * x86 specific timer routines
 */

#include <libpayload.h>
#include <arch/rdtsc.h>
#include <arch/cpuid.h>
#include <arch/msr.h>

#define MSR_PLATFORM_INFO		0xce

/**
 * @ingroup arch
 * Global variable containing the speed of the processor in KHz.
 */
uint32_t cpu_khz;

/**
 * @brief Measure the speed of the processor for use in delays
 *
 * @return The CPU speed in kHz.
 */
static unsigned int calibrate_pit(void)
{
	unsigned long long start, end;
	const uint32_t clock_rate = 1193182; // 1.193182 MHz
	const uint16_t interval = (2 * clock_rate) / 1000; // 2 ms

	/* Set up the PPC port - disable the speaker, enable the T2 gate. */
	outb((inb(0x61) & ~0x02) | 0x01, 0x61);

	/* Set the PIT to Mode 0, counter 2, word access. */
	outb(0xB0, 0x43);

	/* Load the interval into the counter. */
	outb(interval & 0xff, 0x42);
	outb((interval >> 8) & 0xff, 0x42);

	/* Read the number of ticks during the period. */
	start = rdtsc();
	while (!(inb(0x61) & 0x20)) ;
	end = rdtsc();

	/*
	 * The number of milliseconds for a period is
	 * clock_rate / (interval * 1000). Multiply that by the number of
	 * measured clocks to get the kHz value.
	 */
	return (end - start) * clock_rate / (1000 * interval);
}

/**
 * @brief Calculates the core clock frequency via CPUID 0x15
 *
 * Newer Intel CPUs report their core clock in CPUID leaf 0x15. Early models
 * supporting this leaf didn't provide the nominal crystal frequency in ecx,
 * hence we use hard coded values for them.
 */
static int get_cpu_khz_xtal(void)
{
	uint32_t ecx, edx, num, denom;
	uint64_t nominal;

	if (cpuid_max() < 0x15)
		return -1;
	cpuid(0x15, denom, num, ecx, edx);

	if (denom == 0 || num == 0)
		return -1;

	if (ecx != 0) {
		nominal = ecx;
	} else {
		if (cpuid_family() != 6)
			return -1;

		switch (cpuid_model()) {
		case SKYLAKE_U_Y:
		case SKYLAKE_S_H:
		case KABYLAKE_U_Y:
		case KABYLAKE_S_H:
			nominal = 24000000;
			break;
		case APOLLOLAKE:
			nominal = 19200000;
			break;
		default:
			return -1;
		}
	}

	return nominal * num / denom / 1000;
}

/**
 * @brief Returns three times the bus clock in kHz
 *
 * The result of calculations with the returned value shall be divided by 3.
 * This helps to avoid rounding errors.
 */
static int get_bus_khz_x3(void)
{
	if (cpuid_family() != 6)
		return -1;

	switch (cpuid_model()) {
	case NEHALEM:
		return 400 * 1000; /* 133 MHz */
	case SANDYBRIDGE:
	case IVYBRIDGE:
	case HASWELL:
	case HASWELL_U:
	case HASWELL_GT3E:
	case BROADWELL:
	case BROADWELL_U:
		return 300 * 1000; /* 100 MHz */
	default:
		return -1;
	}
}

/**
 * @brief Returns the calculated CPU frequency
 *
 * Over the years, multiple ways to discover the CPU frequency have been
 * exposed through CPUID and MSRs. Try the most recent and accurate first
 * (crystal information in CPUID leaf 0x15) and then fall back to older
 * methods.
 *
 * This should cover all Intel Core i processors at least. For older
 * processors we fall back to the PIT calibration.
 */
static int get_cpu_khz_fast(void)
{
	/* Try core crystal clock frequency first (supposed to be more accurate). */
	const int cpu_khz_xtal = get_cpu_khz_xtal();
	if (cpu_khz_xtal > 0)
		return cpu_khz_xtal;

	/* Try `bus clock * speedstep multiplier`. */
	const int bus_x3 = get_bus_khz_x3();
	if (bus_x3 <= 0)
		return -1;
	/*
	 * Systems with an invariant TSC report the multiplier (maximum
	 * non-turbo ratio) in MSR_PLATFORM_INFO[15:8].
	 */
	const unsigned int mult = _rdmsr(MSR_PLATFORM_INFO) >> 8 & 0xff;
	return bus_x3 * mult / 3;
}

unsigned int get_cpu_speed(void)
{
	const int cpu_khz_fast = get_cpu_khz_fast();
	if (cpu_khz_fast > 0)
		cpu_khz = (unsigned int)cpu_khz_fast;
	else
		cpu_khz = calibrate_pit();

	return cpu_khz;
}
