#include "audio_i2s.h"
#include "core_cm7.h"
#include <string.h>

/* The buffers are deliberately static and cache-line aligned. They must not
   be placed in DTCM because DMA cannot access DTCM on STM32H7. */
static uint16_t tx_dma[AUDIO_I2S_DMA_BUFFER_HALFWORDS]
    __attribute__((aligned(32), section(".audio_dma")));
static uint16_t rx_dma[AUDIO_I2S_DMA_BUFFER_HALFWORDS]
    __attribute__((aligned(32), section(".audio_dma")));

static float input_left[AUDIO_I2S_MAX_BLOCK_SIZE] __attribute__((aligned(32)));
static float input_right[AUDIO_I2S_MAX_BLOCK_SIZE] __attribute__((aligned(32)));
static float output_left[AUDIO_I2S_MAX_BLOCK_SIZE] __attribute__((aligned(32)));
static float output_right[AUDIO_I2S_MAX_BLOCK_SIZE] __attribute__((aligned(32)));

static AudioI2S_Handle_t *active_audio;

static int32_t unpack_24bit(const uint16_t *p)
{
    /* STM32 HAL represents 24-bit I2S data left-justified in a 32-bit slot. */
    uint32_t word = ((uint32_t)p[0] << 16U) | p[1];
    return ((int32_t)word) >> 8U;
}

static void pack_24bit(uint16_t *p, float sample)
{
    if (sample > 1.0f)  sample = 1.0f;
    if (sample < -1.0f) sample = -1.0f;

    int32_t value = (int32_t)(sample * 8388607.0f);
    uint32_t word = ((uint32_t)value) << 8U;

    p[0] = (uint16_t)(word >> 16U);
    p[1] = (uint16_t)word;
}

static void process_block(AudioI2S_Handle_t *audio, uint32_t offset)
{
    const uint32_t frames = audio->block_size;
    const uint32_t halfwords_per_block = frames * AUDIO_I2S_DMA_HALFWORDS_PER_FRAME;
    uint16_t *rx = &rx_dma[offset * halfwords_per_block];
    uint16_t *tx = &tx_dma[offset * halfwords_per_block];

    SCB_InvalidateDCache_by_Addr((uint32_t *)rx,
                                  (int32_t)(halfwords_per_block * sizeof(uint16_t)));

    for (uint32_t n = 0; n < frames; n++)
    {
        const uint32_t adc_frame = n * 4U;
        input_left[n]  = (float)unpack_24bit(&rx[adc_frame + 0U]) / 8388608.0f;
        input_right[n] = (float)unpack_24bit(&rx[adc_frame + 2U]) / 8388608.0f;
        output_left[n] = input_left[n];
        output_right[n] = input_right[n];
    }

    AudioI2S_Input_t input = { input_left, input_right, frames };
    AudioI2S_Output_t output = { output_left, output_right, frames };

    if (audio->callback != NULL)
    {
        audio->callback(&input, &output, frames);
    }

    for (uint32_t n = 0; n < frames; n++)
    {
        const uint32_t dac_frame = n * 4U;
        pack_24bit(&tx[dac_frame + 0U], output.left[n]);
        pack_24bit(&tx[dac_frame + 2U], output.right[n]);
    }

    SCB_CleanDCache_by_Addr((uint32_t *)tx,
                            (int32_t)(halfwords_per_block * sizeof(uint16_t)));
}

HAL_StatusTypeDef AudioI2S_Init(AudioI2S_Handle_t *audio,
                                I2S_HandleTypeDef *hi2s,
                                uint32_t sample_rate,
                                uint32_t block_size)
{
    if ((audio == NULL) || (hi2s == NULL) ||
        (block_size == 0U) || (block_size > AUDIO_I2S_MAX_BLOCK_SIZE))
    {
        return HAL_ERROR;
    }

    memset(audio, 0, sizeof(*audio));
    audio->hi2s = hi2s;
    audio->sample_rate = sample_rate;
    audio->block_size = block_size;

    memset(tx_dma, 0, sizeof(tx_dma));
    memset(rx_dma, 0, sizeof(rx_dma));
    SCB_CleanDCache_by_Addr((uint32_t *)tx_dma, sizeof(tx_dma));
    SCB_CleanDCache_by_Addr((uint32_t *)rx_dma, sizeof(rx_dma));

    return HAL_OK;
}

HAL_StatusTypeDef AudioI2S_Start(AudioI2S_Handle_t *audio,
                                 AudioI2S_Callback_t callback)
{
    if ((audio == NULL) || (audio->hi2s == NULL) || (callback == NULL))
    {
        return HAL_ERROR;
    }

    audio->callback = callback;
    active_audio = audio;

    const uint16_t dma_halfwords =
        (uint16_t)(audio->block_size * AUDIO_I2S_DMA_HALFWORDS_PER_FRAME * 2U);

    HAL_StatusTypeDef status = HAL_I2SEx_TransmitReceive_DMA(
        audio->hi2s, tx_dma, rx_dma, dma_halfwords);

    audio->running = (status == HAL_OK) ? 1U : 0U;
    return status;
}

HAL_StatusTypeDef AudioI2S_Stop(AudioI2S_Handle_t *audio)
{
    if ((audio == NULL) || (audio->hi2s == NULL))
    {
        return HAL_ERROR;
    }

    audio->running = 0U;
    if (active_audio == audio)
    {
        active_audio = NULL;
    }
    return HAL_I2S_DMAStop(audio->hi2s);
}

uint32_t AudioI2S_IsRunning(const AudioI2S_Handle_t *audio)
{
    return (audio != NULL) ? audio->running : 0U;
}

uint32_t AudioI2S_GetSampleRate(const AudioI2S_Handle_t *audio)
{
    return (audio != NULL) ? audio->sample_rate : 0U;
}

uint32_t AudioI2S_GetBlockSize(const AudioI2S_Handle_t *audio)
{
    return (audio != NULL) ? audio->block_size : 0U;
}

uint32_t AudioI2S_GetErrorCount(const AudioI2S_Handle_t *audio)
{
    return (audio != NULL) ? audio->error_count : 0U;
}

void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if ((active_audio != NULL) && (active_audio->hi2s == hi2s))
    {
        process_block(active_audio, 0U);
    }
}

void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if ((active_audio != NULL) && (active_audio->hi2s == hi2s))
    {
        process_block(active_audio, 1U);
    }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
    if ((active_audio != NULL) && (active_audio->hi2s == hi2s))
    {
        active_audio->error_count++;
    }
}
