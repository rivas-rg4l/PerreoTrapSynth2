#include "Presets.h"

//==============================================================================
// Referencia rapida de parametros (ids):
//
//  OSC A / B : aWave aOct aSemi aFine aLvl   |  bWave bOct bSemi bFine bLvl
//  Unison    : uniVoices (1-7) uniDetune (0-1) uniSpread (0-1)
//  Mix       : subLvl noiseLvl pw (ancho de pulso)
//  Filtro    : fltType cutoff reso fltEnv (-1..1)    fA fD fS fR
//  Amp       : aA aD aS aR
//  LFO       : lfoShape lfoDest lfoRate lfoDepth
//  Pitch env : pitEnv (semitonos) pitDec (s)
//  FX        : drvOn drive | choOn choRate choDepth choMix
//              dlyOn dlyTime dlyFb dlyMix | revOn revSize revMix
//  Master    : master
//==============================================================================
namespace
{
    // Ondas
    constexpr float SIN = 0, SAW = 1, SQR = 2, TRI = 3, PUL = 4;
    // Filtro
    constexpr float LP = 0, HP = 1, BP = 2, NOTCH = 3;
    // LFO
    constexpr float L_SIN = 0, L_TRI = 1, L_SAW = 2, L_SQR = 3, L_SH = 4;
    constexpr float D_OFF = 0, D_CUT = 1, D_PITCH = 2, D_AMP = 3, D_PW = 4;
}

const std::vector<PerreoPreset>& getPresetTable()
{
    static const std::vector<PerreoPreset> table = {

    //==========================================================================
    //  T R A P
    //==========================================================================

    // ---------------- PLUCKS ----------------
    { "Dark Bell Pluck", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",1}, {"bLvl",0.5f},
        {"cutoff",2500}, {"reso",1.2f}, {"fltEnv",0.5f}, {"fD",0.25f}, {"fS",0},
        {"aD",0.35f}, {"aS",0}, {"aR",0.35f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.25f} } },

    { "Ice Pluck", {
        {"aWave",SAW}, {"bWave",SQR}, {"bOct",1}, {"bLvl",0.35f},
        {"uniVoices",3}, {"uniDetune",0.15f},
        {"cutoff",1800}, {"reso",2.2f}, {"fltEnv",0.7f}, {"fD",0.18f}, {"fS",0},
        {"aD",0.22f}, {"aS",0}, {"aR",0.2f},
        {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },

    { "Pizzicato Trap", {
        {"aWave",PUL}, {"pw",0.25f},
        {"cutoff",1400}, {"reso",1.5f}, {"fltEnv",0.6f}, {"fD",0.12f}, {"fS",0},
        {"aD",0.18f}, {"aS",0}, {"aR",0.12f},
        {"choOn",1}, {"choRate",0.8f}, {"choDepth",0.4f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },

    { "Harp Drop", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.4f},
        {"pitEnv",3}, {"pitDec",0.03f},
        {"cutoff",4500}, {"reso",1.0f},
        {"aD",0.7f}, {"aS",0}, {"aR",0.6f},
        {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.4f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.4f} } },

    { "Glass Crusher", {
        {"aWave",SQR}, {"bWave",SAW}, {"bFine",8}, {"bLvl",0.6f},
        {"cutoff",3000}, {"reso",3.0f}, {"fltEnv",0.5f}, {"fD",0.2f}, {"fS",0},
        {"aD",0.3f}, {"aS",0}, {"aR",0.2f},
        {"drvOn",1}, {"drive",3.0f},
        {"revOn",1}, {"revSize",0.55f}, {"revMix",0.22f} } },

    // ---------------- LEADS ----------------
    { "Evil Saw Lead", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",10}, {"bLvl",0.7f},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"cutoff",4500}, {"reso",2.0f}, {"fltEnv",0.2f}, {"fD",0.3f}, {"fS",0.6f},
        {"aS",0.85f}, {"aR",0.2f},
        {"drvOn",1}, {"drive",3.5f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.18f} } },

    { "Wavy Trap Lead", {
        {"aWave",SAW}, {"uniVoices",2}, {"uniDetune",0.12f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.5f}, {"lfoDepth",0.2f},
        {"cutoff",6000}, {"reso",1.5f},
        {"aA",0.01f}, {"aS",0.85f}, {"aR",0.25f},
        {"choOn",1}, {"choRate",0.6f}, {"choDepth",0.5f}, {"choMix",0.35f},
        {"revOn",1}, {"revSize",0.55f}, {"revMix",0.25f} } },

    { "Screamer Lead", {
        {"aWave",SQR}, {"bWave",SAW}, {"bSemi",12}, {"bLvl",0.5f},
        {"uniVoices",2}, {"uniDetune",0.25f},
        {"cutoff",7000}, {"reso",3.0f},
        {"aS",0.9f}, {"aR",0.15f},
        {"drvOn",1}, {"drive",6.5f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.12f}, {"master",0.6f} } },

    { "Mournful Lead", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",1}, {"bLvl",0.3f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",4.8f}, {"lfoDepth",0.25f},
        {"cutoff",3500}, {"reso",1.0f},
        {"aA",0.03f}, {"aS",0.9f}, {"aR",0.5f},
        {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.4f}, {"dlyMix",0.3f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.35f} } },

    { "Distorted Sine", {
        {"aWave",SIN}, {"bWave",SAW}, {"bLvl",0.3f},
        {"cutoff",5000}, {"reso",1.0f},
        {"aS",0.9f}, {"aR",0.3f},
        {"drvOn",1}, {"drive",7.5f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f}, {"master",0.6f} } },

    // ---------------- PADS ----------------
    { "Dark Matter", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",12}, {"bLvl",0.8f},
        {"uniVoices",5}, {"uniDetune",0.35f}, {"uniSpread",1.0f},
        {"cutoff",900}, {"reso",1.2f}, {"fltEnv",0.3f}, {"fA",1.2f}, {"fD",2.0f}, {"fS",0.5f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.15f}, {"lfoDepth",0.25f},
        {"aA",1.0f}, {"aD",1.0f}, {"aS",0.8f}, {"aR",2.5f},
        {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.85f}, {"revMix",0.5f}, {"master",0.6f} } },

    { "Cold Space", {
        {"aWave",TRI}, {"bWave",SAW}, {"bOct",1}, {"bLvl",0.3f},
        {"uniVoices",4}, {"uniDetune",0.25f}, {"uniSpread",0.9f},
        {"cutoff",1800}, {"reso",1.0f},
        {"aA",0.8f}, {"aS",0.85f}, {"aR",3.0f},
        {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.4f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.9f}, {"revMix",0.55f}, {"master",0.6f} } },

    { "Night Rain", {
        {"noiseLvl",0.04f},
        {"aWave",PUL}, {"pw",0.35f}, {"bWave",SIN}, {"bLvl",0.5f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PW}, {"lfoRate",0.3f}, {"lfoDepth",0.6f},
        {"cutoff",1500}, {"reso",1.0f},
        {"aA",0.6f}, {"aS",0.85f}, {"aR",2.5f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.45f}, {"master",0.62f} } },

    { "Abyss", {
        {"aWave",SAW}, {"aOct",-1}, {"bWave",SAW}, {"bFine",9}, {"bLvl",0.7f},
        {"uniVoices",6}, {"uniDetune",0.4f}, {"uniSpread",1.0f},
        {"cutoff",600}, {"reso",2.0f}, {"fltEnv",0.5f}, {"fA",2.0f}, {"fD",3.0f}, {"fS",0.4f},
        {"aA",1.5f}, {"aS",0.9f}, {"aR",4.0f},
        {"revOn",1}, {"revSize",0.95f}, {"revMix",0.55f}, {"master",0.55f} } },

    { "Phantom Pad", {
        {"aWave",SQR}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.5f},
        {"uniVoices",4}, {"uniDetune",0.3f},
        {"fltType",BP}, {"cutoff",1200}, {"reso",1.5f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.25f}, {"lfoDepth",0.4f},
        {"aA",0.7f}, {"aS",0.85f}, {"aR",2.5f},
        {"choOn",1}, {"choRate",0.35f}, {"choDepth",0.7f}, {"choMix",0.45f},
        {"revOn",1}, {"revSize",0.85f}, {"revMix",0.45f}, {"master",0.75f} } },

    // ---------------- KEYS ----------------
    { "Dark Rhodes", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.25f},
        {"cutoff",2200}, {"reso",1.0f}, {"fltEnv",0.3f}, {"fD",0.4f}, {"fS",0.2f},
        {"aD",0.9f}, {"aS",0.25f}, {"aR",0.5f},
        {"choOn",1}, {"choRate",0.8f}, {"choDepth",0.4f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.55f}, {"revMix",0.28f} } },

    { "Trap Piano", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.2f},
        {"cutoff",2800}, {"reso",1.0f}, {"fltEnv",0.5f}, {"fD",0.25f}, {"fS",0},
        {"aD",1.0f}, {"aS",0}, {"aR",0.4f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f} } },

    { "Music Box", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",2}, {"bLvl",0.4f},
        {"cutoff",9000}, {"reso",0.7f},
        {"aD",0.8f}, {"aS",0}, {"aR",0.8f},
        {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.35f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.4f} } },

    { "Lofi Keys", {
        {"aWave",TRI}, {"bWave",TRI}, {"bFine",12}, {"bLvl",0.8f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",0.6f}, {"lfoDepth",0.12f},
        {"cutoff",1400}, {"reso",1.0f},
        {"aD",0.7f}, {"aS",0.3f}, {"aR",0.5f},
        {"drvOn",1}, {"drive",1.5f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.6f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.45f}, {"revMix",0.2f} } },

    { "Bell Keys", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",1}, {"bSemi",7}, {"bLvl",0.4f},
        {"cutoff",12000}, {"reso",0.7f},
        {"aD",1.5f}, {"aS",0}, {"aR",1.0f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.5f} } },

    // ---------------- SYNTHS ----------------
    { "808 Glide Synth", {
        {"aWave",SAW}, {"bWave",SQR}, {"bOct",-1}, {"bLvl",0.6f},
        {"pitEnv",-5}, {"pitDec",0.12f},
        {"cutoff",1800}, {"reso",4.0f}, {"fltEnv",0.4f}, {"fD",0.3f}, {"fS",0.3f},
        {"aS",0.75f}, {"aR",0.25f},
        {"drvOn",1}, {"drive",3.0f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.12f} } },

    { "Hyper Detune", {
        {"aWave",SAW}, {"bWave",SAW}, {"bLvl",0.6f},
        {"uniVoices",7}, {"uniDetune",0.5f}, {"uniSpread",1.0f},
        {"cutoff",7000}, {"reso",1.0f},
        {"aS",0.85f}, {"aR",0.3f},
        {"choOn",1}, {"choRate",0.7f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.25f}, {"master",0.55f} } },

    { "Acid Trap", {
        {"aWave",SAW},
        {"cutoff",900}, {"reso",8.0f}, {"fltEnv",0.8f}, {"fD",0.25f}, {"fS",0.1f},
        {"aS",0.6f}, {"aR",0.15f},
        {"drvOn",1}, {"drive",4.0f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.2f},
        {"revOn",0}, {"master",0.6f} } },

    { "Rage Synth", {
        {"aWave",SQR}, {"bWave",SAW}, {"bLvl",0.7f}, {"bFine",-14},
        {"uniVoices",3}, {"uniDetune",0.3f},
        {"cutoff",5000}, {"reso",2.5f}, {"fltEnv",0.3f}, {"fD",0.25f}, {"fS",0.5f},
        {"aS",0.85f}, {"aR",0.15f},
        {"drvOn",1}, {"drive",6.0f},
        {"dlyOn",1}, {"dlyTime",187}, {"dlyFb",0.3f}, {"dlyMix",0.18f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.12f}, {"master",0.58f} } },

    { "Wobble Synth", {
        {"aWave",SAW}, {"subLvl",0.6f},
        {"cutoff",700}, {"reso",5.0f},
        {"lfoShape",L_TRI}, {"lfoDest",D_CUT}, {"lfoRate",3.0f}, {"lfoDepth",0.6f},
        {"aS",0.9f}, {"aR",0.2f},
        {"drvOn",1}, {"drive",3.0f},
        {"revOn",0}, {"master",0.6f} } },

    // ---------------- CHOIRS ----------------
    { "Dark Choir", {
        {"aWave",TRI}, {"bWave",PUL}, {"pw",0.3f}, {"bLvl",0.5f},
        {"uniVoices",5}, {"uniDetune",0.2f}, {"uniSpread",0.9f},
        {"cutoff",1500}, {"reso",1.5f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.0f}, {"lfoDepth",0.15f},
        {"aA",0.5f}, {"aS",0.9f}, {"aR",1.8f},
        {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.55f}, {"master",0.65f} } },

    { "Gregorian", {
        {"aWave",SAW}, {"bWave",TRI}, {"bLvl",0.6f}, {"bFine",7},
        {"uniVoices",3}, {"uniDetune",0.15f},
        {"cutoff",1100}, {"reso",2.5f},
        {"aA",0.4f}, {"aS",0.9f}, {"aR",1.5f},
        {"dlyOn",1}, {"dlyTime",600}, {"dlyFb",0.35f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.95f}, {"revMix",0.6f}, {"master",0.65f} } },

    { "Angel Ooh", {
        {"noiseLvl",0.03f},
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.4f},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"cutoff",2500}, {"reso",1.0f},
        {"aA",0.5f}, {"aS",0.9f}, {"aR",2.0f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.5f} } },

    { "Haunted Voices", {
        {"noiseLvl",0.06f},
        {"aWave",PUL}, {"pw",0.4f}, {"bWave",SAW}, {"bLvl",0.4f},
        {"uniVoices",3}, {"uniDetune",0.25f},
        {"fltType",BP}, {"cutoff",1000}, {"reso",3.0f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.4f}, {"lfoDepth",0.5f},
        {"aA",0.8f}, {"aS",0.9f}, {"aR",2.0f},
        {"revOn",1}, {"revSize",0.9f}, {"revMix",0.55f}, {"master",0.8f} } },

    { "Choir Stab", {
        {"aWave",SAW}, {"bWave",TRI}, {"bLvl",0.7f},
        {"uniVoices",5}, {"uniDetune",0.25f}, {"uniSpread",1.0f},
        {"cutoff",1600}, {"reso",1.2f}, {"fltEnv",0.5f}, {"fD",0.3f}, {"fS",0.2f},
        {"aA",0.02f}, {"aD",0.6f}, {"aS",0.2f}, {"aR",0.6f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.4f}, {"master",0.65f} } },

    //==========================================================================
    //  R E G G A E T O N
    //==========================================================================

    // ---------------- PLUCKS ----------------
    { "Dembow Pluck", {
        {"aWave",SQR}, {"bWave",SAW}, {"bOct",1}, {"bLvl",0.5f},
        {"cutoff",3500}, {"reso",2.0f}, {"fltEnv",0.6f}, {"fD",0.15f}, {"fS",0},
        {"aD",0.2f}, {"aS",0}, {"aR",0.15f},
        {"choOn",1}, {"choRate",1.0f}, {"choDepth",0.4f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },

    { "Latin Marimba", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",2}, {"bLvl",0.3f},
        {"pitEnv",2}, {"pitDec",0.02f},
        {"cutoff",5000}, {"reso",1.0f},
        {"aD",0.35f}, {"aS",0}, {"aR",0.3f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.3f} } },

    { "Latin Guitar", {
        {"aWave",SAW}, {"bWave",TRI}, {"bLvl",0.5f}, {"bFine",6},
        {"cutoff",2800}, {"reso",1.5f}, {"fltEnv",0.7f}, {"fD",0.12f}, {"fS",0},
        {"aD",0.45f}, {"aS",0}, {"aR",0.2f},
        {"choOn",1}, {"choRate",0.9f}, {"choDepth",0.3f}, {"choMix",0.25f},
        {"dlyOn",1}, {"dlyTime",220}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.45f}, {"revMix",0.18f} } },

    { "Bubble Perreo", {
        {"aWave",SIN}, {"bWave",SQR}, {"bOct",1}, {"bLvl",0.2f},
        {"pitEnv",7}, {"pitDec",0.04f},
        {"cutoff",3000}, {"reso",3.0f}, {"fltEnv",0.4f}, {"fD",0.15f}, {"fS",0},
        {"aD",0.22f}, {"aS",0}, {"aR",0.15f},
        {"revOn",1}, {"revSize",0.35f}, {"revMix",0.15f} } },

    { "Neon Kalimba", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",2}, {"bLvl",0.3f},
        {"cutoff",6000}, {"reso",1.0f},
        {"aD",0.5f}, {"aS",0}, {"aR",0.4f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.4f}, {"dlyMix",0.3f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f} } },

    // ---------------- LEADS ----------------
    { "Perreo Lead", {
        {"aWave",SAW}, {"bWave",SQR}, {"bFine",8}, {"bLvl",0.5f},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.5f}, {"lfoDepth",0.12f},
        {"cutoff",6500}, {"reso",1.5f},
        {"aA",0.008f}, {"aS",0.85f}, {"aR",0.25f},
        {"choOn",1}, {"choRate",0.7f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },

    { "Reggaeton Siren", {
        {"aWave",SAW}, {"bWave",SQR}, {"bLvl",0.3f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PITCH}, {"lfoRate",3.2f}, {"lfoDepth",1.0f},
        {"cutoff",8000}, {"reso",1.5f},
        {"aS",0.9f}, {"aR",0.2f},
        {"drvOn",1}, {"drive",2.0f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f}, {"master",0.6f} } },

    { "Hyper Flow", {
        {"aWave",SAW}, {"bWave",SAW}, {"bOct",1}, {"bLvl",0.4f},
        {"uniVoices",5}, {"uniDetune",0.35f}, {"uniSpread",1.0f},
        {"cutoff",9000}, {"reso",1.2f},
        {"aS",0.85f}, {"aR",0.2f},
        {"drvOn",1}, {"drive",2.0f},
        {"choOn",1}, {"choRate",0.8f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.18f}, {"master",0.55f} } },

    { "Romantic Lead", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",1}, {"bLvl",0.3f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.2f}, {"lfoDepth",0.2f},
        {"cutoff",4500}, {"reso",0.9f},
        {"aA",0.02f}, {"aS",0.9f}, {"aR",0.5f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.4f} } },

    { "Whistle Lead", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.2f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",6.0f}, {"lfoDepth",0.2f},
        {"cutoff",7000}, {"reso",1.0f},
        {"aA",0.04f}, {"aS",0.9f}, {"aR",0.35f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f} } },

    // ---------------- PADS ----------------
    { "Miami Pad", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",12}, {"bLvl",0.8f},
        {"uniVoices",5}, {"uniDetune",0.3f}, {"uniSpread",1.0f},
        {"cutoff",2200}, {"reso",1.0f},
        {"aA",0.6f}, {"aS",0.85f}, {"aR",2.0f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.4f}, {"master",0.6f} } },

    { "Sunset Pad", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.5f}, {"bFine",8},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"cutoff",2800}, {"reso",1.0f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.2f}, {"lfoDepth",0.3f},
        {"aA",0.9f}, {"aS",0.9f}, {"aR",2.5f},
        {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.5f}, {"choMix",0.35f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.45f}, {"master",0.7f} } },

    { "Tropical Pad", {
        {"aWave",PUL}, {"pw",0.4f}, {"bWave",TRI}, {"bLvl",0.6f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PW}, {"lfoRate",0.35f}, {"lfoDepth",0.5f},
        {"cutoff",3000}, {"reso",1.0f},
        {"aA",0.5f}, {"aS",0.85f}, {"aR",1.8f},
        {"choOn",1}, {"choRate",0.6f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.3f}, {"dlyMix",0.15f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.35f}, {"master",0.72f} } },

    { "Velvet Pad", {
        {"aWave",SAW}, {"bWave",SIN}, {"bOct",-1}, {"bLvl",0.5f},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"cutoff",1500}, {"reso",1.2f},
        {"aA",1.0f}, {"aS",0.9f}, {"aR",2.5f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f}, {"master",0.65f} } },

    { "Glitter Pad", {
        {"aWave",SAW}, {"bWave",SQR}, {"bOct",1}, {"bLvl",0.3f},
        {"uniVoices",6}, {"uniDetune",0.4f}, {"uniSpread",1.0f},
        {"cutoff",5500}, {"reso",1.2f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.3f}, {"lfoDepth",0.25f},
        {"aA",0.7f}, {"aS",0.85f}, {"aR",2.0f},
        {"choOn",1}, {"choRate",0.7f}, {"choDepth",0.5f}, {"choMix",0.35f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.4f}, {"master",0.5f} } },

    // ---------------- KEYS ----------------
    { "Latin Piano", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.2f},
        {"cutoff",3500}, {"reso",1.0f}, {"fltEnv",0.4f}, {"fD",0.3f}, {"fS",0},
        {"aD",1.1f}, {"aS",0}, {"aR",0.35f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.25f} } },

    { "Salsa Keys", {
        {"aWave",SQR}, {"bWave",SAW}, {"bLvl",0.3f}, {"bOct",1},
        {"cutoff",2500}, {"reso",1.5f}, {"fltEnv",0.4f}, {"fD",0.2f}, {"fS",0.3f},
        {"aD",0.5f}, {"aS",0.4f}, {"aR",0.3f},
        {"drvOn",1}, {"drive",1.0f},
        {"choOn",1}, {"choRate",1.0f}, {"choDepth",0.4f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.18f} } },

    { "Rhodes Latino", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.2f},
        {"cutoff",3000}, {"reso",1.0f}, {"fltEnv",0.3f}, {"fD",0.4f}, {"fS",0.3f},
        {"aD",1.0f}, {"aS",0.3f}, {"aR",0.5f},
        {"choOn",1}, {"choRate",0.9f}, {"choDepth",0.7f}, {"choMix",0.5f},
        {"revOn",1}, {"revSize",0.55f}, {"revMix",0.25f} } },

    { "Vibraphone", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",2}, {"bLvl",0.2f},
        {"lfoShape",L_SIN}, {"lfoDest",D_AMP}, {"lfoRate",5.0f}, {"lfoDepth",0.35f},
        {"cutoff",8000}, {"reso",0.7f},
        {"aD",1.5f}, {"aS",0.2f}, {"aR",1.0f},
        {"revOn",1}, {"revSize",0.65f}, {"revMix",0.35f} } },

    { "Sweet Organ", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",1}, {"bLvl",0.5f}, {"subLvl",0.3f},
        {"cutoff",5000}, {"reso",0.8f},
        {"aA",0.01f}, {"aS",1.0f}, {"aR",0.08f},
        {"choOn",1}, {"choRate",1.2f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.2f} } },

    // ---------------- SYNTHS ----------------
    { "Perreo Synth", {
        {"aWave",SAW}, {"bWave",SQR}, {"bFine",6}, {"bLvl",0.6f},
        {"cutoff",3500}, {"reso",3.0f}, {"fltEnv",0.5f}, {"fD",0.3f}, {"fS",0.3f},
        {"aS",0.75f}, {"aR",0.2f},
        {"choOn",1}, {"choRate",0.8f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },

    { "Dembow Stab", {
        {"aWave",SAW}, {"bWave",SAW}, {"bOct",1}, {"bLvl",0.5f},
        {"cutoff",2800}, {"reso",2.0f}, {"fltEnv",0.8f}, {"fD",0.15f}, {"fS",0},
        {"aD",0.25f}, {"aS",0}, {"aR",0.12f},
        {"drvOn",1}, {"drive",2.0f},
        {"revOn",1}, {"revSize",0.45f}, {"revMix",0.2f} } },

    { "Retro Flow", {
        {"aWave",SAW}, {"bWave",SAW}, {"bLvl",0.6f},
        {"uniVoices",3}, {"uniDetune",0.25f},
        {"cutoff",4500}, {"reso",2.0f},
        {"aS",0.8f}, {"aR",0.25f},
        {"choOn",1}, {"choRate",0.6f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.35f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },

    { "Fire Synth", {
        {"aWave",SQR}, {"bWave",SAW}, {"bOct",1}, {"bLvl",0.5f},
        {"cutoff",6000}, {"reso",2.5f},
        {"aS",0.85f}, {"aR",0.15f},
        {"drvOn",1}, {"drive",5.0f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.12f}, {"master",0.6f} } },

    { "Glide Synth", {
        {"aWave",SAW}, {"bWave",TRI}, {"bLvl",0.5f}, {"bFine",7},
        {"pitEnv",-5}, {"pitDec",0.1f},
        {"cutoff",3500}, {"reso",2.0f}, {"fltEnv",0.3f}, {"fD",0.3f}, {"fS",0.5f},
        {"aS",0.8f}, {"aR",0.25f},
        {"choOn",1}, {"choRate",0.7f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },

    // ---------------- CHOIRS ----------------
    { "Choir Latino", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.4f},
        {"uniVoices",4}, {"uniDetune",0.2f}, {"uniSpread",0.9f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.0f}, {"lfoDepth",0.12f},
        {"cutoff",2400}, {"reso",1.2f},
        {"aA",0.4f}, {"aS",0.9f}, {"aR",1.5f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.5f}, {"master",0.65f} } },

    { "Angelic Choir", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.5f},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.5f}, {"lfoDepth",0.15f},
        {"cutoff",3500}, {"reso",1.0f},
        {"aA",0.35f}, {"aS",0.9f}, {"aR",1.8f},
        {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.5f} } },

    { "Epic Choir Hit", {
        {"aWave",SAW}, {"bWave",TRI}, {"bLvl",0.6f},
        {"uniVoices",5}, {"uniDetune",0.25f}, {"uniSpread",1.0f},
        {"cutoff",2000}, {"reso",1.2f}, {"fltEnv",0.4f}, {"fD",0.4f}, {"fS",0.4f},
        {"aA",0.05f}, {"aD",0.8f}, {"aS",0.3f}, {"aR",1.2f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.4f}, {"master",0.6f} } },

    { "Gospel Choir", {
        {"aWave",PUL}, {"pw",0.45f}, {"bWave",TRI}, {"bLvl",0.6f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PW}, {"lfoRate",0.4f}, {"lfoDepth",0.4f},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"cutoff",2000}, {"reso",1.0f},
        {"aA",0.6f}, {"aS",0.9f}, {"aR",2.0f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.85f}, {"revMix",0.5f}, {"master",0.7f} } },

    { "Soft Ooh", {
        {"aWave",SIN}, {"bWave",TRI}, {"bLvl",0.5f}, {"bFine",5},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.0f}, {"lfoDepth",0.15f},
        {"cutoff",3000}, {"reso",1.0f},
        {"aA",0.5f}, {"aS",0.9f}, {"aR",2.0f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.45f} } }
    };

    return table;
}
