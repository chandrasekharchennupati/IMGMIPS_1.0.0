/**
 * Copyright (c) 2015 Runtime Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef H_BSP_H
#define H_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* LED pins */
#define LED_BLINK_PIN   (17)

/* UART info */
#define CONSOLE_UART    0

/* Declaration of "non-zeroed" bss */
#define nzbss_t   __attribute__((section(".nzbss")))

#ifdef __cplusplus
}
#endif

#endif  /* H_BSP_H */
