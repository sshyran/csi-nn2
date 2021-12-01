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

#include "csi_ovx.h"
#include "vsi_nn_pub.h"

int csi_ovx_softmax(struct csi_tensor *input,
                    struct csi_tensor *output,
                    struct softmax_params *params)
{
    vsi_nn_node_t *node;
    vsi_nn_node_id_t node_id;
    vsi_nn_tensor_id_t input_id;
    vsi_nn_tensor_id_t output_id;
    vsi_nn_tensor_attr_t attr;
    vsi_nn_graph_t *graph = csi_ovx_get_graph(input->sess);
    output->sess = input->sess;
    uint32_t input_num = 1;
    uint32_t output_num = 1;
    node = vsi_nn_AddNode(graph, VSI_NN_OP_SOFTMAX, input_num, output_num, &node_id);
    node->nn_param.softmax.beta = 1.0;
    node->nn_param.softmax.axis = input->dim_count - 1 - params->axis;

    attr.dtype.fmt = VSI_NN_DIM_FMT_NCHW;

    /* input */
    node->input.tensors[0] = (vsi_nn_tensor_id_t)input->data;

    /* output */
    memset(attr.size, 0, VSI_NN_MAX_DIM_NUM * sizeof(uint32_t));
    attr.dim_num = output->dim_count;
    for (int i = 0; i < output->dim_count; i++) {
        attr.size[i] = output->dim[output->dim_count-i-1];
    }
    attr.dtype.scale = output->qinfo->scale;
    attr.dtype.zero_point = output->qinfo->zero_point;
    attr.dtype.qnt_type = VSI_NN_QNT_TYPE_AFFINE_ASYMMETRIC;
    attr.vtl = FALSE;
    attr.is_const = FALSE;
    attr.dtype.vx_type = VSI_NN_TYPE_UINT8;
    output_id = vsi_nn_AddTensor(graph, VSI_NN_TENSOR_ID_AUTO, &attr, NULL);
    node->output.tensors[0] = output_id;
    output->data = (void *)output_id;
}
