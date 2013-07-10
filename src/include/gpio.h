/*
 * Copyright 2012, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Google Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GPIO_H
#define GPIO_H

enum gpio_types {
	GPIO_IN,
	GPIO_OUT,
	GPIO_ALT,		/* catch-all for alternate functions */
};

/*
 * Many-value logic (3 states). This can be used for inputs whereby presence
 * of external pull-up or pull-down resistors can be added to overcome internal
 * pull-ups/pull-downs and force a single value.
 *
 * Thus, external pull resistors can force a 0 or 1 and if the value changes
 * along with internal pull-up/down enable then the input is floating.
 *
 *     Vpd | Vpu | MVL
 *    -----------------
 *      0  |  0  | 0
 *    -----------------
 *      0  |  1  | Z    <-- floating input will follow internal pull up/down
 *    -----------------
 *      1  |  1  | 1
 */
enum mvl3 {
	LOGIC_0,
	LOGIC_1,
	LOGIC_Z,		/* high impedance / tri-stated / floating */
};

#endif	/* GPIO_H */
