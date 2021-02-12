#include <Porting.h>
#include <AMD.h>
#include <AGESA.h>

#include "CommonReturns.h"

#include <heapManager.h>
#include <CreateStruct.h>

#include <Options.h>

#include <agesa-entry-cfg.h>

CONST FUNCTION_PARAMS_INFO ROMDATA FuncParamsInfo[] =
{
  #if AGESA_ENTRY_INIT_RESET == TRUE
    { AMD_INIT_RESET,
      sizeof (AMD_RESET_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitResetConstructor,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_RESET_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_EARLY == TRUE
    { AMD_INIT_EARLY,
      sizeof (AMD_EARLY_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitEarlyInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_EARLY_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_ENV == TRUE
    { AMD_INIT_ENV,
      sizeof (AMD_ENV_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitEnvInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_ENV_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_LATE == TRUE
    { AMD_INIT_LATE,
      sizeof (AMD_LATE_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitLateInitializer,
      (PF_AGESA_DESTRUCTOR) AmdInitLateDestructor,
      AMD_INIT_LATE_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_MID == TRUE
    { AMD_INIT_MID,
      sizeof (AMD_MID_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitMidInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_MID_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_POST == TRUE
    { AMD_INIT_POST,
      sizeof (AMD_POST_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitPostInitializer,
      (PF_AGESA_DESTRUCTOR) AmdInitPostDestructor,
      AMD_INIT_POST_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_RESUME == TRUE
    { AMD_INIT_RESUME,
      sizeof (AMD_RESUME_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitResumeInitializer,
      (PF_AGESA_DESTRUCTOR) AmdInitResumeDestructor,
      AMD_INIT_RESUME_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
    { AMD_S3LATE_RESTORE,
      sizeof (AMD_S3LATE_PARAMS),
      (PF_AGESA_FUNCTION) AmdS3LateRestoreInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_S3_LATE_RESTORE_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_S3SAVE == TRUE
    { AMD_S3_SAVE,
      sizeof (AMD_S3SAVE_PARAMS),
      (PF_AGESA_FUNCTION) AmdS3SaveInitializer,
      (PF_AGESA_DESTRUCTOR) AmdS3SaveDestructor,
      AMD_S3_SAVE_HANDLE
    },
  #endif

  #if AGESA_ENTRY_LATE_RUN_AP_TASK == TRUE
    { AMD_LATE_RUN_AP_TASK,
      sizeof (AP_EXE_PARAMS),
      (PF_AGESA_FUNCTION) AmdLateRunApTaskInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_LATE_RUN_AP_TASK_HANDLE
    },
  #endif
  { 0, 0, NULL }
};

CONST UINTN InitializerCount = ARRAY_SIZE(FuncParamsInfo);

CONST DISPATCH_TABLE ROMDATA DispatchTable[] =
{
  { AMD_CREATE_STRUCT, (IMAGE_ENTRY)AmdCreateStruct },
  { AMD_RELEASE_STRUCT, (IMAGE_ENTRY)AmdReleaseStruct },

  #if AGESA_ENTRY_INIT_RESET == TRUE
    { AMD_INIT_RESET, (IMAGE_ENTRY)AmdInitReset },
  #endif

  #if AGESA_ENTRY_INIT_EARLY == TRUE
    { AMD_INIT_EARLY, (IMAGE_ENTRY)AmdInitEarly },
  #endif

  #if AGESA_ENTRY_INIT_POST == TRUE
    { AMD_INIT_POST, (IMAGE_ENTRY)AmdInitPost },
  #endif

  #if AGESA_ENTRY_INIT_ENV == TRUE
    { AMD_INIT_ENV, (IMAGE_ENTRY)AmdInitEnv },
  #endif

  #if AGESA_ENTRY_INIT_MID == TRUE
    { AMD_INIT_MID, (IMAGE_ENTRY)AmdInitMid },
  #endif

  #if AGESA_ENTRY_INIT_LATE == TRUE
    { AMD_INIT_LATE, (IMAGE_ENTRY)AmdInitLate },
  #endif

  #if AGESA_ENTRY_INIT_S3SAVE == TRUE
    { AMD_S3_SAVE, (IMAGE_ENTRY)AmdS3Save },
  #endif

  #if AGESA_ENTRY_INIT_RESUME == TRUE
    { AMD_INIT_RESUME, (IMAGE_ENTRY)AmdInitResume },
  #endif

  #if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
    { AMD_S3LATE_RESTORE, (IMAGE_ENTRY)AmdS3LateRestore },
  #endif

    { AMD_READ_EVENT_LOG, (IMAGE_ENTRY)AmdReadEventLog },

  #if AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE
    { AMD_GET_APIC_ID, (IMAGE_ENTRY)AmdGetApicId },
    { AMD_GET_PCI_ADDRESS, (IMAGE_ENTRY)AmdGetPciAddress },
    { AMD_IDENTIFY_CORE, (IMAGE_ENTRY)AmdIdentifyCore },
    { AMD_IDENTIFY_DIMMS, (IMAGE_ENTRY)AmdIdentifyDimm },
    { AMD_GET_EXECACHE_SIZE, (IMAGE_ENTRY)AmdGetAvailableExeCacheSize },
  #endif

  #if AGESA_ENTRY_LATE_RUN_AP_TASK == TRUE
    { AMD_LATE_RUN_AP_TASK, (IMAGE_ENTRY)AmdLateRunApTask },
  #endif
  { 0, NULL }
};
