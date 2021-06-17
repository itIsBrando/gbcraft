#include "sound.h"

// **ALL CONSTANTS FROM TONC**

// Rates for traditional notes in octave +5
const uint __snd_rates[12]=
{
    8013, 7566, 7144, 6742, // C , C#, D , D#
    6362, 6005, 5666, 5346, // E , F , F#, G
    5048, 4766, 4499, 4246  // G#, A , A#, B
};

/**
 * @param note see above for note [0, 12)
 * @param oct [-2, 6]
 */
#define SND_RATE(note, oct) ( 2048-(__snd_rates[note]>>(4+(oct))) )


#define SND_ENABLE() REG_SND_STATUS = (1 << 7)

const snd_square_t SOUND_EFFECT_PUNCH = {
    .sweep=SND_SWEEP_MAKE(1, SND_SWEEP_DEC, 0),
    // plays D  
    .freq=SND_FREQ_MAKE(SND_RATE(2, 2), SND_FREQ_TIMED),
    .control=SND_SQU_CNT_MAKE(28, SND_SQR_DUTY_75, 1, SND_SQR_DEC, 10),
};


void snd_play_square(const snd_square_t *snd)
{
    SND_ENABLE();
    REG_MST_DMG_CONTROL = SND_DMG_CONTROL_MAKE(7, 7, SND_DMG_LEFT_SQR1 | SND_DMG_RIGHT_SQR1);
    
    REG_MST_DS_CONTROL = SND_DMG_100;

    REG_1_SWEEP = snd->sweep;
    REG_1_FREQ = snd->freq | SND_FREQ_RESET;
    REG_1_CNT = snd->control;
}