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
    init_testsuite("Testing function of concat f32.\n");
    int in_size = 1;
    int out_size = 1;
    int *buffer = read_input_data_f32(argv[1]);

    struct csinn_concat_params *params =
        csinn_alloc_params(sizeof(struct csinn_concat_params), NULL);

    params->inputs_count = buffer[4];

    struct csinn_tensor *output = csinn_alloc_tensor(NULL);
    struct csinn_tensor *reference = csinn_alloc_tensor(NULL);
    struct csinn_tensor *input[params->inputs_count];

    for (int i = 0; i < params->inputs_count; i++) {
        input[i] = csinn_alloc_tensor(NULL);
    }

    params->axis = buffer[5];
    output->dim_count = 4;

    for (int i = 0; i < output->dim_count; i++) {
        if (i == params->axis) {
            output->dim[i] = params->inputs_count * buffer[i];
        } else {
            output->dim[i] = buffer[i];
        }
        out_size *= output->dim[i];
    }
    in_size = out_size / params->inputs_count;
    params->base.api = CSINN_API;

    for (int i = 0; i < params->inputs_count; i++) {
        input[i]->data = (float *)(buffer + 6 + in_size * i);
        input[i]->dim[0] = buffer[0];  // batch
        input[i]->dim[1] = buffer[1];  // height
        input[i]->dim[2] = buffer[2];  // width
        input[i]->dim[3] = buffer[3];  // channel
        input[i]->dim_count = 4;
        input[i]->dtype = CSINN_DTYPE_FLOAT32;
    }

    output->dtype = CSINN_DTYPE_FLOAT32;
    reference->data = (float *)(buffer + 6 + in_size * params->inputs_count);
    output->data = (float *)malloc(out_size * sizeof(float));
    float difference = argc > 2 ? atof(argv[2]) : 0.9;

    if (csinn_concat_init((struct csinn_tensor **)input, output, params) == CSINN_TRUE) {
        csinn_concat((struct csinn_tensor **)input, output, params);
    }

    result_verify_f32(reference->data, output->data, input[0]->data, difference, out_size, false);

    free(buffer);
    free(output->data);
    return done_testing();
}
