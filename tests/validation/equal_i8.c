/*
 * Copyright (C) 2016-2021 C-SKY Limited. All rights reserved.
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

#include "test_utils.h"
#include "csi_nn.h"
#include "math_snr.h"

int main(int argc, char** argv)
{
    init_testsuite("Testing function of equal i8.\n");

    struct csi_tensor *input0 = csi_alloc_tensor(NULL);
    struct csi_tensor *input1 = csi_alloc_tensor(NULL);
    struct csi_tensor *output = csi_alloc_tensor(NULL);
    struct csi_tensor *reference = csi_alloc_tensor(NULL);
    struct diso_params params;
    int in_size = 1;
    int out_size = 1;
    int zp, quantized_multiplier, shift;
    float scale, min_value, max_value;
    float error[2] = {0};
    float max_error;

    int *buffer = read_input_data_f32(argv[1]);
    input0->dim_count = input1->dim_count = buffer[0];
    output->dim_count = input0->dim_count;

    for (int i = 0; i < input0->dim_count; i++ ) {
        input0->dim[i] = buffer[1 + i];
        input1->dim[i] = input0->dim[i];
        output->dim[i] = input0->dim[i];
        in_size = in_size * input0->dim[i];
    }

    out_size = in_size;

    input0->dtype = CSINN_DTYPE_INT8;
    output->dtype = CSINN_DTYPE_INT8;
    params.base.api = CSINN_API;
    params.base.run_mode = CSINN_RM_LAYER;


    int8_t *input_tmp0 = malloc(in_size * sizeof(char));
    int8_t *input_tmp1 = malloc(in_size * sizeof(char));
    float   *src_in0   = (float *)(buffer + 1 + input0->dim_count);
    float   *src_in1   = (float *)(buffer + 1 + input0->dim_count + in_size);
    float   *ref       = (float *)(buffer + 1 + input0->dim_count + 2*in_size);

    input0->qinfo = get_quant_info_i8(src_in0, in_size);

    for(int i = 0; i < in_size; i++) {
        input_tmp0[i] = csi_ref_quantize_f32_to_i8(src_in0[i], input0->qinfo);
    }
    /* compute the max quantize error */
    for(int i = 0; i < in_size; i++) {
        float error1;
        float output_tmp  = csi_ref_dequantize_i8_to_f32(input_tmp0[i], input0->qinfo);
        if(src_in0[i] == INFINITY && output_tmp == INFINITY || src_in0[i] == NAN && output_tmp == NAN){
            continue;
        } else {
            error1 = fabs(src_in0[i] - output_tmp);
            if(error1 > 1e-6) {
                error1 = fabs(src_in0[i] - output_tmp)/fabs(src_in0[i] + 1e-9);
            }
        }
        if(error1 > error[0]) {
            error[0] = error1;
        }
    }

    input1->qinfo = get_quant_info_i8(src_in1, in_size);

    for(int i = 0; i < in_size; i++) {
        input_tmp1[i] = csi_ref_quantize_f32_to_i8(src_in1[i], input1->qinfo);
    }

    /* compute the max quantize error */
    for(int i = 0; i < in_size; i++) {
        float error1;
        float output_tmp  = csi_ref_dequantize_i8_to_f32(input_tmp1[i], input1->qinfo);
        if(src_in1[i] == INFINITY && output_tmp == INFINITY || input_tmp1[i] == NAN && output_tmp == NAN){
            continue;
        } else {
            error1 = fabs(src_in1[i] - output_tmp);
            if(error1 > 1e-6) {
                error1 = fabs(src_in1[i] - output_tmp)/fabs(src_in1[i] + 1e-9);
            }
        }
        if(error1 > error[1]) {
            error[1] = error1;
        }
    }

    max_error = (error[0] + error[1]);

    output->qinfo = get_quant_info_i8(ref, out_size);

    input0->data     = input_tmp0;
    input1->data       = input_tmp1;
    reference->data = ref;
    output->data    = malloc(out_size * sizeof(char));


    float difference = argc > 2 ? atof(argv[2]) : max_error;

    if (csi_equal_init(input0, input1, output, &params) == CSINN_TRUE) {
        csi_equal(input0, input1, output, &params);
    }

    result_verify_8(reference->data, output, input0->data, difference, out_size, false);

    free(buffer);
    free(input_tmp0);
    free(input_tmp1);
    free(output->data);
    return done_testing();
}
