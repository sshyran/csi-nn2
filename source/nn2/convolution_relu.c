
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
#include "shl_utils.h"

int csinn_conv2d_relu_init(struct csinn_tensor *input, struct csinn_tensor *output,
                           struct csinn_tensor *kernel, struct csinn_tensor *bias,
                           struct csinn_conv2d_params *params)
{
    if (params->base.layout == CSINN_LAYOUT_NCHW) {
        if (params->group == 1) {
            shl_op_callback_map(&params->base, CSINN_OP_CONV2D_RELU, input->dtype);
        } else if (params->group == input->dim[1] && kernel->dim[1] == 1) {
            shl_op_callback_map(&params->base, CSINN_OP_DEPTHWISE_CONV2D_RELU, input->dtype);
        } else {
            shl_op_callback_map(&params->base, CSINN_OP_GROUP_CONV2D_RELU, input->dtype);
        }
    } else if (params->base.layout == CSINN_LAYOUT_NHWC) {
        if (params->group == 1) {
            shl_op_callback_map(&params->base, CSINN_OP_CONV2D_RELU, input->dtype);
        } else if (params->group == input->dim[3] && kernel->dim[0] == 1) {
            shl_op_callback_map(&params->base, CSINN_OP_DEPTHWISE_CONV2D_RELU, input->dtype);
        } else {
            shl_op_callback_map(&params->base, CSINN_OP_GROUP_CONV2D_RELU, input->dtype);
        }
    } else {
        return CSINN_UNSUPPORT_LAYOUT;
    }
    struct csinn_callback *cb = params->base.cb;
    int (*func)() = shl_get_init_cb(&params->base);
    if (func != NULL) {
        func(input, output, kernel, bias, params);
    }
    return CSINN_TRUE;
}

int csinn_conv2d_relu(struct csinn_tensor *input, struct csinn_tensor *output,
                      struct csinn_tensor *kernel, struct csinn_tensor *bias,
                      struct csinn_conv2d_params *params)
{
    SHL_DEBUG_CALL(shl_conv2d_debug_info(input, output, kernel, bias, params, __func__));
    int (*func)() = shl_get_p0_cb(&params->base);
    if (func != NULL) {
        func(input, output, kernel, bias, params);
    } else {
        return CSINN_CALLBACK_UNSET;
    }
    return CSINN_TRUE;
}
