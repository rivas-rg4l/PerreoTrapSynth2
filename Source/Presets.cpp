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

static const std::vector<PerreoPreset>& getBasePresets()
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


//==============================================================================
//  Presets extra: 4 por categoria (2 divisiones x 6 categorias x 4 = 48)
//  Orden: TRAP (plucks, leads, pads, keys, synths, choirs) y luego REGGAETON.
//==============================================================================
static const std::vector<PerreoPreset>& getExtraPresets()
{
    static const std::vector<PerreoPreset> table = {

    //========================= T R A P =========================
    // PLUCKS
    { "Frost Bite", {
        {"aWave",TRI}, {"bWave",SQR}, {"bOct",1}, {"bLvl",0.3f},
        {"cutoff",3200}, {"reso",1.8f}, {"fltEnv",0.6f}, {"fD",0.14f}, {"fS",0},
        {"aD",0.2f}, {"aS",0}, {"aR",0.18f},
        {"dlyOn",1}, {"dlyTime",333}, {"dlyFb",0.3f}, {"dlyMix",0.18f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },
    { "Crystal Drip", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",2}, {"bLvl",0.35f},
        {"pitEnv",5}, {"pitDec",0.02f},
        {"cutoff",8000}, {"aD",0.45f}, {"aS",0}, {"aR",0.5f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.45f}, {"dlyMix",0.3f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f} } },
    { "Dark Kalimba", {
        {"aWave",TRI}, {"bWave",SIN}, {"bSemi",12}, {"bLvl",0.25f},
        {"cutoff",1800}, {"reso",1.0f}, {"fltEnv",0.4f}, {"fD",0.2f}, {"fS",0},
        {"aD",0.5f}, {"aS",0}, {"aR",0.4f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f} } },
    { "Sad Marimba", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.3f},
        {"pitEnv",2}, {"pitDec",0.015f},
        {"cutoff",2600}, {"aD",0.6f}, {"aS",0}, {"aR",0.5f},
        {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.35f} } },

    // LEADS
    { "Cold Flute Lead", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.25f}, {"noiseLvl",0.05f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.2f}, {"lfoDepth",0.15f},
        {"cutoff",5000}, {"aA",0.08f}, {"aS",0.9f}, {"aR",0.35f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.22f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.35f} } },
    { "Villain Lead", {
        {"aWave",SAW}, {"bWave",SQR}, {"bFine",-9}, {"bLvl",0.55f},
        {"uniVoices",2}, {"uniDetune",0.3f},
        {"cutoff",3000}, {"reso",3.5f}, {"fltEnv",0.4f}, {"fD",0.4f}, {"fS",0.4f},
        {"aS",0.8f}, {"aR",0.2f},
        {"drvOn",1}, {"drive",4.5f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.15f}, {"master",0.65f} } },
    { "Whistle Lead", {
        {"aWave",SIN}, {"pitEnv",-2}, {"pitDec",0.08f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",6.0f}, {"lfoDepth",0.3f},
        {"cutoff",9000}, {"aS",1.0f}, {"aR",0.3f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },
    { "Detuned Siren", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",25}, {"bLvl",0.8f},
        {"uniVoices",3}, {"uniDetune",0.4f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PITCH}, {"lfoRate",0.7f}, {"lfoDepth",0.6f},
        {"cutoff",6000}, {"reso",1.5f}, {"aS",0.9f}, {"aR",0.3f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.25f}, {"master",0.6f} } },

    // PADS
    { "Midnight Fog", {
        {"aWave",SAW}, {"bWave",TRI}, {"bLvl",0.7f},
        {"uniVoices",4}, {"uniDetune",0.3f}, {"uniSpread",1.0f},
        {"cutoff",700}, {"reso",1.0f}, {"fltEnv",0.2f}, {"fA",2.0f}, {"fD",2.0f}, {"fS",0.5f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.1f}, {"lfoDepth",0.3f},
        {"aA",1.5f}, {"aD",1.0f}, {"aS",0.85f}, {"aR",3.0f},
        {"choOn",1}, {"choRate",0.3f}, {"choDepth",0.7f}, {"choMix",0.5f},
        {"revOn",1}, {"revSize",0.9f}, {"revMix",0.55f}, {"master",0.55f} } },
    { "Abyss Pad", {
        {"aWave",SQR}, {"bWave",SAW}, {"bOct",-1}, {"bLvl",0.6f}, {"subLvl",0.3f},
        {"uniVoices",3}, {"uniDetune",0.25f},
        {"cutoff",500}, {"reso",1.5f},
        {"aA",2.0f}, {"aS",1.0f}, {"aR",3.5f},
        {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.55f} } },
    { "Cold Strings", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",10}, {"bLvl",0.8f},
        {"uniVoices",5}, {"uniDetune",0.22f}, {"uniSpread",1.0f},
        {"cutoff",2200}, {"reso",1.0f}, {"fltEnv",0.25f}, {"fA",0.6f}, {"fD",1.0f}, {"fS",0.7f},
        {"aA",0.7f}, {"aS",0.9f}, {"aR",1.5f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f}, {"master",0.6f} } },
    { "Ghost Air", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",1}, {"bLvl",0.5f}, {"noiseLvl",0.12f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.2f}, {"lfoDepth",0.4f},
        {"cutoff",3500}, {"reso",2.0f},
        {"aA",1.2f}, {"aS",0.9f}, {"aR",3.0f},
        {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.5f}, {"dlyMix",0.3f},
        {"revOn",1}, {"revSize",0.95f}, {"revMix",0.6f}, {"master",0.55f} } },

    // KEYS
    { "Dark Piano", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.25f},
        {"cutoff",2200}, {"reso",1.0f}, {"fltEnv",0.5f}, {"fD",0.5f}, {"fS",0.15f},
        {"aD",1.2f}, {"aS",0.1f}, {"aR",0.5f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.25f} } },
    { "Lofi Rhodes", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.3f}, {"bFine",4},
        {"lfoShape",L_SIN}, {"lfoDest",D_AMP}, {"lfoRate",4.5f}, {"lfoDepth",0.15f},
        {"cutoff",3000}, {"aD",0.9f}, {"aS",0.3f}, {"aR",0.4f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.4f}, {"choMix",0.35f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },
    { "Music Box", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",2}, {"bLvl",0.4f},
        {"cutoff",9000}, {"aD",0.8f}, {"aS",0}, {"aR",0.9f},
        {"dlyOn",1}, {"dlyTime",333}, {"dlyFb",0.35f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f} } },
    { "Haunted Bells", {
        {"aWave",SIN}, {"bWave",SQR}, {"bSemi",7}, {"bLvl",0.3f},
        {"pitEnv",1}, {"pitDec",0.05f},
        {"cutoff",4000}, {"reso",1.5f}, {"aD",1.6f}, {"aS",0}, {"aR",1.5f},
        {"choOn",1}, {"choRate",0.3f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.5f}, {"dlyMix",0.3f},
        {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.6f} } },

    // SYNTHS
    { "808 Stab", {
        {"aWave",SAW}, {"bWave",SQR}, {"bOct",-1}, {"bLvl",0.6f}, {"subLvl",0.3f},
        {"cutoff",2400}, {"reso",2.0f}, {"fltEnv",0.8f}, {"fD",0.18f}, {"fS",0},
        {"aD",0.3f}, {"aS",0}, {"aR",0.15f},
        {"drvOn",1}, {"drive",3.0f} } },
    { "Phonk Brass", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",8}, {"bLvl",0.7f},
        {"uniVoices",3}, {"uniDetune",0.15f},
        {"cutoff",1500}, {"reso",1.5f}, {"fltEnv",0.7f}, {"fA",0.06f}, {"fD",0.3f}, {"fS",0.5f},
        {"aA",0.03f}, {"aS",0.8f}, {"aR",0.15f},
        {"drvOn",1}, {"drive",2.5f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.12f} } },
    { "Wide Saw Stack", {
        {"aWave",SAW}, {"bWave",SAW}, {"bOct",1}, {"bLvl",0.5f}, {"bFine",6},
        {"uniVoices",7}, {"uniDetune",0.45f}, {"uniSpread",1.0f},
        {"cutoff",5500}, {"reso",1.2f}, {"aS",0.85f}, {"aR",0.3f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f}, {"master",0.5f} } },
    { "Hollow Pulse", {
        {"aWave",PUL}, {"pw",0.3f}, {"bWave",PUL}, {"bFine",7}, {"bLvl",0.5f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PW}, {"lfoRate",0.5f}, {"lfoDepth",0.5f},
        {"cutoff",2800}, {"reso",2.0f}, {"aS",0.8f}, {"aR",0.2f},
        {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },

    // CHOIRS
    { "Dark Choir", {
        {"aWave",SAW}, {"bWave",TRI}, {"bLvl",0.5f},
        {"uniVoices",4}, {"uniDetune",0.2f}, {"uniSpread",0.9f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",4.8f}, {"lfoDepth",0.1f},
        {"cutoff",1300}, {"reso",1.5f}, {"aA",0.6f}, {"aS",0.9f}, {"aR",2.2f},
        {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.85f}, {"revMix",0.5f}, {"master",0.6f} } },
    { "Hell Choir", {
        {"aWave",SQR}, {"bWave",SAW}, {"bOct",-1}, {"bLvl",0.5f},
        {"uniVoices",5}, {"uniDetune",0.3f},
        {"cutoff",1000}, {"reso",2.5f}, {"fltEnv",0.3f}, {"fA",0.8f}, {"fD",1.5f}, {"fS",0.4f},
        {"aA",0.8f}, {"aS",0.9f}, {"aR",2.5f},
        {"drvOn",1}, {"drive",2.0f},
        {"revOn",1}, {"revSize",0.9f}, {"revMix",0.55f}, {"master",0.55f} } },
    { "Whisper Voices", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",1}, {"bLvl",0.4f}, {"noiseLvl",0.2f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.0f}, {"lfoDepth",0.1f},
        {"cutoff",2500}, {"reso",3.0f}, {"aA",0.4f}, {"aS",0.85f}, {"aR",1.8f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.4f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.85f}, {"revMix",0.5f} } },
    { "Cathedral Pad", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.6f},
        {"uniVoices",4}, {"uniDetune",0.15f}, {"uniSpread",1.0f},
        {"cutoff",3200}, {"aA",1.0f}, {"aS",1.0f}, {"aR",3.0f},
        {"choOn",1}, {"choRate",0.3f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.95f}, {"revMix",0.65f}, {"master",0.55f} } },

    //===================== R E G G A E T O N =====================
    // PLUCKS
    { "Latin Pluck", {
        {"aWave",SAW}, {"bWave",SQR}, {"bOct",1}, {"bLvl",0.3f},
        {"cutoff",2600}, {"reso",1.5f}, {"fltEnv",0.7f}, {"fD",0.15f}, {"fS",0},
        {"aD",0.25f}, {"aS",0}, {"aR",0.18f},
        {"choOn",1}, {"choRate",0.8f}, {"choDepth",0.4f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },
    { "Nylon Perreo", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.2f},
        {"cutoff",2000}, {"reso",1.0f}, {"fltEnv",0.5f}, {"fD",0.2f}, {"fS",0},
        {"aD",0.35f}, {"aS",0}, {"aR",0.25f},
        {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },
    { "Steel Drum Pluck", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",1}, {"bFine",10}, {"bLvl",0.45f},
        {"pitEnv",1.5f}, {"pitDec",0.01f},
        {"cutoff",6000}, {"aD",0.4f}, {"aS",0}, {"aR",0.4f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.55f}, {"revMix",0.25f} } },
    { "Bouncy Pluck", {
        {"aWave",PUL}, {"pw",0.35f}, {"uniVoices",2}, {"uniDetune",0.1f},
        {"cutoff",1600}, {"reso",2.5f}, {"fltEnv",0.7f}, {"fD",0.1f}, {"fS",0},
        {"aD",0.15f}, {"aS",0}, {"aR",0.1f},
        {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.35f}, {"revMix",0.12f} } },

    // LEADS
    { "Dembow Lead", {
        {"aWave",SAW}, {"bWave",SQR}, {"bFine",7}, {"bLvl",0.5f},
        {"uniVoices",2}, {"uniDetune",0.2f},
        {"cutoff",5500}, {"reso",2.2f}, {"aS",0.85f}, {"aR",0.2f},
        {"drvOn",1}, {"drive",2.5f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.18f},
        {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f}, {"master",0.65f} } },
    { "Flauta Urbana", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.3f}, {"noiseLvl",0.06f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.5f}, {"lfoDepth",0.2f},
        {"cutoff",6000}, {"aA",0.06f}, {"aS",0.9f}, {"aR",0.25f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.55f}, {"revMix",0.25f} } },
    { "Hook Lead", {
        {"aWave",SQR}, {"bWave",SAW}, {"bSemi",12}, {"bLvl",0.35f},
        {"uniVoices",2}, {"uniDetune",0.18f},
        {"cutoff",4500}, {"reso",2.0f}, {"fltEnv",0.3f}, {"fD",0.3f}, {"fS",0.6f},
        {"aA",0.01f}, {"aS",0.9f}, {"aR",0.2f},
        {"choOn",1}, {"choRate",0.7f}, {"choDepth",0.5f}, {"choMix",0.3f},
        {"revOn",1}, {"revSize",0.45f}, {"revMix",0.18f} } },
    { "Sunset Lead", {
        {"aWave",SAW}, {"bWave",TRI}, {"bFine",5}, {"bLvl",0.6f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PITCH}, {"lfoRate",4.5f}, {"lfoDepth",0.15f},
        {"cutoff",3800}, {"reso",1.2f}, {"aA",0.02f}, {"aS",0.9f}, {"aR",0.4f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.35f},
        {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.22f},
        {"revOn",1}, {"revSize",0.65f}, {"revMix",0.3f} } },

    // PADS
    { "Caribbean Pad", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.5f},
        {"uniVoices",4}, {"uniDetune",0.2f}, {"uniSpread",1.0f},
        {"cutoff",3000}, {"reso",1.0f}, {"fltEnv",0.2f}, {"fA",0.8f}, {"fD",1.0f}, {"fS",0.7f},
        {"aA",0.8f}, {"aS",0.9f}, {"aR",1.8f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.6f}, {"choMix",0.45f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.4f}, {"master",0.6f} } },
    { "Neon Night", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",14}, {"bLvl",0.8f},
        {"uniVoices",5}, {"uniDetune",0.3f},
        {"lfoShape",L_SIN}, {"lfoDest",D_CUT}, {"lfoRate",0.25f}, {"lfoDepth",0.35f},
        {"cutoff",1800}, {"reso",2.0f},
        {"aA",1.0f}, {"aS",0.85f}, {"aR",2.5f},
        {"choOn",1}, {"choRate",0.35f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.85f}, {"revMix",0.5f}, {"master",0.55f} } },
    { "Warm Beach", {
        {"aWave",SIN}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.5f}, {"subLvl",0.15f},
        {"cutoff",2500}, {"aA",1.2f}, {"aS",1.0f}, {"aR",2.8f},
        {"choOn",1}, {"choRate",0.3f}, {"choDepth",0.5f}, {"choMix",0.5f},
        {"revOn",1}, {"revSize",0.8f}, {"revMix",0.45f}, {"master",0.6f} } },
    { "Club Haze", {
        {"aWave",PUL}, {"pw",0.4f}, {"bWave",SAW}, {"bLvl",0.5f},
        {"uniVoices",3}, {"uniDetune",0.3f},
        {"lfoShape",L_TRI}, {"lfoDest",D_PW}, {"lfoRate",0.3f}, {"lfoDepth",0.4f},
        {"cutoff",2000}, {"reso",1.5f}, {"aA",0.5f}, {"aS",0.85f}, {"aR",1.6f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.4f}, {"dlyMix",0.25f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.35f} } },

    // KEYS
    { "Salsa Piano", {
        {"aWave",SAW}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.4f},
        {"cutoff",3500}, {"reso",1.0f}, {"fltEnv",0.5f}, {"fD",0.3f}, {"fS",0.25f},
        {"aD",0.8f}, {"aS",0.2f}, {"aR",0.3f},
        {"revOn",1}, {"revSize",0.35f}, {"revMix",0.12f} } },
    { "Organ Perreo", {
        {"aWave",SQR}, {"bWave",SQR}, {"bOct",1}, {"bLvl",0.5f},
        {"lfoShape",L_SIN}, {"lfoDest",D_AMP}, {"lfoRate",6.0f}, {"lfoDepth",0.1f},
        {"cutoff",4500}, {"aA",0.01f}, {"aS",1.0f}, {"aR",0.06f},
        {"choOn",1}, {"choRate",1.2f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.35f}, {"revMix",0.15f} } },
    { "Electric Keys", {
        {"aWave",TRI}, {"bWave",SIN}, {"bOct",1}, {"bLvl",0.4f},
        {"lfoShape",L_SIN}, {"lfoDest",D_AMP}, {"lfoRate",5.0f}, {"lfoDepth",0.12f},
        {"cutoff",3200}, {"aD",0.7f}, {"aS",0.35f}, {"aR",0.3f},
        {"choOn",1}, {"choRate",0.6f}, {"choDepth",0.5f}, {"choMix",0.35f},
        {"revOn",1}, {"revSize",0.45f}, {"revMix",0.18f} } },
    { "Bell Keys", {
        {"aWave",SIN}, {"bWave",SIN}, {"bOct",1}, {"bSemi",7}, {"bLvl",0.45f},
        {"pitEnv",1.5f}, {"pitDec",0.02f},
        {"cutoff",7000}, {"aD",1.0f}, {"aS",0}, {"aR",1.0f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.3f} } },

    // SYNTHS
    { "Bubble Synth", {
        {"aWave",SQR}, {"bWave",SAW}, {"bOct",1}, {"bLvl",0.4f},
        {"pitEnv",4}, {"pitDec",0.04f},
        {"cutoff",2200}, {"reso",3.0f}, {"fltEnv",0.6f}, {"fD",0.2f}, {"fS",0.2f},
        {"aS",0.7f}, {"aR",0.15f},
        {"revOn",1}, {"revSize",0.35f}, {"revMix",0.12f} } },
    { "Reggaeton Bass Stab", {
        {"aWave",SAW}, {"bWave",SQR}, {"bOct",-1}, {"bLvl",0.6f}, {"subLvl",0.35f},
        {"cutoff",1800}, {"reso",2.0f}, {"fltEnv",0.8f}, {"fD",0.14f}, {"fS",0},
        {"aD",0.2f}, {"aS",0}, {"aR",0.1f},
        {"drvOn",1}, {"drive",3.0f} } },
    { "Moombah Saw", {
        {"aWave",SAW}, {"bWave",SAW}, {"bFine",12}, {"bLvl",0.8f},
        {"uniVoices",3}, {"uniDetune",0.35f},
        {"cutoff",4000}, {"reso",3.0f}, {"fltEnv",0.5f}, {"fD",0.25f}, {"fS",0.4f},
        {"aS",0.85f}, {"aR",0.12f},
        {"drvOn",1}, {"drive",4.0f},
        {"revOn",1}, {"revSize",0.3f}, {"revMix",0.1f}, {"master",0.6f} } },
    { "Vintage Flow", {
        {"aWave",SAW}, {"bWave",PUL}, {"pw",0.4f}, {"bLvl",0.5f},
        {"uniVoices",2}, {"uniDetune",0.15f},
        {"cutoff",2800}, {"reso",1.8f}, {"fltEnv",0.4f}, {"fD",0.4f}, {"fS",0.4f},
        {"aS",0.8f}, {"aR",0.25f},
        {"choOn",1}, {"choRate",0.6f}, {"choDepth",0.6f}, {"choMix",0.4f},
        {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.15f},
        {"revOn",1}, {"revSize",0.45f}, {"revMix",0.18f} } },

    // CHOIRS
    { "Coro Urbano", {
        {"aWave",TRI}, {"bWave",SAW}, {"bLvl",0.35f},
        {"uniVoices",4}, {"uniDetune",0.18f}, {"uniSpread",1.0f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.0f}, {"lfoDepth",0.12f},
        {"cutoff",2800}, {"reso",1.3f}, {"aA",0.3f}, {"aS",0.9f}, {"aR",1.4f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.4f},
        {"revOn",1}, {"revSize",0.7f}, {"revMix",0.4f}, {"master",0.65f} } },
    { "Vocal Chops Pad", {
        {"aWave",PUL}, {"pw",0.4f}, {"bWave",TRI}, {"bLvl",0.5f},
        {"lfoShape",L_SH}, {"lfoDest",D_CUT}, {"lfoRate",4.0f}, {"lfoDepth",0.3f},
        {"cutoff",2200}, {"reso",3.0f}, {"aA",0.02f}, {"aS",0.8f}, {"aR",0.4f},
        {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f} } },
    { "Ooh Reggaeton", {
        {"aWave",SIN}, {"bWave",TRI}, {"bFine",6}, {"bLvl",0.6f},
        {"uniVoices",3}, {"uniDetune",0.2f},
        {"lfoShape",L_SIN}, {"lfoDest",D_PITCH}, {"lfoRate",5.2f}, {"lfoDepth",0.14f},
        {"cutoff",2400}, {"reso",1.0f}, {"aA",0.35f}, {"aS",0.9f}, {"aR",1.6f},
        {"choOn",1}, {"choRate",0.45f}, {"choDepth",0.5f}, {"choMix",0.35f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.4f} } },
    { "Aah Latino", {
        {"aWave",SAW}, {"bWave",TRI}, {"bOct",1}, {"bLvl",0.5f},
        {"uniVoices",4}, {"uniDetune",0.22f}, {"uniSpread",0.9f},
        {"cutoff",1800}, {"reso",2.0f}, {"fltEnv",0.3f}, {"fA",0.3f}, {"fD",0.6f}, {"fS",0.6f},
        {"aA",0.3f}, {"aS",0.9f}, {"aR",1.6f},
        {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.35f},
        {"revOn",1}, {"revSize",0.75f}, {"revMix",0.4f}, {"master",0.6f} } }
    };

    return table;
}

//==============================================================================
//  Categorias nuevas: BASS, BELLS, ARPS, FX (9 por division y categoria = 72)
//  Orden: TRAP (bass, bells, arps, fx) y luego REGGAETON.
//  Nota: los ARPS no llevan arpegiador; son gates ritmicos (LFO) para acordes.
//==============================================================================
static const std::vector<PerreoPreset>& getNewCategoryPresets()
{
    static const std::vector<PerreoPreset> table = {
    // ===== TRAP =====
    // BASS
    { "808 Classic", { {"aWave",0}, {"aOct",-1}, {"pitEnv",4}, {"pitDec",0.04f}, {"cutoff",1400}, {"aD",1.6f}, {"aS",0.3f}, {"aR",0.4f}, {"drvOn",1}, {"drive",1.5f}, {"master",0.8f} } },
    { "808 Long Glide", { {"aWave",0}, {"aOct",-1}, {"subLvl",0.2f}, {"pitEnv",7}, {"pitDec",0.1f}, {"cutoff",1200}, {"aD",3}, {"aS",0.5f}, {"aR",0.6f}, {"master",0.8f} } },
    { "Distorted 808", { {"aWave",0}, {"aOct",-1}, {"bWave",1}, {"bOct",-2}, {"bLvl",0.15f}, {"pitEnv",4}, {"pitDec",0.05f}, {"cutoff",2000}, {"aD",1.8f}, {"aS",0.3f}, {"aR",0.4f}, {"drvOn",1}, {"drive",5}, {"master",0.6f} } },
    { "Sub Hum", { {"aWave",0}, {"aOct",-1}, {"subLvl",0.4f}, {"cutoff",800}, {"aA",0.01f}, {"aS",1}, {"aR",0.15f}, {"master",0.8f} } },
    { "Reese Dark", { {"aWave",1}, {"aOct",-1}, {"bWave",1}, {"bOct",-1}, {"bFine",20}, {"bLvl",0.9f}, {"uniVoices",3}, {"uniDetune",0.3f}, {"cutoff",700}, {"reso",2}, {"aS",0.9f}, {"aR",0.3f}, {"drvOn",1}, {"drive",2.5f}, {"master",0.6f} } },
    { "Phonk Cowbell Bass", { {"aWave",2}, {"aOct",-1}, {"bWave",1}, {"bOct",-1}, {"bLvl",0.4f}, {"cutoff",900}, {"reso",3}, {"fltEnv",0.5f}, {"fD",0.2f}, {"fS",0.3f}, {"aD",0.5f}, {"aS",0.5f}, {"aR",0.15f}, {"drvOn",1}, {"drive",3}, {"master",0.7f} } },
    { "Hard 808 Click", { {"aWave",0}, {"aOct",-1}, {"bWave",4}, {"bLvl",0.2f}, {"pitEnv",12}, {"pitDec",0.02f}, {"cutoff",2500}, {"aD",1.4f}, {"aS",0.2f}, {"aR",0.3f}, {"drvOn",1}, {"drive",3}, {"master",0.7f} } },
    { "Wobble Bass", { {"aWave",1}, {"aOct",-1}, {"bWave",2}, {"bOct",-1}, {"bLvl",0.5f}, {"lfoShape",0}, {"lfoDest",1}, {"lfoRate",2}, {"lfoDepth",0.8f}, {"cutoff",600}, {"reso",4}, {"aS",0.9f}, {"aR",0.15f}, {"drvOn",1}, {"drive",2}, {"master",0.6f} } },
    { "Slide Sub", { {"aWave",0}, {"aOct",-1}, {"subLvl",0.3f}, {"pitEnv",-5}, {"pitDec",0.15f}, {"cutoff",900}, {"aD",1.2f}, {"aS",0.5f}, {"aR",0.3f}, {"master",0.8f} } },
    // BELLS
    { "Ice Bell", { {"aWave",0}, {"bWave",0}, {"bOct",2}, {"bSemi",7}, {"bLvl",0.4f}, {"pitEnv",1.5f}, {"pitDec",0.02f}, {"cutoff",10000}, {"aD",1.8f}, {"aS",0}, {"aR",2}, {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.5f}, {"dlyMix",0.3f}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.55f} } },
    { "Dark Tubular", { {"aWave",11}, {"bWave",0}, {"bOct",1}, {"bLvl",0.3f}, {"cutoff",3500}, {"aD",2.2f}, {"aS",0}, {"aR",2}, {"revOn",1}, {"revSize",0.85f}, {"revMix",0.45f}, {"master",0.6f} } },
    { "Glass Bell", { {"aWave",0}, {"bWave",0}, {"bOct",2}, {"bFine",6}, {"bLvl",0.5f}, {"cutoff",9000}, {"aD",1.4f}, {"aS",0}, {"aR",1.6f}, {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.5f}, {"choMix",0.3f}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.55f} } },
    { "Haunted Chime", { {"aWave",0}, {"bWave",2}, {"bSemi",7}, {"bLvl",0.25f}, {"pitEnv",1}, {"pitDec",0.05f}, {"cutoff",4000}, {"reso",1.5f}, {"aD",1.8f}, {"aS",0}, {"aR",1.8f}, {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.5f}, {"dlyMix",0.3f}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.6f} } },
    { "Crystal Bell", { {"aWave",0}, {"aSemi",12}, {"bWave",0}, {"bOct",1}, {"bSemi",7}, {"bLvl",0.35f}, {"cutoff",12000}, {"aD",2}, {"aS",0}, {"aR",2.5f}, {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.5f}, {"dlyMix",0.35f}, {"revOn",1}, {"revSize",0.95f}, {"revMix",0.6f}, {"master",0.5f} } },
    { "Music Box Bell", { {"aWave",0}, {"bWave",3}, {"bOct",2}, {"bLvl",0.35f}, {"cutoff",8000}, {"aD",1}, {"aS",0}, {"aR",1.4f}, {"dlyOn",1}, {"dlyTime",333}, {"dlyFb",0.45f}, {"dlyMix",0.3f}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f} } },
    { "Church Bell Low", { {"aWave",11}, {"aOct",-1}, {"bWave",0}, {"bSemi",7}, {"bLvl",0.3f}, {"cutoff",2800}, {"aD",3}, {"aS",0}, {"aR",3}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.65f} } },
    { "Phonk Bell", { {"aWave",2}, {"bWave",0}, {"bOct",1}, {"bSemi",7}, {"bLvl",0.35f}, {"cutoff",5000}, {"reso",2}, {"aD",1.2f}, {"aS",0}, {"aR",0.8f}, {"drvOn",1}, {"drive",2}, {"dlyOn",1}, {"dlyTime",333}, {"dlyFb",0.4f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.35f}, {"master",0.6f} } },
    { "Cold Celesta", { {"aWave",0}, {"bWave",0}, {"bOct",2}, {"bFine",5}, {"bLvl",0.45f}, {"lfoShape",0}, {"lfoDest",3}, {"lfoRate",4}, {"lfoDepth",0.15f}, {"cutoff",7000}, {"aD",1.4f}, {"aS",0.05f}, {"aR",1.8f}, {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.5f}, {"choMix",0.35f}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.55f} } },
    // ARPS
    { "Dark Gate Arp", { {"aWave",1}, {"bWave",2}, {"bLvl",0.4f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",8}, {"lfoDepth",1}, {"cutoff",2200}, {"reso",2}, {"aS",0.9f}, {"aR",0.1f}, {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.3f}, {"master",0.6f} } },
    { "Ice Pulse Arp", { {"aWave",3}, {"bWave",0}, {"bOct",1}, {"bLvl",0.5f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",6}, {"lfoDepth",1}, {"cutoff",8000}, {"aS",0.9f}, {"aR",0.2f}, {"dlyOn",1}, {"dlyTime",333}, {"dlyFb",0.45f}, {"dlyMix",0.3f}, {"revOn",1}, {"revSize",0.85f}, {"revMix",0.45f}, {"master",0.55f} } },
    { "Trap Stutter", { {"aWave",2}, {"bWave",1}, {"bLvl",0.4f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",12}, {"lfoDepth",1}, {"cutoff",3000}, {"reso",2}, {"aS",0.9f}, {"aR",0.08f}, {"drvOn",1}, {"drive",1.5f}, {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f}, {"master",0.6f} } },
    { "Phonk Gate", { {"aWave",1}, {"bWave",1}, {"bFine",12}, {"bLvl",0.7f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",7}, {"lfoDepth",0.9f}, {"cutoff",1800}, {"reso",2.5f}, {"aS",0.9f}, {"aR",0.1f}, {"drvOn",1}, {"drive",3}, {"master",0.55f} } },
    { "Pluck Gate Arp", { {"aWave",3}, {"bWave",1}, {"bLvl",0.3f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",9}, {"lfoDepth",1}, {"cutoff",3500}, {"fltEnv",0.4f}, {"fD",0.12f}, {"fS",0.4f}, {"aS",0.9f}, {"aR",0.1f}, {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.4f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f} } },
    { "Cold Sequence", { {"aWave",4}, {"pw",0.3f}, {"bWave",1}, {"bLvl",0.4f}, {"lfoShape",4}, {"lfoDest",1}, {"lfoRate",6}, {"lfoDepth",0.8f}, {"cutoff",2000}, {"reso",3}, {"aS",0.9f}, {"aR",0.15f}, {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.4f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.35f}, {"master",0.6f} } },
    { "Drunk Pitch Arp", { {"aWave",2}, {"bWave",3}, {"bOct",1}, {"bLvl",0.4f}, {"lfoShape",4}, {"lfoDest",2}, {"lfoRate",7}, {"lfoDepth",1}, {"cutoff",4500}, {"aS",0.9f}, {"aR",0.15f}, {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.4f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.3f}, {"master",0.6f} } },
    { "Sixteenth Gate", { {"aWave",1}, {"bWave",1}, {"bFine",8}, {"bLvl",0.6f}, {"uniVoices",3}, {"uniDetune",0.2f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",16}, {"lfoDepth",1}, {"cutoff",5000}, {"aS",0.9f}, {"aR",0.05f}, {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f}, {"master",0.5f} } },
    { "Triplet Gate", { {"aWave",3}, {"bWave",2}, {"bLvl",0.35f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",5.3f}, {"lfoDepth",1}, {"cutoff",2800}, {"reso",1.5f}, {"aS",0.9f}, {"aR",0.2f}, {"choOn",1}, {"choRate",0.5f}, {"choDepth",0.5f}, {"choMix",0.3f}, {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.75f}, {"revMix",0.35f} } },
    // FX
    { "Noise Riser", { {"aLvl",0}, {"noiseLvl",0.9f}, {"fltType",0}, {"cutoff",300}, {"fltEnv",1}, {"fA",4}, {"fD",0.5f}, {"fS",1}, {"fR",0.5f}, {"aA",4}, {"aD",0.5f}, {"aS",1}, {"aR",0.3f}, {"revOn",1}, {"revSize",0.8f}, {"revMix",0.35f}, {"master",0.6f} } },
    { "Down Lifter", { {"aLvl",0}, {"noiseLvl",0.9f}, {"fltType",0}, {"cutoff",9000}, {"fltEnv",-0.8f}, {"fA",0.005f}, {"fD",2.5f}, {"fS",0}, {"fR",1}, {"aA",0.005f}, {"aD",2.5f}, {"aS",0}, {"aR",1}, {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f}, {"master",0.6f} } },
    { "Impact Boom", { {"aWave",0}, {"aOct",-2}, {"bLvl",0}, {"noiseLvl",0.35f}, {"pitEnv",12}, {"pitDec",0.35f}, {"cutoff",3000}, {"aA",0.001f}, {"aD",2.5f}, {"aS",0}, {"aR",1.5f}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.8f} } },
    { "Laser Zap", { {"aWave",1}, {"pitEnv",-24}, {"pitDec",0.08f}, {"cutoff",9000}, {"aD",0.3f}, {"aS",0}, {"aR",0.2f}, {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.35f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f}, {"master",0.6f} } },
    { "Siren FX", { {"aWave",1}, {"bWave",1}, {"bFine",15}, {"bLvl",0.6f}, {"lfoShape",1}, {"lfoDest",2}, {"lfoRate",0.8f}, {"lfoDepth",1}, {"cutoff",6000}, {"aS",0.9f}, {"aR",0.4f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.3f}, {"master",0.55f} } },
    { "Vinyl Noise", { {"aLvl",0}, {"noiseLvl",0.25f}, {"fltType",1}, {"cutoff",3500}, {"aA",0.05f}, {"aS",1}, {"aR",0.3f}, {"master",0.5f} } },
    { "Glitch Stutter", { {"aWave",4}, {"pw",0.3f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",18}, {"lfoDepth",1}, {"pitEnv",7}, {"pitDec",0.05f}, {"cutoff",6000}, {"aS",0.9f}, {"aR",0.05f}, {"drvOn",1}, {"drive",3}, {"dlyOn",1}, {"dlyTime",187}, {"dlyFb",0.45f}, {"dlyMix",0.3f}, {"master",0.5f} } },
    { "Reverse Swell", { {"aWave",1}, {"bWave",1}, {"bFine",12}, {"bLvl",0.7f}, {"uniVoices",4}, {"uniDetune",0.3f}, {"cutoff",500}, {"fltEnv",0.9f}, {"fA",3}, {"fD",0.3f}, {"fS",1}, {"aA",3}, {"aD",0.3f}, {"aS",1}, {"aR",0.08f}, {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f}, {"master",0.5f} } },
    { "Alien Texture", { {"aWave",11}, {"bWave",10}, {"bOct",1}, {"bLvl",0.5f}, {"lfoShape",4}, {"lfoDest",1}, {"lfoRate",9}, {"lfoDepth",0.9f}, {"cutoff",3000}, {"reso",4}, {"aA",0.3f}, {"aS",0.9f}, {"aR",1}, {"dlyOn",1}, {"dlyTime",500}, {"dlyFb",0.5f}, {"dlyMix",0.3f}, {"revOn",1}, {"revSize",0.9f}, {"revMix",0.5f}, {"master",0.5f} } },
    // ===== REGGAETON =====
    // BASS
    { "808 Reggaeton", { {"aWave",0}, {"aOct",-1}, {"pitEnv",4}, {"pitDec",0.04f}, {"cutoff",1300}, {"aD",1.2f}, {"aS",0.2f}, {"aR",0.3f}, {"drvOn",1}, {"drive",1.5f}, {"master",0.8f} } },
    { "Dembow Bass", { {"aWave",1}, {"aOct",-1}, {"bWave",2}, {"bOct",-1}, {"bLvl",0.5f}, {"subLvl",0.3f}, {"cutoff",900}, {"reso",2}, {"fltEnv",0.6f}, {"fD",0.15f}, {"fS",0.3f}, {"aD",0.25f}, {"aS",0.6f}, {"aR",0.1f}, {"drvOn",1}, {"drive",2.5f}, {"master",0.7f} } },
    { "Bajo Moog", { {"aWave",1}, {"aOct",-1}, {"bWave",2}, {"bOct",-1}, {"bLvl",0.6f}, {"cutoff",700}, {"reso",3}, {"fltEnv",0.5f}, {"fD",0.25f}, {"fS",0.3f}, {"aD",0.3f}, {"aS",0.7f}, {"aR",0.1f}, {"drvOn",1}, {"drive",1.5f}, {"master",0.7f} } },
    { "Sub Perreo", { {"aWave",0}, {"aOct",-1}, {"subLvl",0.3f}, {"pitEnv",3}, {"pitDec",0.05f}, {"cutoff",1000}, {"aD",1.5f}, {"aS",0.4f}, {"aR",0.3f}, {"master",0.8f} } },
    { "Pluck Bass Latino", { {"aWave",3}, {"aOct",-1}, {"bWave",1}, {"bOct",-1}, {"bLvl",0.3f}, {"cutoff",800}, {"reso",1.5f}, {"fltEnv",0.6f}, {"fD",0.15f}, {"fS",0}, {"aD",0.3f}, {"aS",0}, {"aR",0.1f}, {"master",0.8f} } },
    { "Upright Latino", { {"aWave",3}, {"aOct",-1}, {"bWave",1}, {"bOct",-1}, {"bLvl",0.2f}, {"cutoff",500}, {"fltEnv",0.5f}, {"fD",0.15f}, {"fS",0.1f}, {"aD",0.4f}, {"aS",0.1f}, {"aR",0.2f}, {"master",0.8f} } },
    { "Dirty Dembow Bass", { {"aWave",1}, {"aOct",-1}, {"bWave",2}, {"bOct",-1}, {"bLvl",0.5f}, {"subLvl",0.3f}, {"cutoff",900}, {"reso",2}, {"fltEnv",0.4f}, {"fD",0.2f}, {"fS",0.5f}, {"aS",0.8f}, {"aR",0.1f}, {"drvOn",1}, {"drive",4}, {"master",0.65f} } },
    { "Club Sub", { {"aWave",0}, {"aOct",-1}, {"subLvl",0.4f}, {"cutoff",700}, {"aA",0.005f}, {"aD",0.8f}, {"aS",0.7f}, {"aR",0.12f}, {"drvOn",1}, {"drive",1}, {"master",0.8f} } },
    { "Bass Stab Calle", { {"aWave",1}, {"aOct",-1}, {"bWave",1}, {"bOct",-1}, {"bFine",10}, {"bLvl",0.7f}, {"uniVoices",2}, {"uniDetune",0.15f}, {"cutoff",1600}, {"reso",2}, {"fltEnv",0.8f}, {"fD",0.14f}, {"fS",0}, {"aD",0.2f}, {"aS",0}, {"aR",0.1f}, {"drvOn",1}, {"drive",3}, {"master",0.65f} } },
    // BELLS
    { "Campana Latina", { {"aWave",0}, {"bWave",0}, {"bOct",1}, {"bSemi",7}, {"bLvl",0.4f}, {"pitEnv",1.5f}, {"pitDec",0.02f}, {"cutoff",7000}, {"aD",1.4f}, {"aS",0}, {"aR",1.4f}, {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.4f}, {"dlyMix",0.3f}, {"revOn",1}, {"revSize",0.85f}, {"revMix",0.45f} } },
    { "Steel Drum Bell", { {"aWave",0}, {"bWave",0}, {"bOct",1}, {"bFine",10}, {"bLvl",0.45f}, {"pitEnv",1.5f}, {"pitDec",0.01f}, {"cutoff",6000}, {"aD",0.5f}, {"aS",0}, {"aR",0.5f}, {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.55f}, {"revMix",0.25f} } },
    { "Marimba Bell", { {"aWave",0}, {"bWave",3}, {"bOct",1}, {"bLvl",0.3f}, {"pitEnv",2}, {"pitDec",0.015f}, {"cutoff",3500}, {"aD",0.8f}, {"aS",0}, {"aR",0.7f}, {"revOn",1}, {"revSize",0.6f}, {"revMix",0.28f} } },
    { "Cowbell Perreo", { {"aWave",2}, {"bWave",2}, {"bSemi",7}, {"bLvl",0.8f}, {"cutoff",2600}, {"reso",2}, {"aD",0.18f}, {"aS",0}, {"aR",0.1f}, {"fltType",2}, {"master",0.7f} } },
    { "Vibes Bell", { {"aWave",0}, {"bWave",0}, {"bOct",2}, {"bFine",6}, {"bLvl",0.3f}, {"cutoff",6500}, {"lfoShape",0}, {"lfoDest",3}, {"lfoRate",5}, {"lfoDepth",0.2f}, {"aD",1.2f}, {"aS",0}, {"aR",0.9f}, {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f} } },
    { "Kalimba Bell", { {"aWave",3}, {"bWave",0}, {"bSemi",12}, {"bLvl",0.3f}, {"cutoff",2600}, {"fltEnv",0.4f}, {"fD",0.18f}, {"fS",0}, {"aD",0.5f}, {"aS",0}, {"aR",0.4f}, {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f} } },
    { "Glock Latino", { {"aWave",0}, {"aSemi",12}, {"bWave",0}, {"bOct",1}, {"bSemi",7}, {"bLvl",0.4f}, {"cutoff",11000}, {"aD",0.9f}, {"aS",0}, {"aR",1}, {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.35f} } },
    { "Tubular Romance", { {"aWave",11}, {"bWave",0}, {"bOct",1}, {"bLvl",0.25f}, {"cutoff",4000}, {"aD",2}, {"aS",0}, {"aR",2}, {"choOn",1}, {"choRate",0.4f}, {"choDepth",0.5f}, {"choMix",0.3f}, {"revOn",1}, {"revSize",0.85f}, {"revMix",0.45f}, {"master",0.6f} } },
    { "Chime Calle", { {"aWave",0}, {"bWave",7}, {"bOct",1}, {"bLvl",0.3f}, {"cutoff",8000}, {"aD",1.5f}, {"aS",0}, {"aR",1.5f}, {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.35f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f} } },
    // ARPS
    { "Dembow Gate", { {"aWave",1}, {"bWave",2}, {"bLvl",0.4f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",7}, {"lfoDepth",1}, {"cutoff",3200}, {"reso",2}, {"aS",0.9f}, {"aR",0.1f}, {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.45f}, {"revMix",0.2f}, {"master",0.6f} } },
    { "Perreo Pulse Arp", { {"aWave",4}, {"pw",0.35f}, {"bWave",1}, {"bLvl",0.4f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",8.5f}, {"lfoDepth",1}, {"cutoff",3000}, {"reso",2}, {"aS",0.9f}, {"aR",0.1f}, {"choOn",1}, {"choRate",0.7f}, {"choDepth",0.4f}, {"choMix",0.3f}, {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },
    { "Latin Gate Arp", { {"aWave",3}, {"bWave",1}, {"bLvl",0.3f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",6.5f}, {"lfoDepth",1}, {"cutoff",3800}, {"aS",0.9f}, {"aR",0.15f}, {"dlyOn",1}, {"dlyTime",375}, {"dlyFb",0.3f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.55f}, {"revMix",0.25f} } },
    { "Guitar Gate", { {"aWave",1}, {"bWave",3}, {"bLvl",0.3f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",9}, {"lfoDepth",1}, {"cutoff",2200}, {"fltEnv",0.5f}, {"fD",0.1f}, {"fS",0.3f}, {"aS",0.9f}, {"aR",0.08f}, {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },
    { "Neon Arp", { {"aWave",1}, {"bWave",1}, {"bFine",14}, {"bLvl",0.8f}, {"uniVoices",3}, {"uniDetune",0.25f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",10}, {"lfoDepth",1}, {"cutoff",6000}, {"aS",0.9f}, {"aR",0.1f}, {"choOn",1}, {"choRate",0.6f}, {"choDepth",0.5f}, {"choMix",0.35f}, {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.35f}, {"dlyMix",0.25f}, {"revOn",1}, {"revSize",0.6f}, {"revMix",0.3f}, {"master",0.5f} } },
    { "Reggaeton Sequence", { {"aWave",2}, {"bWave",1}, {"bLvl",0.4f}, {"lfoShape",4}, {"lfoDest",1}, {"lfoRate",7}, {"lfoDepth",0.8f}, {"cutoff",2400}, {"reso",3}, {"aS",0.9f}, {"aR",0.12f}, {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f}, {"master",0.6f} } },
    { "Flow Stutter", { {"aWave",1}, {"bWave",2}, {"bLvl",0.4f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",12}, {"lfoDepth",1}, {"cutoff",3500}, {"reso",2}, {"aS",0.9f}, {"aR",0.06f}, {"drvOn",1}, {"drive",1.5f}, {"master",0.6f} } },
    { "Bounce Gate", { {"aWave",4}, {"pw",0.25f}, {"bWave",1}, {"bLvl",0.3f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",5}, {"lfoDepth",0.9f}, {"cutoff",2800}, {"reso",2.5f}, {"aS",0.9f}, {"aR",0.15f}, {"dlyOn",1}, {"dlyTime",250}, {"dlyFb",0.3f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f} } },
    { "Club Arp", { {"aWave",1}, {"bWave",1}, {"bOct",1}, {"bFine",10}, {"bLvl",0.5f}, {"uniVoices",2}, {"uniDetune",0.2f}, {"lfoShape",3}, {"lfoDest",3}, {"lfoRate",14}, {"lfoDepth",1}, {"cutoff",5500}, {"aS",0.9f}, {"aR",0.06f}, {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f}, {"master",0.55f} } },
    // FX
    { "Reggaeton Riser", { {"aLvl",0}, {"noiseLvl",0.8f}, {"fltType",0}, {"cutoff",400}, {"fltEnv",1}, {"fA",3}, {"fD",0.5f}, {"fS",1}, {"fR",0.4f}, {"aA",3}, {"aD",0.5f}, {"aS",1}, {"aR",0.2f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.3f}, {"master",0.6f} } },
    { "Perreo Impact", { {"aWave",0}, {"aOct",-2}, {"noiseLvl",0.3f}, {"pitEnv",10}, {"pitDec",0.25f}, {"cutoff",3500}, {"aA",0.001f}, {"aD",1.8f}, {"aS",0}, {"aR",1}, {"revOn",1}, {"revSize",0.8f}, {"revMix",0.4f}, {"master",0.8f} } },
    { "Sweep Up", { {"aLvl",0}, {"noiseLvl",0.7f}, {"fltType",2}, {"cutoff",500}, {"reso",4}, {"fltEnv",1}, {"fA",2.5f}, {"fD",0.3f}, {"fS",1}, {"fR",0.3f}, {"aA",2.5f}, {"aD",0.3f}, {"aS",1}, {"aR",0.2f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.3f}, {"master",0.6f} } },
    { "Sweep Down", { {"aLvl",0}, {"noiseLvl",0.7f}, {"fltType",2}, {"cutoff",8000}, {"reso",4}, {"fltEnv",-0.9f}, {"fA",0.005f}, {"fD",2}, {"fS",0}, {"fR",0.5f}, {"aA",0.005f}, {"aD",2}, {"aS",0}, {"aR",0.5f}, {"revOn",1}, {"revSize",0.7f}, {"revMix",0.3f}, {"master",0.6f} } },
    { "Air Horn", { {"aWave",1}, {"bWave",1}, {"bSemi",3}, {"bLvl",0.9f}, {"uniVoices",3}, {"uniDetune",0.15f}, {"cutoff",3500}, {"reso",1.5f}, {"aA",0.01f}, {"aS",0.9f}, {"aR",0.15f}, {"drvOn",1}, {"drive",2.5f}, {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f}, {"master",0.6f} } },
    { "Club Siren", { {"aWave",2}, {"bWave",1}, {"bLvl",0.5f}, {"lfoShape",1}, {"lfoDest",2}, {"lfoRate",1.2f}, {"lfoDepth",1}, {"cutoff",5000}, {"aS",0.9f}, {"aR",0.2f}, {"drvOn",1}, {"drive",2}, {"revOn",1}, {"revSize",0.5f}, {"revMix",0.2f}, {"master",0.55f} } },
    { "Whoosh", { {"aLvl",0}, {"noiseLvl",0.8f}, {"fltType",2}, {"cutoff",600}, {"reso",2}, {"fltEnv",1}, {"fA",0.6f}, {"fD",0.8f}, {"fS",0}, {"fR",0.5f}, {"aA",0.5f}, {"aD",0.8f}, {"aS",0}, {"aR",0.4f}, {"revOn",1}, {"revSize",0.6f}, {"revMix",0.25f}, {"master",0.6f} } },
    { "Laser Dembow", { {"aWave",2}, {"pitEnv",-24}, {"pitDec",0.06f}, {"cutoff",7000}, {"aD",0.25f}, {"aS",0}, {"aR",0.15f}, {"dlyOn",1}, {"dlyTime",280}, {"dlyFb",0.3f}, {"dlyMix",0.2f}, {"revOn",1}, {"revSize",0.4f}, {"revMix",0.15f}, {"master",0.6f} } },
    { "Record Scratch", { {"aWave",1}, {"noiseLvl",0.2f}, {"pitEnv",-12}, {"pitDec",0.3f}, {"cutoff",4000}, {"reso",2}, {"aD",0.5f}, {"aS",0}, {"aR",0.2f}, {"master",0.6f} } }
    };

    return table;
}

//==============================================================================
// Tabla final: por cada (genero, categoria)
//   - PLUCKS..CHOIRS (6 primeras): 5 presets base + 4 extra
//   - BASS, BELLS, ARPS, FX: 9 presets propios
//==============================================================================
const std::vector<PerreoPreset>& getPresetTable()
{
    static const std::vector<PerreoPreset> table = []
    {
        constexpr int classic = 6;      // categorias con presets base + extra
        constexpr int baseN   = 5;
        constexpr int extraN  = PresetLayout::perCategory - baseN;
        static_assert (extraN == 4, "perCategory debe ser 9 (5 base + 4 extra)");
        static_assert (PresetLayout::numCategories == classic + 4, "6 clasicas + BASS/BELLS/ARPS/FX");

        const auto& base  = getBasePresets();
        const auto& extra = getExtraPresets();
        const auto& fresh = getNewCategoryPresets();

        std::vector<PerreoPreset> out;
        out.reserve ((size_t) PresetLayout::total);

        for (int g = 0; g < PresetLayout::numGenres; ++g)
        {
            for (int c = 0; c < PresetLayout::numCategories; ++c)
            {
                if (c < classic)
                {
                    const int group = g * classic + c;

                    for (int i = 0; i < baseN; ++i)
                        out.push_back (base[(size_t) (group * baseN + i)]);

                    for (int i = 0; i < extraN; ++i)
                        out.push_back (extra[(size_t) (group * extraN + i)]);
                }
                else
                {
                    const int group = g * (PresetLayout::numCategories - classic) + (c - classic);

                    for (int i = 0; i < PresetLayout::perCategory; ++i)
                        out.push_back (fresh[(size_t) (group * PresetLayout::perCategory + i)]);
                }
            }
        }

        return out;
    }();

    return table;
}
