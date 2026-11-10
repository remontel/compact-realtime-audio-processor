/*
 * audio_i2s.h
 *
 *  Created on: Nov 9, 2026
 *      Author: zapie
 */

#ifndef INC_AUDIO_I2S_H_
#define INC_AUDIO_I2S_H_

#include "main.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_I2S_MAX_BLOCK_SIZE 256U
#define AUDIO_I2S_CHANNELS       2U
#define AUDIO_I2S_DMA_HALFWORDS_PER_FRAME 4U
#define AUDIO_I2S_DMA_BUFFER_HALFWORDS \
    (AUDIO_I2S_MAX_BLOCK_SIZE * AUDIO_I2S_DMA_HALFWORDS_PER_FRAME * 2U)

typedef struct
{
    float *left;
    float *right;
    size_t size;
} AudioI2S_Input_t;

typedef struct
{
    float *left;
    float *right;
    size_t size;
} AudioI2S_Output_t;

typedef void (*AudioI2S_Callback_t)(const AudioI2S_Input_t *input,
                                    AudioI2S_Output_t *output,
                                    size_t size);

typedef struct
{
    I2S_HandleTypeDef *hi2s;
    uint32_t sample_rate;
    uint32_t block_size;
    AudioI2S_Callback_t callback;
    volatile uint32_t running;
    volatile uint32_t error_count;
} AudioI2S_Handle_t;

HAL_StatusTypeDef AudioI2S_Init(AudioI2S_Handle_t *audio,
                                I2S_HandleTypeDef *hi2s,
                                uint32_t sample_rate,
                                uint32_t block_size);

HAL_StatusTypeDef AudioI2S_Start(AudioI2S_Handle_t *audio,
                                 AudioI2S_Callback_t callback);

HAL_StatusTypeDef AudioI2S_Stop(AudioI2S_Handle_t *audio);

uint32_t AudioI2S_IsRunning(const AudioI2S_Handle_t *audio);
uint32_t AudioI2S_GetSampleRate(const AudioI2S_Handle_t *audio);
uint32_t AudioI2S_GetBlockSize(const AudioI2S_Handle_t *audio);
uint32_t AudioI2S_GetErrorCount(const AudioI2S_Handle_t *audio);

/* Connect these to HAL's global I2S callbacks in audio_i2s.c. */
void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s);

#ifdef __cplusplus
}
#endif



#endif /* INC_AUDIO_I2S_H_ */
