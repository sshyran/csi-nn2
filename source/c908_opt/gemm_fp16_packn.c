/*
 * Copyright (C) 2016-2022 T-Head Semiconductor Co., Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* CSI-NN2 version 2.0.x */

#include "shl_c908.h"

void gemm_fp16_ncxhwx_12xpack2n(__fp16 *output, const __fp16 *kernel, const __fp16 *input,
                                const __fp16 *bias, int m, int k, int n, bool fuse_relu);
void gemm_fp16_ncxhwx_12xpackn(__fp16 *output, const __fp16 *kernel, const __fp16 *input,
                               const __fp16 *bias, int m, int k, int n, bool fuse_relu);

void shl_c908_ncxhwx_gemm_12xpack2n_fp16(__fp16 *dst, const __fp16 *sa, const __fp16 *sb,
                                         const __fp16 *bias, int m, int k, int n, bool fuse_relu)
{
    const int packn = csrr_vlenb() / sizeof(__fp16);
    const int pack2n = packn * 2;

    int oc = 0;
    for (; oc + pack2n - 1 < m; oc += pack2n) {
        gemm_fp16_ncxhwx_12xpack2n(dst, sa, sb, bias, packn, k, n, fuse_relu);
        sa += pack2n * k;
        dst += pack2n * n;
        if (bias) {
            bias += pack2n;
        }
    }
    for (; oc + packn - 1 < m; oc += packn) {
        gemm_fp16_ncxhwx_12xpackn(dst, sa, sb, bias, packn, k, n, fuse_relu);
        sa += packn * k;
        dst += packn * n;
        if (bias) {
            bias += packn;
        }
    }
    if (oc < m) {
        gemm_fp16_ncxhwx_12xpackn(dst, sa, sb, bias, m - oc, k, n, fuse_relu);
    }
}