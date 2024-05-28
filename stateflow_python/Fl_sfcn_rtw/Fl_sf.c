/*
 * Fl_sf.c
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

#include "Fl_sf.h"
#include "rtwtypes.h"
#include "Fl_sf_private.h"
#include "multiword_types.h"
#include "simstruc.h"
#include "fixedpoint.h"
#if defined(RT_MALLOC) || defined(MATLAB_MEX_FILE)

extern void *Fl_malloc(SimStruct *S);

#endif

#ifndef __RTW_UTFREE__
#if defined (MATLAB_MEX_FILE)

extern void * utMalloc(size_t);
extern void utFree(void *);

#endif
#endif                                 /* #ifndef __RTW_UTFREE__ */

#if defined(MATLAB_MEX_FILE)
#include "rt_nonfinite.c"
#endif

static const char_T *RT_MEMORY_ALLOCATION_ERROR =
  "memory allocation error in generated S-Function";

/*
 * Time delay interpolation routine
 *
 * The linear interpolation is performed using the formula:
 *
 * (t2 - tMinusDelay)         (tMinusDelay - t1)
 * u(t)  =  ----------------- * u1  +  ------------------- * u2
 * (t2 - t1)                  (t2 - t1)
 */
real_T Fl_sf_rt_TDelayInterpolate(
  real_T tMinusDelay,                 /* tMinusDelay = currentSimTime - delay */
  real_T tStart,
  real_T *uBuf,
  int_T bufSz,
  int_T *lastIdx,
  int_T oldestIdx,
  int_T newIdx,
  real_T initOutput,
  boolean_T discrete,
  boolean_T minorStepAndTAtLastMajorOutput)
{
  int_T i;
  real_T yout, t1, t2, u1, u2;
  real_T* tBuf = uBuf + bufSz;

  /*
   * If there is only one data point in the buffer, this data point must be
   * the t= 0 and tMinusDelay > t0, it ask for something unknown. The best
   * guess if initial output as well
   */
  if ((newIdx == 0) && (oldestIdx ==0 ) && (tMinusDelay > tStart))
    return initOutput;

  /*
   * If tMinusDelay is less than zero, should output initial value
   */
  if (tMinusDelay <= tStart)
    return initOutput;

  /* For fixed buffer extrapolation:
   * if tMinusDelay is small than the time at oldestIdx, if discrete, output
   * tailptr value,  else use tailptr and tailptr+1 value to extrapolate
   * It is also for fixed buffer. Note: The same condition can happen for transport delay block where
   * use tStart and and t[tail] other than using t[tail] and t[tail+1].
   * See below
   */
  if ((tMinusDelay <= tBuf[oldestIdx] ) ) {
    if (discrete) {
      return(uBuf[oldestIdx]);
    } else {
      int_T tempIdx= oldestIdx + 1;
      if (oldestIdx == bufSz-1)
        tempIdx = 0;
      t1= tBuf[oldestIdx];
      t2= tBuf[tempIdx];
      u1= uBuf[oldestIdx];
      u2= uBuf[tempIdx];
      if (t2 == t1) {
        if (tMinusDelay >= t2) {
          yout = u2;
        } else {
          yout = u1;
        }
      } else {
        real_T f1 = (t2-tMinusDelay) / (t2-t1);
        real_T f2 = 1.0 - f1;

        /*
         * Use Lagrange's interpolation formula.  Exact outputs at t1, t2.
         */
        yout = f1*u1 + f2*u2;
      }

      return yout;
    }
  }

  /*
   * When block does not have direct feedthrough, we use the table of
   * values to extrapolate off the end of the table for delays that are less
   * than 0 (less then step size).  This is not completely accurate.  The
   * chain of events is as follows for a given time t.  Major output - look
   * in table.  Update - add entry to table.  Now, if we call the output at
   * time t again, there is a new entry in the table. For very small delays,
   * this means that we will have a different answer from the previous call
   * to the output fcn at the same time t.  The following code prevents this
   * from happening.
   */
  if (minorStepAndTAtLastMajorOutput) {
    /* pretend that the new entry has not been added to table */
    if (newIdx != 0) {
      if (*lastIdx == newIdx) {
        (*lastIdx)--;
      }

      newIdx--;
    } else {
      if (*lastIdx == newIdx) {
        *lastIdx = bufSz-1;
      }

      newIdx = bufSz - 1;
    }
  }

  i = *lastIdx;
  if (tBuf[i] < tMinusDelay) {
    /* Look forward starting at last index */
    while (tBuf[i] < tMinusDelay) {
      /* May occur if the delay is less than step-size - extrapolate */
      if (i == newIdx)
        break;
      i = ( i < (bufSz-1) ) ? (i+1) : 0;/* move through buffer */
    }
  } else {
    /*
     * Look backwards starting at last index which can happen when the
     * delay time increases.
     */
    while (tBuf[i] >= tMinusDelay) {
      /*
       * Due to the entry condition at top of function, we
       * should never hit the end.
       */
      i = (i > 0) ? i-1 : (bufSz-1);   /* move through buffer */
    }

    i = ( i < (bufSz-1) ) ? (i+1) : 0;
  }

  *lastIdx = i;
  if (discrete) {
    /*
     * tempEps = 128 * eps;
     * localEps = max(tempEps, tempEps*fabs(tBuf[i]))/2;
     */
    double tempEps = (DBL_EPSILON) * 128.0;
    double localEps = tempEps * fabs(tBuf[i]);
    if (tempEps > localEps) {
      localEps = tempEps;
    }

    localEps = localEps / 2.0;
    if (tMinusDelay >= (tBuf[i] - localEps)) {
      yout = uBuf[i];
    } else {
      if (i == 0) {
        yout = uBuf[bufSz-1];
      } else {
        yout = uBuf[i-1];
      }
    }
  } else {
    if (i == 0) {
      t1 = tBuf[bufSz-1];
      u1 = uBuf[bufSz-1];
    } else {
      t1 = tBuf[i-1];
      u1 = uBuf[i-1];
    }

    t2 = tBuf[i];
    u2 = uBuf[i];
    if (t2 == t1) {
      if (tMinusDelay >= t2) {
        yout = u2;
      } else {
        yout = u1;
      }
    } else {
      real_T f1 = (t2-tMinusDelay) / (t2-t1);
      real_T f2 = 1.0 - f1;

      /*
       * Use Lagrange's interpolation formula.  Exact outputs at t1, t2.
       */
      yout = f1*u1 + f2*u2;
    }
  }

  return(yout);
}

real_T Fl_look1_binlxpw(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T maxIndex)
{
  real_T frac;
  real_T yL_0d0;
  uint32_T iLeft;

  /* Column-major Lookup 1-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  } else if (u0 < bp0[maxIndex]) {
    uint32_T bpIdx;
    uint32_T iRght;

    /* Binary Search */
    bpIdx = maxIndex >> 1U;
    iLeft = 0U;
    iRght = maxIndex;
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = (u0 - bp0[maxIndex - 1U]) / (bp0[maxIndex] - bp0[maxIndex - 1U]);
  }

  /* Column-major Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  yL_0d0 = table[iLeft];
  return (table[iLeft + 1U] - yL_0d0) * frac + yL_0d0;
}

/* System initialize for root system: '<Root>' */
#define MDL_INITIALIZE_CONDITIONS

static void mdlInitializeConditions(SimStruct *S)
{
  if (ssIsFirstInitCond(S)) {
    X_Fl_T *_rtX;
    _rtX = ((X_Fl_T *) ssGetContStates(S));

    /* InitializeConditions for TransferFcn: '<S1>/Transfer Fcn' */
    _rtX->TransferFcn_CSTATE = 0.0;

    /* InitializeConditions for Integrator: '<S1>/Integrator' */
    _rtX->Integrator_CSTATE = 0.0;
  } else {
    X_Fl_T *_rtX;
    _rtX = ((X_Fl_T *) ssGetContStates(S));

    /* InitializeConditions for TransferFcn: '<S1>/Transfer Fcn' */
    _rtX->TransferFcn_CSTATE = 0.0;

    /* InitializeConditions for Integrator: '<S1>/Integrator' */
    _rtX->Integrator_CSTATE = 0.0;
  }
}

/* Start for root system: '<Root>' */
#define MDL_START

static void mdlStart(SimStruct *S)
{
  /* instance underlying S-Function data */
#if defined(RT_MALLOC) || defined(MATLAB_MEX_FILE)
#if defined(MATLAB_MEX_FILE)

  /* non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* Check for invalid switching between solver types */
  if (!ssIsVariableStepSolver(S)) {
    ssSetErrorStatus(S, "This Simulink Coder generated "
                     "S-Function cannot be used in a simulation with "
                     "a solver type of fixed-step "
                     "because this S-Function was created from a model with "
                     "solver type of variable-step solver and it has continuous time blocks. "
                     "See the Solver page of the simulation parameters dialog.");
    return;
  }

#endif

  Fl_malloc(S);
  if (ssGetErrorStatus(S) != (NULL) ) {
    return;
  }

#endif

  {
    B_Fl_T *_rtB;
    _rtB = ((B_Fl_T *) ssGetLocalBlockIO(S));

    /* Start for TransportDelay: '<S1>/Transfghport Delay' */
    {
      real_T *pBuffer = &((real_T*) ssGetDWork(S, 0))[1];
      ((int_T*) ssGetDWork(S, 2))[0] = 0;
      ((int_T*) ssGetDWork(S, 2))[1] = 0;
      ((int_T*) ssGetDWork(S, 2))[2] = 0;
      ((int_T*) ssGetDWork(S, 2))[3] = 1024;
      pBuffer[0] = 0.0;
      pBuffer[1024] = ssGetT(S);
      ((void**) ssGetDWork(S, 1))[0] = (void *) &pBuffer[0];
    }
  }
}

/* Outputs for root system: '<Root>' */
static void mdlOutputs(SimStruct *S, int_T tid)
{
  B_Fl_T *_rtB;
  X_Fl_T *_rtX;
  _rtX = ((X_Fl_T *) ssGetContStates(S));
  _rtB = ((B_Fl_T *) ssGetLocalBlockIO(S));
  if (1) {
    /* TransferFcn: '<S1>/Transfer Fcn' */
    _rtB->qmlsec = 0.0;
    _rtB->qmlsec += 4.4444444444444446 * _rtX->TransferFcn_CSTATE;

    /* Outport: '<Root>/q [ml//sec]' */
    ((real_T *)ssGetOutputPortSignal(S, 0))[0] = _rtB->qmlsec;

    /* Integrator: '<S1>/Integrator' */
    /* Limited  Integrator  */
    if (ssIsModeUpdateTimeStep(S)) {
      if (_rtX->Integrator_CSTATE >= 153.0) {
        if (_rtX->Integrator_CSTATE != 153.0) {
          _rtX->Integrator_CSTATE = 153.0;
          ssSetBlockStateForSolverChangedAtMajorStep(S);
        }

        ((int_T *)ssGetDWork(S, 3))[0] = 3;
      } else if (_rtX->Integrator_CSTATE <= 0.0) {
        if (_rtX->Integrator_CSTATE != 0.0) {
          _rtX->Integrator_CSTATE = 0.0;
          ssSetBlockStateForSolverChangedAtMajorStep(S);
        }

        ((int_T *)ssGetDWork(S, 3))[0] = 4;
      } else {
        if (((int_T *)ssGetDWork(S, 3))[0] != 0) {
          ssSetBlockStateForSolverChangedAtMajorStep(S);
        }

        ((int_T *)ssGetDWork(S, 3))[0] = 0;
      }

      /* Integrator: '<S1>/Integrator' */
      _rtB->hmm = _rtX->Integrator_CSTATE;
    } else {
      /* Integrator: '<S1>/Integrator' */
      _rtB->hmm = _rtX->Integrator_CSTATE;
    }

    /* End of Integrator: '<S1>/Integrator' */

    /* Outport: '<Root>/h [mm]' */
    ((real_T *)ssGetOutputPortSignal(S, 1))[0] = _rtB->hmm;
  }

  if (ssIsSampleHit(S, 1, 0)) {
    /* Lookup_n-D: '<S1>/1-D Lookup Table' */
    _rtB->uDLookupTable = Fl_look1_binlxpw(*((const real_T **)
      ssGetInputPortSignalPtrs(S, 0))[0], Fl_ConstP.uDLookupTable_bp01Data,
      Fl_ConstP.uDLookupTable_tableData, 15U);
  }

  if (1) {
    /* Gain: '<S1>/ml -> mm³' */
    _rtB->mlmm = 1000.0 * _rtB->qmlsec;

    /* Gain: '<S1>/1//A' */
    _rtB->uA = 0.0015384615384615385 * _rtB->mlmm;

    /* Sum: '<S1>/Add4' */
    _rtB->Add4 = _rtB->uA - *((const real_T **)ssGetInputPortSignalPtrs(S, 1))[0];

    /* TransportDelay: '<S1>/Transfghport Delay' */
    {
      real_T **uBuffer = (real_T**)&((void**) ssGetDWork(S, 1))[0];
      real_T simTime = ssGetT(S);
      real_T tMinusDelay = simTime - 0.25;
      _rtB->TransfghportDelay = Fl_sf_rt_TDelayInterpolate(
        tMinusDelay,
        0.0,
        *uBuffer,
        ((int_T*) ssGetDWork(S, 2))[3],
        &((int_T*) ssGetDWork(S, 2))[2],
        ((int_T*) ssGetDWork(S, 2))[0],
        ((int_T*) ssGetDWork(S, 2))[1],
        0.0,
        1,
        (boolean_T) (ssIsMinorTimeStep(S) && ((*uBuffer + ((int_T*) ssGetDWork(S,
        2))[3])[((int_T*) ssGetDWork(S, 2))[1]] == ssGetT(S))));
    }
  }

  {
    nonContDerivSigCache_Fl_T *pNonContDerivSig = (nonContDerivSigCache_Fl_T *)
      ssGetLocalNonContDerivSig(S);
    if (memcmp(pNonContDerivSig->cache_0, (char_T *)&((B_Fl_T *)
          ssGetLocalBlockIO(S))->uDLookupTable,
               1*sizeof(real_T)) != 0) {
      (void) memcpy(pNonContDerivSig->cache_0, (char_T *)&((B_Fl_T *)
        ssGetLocalBlockIO(S))->uDLookupTable,
                    1*sizeof(real_T));
      ssSetSolverNeedsReset(S);
    }
  }

  UNUSED_PARAMETER(tid);
}

/* Update for root system: '<Root>' */
#define MDL_UPDATE

static void mdlUpdate(SimStruct *S, int_T tid)
{
  B_Fl_T *_rtB;
  XDis_Fl_T *_rtXdis;
  _rtXdis = ((XDis_Fl_T *) ssGetContStateDisabled(S));
  _rtB = ((B_Fl_T *) ssGetLocalBlockIO(S));
  if (1) {
    /* Update for Integrator: '<S1>/Integrator' */
    switch (((int_T *)ssGetDWork(S, 3))[0]) {
     case 3:
      if (_rtB->Add4 < 0.0) {
        ((int_T *)ssGetDWork(S, 3))[0] = 1;
        ssSetBlockStateForSolverChangedAtMajorStep(S);
      }
      break;

     case 4:
      if (_rtB->Add4 > 0.0) {
        ((int_T *)ssGetDWork(S, 3))[0] = 2;
        ssSetBlockStateForSolverChangedAtMajorStep(S);
      }
      break;
    }

    _rtXdis->Integrator_CSTATE = ((((int_T *)ssGetDWork(S, 3))[0] == 3) ||
      (((int_T *)ssGetDWork(S, 3))[0] == 4));

    /* End of Update for Integrator: '<S1>/Integrator' */

    /* Update for TransportDelay: '<S1>/Transfghport Delay' */
    {
      real_T **uBuffer = (real_T**)&((void**) ssGetDWork(S, 1))[0];
      real_T simTime = ssGetT(S);
      ((int_T*) ssGetDWork(S, 2))[1] = ((((int_T*) ssGetDWork(S, 2))[1] <
        (((int_T*) ssGetDWork(S, 2))[3]-1)) ? (((int_T*) ssGetDWork(S, 2))[1]+1)
        : 0);
      if (((int_T*) ssGetDWork(S, 2))[1] == ((int_T*) ssGetDWork(S, 2))[0]) {
        ((int_T*) ssGetDWork(S, 2))[0] = ((((int_T*) ssGetDWork(S, 2))[0] <
          (((int_T*) ssGetDWork(S, 2))[3]-1)) ? (((int_T*) ssGetDWork(S, 2))[0]+
          1) : 0);
      }

      (*uBuffer + ((int_T*) ssGetDWork(S, 2))[3])[((int_T*) ssGetDWork(S, 2))[1]]
        = simTime;
      (*uBuffer)[((int_T*) ssGetDWork(S, 2))[1]] = _rtB->uDLookupTable;
    }
  }

  UNUSED_PARAMETER(tid);
}

/* Derivatives for root system: '<Root>' */
#define MDL_DERIVATIVES

static void mdlDerivatives(SimStruct *S)
{
  B_Fl_T *_rtB;
  XDis_Fl_T *_rtXdis;
  XDot_Fl_T *_rtXdot;
  X_Fl_T *_rtX;
  _rtXdis = ((XDis_Fl_T *) ssGetContStateDisabled(S));
  _rtXdot = ((XDot_Fl_T *) ssGetdX(S));
  _rtX = ((X_Fl_T *) ssGetContStates(S));
  _rtB = ((B_Fl_T *) ssGetLocalBlockIO(S));

  /* Derivatives for TransferFcn: '<S1>/Transfer Fcn' */
  _rtXdot->TransferFcn_CSTATE = -4.4444444444444446 * _rtX->TransferFcn_CSTATE;
  _rtXdot->TransferFcn_CSTATE += _rtB->TransfghportDelay;

  /* Derivatives for Integrator: '<S1>/Integrator' */
  if (_rtXdis->Integrator_CSTATE) {
    _rtXdot->Integrator_CSTATE = 0.0;
  } else {
    _rtXdot->Integrator_CSTATE = _rtB->Add4;
  }

  /* End of Derivatives for Integrator: '<S1>/Integrator' */
}

/* ZeroCrossings for root system: '<Root>' */
#define MDL_ZERO_CROSSINGS

static void mdlZeroCrossings(SimStruct *S)
{
  B_Fl_T *_rtB;
  X_Fl_T *_rtX;
  ZCV_Fl_T *_rtZCSV;
  _rtZCSV = ((ZCV_Fl_T *) ssGetNonsampledZCs(S));
  _rtX = ((X_Fl_T *) ssGetContStates(S));
  _rtB = ((B_Fl_T *) ssGetLocalBlockIO(S));

  /* ZeroCrossings for Integrator: '<S1>/Integrator' */
  switch (((int_T *)ssGetDWork(S, 3))[0]) {
   case 1:
    _rtZCSV->Integrator_IntgUpLimit_ZC = 0.0;
    _rtZCSV->Integrator_IntgLoLimit_ZC = 153.0;
    break;

   case 2:
    _rtZCSV->Integrator_IntgUpLimit_ZC = -153.0;
    _rtZCSV->Integrator_IntgLoLimit_ZC = 0.0;
    break;

   default:
    _rtZCSV->Integrator_IntgUpLimit_ZC = _rtX->Integrator_CSTATE - 153.0;
    _rtZCSV->Integrator_IntgLoLimit_ZC = _rtX->Integrator_CSTATE;
    break;
  }

  if ((((int_T *)ssGetDWork(S, 3))[0] == 3) || (((int_T *)ssGetDWork(S, 3))[0] ==
       4)) {
    _rtZCSV->Integrator_LeaveSaturate_ZC = _rtB->Add4;
  } else {
    _rtZCSV->Integrator_LeaveSaturate_ZC = 0.0;
  }

  /* End of ZeroCrossings for Integrator: '<S1>/Integrator' */
}

/* Termination for root system: '<Root>' */
static void mdlTerminate(SimStruct *S)
{
  B_Fl_T *_rtB;
  _rtB = ((B_Fl_T *) ssGetLocalBlockIO(S));

#if defined(RT_MALLOC) || defined(MATLAB_MEX_FILE)

  if (ssGetUserData(S) != (NULL) ) {
    rt_FREE(ssGetLocalBlockIO(S));
    rt_FREE(ssGetLocalNonContDerivSig(S));
  }

  rt_FREE(ssGetUserData(S));

#endif

}

#if defined(RT_MALLOC) || defined(MATLAB_MEX_FILE)
#include "Fl_mid.h"
#endif

/* Function to initialize sizes. */
static void mdlInitializeSizes(SimStruct *S)
{
  ssSetNumSampleTimes(S, 2);           /* Number of sample times */
  ssSetNumContStates(S, 2);            /* Number of continuous states */
  ssSetNumPeriodicContStates(S, 0);   /* Number of periodic continuous states */
  ssSetNumNonsampledZCs(S, 3);         /* Number of nonsampled ZCs */
  ssSetZCCacheNeedsReset(S, 1);
  ssSetDerivCacheNeedsReset(S, 0);

  /* Number of output ports */
  if (!ssSetNumOutputPorts(S, 2))
    return;

  /* outport number: 0 */
  if (!ssSetOutputPortVectorDimension(S, 0, 1))
    return;
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetOutputPortDataType(S, 0, SS_DOUBLE);
  }

  ssSetOutputPortSampleTime(S, 0, 0.0);
  ssSetOutputPortOffsetTime(S, 0, 0.0);
  ssSetOutputPortOptimOpts(S, 0, SS_REUSABLE_AND_LOCAL);

  /* outport number: 1 */
  if (!ssSetOutputPortVectorDimension(S, 1, 1))
    return;
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetOutputPortDataType(S, 1, SS_DOUBLE);
  }

  ssSetOutputPortSampleTime(S, 1, 0.0);
  ssSetOutputPortOffsetTime(S, 1, 0.0);
  ssSetOutputPortOptimOpts(S, 1, SS_REUSABLE_AND_LOCAL);

  /* Number of input ports */
  if (!ssSetNumInputPorts(S, 2))
    return;

  /* inport number: 0 */
  {
    if (!ssSetInputPortVectorDimension(S, 0, 1))
      return;
    if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
      ssSetInputPortDataType(S, 0, SS_DOUBLE);
    }

    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortSampleTime(S, 0, 0.0);
    ssSetInputPortOffsetTime(S, 0, 1.0);
    ssSetInputPortOverWritable(S, 0, 0);
    ssSetInputPortOptimOpts(S, 0, SS_NOT_REUSABLE_AND_GLOBAL);
  }

  /* inport number: 1 */
  {
    if (!ssSetInputPortVectorDimension(S, 1, 1))
      return;
    if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
      ssSetInputPortDataType(S, 1, SS_DOUBLE);
    }

    ssSetInputPortDirectFeedThrough(S, 1, 1);
    ssSetInputPortSampleTime(S, 1, 0.0);
    ssSetInputPortOffsetTime(S, 1, 1.0);
    ssSetInputPortOverWritable(S, 1, 0);
    ssSetInputPortOptimOpts(S, 1, SS_NOT_REUSABLE_AND_GLOBAL);
  }

  ssSetRTWGeneratedSFcn(S, 1);         /* Generated S-function */

  /* DWork */
  if (!ssSetNumDWork(S, 4)) {
    return;
  }

  /* '<S1>/Transfghport Delay': RWORK */
  ssSetDWorkName(S, 0, "DWORK0");
  ssSetDWorkWidth(S, 0, 2049);

  /* '<S1>/Transfghport Delay': PWORK */
  ssSetDWorkName(S, 1, "DWORK1");
  ssSetDWorkWidth(S, 1, 2);
  ssSetDWorkDataType(S, 1, SS_POINTER);

  /* '<S1>/Transfghport Delay': IWORK */
  ssSetDWorkName(S, 2, "DWORK2");
  ssSetDWorkWidth(S, 2, 4);
  ssSetDWorkDataType(S, 2, SS_INTEGER);

  /* '<S1>/Integrator': MODE */
  ssSetDWorkName(S, 3, "DWORK3");
  ssSetDWorkWidth(S, 3, 1);
  ssSetDWorkDataType(S, 3, SS_INTEGER);

  /* Tunable Parameters */
  ssSetNumSFcnParams(S, 0);

  /* Number of expected parameters */
#if defined(MATLAB_MEX_FILE)

  if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {

#if defined(MDL_CHECK_PARAMETERS)

    mdlCheckParameters(S);

#endif                                 /* MDL_CHECK_PARAMETERS */

    if (ssGetErrorStatus(S) != (NULL) ) {
      return;
    }
  } else {
    return;                /* Parameter mismatch will be reported by Simulink */
  }

#endif                                 /* MATLAB_MEX_FILE */

  /* Options */
  ssSetOptions(S, (SS_OPTION_RUNTIME_EXCEPTION_FREE_CODE |
                   SS_OPTION_PORT_SAMPLE_TIMES_ASSIGNED ));

#if SS_SFCN_FOR_SIM

  {
    ssSupportsMultipleExecInstances(S, true);
    ssHasStateInsideForEachSS(S, false);
  }

#endif

}

/* Function to initialize sample times. */
static void mdlInitializeSampleTimes(SimStruct *S)
{
  /* task periods */
  ssSetSampleTime(S, 0, 0.0);
  ssSetSampleTime(S, 1, 0.0);

  /* task offsets */
  ssSetOffsetTime(S, 0, 0.0);
  ssSetOffsetTime(S, 1, 1.0);
}

#if defined(MATLAB_MEX_FILE)
#include "fixedpoint.c"
#include "simulink.c"
#else
#undef S_FUNCTION_NAME
#define S_FUNCTION_NAME                Fl_sf
#include "cg_sfun.h"
#endif                                 /* defined(MATLAB_MEX_FILE) */
