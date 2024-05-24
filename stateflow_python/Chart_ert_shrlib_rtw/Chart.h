/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Chart.h
 *
 * Code generated for Simulink model 'Chart'.
 *
 * Model version                  : 1.4
 * Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
 * C/C++ source code generated on : Fri May 24 21:02:48 2024
 *
 * Target selection: ert_shrlib.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Emulation hardware selection:
 *    Differs from embedded hardware (MATLAB Host)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Chart_h_
#define RTW_HEADER_Chart_h_
#ifndef Chart_COMMON_INCLUDES_
#define Chart_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Chart_COMMON_INCLUDES_ */

#include "Chart_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  uint8_T is_active_c3_Chart;          /* '<Root>/Chart' */
  uint8_T is_c3_Chart;                 /* '<Root>/Chart' */
} DW_Chart_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  boolean_T inA;                       /* '<Root>/inA' */
  boolean_T inB;                       /* '<Root>/inB' */
} ExtU_Chart_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  boolean_T out;                       /* '<Root>/out' */
} ExtY_Chart_T;

/* Real-time Model Data Structure */
struct tag_RTM_Chart_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_Chart_T Chart_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_Chart_T Chart_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_Chart_T Chart_Y;

/* Model entry point functions */
extern void Chart_initialize(void);
extern void Chart_step(void);
extern void Chart_terminate(void);

/* Real-time Model object */
extern RT_MODEL_Chart_T *const Chart_M;

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
 * hilite_system('AB/Chart')    - opens subsystem AB/Chart
 * hilite_system('AB/Chart/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'AB'
 * '<S1>'   : 'AB/Chart'
 */
#endif                                 /* RTW_HEADER_Chart_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
