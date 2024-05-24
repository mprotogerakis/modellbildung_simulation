/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Chart.c
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

#include "Chart.h"
#include "rtwtypes.h"

/* Named constants for Chart: '<Root>/Chart' */
#define Chart_IN_A                     ((uint8_T)1U)
#define Chart_IN_B                     ((uint8_T)2U)

/* Block states (default storage) */
DW_Chart_T Chart_DW;

/* External inputs (root inport signals with default storage) */
ExtU_Chart_T Chart_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_Chart_T Chart_Y;

/* Real-time model */
static RT_MODEL_Chart_T Chart_M_;
RT_MODEL_Chart_T *const Chart_M = &Chart_M_;

/* Model step function */
void Chart_step(void)
{
  /* Chart: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/inA'
   *  Inport: '<Root>/inB'
   */
  /* Gateway: Chart */
  /* During: Chart */
  if (Chart_DW.is_active_c3_Chart == 0U) {
    /* Entry: Chart */
    Chart_DW.is_active_c3_Chart = 1U;

    /* Entry Internal: Chart */
    /* Transition: '<S1>:2' */
    Chart_DW.is_c3_Chart = Chart_IN_A;

    /* Outport: '<Root>/out' */
    /* Entry 'A': '<S1>:1' */
    Chart_Y.out = true;
  } else if (Chart_DW.is_c3_Chart == Chart_IN_A) {
    /* During 'A': '<S1>:1' */
    if (Chart_U.inB) {
      /* Transition: '<S1>:4' */
      Chart_DW.is_c3_Chart = Chart_IN_B;

      /* Outport: '<Root>/out' */
      /* Entry 'B': '<S1>:3' */
      Chart_Y.out = false;
    } else {
      /* Outport: '<Root>/out' */
      Chart_Y.out = true;
    }

    /* During 'B': '<S1>:3' */
  } else if (Chart_U.inA) {
    /* Transition: '<S1>:5' */
    Chart_DW.is_c3_Chart = Chart_IN_A;

    /* Outport: '<Root>/out' */
    /* Entry 'A': '<S1>:1' */
    Chart_Y.out = true;
  } else {
    /* Outport: '<Root>/out' */
    Chart_Y.out = false;
  }

  /* End of Chart: '<Root>/Chart' */
}

/* Model initialize function */
void Chart_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void Chart_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
