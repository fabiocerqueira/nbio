#ifndef _NITGEN_NBioAPI_EXTENSION_H
#define _NITGEN_NBioAPI_EXTENSION_H

#include "NBioAPI.h"
#include "NBioAPI_ExportType.h"

#ifdef __cplusplus
extern "C" {
#endif


/* NBioAPI_CAPTURE_OPTION */
typedef struct nbioapi_capture_option {
   NBioAPI_UINT32   Length;
   NBioAPI_UINT32   Reserved1[8];
   NBioAPI_VOID_PTR Reserved2;
} NBioAPI_CAPTURE_OPTION, *NBioAPI_CAPTURE_OPTION_PTR;


#ifndef _NBioAPI_LOAD_LIBRARY

/* Extension Functions */
NBioAPI_RETURN NBioAPI NBioAPI_CompareTwoFIR(
                                          NBioAPI_HANDLE                  hHandle, 
                                          const NBioAPI_INPUT_FIR_PTR     pProcessedFIR,
                                          const NBioAPI_INPUT_FIR_PTR     pStoredTemplate,
                                          NBioAPI_SINT32*                 pMatchScore,
                                          NBioAPI_SINT32*                 pHitNumber,
                                          const NBioAPI_MATCH_OPTION_PTR  pMatchOption);

NBioAPI_RETURN NBioAPI NBioAPI_CaptureEx(
                                          NBioAPI_HANDLE                  hHandle, 
                                          NBioAPI_FIR_PURPOSE             nPurpose,
                                          NBioAPI_FIR_HANDLE_PTR          phCapturedFIR,
                                          NBioAPI_SINT32                  nTimeout,
                                          NBioAPI_FIR_HANDLE_PTR          phAuditData,
                                          const NBioAPI_WINDOW_OPTION_PTR pWindowOption,
                                          const NBioAPI_CAPTURE_OPTION_PTR pCapOptionEx);

NBioAPI_RETURN NBioAPI NBioAPI_VerifyEx(
                                          NBioAPI_HANDLE                  hHandle, 
                                          const NBioAPI_INPUT_FIR_PTR     piStoredTemplate,
                                          NBioAPI_BOOL*                   pbResult,
                                          NBioAPI_FIR_PAYLOAD_PTR         pPayload,
                                          NBioAPI_SINT32                  nTimeout,
                                          NBioAPI_FIR_HANDLE_PTR          phAuditData,
                                          const NBioAPI_WINDOW_OPTION_PTR pWindowOption,
                                          const NBioAPI_CAPTURE_OPTION_PTR pCapOptionEx);

NBioAPI_RETURN NBioAPI NBioAPI_ProcessEx(
                                          NBioAPI_HANDLE                  hHandle, 
                                          const NBioAPI_INPUT_FIR_PTR     piCapturedFIR,
                                          NBioAPI_FIR_HANDLE_PTR          phProcessedFIR,
                                          NBioAPI_BOOL                    bIsLP);

/* Image Import Function */
NBioAPI_RETURN NBioAPI NBioAPI_ImageToNBioBSPEx(
                                          NBioAPI_HANDLE                  hHandle, 
                                          NBioAPI_EXPORT_AUDIT_DATA_PTR   pExportAuditData,
                                          NBioAPI_FIR_HANDLE_PTR          phAuditFIR,
                                          NBioAPI_BOOL                    bIsWaterMark);

/* Internal Communication */
NBioAPI_RETURN NBioAPI NBioAPI_GetInitInfoEx(NBioAPI_HANDLE hHandle);
NBioAPI_RETURN NBioAPI NBioAPI_SetInitInfoEx(NBioAPI_HANDLE hHandle);

/* UpdateFIR */
NBioAPI_RETURN NBioAPI NBioAPI_UpdateFIR(NBioAPI_HANDLE hHandle,
                                         const NBioAPI_INPUT_FIR_PTR     piProcessedFIR,
                                         const NBioAPI_INPUT_FIR_PTR     piStoredTemplate,
                                         NBioAPI_BOOL*                   pbIsUpdated,
                                         NBioAPI_FIR_HANDLE_PTR          phUpdatedFIR,
                                         NBioAPI_UINT32                  nFlag);
                                          
#endif/*_NBioAPI_LOAD_LIBRARY */


/* typedef for GetProcAddress() */

/* Extension Functions */
typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_CompareTwoFIR)(
                                             NBioAPI_HANDLE                  hHandle, 
                                             const NBioAPI_INPUT_FIR_PTR     pProcessedFIR,
                                             const NBioAPI_INPUT_FIR_PTR     pStoredTemplate,
                                             NBioAPI_SINT32*                 pMatchScore,
                                             NBioAPI_SINT32*                 pHitNumber,
                                             const NBioAPI_MATCH_OPTION_PTR  pMatchOption);

typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_CaptureEx)(
                                         NBioAPI_HANDLE                  hHandle, 
                                         NBioAPI_FIR_PURPOSE             nPurpose,
                                         NBioAPI_FIR_HANDLE_PTR          phCapturedFIR,
                                         NBioAPI_SINT32                  nTimeout,
                                         NBioAPI_FIR_HANDLE_PTR          phAuditData,
                                         const NBioAPI_WINDOW_OPTION_PTR pWindowOption,
                                         const NBioAPI_CAPTURE_OPTION_PTR pCapOptionEx);

typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_VerifyEx)(
                                        NBioAPI_HANDLE                  hHandle, 
                                        const NBioAPI_INPUT_FIR_PTR     piStoredTemplate,
                                        NBioAPI_BOOL*                   pbResult,
                                        NBioAPI_FIR_PAYLOAD_PTR         pPayload,
                                        NBioAPI_SINT32                  nTimeout,
                                        NBioAPI_FIR_HANDLE_PTR          phAuditData,
                                        const NBioAPI_WINDOW_OPTION_PTR pWindowOption,
                                        const NBioAPI_CAPTURE_OPTION_PTR pCapOptionEx);

typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_ProcessEx)(
                                         NBioAPI_HANDLE                  hHandle, 
                                         const NBioAPI_INPUT_FIR_PTR     piCapturedFIR,
                                         NBioAPI_FIR_HANDLE_PTR          phProcessedFIR,
                                         NBioAPI_BOOL                    bIsLP);

/* Image Import Function */
typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_ImageToNBioBSPEx)(
                                          NBioAPI_HANDLE                  hHandle, 
                                          NBioAPI_EXPORT_AUDIT_DATA_PTR   pExportAuditData,
                                          NBioAPI_FIR_HANDLE_PTR          phAuditFIR,
                                          NBioAPI_BOOL                    bIsWaterMark);

/* Internal Communication */
typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_GetInitInfoEx)(NBioAPI_HANDLE hHandle);
typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_SetInitInfoEx)(NBioAPI_HANDLE hHandle);

/* UpdateFIR */
typedef NBioAPI_RETURN (NBioAPI* FP_NBioAPI_UpdateFIR)(NBioAPI_HANDLE hHandle,
                                         const NBioAPI_INPUT_FIR_PTR     piProcessedFIR,
                                         const NBioAPI_INPUT_FIR_PTR     piStoredTemplate,
                                         NBioAPI_BOOL*                   pbIsUpdated,
                                         NBioAPI_FIR_HANDLE_PTR          phUpdatedFIR,
                                         NBioAPI_UINT32                  nFlag);

#ifdef __cplusplus
}
#endif

#endif/*_NITGEN_NBioAPI_EXTENSION_H */
