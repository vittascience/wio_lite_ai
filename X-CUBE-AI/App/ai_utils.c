
#include <ai_utils.h>
#include "ai_platform.h"
#include "network.h"
#include "network_data.h"
#include <stdio.h>
#include <math.h>

// void aiInit(void)
//{
//  ai_error err;
//
//  /* 1 - Create an instance of the model */
//  err = ai_network_create(&network, AI_NETWORK_DATA_CONFIG);
//  if (err.type != AI_ERROR_NONE) {
//    printf("ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
//    Error_Handler();
//  }
//
//  /* 2 - Initialize the instance */
//  const ai_network_params params = AI_NETWORK_PARAMS_INIT(
//      AI_NETWORK_DATA_WEIGHTS(ai_network_data_weights_get()),
//      AI_NETWORK_DATA_ACTIVATIONS(activations)
//  );
//
//  if (!ai_network_init(network, &params)) {
//    err = ai_network_get_error(network);
//    printf("ai_network_init error - type=%d code=%d\r\n", err.type, err.code);
//    Error_Handler();
//  }
//
//
//
//}

// void aiRun(ai_u8 *pIn, ai_u8 *pOut)
//{
//   ai_i32 batch;
//   ai_error err;
//
//   /* 1 - Create the AI buffer IO handlers with the default definition */
//   ai_buffer ai_input[AI_NETWORK_IN_NUM] =  AI_NETWORK_IN;
//   ai_buffer ai_output[AI_NETWORK_OUT_NUM] = AI_NETWORK_OUT;
//
//   /* 2 - Update IO handlers with the data payload */
//   ai_input[0].data = AI_HANDLE_PTR(pIn);
//   ai_output[0].data = AI_HANDLE_PTR(pOut);
//
//   batch = ai_network_run(network, ai_input, ai_output);
//   if (batch != 1) {
//     err = ai_network_get_error(network);
//     printf("AI ai_network_run error - type=%d code=%d\r\n", err.type, err.code);
//     Error_Handler();
//   }
// }

void UTILS_Bubblesort(float *prob, int *classes, int size)
{
  float p;
  int c;

  for (int i = 0; i < size; i++)
  {
    for (int ii = 0; ii < size - i - 1; ii++)
    {
      if (prob[ii] < prob[ii + 1])
      {
        p = prob[ii];
        prob[ii] = prob[ii + 1];
        prob[ii + 1] = p;
        c = classes[ii];
        classes[ii] = classes[ii + 1];
        classes[ii + 1] = c;
      }
    }
  }
}

/**
 * @brief Normalize output into probability distribution
 * @param nn_buff Pointer to the AI NN de-quantized buffer
 */
void AI_Softmax(float *nn_buff)
{
  // float *nn_buff = Ai_Context_Ptr->nn_output_buffer;
  float max = nn_buff[0];
  float sum = 0.0f;

  /* Compute maximum */
  for (uint32_t i = 0; i < AI_NET_OUTPUT_SIZE; i++)
  {
    max = (max > nn_buff[i]) ? max : nn_buff[i];
  }

  /* subtract maximum to avoid softmax saturation and calculate sum */
  for (uint32_t i = 0; i < AI_NET_OUTPUT_SIZE; i++)
  {
    nn_buff[i] = expf(nn_buff[i] - max);
    sum += nn_buff[i];
  }

  /* normalize probabilities multiplying by the inverse of sum */
  sum = 1.0f / sum;
  for (uint32_t i = 0; i < AI_NET_OUTPUT_SIZE; i++)
  {
    nn_buff[i] *= sum;
  }
}

///**
// * @brief Run post-processing operation
// * @param ai_output_buffer pointer to NN output
// */
//
// void postprocess(void* ai_output_buffer)
//{
//  int ranking[NN_OUTPUT_CLASS_NUMBER];
//
//  /**NN ouput dequantization if required**/
//  for(int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
//  {
//	int32_t q_value = (int32_t) *(((uint8_t *) ai_output_buffer) + i);
//
//	*(((float *)ai_output_buffer) + i) = 0.01627892255783081f * ((float) q_value - 113.0f);
//  }
//
//  /* Add missing softmax layer to get a normalized probability distribution */
//  AI_Softmax((float *)ai_output_buffer);
//
//  /**Perform ranking**/
//  for (int i = 0; i < NN_OUTPUT_CLASS_NUMBER; i++)
//  {
//   ranking[i] = i;
//  }
//
//  UTILS_Bubblesort((float*)(ai_output_buffer), ranking, NN_OUTPUT_CLASS_NUMBER);
//
//  sprintf(msg, "%s %.0f%%", output_labels[ranking[0]], 100* (*(float*)(ai_output_buffer)));
//
//
//  //sprintf(msg, "%.3f, %.3f, %.3f", ((float*)(ai_output_buffer))[0], ((float*)(ai_output_buffer))[1], ((float*)(ai_output_buffer))[2]);
//
//}
