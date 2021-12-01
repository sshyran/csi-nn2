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

#include "csi_gref.h"

int csi_gref_proposal(struct csi_tensor *cls_prob,
                      struct csi_tensor *bbox_pred,
                      struct csi_tensor *im_info,
                      struct csi_tensor *output,
                      struct proposal_params *params)
{
    csi_debug_error("csi_gref_proposal unsupport\n");
    return CSINN_FALSE;
}
