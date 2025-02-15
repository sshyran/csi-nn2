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

#include "csi_nn.h"
#include "math_snr.h"
#include "test_utils.h"

int main(int argc, char **argv)
{
    init_testsuite("Testing function of matmul f32.\n");

    struct csinn_tensor *input0 = csinn_alloc_tensor(NULL);
    struct csinn_tensor *input1 = csinn_alloc_tensor(NULL);
    struct csinn_tensor *output = csinn_alloc_tensor(NULL);
    struct csinn_tensor *reference = csinn_alloc_tensor(NULL);
    struct csinn_matmul_params *params =
        csinn_alloc_params(sizeof(struct csinn_matmul_params), NULL);
    int in_size0, in_size1, out_size;

    int *buffer = read_input_data_f32(argv[1]);
    input0->dim_count = input1->dim_count = buffer[2];
    output->dim_count = input0->dim_count;
    params->trans_a = buffer[0];
    params->trans_b = buffer[1];
    for (int i = 0; i < input0->dim_count; ++i) {
        input0->dim[i] = buffer[3 + i];
        input1->dim[i] = buffer[3 + input0->dim_count + i];
        output->dim[i] = buffer[3 + 2 * input0->dim_count + i];
    }

    in_size0 = 1;
    for (int i = 0; i < input0->dim_count; ++i) {
        in_size0 *= input0->dim[i];
    }

    in_size1 = 1;
    for (int i = 0; i < input1->dim_count; ++i) {
        in_size1 *= input1->dim[i];
    }

    out_size = 1;
    for (int i = 0; i < output->dim_count; ++i) {
        out_size *= output->dim[i];
    }

    input0->dtype = CSINN_DTYPE_FLOAT32;
    input1->dtype = CSINN_DTYPE_FLOAT32;
    output->dtype = CSINN_DTYPE_FLOAT32;
    params->base.api = CSINN_API;

    input0->data = (float *)(buffer + 3 + 3 * input0->dim_count);
    input1->data = (float *)(buffer + 3 + 3 * input0->dim_count + in_size0);
    reference->data = (float *)(buffer + 3 + 3 * input0->dim_count + in_size0 + in_size1);
    output->data = malloc(out_size * sizeof(float));
    float difference = argc > 2 ? atof(argv[2]) : 0.9;

    if (csinn_matmul_init(input0, input1, output, params) == CSINN_TRUE) {
        csinn_matmul(input0, input1, output, params);
    }

    result_verify_f32(reference->data, output->data, input0->data, difference, out_size, false);

    free(buffer);
    free(output->data);
    return done_testing();
}
