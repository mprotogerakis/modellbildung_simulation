/*
 * Fl_sf.h
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "Fl_sf".
 *
 * Model version              : 1.7
 * Simulink Coder version : 9.9 (R2023a) 19-Nov-2022
 * C source code generated on : Sat May 25 14:43:41 2024
 *
 * Target selection: rtwsfcn.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Emulation hardware selection:
 *    Differs from embedded hardware (MATLAB Host)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Fl_sf_h_
#define RTW_HEADER_Fl_sf_h_
#ifndef Fl_sf_COMMON_INCLUDES_
#define Fl_sf_COMMON_INCLUDES_
#include <stdlib.h>
#define S_FUNCTION_NAME                Fl_sf
#define S_FUNCTION_LEVEL               2
#ifndef RTW_GENERATED_S_FUNCTION
#define RTW_GENERATED_S_FUNCTION
#endif

#include "rtwtypes.h"
#include "simstruc.h"
#include "fixedpoint.h"
#if !defined(MATLAB_MEX_FILE)
#include "rt_matrx.h"
#endif

#if !defined(RTW_SFUNCTION_DEFINES)
#define RTW_SFUNCTION_DEFINES

typedef struct {
  void *blockIO;
  void *defaultParam;
  void *nonContDerivSig;
} LocalS;

#define ssSetLocalBlockIO(S, io)       ((LocalS *)ssGetUserData(S))->blockIO = ((void *)(io))
#define ssGetLocalBlockIO(S)           ((LocalS *)ssGetUserData(S))->blockIO
#define ssSetLocalDefaultParam(S, paramVector) ((LocalS *)ssGetUserData(S))->defaultParam = (paramVector)
#define ssGetLocalDefaultParam(S)      ((LocalS *)ssGetUserData(S))->defaultParam
#define ssSetLocalNonContDerivSig(S, pSig) ((LocalS *)ssGetUserData(S))->nonContDerivSig = (pSig)
#define ssGetLocalNonContDerivSig(S)   ((LocalS *)ssGetUserData(S))->nonContDerivSig
#endif
#endif                                 /* Fl_sf_COMMON_INCLUDES_ */

#include "Fl_sf_types.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include "rt_nonfinite.h"
#include <stddef.h>
#include "rt_defines.h"

typedef struct {
  char cache_0[1*sizeof(real_T)];
} nonContDerivSigCache_Fl_T;

/* Block signals (default storage) */
typedef struct {
  real_T qmlsec;                       /* '<S1>/Transfer Fcn' */
  real_T hmm;                          /* '<S1>/Integrator' */
  real_T uDLookupTable;                /* '<S1>/1-D Lookup Table' */
  real_T mlmm;                         /* '<S1>/ml -> mm³' */
  real_T uA;                           /* '<S1>/1//A' */
  real_T Add4;                         /* '<S1>/Add4' */
  real_T TransfghportDelay;            /* '<S1>/Transfghport Delay' */
} B_Fl_T;

/* Continuous states (default storage) */
typedef struct {
  real_T TransferFcn_CSTATE;           /* '<S1>/Transfer Fcn' */
  real_T Integrator_CSTATE;            /* '<S1>/Integrator' */
} X_Fl_T;

/* State derivatives (default storage) */
typedef struct {
  real_T TransferFcn_CSTATE;           /* '<S1>/Transfer Fcn' */
  real_T Integrator_CSTATE;            /* '<S1>/Integrator' */
} XDot_Fl_T;

/* State disabled  */
typedef struct {
  boolean_T TransferFcn_CSTATE;        /* '<S1>/Transfer Fcn' */
  boolean_T Integrator_CSTATE;         /* '<S1>/Integrator' */
} XDis_Fl_T;

/* Continuous State Absolute Tolerance  */
typedef struct {
  real_T TransferFcn_CSTATE;           /* '<S1>/Transfer Fcn' */
  real_T Integrator_CSTATE;            /* '<S1>/Integrator' */
} CStateAbsTol_Fl_T;

/* Continuous State Perturb Min  */
typedef struct {
  real_T TransferFcn_CSTATE;           /* '<S1>/Transfer Fcn' */
  real_T Integrator_CSTATE;            /* '<S1>/Integrator' */
} CXPtMin_Fl_T;

/* Continuous State Perturb Max  */
typedef struct {
  real_T TransferFcn_CSTATE;           /* '<S1>/Transfer Fcn' */
  real_T Integrator_CSTATE;            /* '<S1>/Integrator' */
} CXPtMax_Fl_T;

/* Zero-crossing (trigger) state */
typedef struct {
  real_T Integrator_IntgUpLimit_ZC;    /* '<S1>/Integrator' */
  real_T Integrator_IntgLoLimit_ZC;    /* '<S1>/Integrator' */
  real_T Integrator_LeaveSaturate_ZC;  /* '<S1>/Integrator' */
} ZCV_Fl_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Expression: [-11.6,-10.1,-9.0,-7.65,-6.15,-4.4,-2.65,0,0,2.65,4.4,6.15,7.65,9.0,10.1,11.6]
   * Referenced by: '<S1>/1-D Lookup Table'
   */
  real_T uDLookupTable_tableData[16];

  /* Expression: [-10,-9,-8,-7,-6,-5,-4,-3,3,4,5,6,7,8,9,10]
   * Referenced by: '<S1>/1-D Lookup Table'
   */
  real_T uDLookupTable_bp01Data[16];
} ConstP_Fl_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T *V_IN;                        /* '<Root>/V_IN' */
  real_T *h_abmmsec;                   /* '<Root>/h_ab[mm//sec]' */
} ExternalUPtrs_Fl_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T *qmlsec;                      /* '<Root>/q [ml//sec]' */
  real_T *hmm;                         /* '<Root>/h [mm]' */
} ExtY_Fl_T;

/* Constant parameters (default storage) */
extern const ConstP_Fl_T Fl_ConstP;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('ModellRegelstrecke/Flüssigkeitsstrecke')    - opens subsystem ModellRegelstrecke/Flüssigkeitsstrecke
 * hilite_system('ModellRegelstrecke/Flüssigkeitsstrecke/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'ModellRegelstrecke'
 * '<S1>'   : 'ModellRegelstrecke/Flüssigkeitsstrecke'
 */
#endif                                 /* RTW_HEADER_Fl_sf_h_ */
