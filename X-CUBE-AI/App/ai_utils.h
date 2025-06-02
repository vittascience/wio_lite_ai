#ifndef __AI_INFERENCE_H__
#define __AI_INFERENCE_H__

#include "ai_platform.h"

#define AI_NET_OUTPUT_SIZE AI_NETWORK_OUT_1_SIZE
#define NN_OUTPUT_CLASS_NUMBER AI_NET_OUTPUT_SIZE

#define AI_NET_INPUT_SIZE_BYTES AI_NETWORK_IN_1_SIZE_BYTES

#define AI_NET_OUTPUT_SIZE AI_NETWORK_OUT_1_SIZE
#define AI_NET_OUTPUT_SIZE_BYTES AI_NETWORK_OUT_1_SIZE_BYTES

#define AI_ACTIVATION_SIZE_BYTES AI_NETWORK_DATA_ACTIVATIONS_SIZE
#define AI_WEIGHT_SIZE_BYTES AI_NETWORK_DATA_WEIGHTS_SIZE

#define AI_NETWORK_IN_SHIFT 1
#define AI_NETWORK_OUT_SHIFT 7

#define AI_NETWORK_WIDTH AI_NETWORK_IN_1_WIDTH
#define AI_NETWORK_HEIGHT AI_NETWORK_IN_1_HEIGHT

// void aiInit(void);
// void aiRun(ai_u8 *pIn, ai_u8 *pOut);
void UTILS_Bubblesort(float *prob, int *classes, int size);
void AI_Softmax(float *nn_buff);
void AI_Output_Dequantize(void *ai_output_buffer);
// void postprocess(void* ai_output_buffer);

#endif /*__AI_INFERENCE_H__*/
