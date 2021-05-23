#ifndef SOUND_H
#define SOUND_H

#include "../defines.h"


#define _BASE 0x04000000
#define REG_1_SWEEP *((vu16*)(_BASE + 0x60))
#define REG_1_CNT   *((vu16*)(_BASE + 0x62))
#define REG_1_FREQ  *((vu32*)(_BASE + 0x64))

#define REG_MST_DMG_CONTROL *((vu16*)(_BASE + 0x80)) // master control register for DMG sound
#define REG_MST_DS_CONTROL *((vu16*)(_BASE + 0x82)) // master control register for DirectSound

#define REG_SND_STATUS *((vu16*)(_BASE + 0x84))

typedef enum {
    SND_LEFT_VOL_MASK  = 0x0007,
    SND_RIGHT_VOL_MASK = 0x00E0,
    
    SND_DMG_LEFT_SQR1  = 1 << 0x8,
    SND_DMG_LEFT_SQR2  = 1 << 0x9,
    SND_DMG_LEFT_WAVE  = 1 << 0xA,
    SND_DMG_LEFT_NOISE = 1 << 0xB,
    
    SND_DMG_RIGHT_SQR1  = 1 << 0xC,
    SND_DMG_RIGHT_SQR2  = 1 << 0xD,
    SND_DMG_RIGHT_WAVE  = 1 << 0xE,
    SND_DMG_RIGHT_NOISE = 1 << 0xF,
} snd_status_t;


/**
 * @param leftVol volume for left speaker [0-7]
 * @param modes enabled channels for each speaker. ex: SMD_DMG_LEFT_SQR1
 */
#define SND_DMG_CONTROL_MAKE(leftVol, rightVol, modes) (((leftVol) & SND_LEFT_VOL_MASK) | (((rightVol) << 5) & SND_RIGHT_VOL_MASK) | modes)


typedef enum {
    SND_DMG_25  = 0b00, // 25% volume
    SND_DMG_50  = 0b01, // 50% volume
    SND_DMG_100 = 0b10, // 100% volume
    
    SND_A_50  = 0x0000,
    SND_A_100 = 0x0004, // Direct Sound A volume ratio 100%
    
    SND_B_50  = 0x0000,
    SND_B_100 = 0x0008, // Direct Sound B volume ratio 100%
    
    SND_A_ENABLE_RIGHT = 1 << 0x8, // Enable DS A on right
    SND_A_ENABLE_LEFT  = 1 << 0x9, // Enable DS A on left
    
    SND_A_TIMER = 1 << 0xA, // Use timer 1 for DS A, otherwise timer 0
    
    SND_A_RESET = 1 << 0xB, // "When using DMA for Direct sound, this will cause DMA to reset the FIFO buffer after it's used."
    
    SND_B_ENABLE_RIGHT = 1 << 0xC, // Enable DS B on right
    SND_B_ENABLE_LEFT  = 1 << 0xD, // Enable DS B on left
    
    SND_B_TIMER = 1 << 0xE, // Use timer 1 for DS B, otherwise timer 0
    
    SND_B_RESET = 1 << 0xF, // "When using DMA for Direct sound, this will cause DMA to reset the FIFO buffer after it's used."
    

} snd_DirectSound_control_t;

// rate function for REG_SND_1_FREQ
// R(f) = 2048 - (2^17) / f


// for REG_SND_x_CNT, where x = 1 or 2
typedef enum {
    SND_SQR_LEN_MASK = 0x001F,
    
    // duty cycle (in percent)
    SND_SQR_DUTY_12_5   = 0b00 << 6,
    SND_SQR_DUTY_25     = 0b01 << 6,
    SND_SQR_DUTY_50     = 0b10 << 6,
    SND_SQR_DUTY_75     = 0b11 << 6,
    
    SND_SQR_STEP_TIME_MASK = 0x0700, // step-time for envelope changes
    SND_SQR_DEC = 0 << 0xb, // default, decrease envelope
    SND_SQR_INC = 1 << 0xb, // default, increase envelope
    
    SND_SQR_ENV_VOL_MASK = 0xF000, // initial envelope volume
} snd_square_control_t;


// for REG_SND_x_FREQ, where x = 1 or 2
typedef enum {
    SND_FREQ_RATE_MASK = 0x07FF, // rate value (**WRITE ONLY**)
    
    SND_FREQ_FOREVER    = 0 << 0xE, // play sound forever
    SND_FREQ_TIMED      = 1 << 0xE, // play until length (SND_SQR_LEN_MASK)
    
    SND_FREQ_RESET      = 1 << 0xF, // resets sound to initial volume and sweep
} snd_square_frequency_t;


typedef enum {
    SND_SWEEP_SHIFT_MASK = 0x0003, // sweep *number* not number of sweeps
    SND_SWEEP_INC = 0 << 3, // increase sweep
    SND_SWEEP_DEC = 1 << 3, // decrease sweep
    SND_SWEEP_TIME_MASK = 0x0030, // measured in 128Hz (~7.8ms * n)
} snd_sweep_t;

/**
 * @param shiftNum idk, check TONC [0-3].
 * @param mode SND_SWEEP_INC/SND_SWEEP_DEC
 * @param time sweep time [0-3]
 */
#define SND_SWEEP_MAKE(shiftNum, mode, time) ((shiftNum & SND_SWEEP_SHIFT_MASK) | (mode) | ((time << 4) & SND_SWEEP_TIME_MASK))

/**
 * @param rate idek [0-0x7FFF]
 * @param mode SND_FREQ_TIMED/SND_FREQ_FOREVER
 */
#define SND_FREQ_MAKE(rate, mode) ((rate & SND_FREQ_RATE_MASK) | (mode))


/**
 * @param len length of sound [0-0x1F]
 * @param duty duty cycles in pecentage: 12_5, 25, 50, 75
 * @param stepTime step time for envelope [0-7]
 * @param mode INC/DEC
 * @param volume volume of sound [0-0xF]
 */
#define SND_SQU_CNT_MAKE(len, duty, stepTime, mode, volume) ((len & SND_SQR_LEN_MASK) | (duty) | (((stepTime) << 8) & SND_SQR_STEP_TIME_MASK) | (mode) | (((volume) << 12) & SND_SQR_ENV_VOL_MASK) )


typedef struct {
    snd_sweep_t sweep;
    snd_square_frequency_t freq;
    snd_square_control_t control;
} snd_square_t;


extern const snd_square_t SOUND_EFFECT_PUNCH;

void snd_play_square(const snd_square_t *snd);


#endif